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
        
        // Carrega as configurações do JSON
        PNH_MissionSettings.Load();
        
        // Espera apenas 15 segundos para lançar a primeira missão quando o servidor ligar
        m_CooldownTimer = 15; 
    }

    static PNH_MissionManager GetInstance() { return m_Instance; }

    void ReloadMissions()
    {
        PNH_MissionSettings.Load();
        PNH_Logger.Log("Missões", "[PNH_CORE] Configurações Mestre recarregadas com sucesso.");
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
            PNH_Logger.Log("Missões", "[PNH_CORE] ERRO: Nenhuma missão ativa no JSON de Settings!");
            m_CooldownTimer = 60; 
            return;
        }

        // Lê se o Admin quer testar uma missão específica ou se é aleatório
        if (config.DebugMission != "") selectedMission = config.DebugMission;
        else selectedMission = config.MissoesAtivas.GetRandomElement();

        // Instancia a classe em C++ baseada no nome
        if (selectedMission == "Camp") m_ActiveMission = new CampMission();
        else if (selectedMission == "Horde") m_ActiveMission = new HordeMission();
        else if (selectedMission == "PlaneCrash") m_ActiveMission = new PlaneCrashMission();
        else if (selectedMission == "Apartment") m_ActiveMission = new ApartmentMission();
        else if (selectedMission == "BearHunt") m_ActiveMission = new BearHuntMission();
        else if (selectedMission == "CityStore") m_ActiveMission = new CityStoreMission();
        else if (selectedMission == "Shrooms") m_ActiveMission = new ShroomsMission();
        else if (selectedMission == "Psilos") m_ActiveMission = new PsilosMission();
        else if (selectedMission == "UrbanMall") m_ActiveMission = new UrbanMallMission();
        else if (selectedMission == "Graveyard") m_ActiveMission = new GraveyardMission();
        else 
        {
            PNH_Logger.Log("Missões", "[PNH_CORE] ERRO: Missão desconhecida na lista: " + selectedMission);
            m_CooldownTimer = 30;
            return;
        }

        if (m_ActiveMission.DeployMission())
        {
            m_MissionState = 1;
            
            if (config.UsarPDA)
            {
                // Notificação Global rápida para o ecrã do PDA
                GPDA_ServerManager.GetInstance().AddChatMessage("PNH_CORE", "Comando PNH", "[ALERTA] Novo contrato disponível. Verifique o seu PDA.");
            }
            else
            {
                PNH_Utils.SendMessageToAll("[RADIO PNH] Alguém na escuta? Recebemos um sinal!");
            }

            GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(this.SendMissionStory, 4000, false);
        }
        else
        {
            m_ActiveMission = null;
            m_CooldownTimer = 10; // Falhou em gerar (ex: não encontrou prédio), tenta rápido de novo
        }
    }

    void SendMissionStory()
    {
        if (m_ActiveMission)
        {
            PNH_MissionSettingsData config = PNH_MissionSettings.GetData();
            
            if (config.UsarPDA)
            {
                // Concatena a história inteira e envia para ficar gravado no histórico do PDA
                string fullMessage = m_ActiveMission.m_MissionMessage1 + " " + m_ActiveMission.m_MissionMessage2 + " " + m_ActiveMission.m_MissionMessage3;
                GPDA_ServerManager.GetInstance().AddChatMessage("PNH_CORE", m_ActiveMission.m_MissionInformant, fullMessage);
            }
            else
            {
                // Modo Clássico (Avisos de Rádio sequenciais)
                PNH_Utils.SendMessageToAll("[" + m_ActiveMission.m_MissionInformant + "] " + m_ActiveMission.m_MissionMessage1);
                GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(PNH_Utils.SendMessageToAll, 6000, false, "[" + m_ActiveMission.m_MissionInformant + "] " + m_ActiveMission.m_MissionMessage2);
                GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(PNH_Utils.SendMessageToAll, 12000, false, "[" + m_ActiveMission.m_MissionInformant + "] " + m_ActiveMission.m_MissionMessage3);
            }
        }
    }

    void EndMission()
    {
        if (m_ActiveMission) m_ActiveMission.CleanUp(); 
        m_ActiveMission = null; 
        m_MissionState = 0;
        
        PNH_MissionSettingsData config = PNH_MissionSettings.GetData();
        m_CooldownTimer = config.TempoEntreMissoes * 60; // Volta para o tempo longo configurado pelo Admin
        
        PNH_Logger.Log("Missões", "[PNH_CORE] Operação encerrada. Próxima em " + config.TempoEntreMissoes + " minutos.");
    }
}