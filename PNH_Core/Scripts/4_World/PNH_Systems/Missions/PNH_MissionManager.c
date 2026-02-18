class PNH_MissionManager
{
    static ref PNH_MissionManager m_Instance; 
    ref PNH_MissionBase m_ActiveMission;
    float m_UpdateTimer;
    int m_MissionState;     
    int m_CooldownTimer;

    void PNH_MissionManager() { 
        m_UpdateTimer = 0; 
        m_MissionState = 0; 
        PNH_MissionSettings.Load(); 
        m_CooldownTimer = 30; // PNH 2.0: Tempo para o rádio anunciar no arranque do servidor
    }

    static PNH_MissionManager GetInstance() { if (!m_Instance) m_Instance = new PNH_MissionManager(); return m_Instance; }

    void OnUpdate(float timeslice)
    {
        m_UpdateTimer += timeslice;
        if (m_UpdateTimer >= 2.0)
        {
            m_UpdateTimer = 0;
            if (m_MissionState == 0) { 
                m_CooldownTimer -= 2; 
                if (m_CooldownTimer <= 0) StartRandomMission(); 
            }
            else if (m_MissionState == 2) { // Materializando
                m_ActiveMission.m_MissionTime += 2;
                if (m_ActiveMission.m_MissionTime >= 15) m_MissionState = 3; 
            }
            else if (m_MissionState == 3) { // Ativa e vigiada
                m_ActiveMission.m_MissionTime += 2;
                if (m_ActiveMission.m_MissionTime >= m_ActiveMission.m_MissionTimeout) EndMission();
                else {
                    array<Man> players = new array<Man>; GetGame().GetPlayers(players);
                    for (int i = 0; i < players.Count(); i++) {
                        PlayerBase p = PlayerBase.Cast(players.Get(i));
                        if (p) m_ActiveMission.PlayerChecks(p);
                    }
                }
            }
        }
    }

    void StartRandomMission()
    {
        m_ActiveMission = new HordeMission(); 
        m_ActiveMission.m_MissionTier = 1;
        m_ActiveMission.m_MissionType = "Horde";
        m_ActiveMission.m_MissionLocation = "Balota_Teste";
        m_ActiveMission.m_MissionPosition = "4400.5 7.3 2517.7".ToVector();

        m_MissionState = 1; 
        // Dispara o anúncio de rádio
        PNH_BroadcastManager.GetInstance().AnnounceMissionAvailable("Balota_Teste");
    }

    void EndMission() { if (m_ActiveMission) m_ActiveMission.CleanUp(); m_ActiveMission = null; m_MissionState = 0; m_CooldownTimer = 300; }
    void ReloadMissions() { PNH_MissionSettings.Load(); }
    void ForceMissionCycle() { EndMission(); m_CooldownTimer = 10; }
}