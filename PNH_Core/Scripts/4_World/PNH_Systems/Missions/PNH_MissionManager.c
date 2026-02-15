class PNH_MissionManager
{
    static PNH_MissionManager m_Instance;

    ref PNH_MissionBase m_ActiveMission;
    float m_UpdateTimer;
    int m_MissionState;     
    int m_CooldownTimer;

    void PNH_MissionManager()
    {
        m_Instance = this; 
        m_UpdateTimer = 0;
        m_MissionState = 0;
        PNH_MissionSettings.Load();
        m_CooldownTimer = 60; 
    }

    static PNH_MissionManager GetInstance() { return m_Instance; }

    void ReloadMissions()
    {
        PNH_MissionSettings.Load();
        PNH_Logger.Log("Missões", "[PNH_CORE] Configurações Mestre recarregadas com sucesso.");
    }

    void ForceMissionCycle()
    {
        if (m_ActiveMission) 
        {
            m_ActiveMission.CleanUp();
            m_ActiveMission = null;
        }
        m_MissionState = 0;
        m_CooldownTimer = 60;
        PNH_Logger.Log("Missões", "[PNH_CORE] Ciclo de missões resetado por Admin. Nova missão em 60s.");
    }

    void OnUpdate(float timeslice)
    {
        m_UpdateTimer += timeslice;
        if (m_UpdateTimer >= 2.0)
        {
            m_UpdateTimer = 0;
            if (m_MissionState == 0)
            {
                m_CooldownTimer -= 2;
                if (m_CooldownTimer <= 0) StartRandomMission();
            }
            else if (m_MissionState == 1 && m_ActiveMission)
            {
                m_ActiveMission.m_MissionTime += 2;
                if (m_ActiveMission.m_MissionTime >= m_ActiveMission.m_MissionTimeout) 
                {
                    EndMission();
                }
                else
                {
                    array<Man> players = new array<Man>;
                    GetGame().GetPlayers(players);
                    for (int i = 0; i < players.Count(); i++)
                    {
                        PlayerBase p = PlayerBase.Cast(players.Get(i));
                        if (p) m_ActiveMission.PlayerChecks(p);
                    }
                }
            }
        }
    }

    void StartRandomMission()
    {
        PNH_MissionSettingsData config = PNH_MissionSettings.GetData();
        string selectedMission = "";

        if (config.MissoesAtivas.Count() == 0)
        {
            PNH_Logger.Log("Missões", "[PNH_CORE] ERRO: Nenhuma missão ativa no JSON!");
            m_CooldownTimer = 60; 
            return;
        }

        if (config.DebugMission != "") selectedMission = config.DebugMission;
        else selectedMission = config.MissoesAtivas.GetRandomElement();

        if (selectedMission == "Apartment") m_ActiveMission = new ApartmentMission();
        else 
        {
            PNH_Logger.Log("Missões", "[PNH_CORE] ERRO: Missão desconhecida: " + selectedMission);
            m_CooldownTimer = 30;
            return;
        }

        PNH_EventsWorldData.Init(); 
        array<int> validIndexes = new array<int>;
        for (int i = 0; i < PNH_EventsWorldData.MissionEvents.Count(); i++)
        {
            if (PNH_EventsWorldData.MissionEvents.Get(i).IndexOf(selectedMission) == 0) 
                validIndexes.Insert(i);
        }

        if (validIndexes.Count() > 0)
        {
            int selectedIdx = validIndexes.GetRandomElement();
            string eventInfo = PNH_EventsWorldData.MissionEvents.Get(selectedIdx);
            m_ActiveMission.m_MissionPosition = PNH_EventsWorldData.MissionPositions.Get(selectedIdx);
            
            TStringArray eventParts = new TStringArray;
            eventInfo.Split(" ", eventParts);
            if (eventParts.Count() >= 2)
            {
                string cityName = eventParts.Get(1);
                cityName.Replace("_", " "); 
                m_ActiveMission.m_MissionLocation = cityName;
            }
        }
        else { m_ActiveMission = null; m_CooldownTimer = 10; return; }

        if (m_ActiveMission.DeployMission())
        {
            m_MissionState = 1;

            // --- NOTIFICAÇÃO DISCORD E LOG DO SCRIPT ---
            string posStr = m_ActiveMission.m_MissionPosition.ToString();
            string logDiscord = "[PNH_CORE] MISSÃO_INICIADA: " + selectedMission + " em " + m_ActiveMission.m_MissionLocation + " | Coordenadas: " + posStr;
            
            // Força a escrita no arquivo script.log do servidor
            Print("[PNH SYSTEM] " + logDiscord); 
            
            // Envia para o Webhook do Discord (Categoria: Missões)
            PNH_Logger.Log("Missões", logDiscord); 
            // --------------------------------------------

            if (config.UsarPDA) 
            {
                GetRPCManager().SendRPC("[GearPDA] ", "SendGlobalMessage", new Param2<string, string>("Comando PNH", "[ALERTA] Novo contrato disponível. Verifique o seu PDA."), true);
            }
            else 
            {
                PNH_Utils.SendMessageToAll("[RADIO PNH] Alguém na escuta? Recebemos um sinal!");
            }
            
            GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(this.SendMissionStory, 4000, false);
        }
    }

    void SendMissionStory()
    {
        if (m_ActiveMission)
        {
            PNH_MissionSettingsData config = PNH_MissionSettings.GetData();
            if (config.UsarPDA)
            {
                GetRPCManager().SendRPC("[GearPDA] ", "SendGlobalMessage", new Param2<string, string>(m_ActiveMission.m_MissionInformant, m_ActiveMission.m_MissionMessage1), true);
                GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(this.SendExtraPDAMessage, 3000, false, m_ActiveMission.m_MissionMessage2);
                GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(this.SendExtraPDAMessage, 6000, false, m_ActiveMission.m_MissionMessage3);
                GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(this.SendExtraPDAMessage, 9000, false, m_ActiveMission.m_MissionMessage4);
                GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(this.SendExtraPDAMessage, 12000, false, m_ActiveMission.m_MissionMessage5);
            }
            else PNH_Utils.SendMessageToAll("[" + m_ActiveMission.m_MissionInformant + "] " + m_ActiveMission.m_MissionMessage1);
        }
    }

    void SendExtraPDAMessage(string message)
    {
        if (m_ActiveMission && message != "")
            GetRPCManager().SendRPC("[GearPDA] ", "SendGlobalMessage", new Param2<string, string>(m_ActiveMission.m_MissionInformant, message), true);
    }

    void EndMission()
    {
        if (m_ActiveMission) m_ActiveMission.CleanUp(); 
        m_ActiveMission = null; 
        m_MissionState = 0;
        m_CooldownTimer = PNH_MissionSettings.GetData().TempoEntreMissoes * 60; 
    }
}