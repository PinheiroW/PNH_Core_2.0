class PNH_MissionManager
{
    ref PNH_MissionBase m_ActiveMission;
    
    float m_UpdateTimer;
    int m_MissionState;     
    int m_CooldownTimer;

    void PNH_MissionManager()
    {
        m_UpdateTimer = 0;
        m_MissionState = 0;
        m_CooldownTimer = 15; 
        
        PNH_Logger.Log("Missões", "[PNH_CORE] Mission Manager Inicializado. Carregando satélites...");
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
            else if (m_MissionState == 1)
            {
                if (m_ActiveMission)
                {
                    m_ActiveMission.m_MissionTime += 2;

                    if (m_ActiveMission.m_MissionTime >= m_ActiveMission.m_MissionTimeout)
                    {
                        PNH_Logger.Log("Missões", "[PNH_CORE] Operação Abortada: Tempo esgotado.");
                        EndMission();
                    }
                    else
                    {
                        // CORREÇÃO: Lógica nativa para verificar jogadores no servidor
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
    }

    void StartRandomMission()
    {
        PNH_EventsWorldData.Init();

        if (PNH_EventsWorldData.MissionEvents.Count() == 0)
        {
            PNH_Logger.Log("Missões", "[PNH_CORE] ERRO: Nenhuma coordenada no PNH_EventsWorldData!");
            m_CooldownTimer = 60;
            return;
        }

        int idx = Math.RandomIntInclusive(0, PNH_EventsWorldData.MissionEvents.Count() - 1);
        string eventData = PNH_EventsWorldData.MissionEvents[idx];
        vector eventPos = PNH_EventsWorldData.MissionPositions[idx];

        TStringArray dataArray = new TStringArray;
        eventData.Split(" ", dataArray);
        
        if (dataArray.Count() < 2) return; 

        string missionType = dataArray[0]; 
        string city = dataArray[1];        
        string dir = "";
        if (dataArray.Count() > 2) dir = dataArray[2];         

        if (missionType == "Transport") m_ActiveMission = new TransportMission();
        else if (missionType == "Apartment") m_ActiveMission = new ApartmentMission();
        else if (missionType == "CityMall") m_ActiveMission = new CityMallMission();
        else if (missionType == "UrbanMall") m_ActiveMission = new UrbanMallMission();
        else if (missionType == "Horde") m_ActiveMission = new HordeMission();
        else if (missionType == "PlaneCrash") m_ActiveMission = new PlaneCrashMission();
        else if (missionType == "Psilos") m_ActiveMission = new PsilosMission();
        else if (missionType == "Shrooms") m_ActiveMission = new ShroomsMission();
        else if (missionType == "CityStore") m_ActiveMission = new CityStoreMission();
        else if (missionType == "Camp") m_ActiveMission = new CampMission();
        else if (missionType == "FreePigs") m_ActiveMission = new FreePigsMission();
        else if (missionType == "Ganja") m_ActiveMission = new GanjaMission();
        else if (missionType == "Graveyard") m_ActiveMission = new GraveyardMission();
        else if (missionType == "BearHunt") m_ActiveMission = new BearHuntMission();
        else return;

        PNH_Logger.Log("Missões", "[PNH_CORE] Operação Escolhida: " + missionType + " em " + city + " | POS: " + eventPos.ToString());

        m_ActiveMission.m_MissionPosition = eventPos;
        m_ActiveMission.m_MissionLocation = city;
        m_ActiveMission.m_MissionLocationDir = dir;
        m_ActiveMission.m_MissionDescription = dataArray;

        if (m_ActiveMission.DeployMission())
        {
            m_MissionState = 1; 
            PNH_Utils.SendMessageToAll("[RADIO PNH] Alguém na escuta? Câmbio!");
            GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(this.SendMissionStory, 3000, false);
        }
        else
        {
            PNH_Logger.Log("Missões", "[PNH_CORE] Falha (Local inválido). Sorteando outra em 10s...");
            m_ActiveMission = null;
            m_CooldownTimer = 10; 
        }
    }

    void SendMissionStory()
    {
        if (m_ActiveMission)
        {
            PNH_Utils.SendMessageToAll("[INFORMANTE] " + m_ActiveMission.m_MissionInformant + ": " + m_ActiveMission.m_MissionMessage1);
            GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(PNH_Utils.SendMessageToAll, 5000, false, "[INFORMANTE] " + m_ActiveMission.m_MissionMessage2);
            GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(PNH_Utils.SendMessageToAll, 10000, false, "[INFORMANTE] " + m_ActiveMission.m_MissionMessage3);
        }
    }

    void EndMission()
    {
        // CORREÇÃO: Removemos a chamada direta ao CleanupMission para evitar erro de compilação
        m_ActiveMission = null; 
        m_MissionState = 0;
        m_CooldownTimer = 1800; 
        PNH_Logger.Log("Missões", "[PNH_CORE] Operação arquivada. Mapa limpo.");
    }
}