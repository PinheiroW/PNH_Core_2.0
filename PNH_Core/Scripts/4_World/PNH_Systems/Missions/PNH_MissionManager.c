class PNH_MissionManager
{
    static PNH_MissionManager m_Instance;

    ref PNH_MissionBase m_ActiveMission;
    ref array<ref PNH_MissionConfig> m_MissionsPool;
    
    float m_UpdateTimer;
    int m_MissionState;     
    int m_CooldownTimer;

    void PNH_MissionManager()
    {
        m_Instance = this; 
        m_UpdateTimer = 0;
        m_MissionState = 0;
        m_CooldownTimer = 15; 
        m_MissionsPool = new array<ref PNH_MissionConfig>;
        ReloadMissions();
    }

    static PNH_MissionManager GetInstance() { return m_Instance; }

    void ReloadMissions()
    {
        PNH_JSONLoader.Carregar(m_MissionsPool);
        PNH_Logger.Log("Missões", "[PNH_CORE] Banco de dados JSON sincronizado. Total: " + m_MissionsPool.Count() + " missões.");
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
                if (m_ActiveMission.m_MissionTime >= m_ActiveMission.m_MissionTimeout) EndMission();
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
        if (m_MissionsPool.Count() == 0)
        {
            m_CooldownTimer = 60; 
            return;
        }

        int idx = Math.RandomIntInclusive(0, m_MissionsPool.Count() - 1);
        ref PNH_MissionConfig config = m_MissionsPool.Get(idx);

        if (!config) return;

        if (config.Tipo == "Camp") m_ActiveMission = new CampMission();
        else if (config.Tipo == "Horde") m_ActiveMission = new HordeMission();
        else if (config.Tipo == "PlaneCrash") m_ActiveMission = new PlaneCrashMission();
        else if (config.Tipo == "Apartment") m_ActiveMission = new ApartmentMission();
        else return;

        // --- A PONTE MÁGICA: Passando os dados do JSON para a missão ---
        m_ActiveMission.m_Config = config; 
        
        m_ActiveMission.m_MissionPosition = config.Posicao;
        m_ActiveMission.m_MissionLocation = config.Local;
        m_ActiveMission.m_MissionInformant = config.Informante;
        
        if (config.Mensagens.Count() >= 3)
        {
            m_ActiveMission.m_MissionMessage1 = config.Mensagens[0];
            m_ActiveMission.m_MissionMessage2 = config.Mensagens[1];
            m_ActiveMission.m_MissionMessage3 = config.Mensagens[2];
        }

        if (m_ActiveMission.DeployMission())
        {
            m_MissionState = 1;
            PNH_Utils.SendMessageToAll("[RADIO PNH] Alguém na escuta? Recebemos um sinal!");
            GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(this.SendMissionStory, 4000, false);
        }
        else
        {
            m_ActiveMission = null;
            m_CooldownTimer = 10;
        }
    }

    void SendMissionStory()
    {
        if (m_ActiveMission)
        {
            PNH_Utils.SendMessageToAll("[INFORMANTE] " + m_ActiveMission.m_MissionInformant + ": " + m_ActiveMission.m_MissionMessage1);
            GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(PNH_Utils.SendMessageToAll, 6000, false, "[INFORMANTE] " + m_ActiveMission.m_MissionMessage2);
            GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(PNH_Utils.SendMessageToAll, 12000, false, "[INFORMANTE] " + m_ActiveMission.m_MissionMessage3);
        }
    }

    void EndMission()
    {
        if (m_ActiveMission) m_ActiveMission.CleanUp(); 
        m_ActiveMission = null; 
        m_MissionState = 0;
        m_CooldownTimer = 1800; 
    }
}