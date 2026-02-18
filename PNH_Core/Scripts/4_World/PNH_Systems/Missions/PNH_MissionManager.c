class PNH_MissionManager
{
    static ref PNH_MissionManager m_Instance; 
    ref PNH_MissionBase m_ActiveMission;
    int m_MissionState;     
    int m_CooldownTimer;
    float m_UpdateFrequency = 2.0; 
    float m_TimerAccumulator;

    void PNH_MissionManager() 
    { 
        m_MissionState = 0; 
        m_TimerAccumulator = 0;
        PNH_MissionSettings.Load(); 
        m_CooldownTimer = 30; 
    }

    static PNH_MissionManager GetInstance() 
    { 
        if (!m_Instance) m_Instance = new PNH_MissionManager(); 
        return m_Instance; 
    }

    void OnUpdate(float timeslice)
    {
        m_TimerAccumulator += timeslice;
        
        if (m_TimerAccumulator >= m_UpdateFrequency)
        {
            m_TimerAccumulator = 0;
            
            switch (m_MissionState)
            {
                case 0: // IDLE
                    m_CooldownTimer -= 2; 
                    if (m_CooldownTimer <= 0) StartRandomMission(); 
                    break;

                case 1: // DISPONÍVEL
                    break;

                case 2: // MATERIALIZANDO
                    m_ActiveMission.m_MissionTime += 2;
                    if (PNH_TimeManager.HasTimeElapsed(m_ActiveMission.m_MissionTime, 15)) 
                    {
                        m_MissionState = 3; 
                        PNH_AuditManager.LogMissionEvent("Sistema", m_ActiveMission.m_MissionType, "Vigilancia Ativada");
                    }
                    break;

                case 3: // ATIVA
                    m_ActiveMission.m_MissionTime += 2;
                    
                    if (PNH_TimeManager.HasTimeElapsed(m_ActiveMission.m_MissionTime, m_ActiveMission.m_MissionTimeout)) 
                    {
                        PNH_AuditManager.LogMissionEvent(m_ActiveMission.m_MissionOwnerName, m_ActiveMission.m_MissionType, "Falhou por Tempo");
                        EndMission();
                    }
                    else 
                    {
                        // CORREÇÃO: Chama diretamente as verificações sem vasculhar a lista de jogadores
                        m_ActiveMission.MissionChecks(); 
                    }
                    break;
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
        
        PNH_AuditManager.LogMissionEvent("Sistema", "Horde", "Sorteada em Balota_Teste");
        PNH_BroadcastManager.GetInstance().AnnounceMissionAvailable("Balota_Teste");
    }

    void EndMission() 
    { 
        if (m_ActiveMission) m_ActiveMission.CleanUp(); 
        m_ActiveMission = null; 
        m_MissionState = 0; 
        m_CooldownTimer = 300; 
    }

    void ReloadMissions() { PNH_MissionSettings.Load(); }
    
    void ForceMissionCycle() 
    { 
        EndMission(); 
        m_CooldownTimer = 10; 
        PNH_AuditManager.LogAdminAction("Admin", "Ciclo de Missoes Forçado");
    }
}