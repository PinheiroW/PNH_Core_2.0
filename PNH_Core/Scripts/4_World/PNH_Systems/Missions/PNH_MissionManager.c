class PNH_MissionManager
{
    static ref PNH_MissionManager m_Instance; 
    ref PNH_MissionBase m_ActiveMission;
    int m_MissionState;     
    int m_CooldownTimer;
    float m_UpdateFrequency = 2.0; // Frequência de atualização em segundos
    float m_TimerAccumulator;

    void PNH_MissionManager() 
    { 
        m_MissionState = 0; 
        m_TimerAccumulator = 0;
        PNH_MissionSettings.Load(); 
        
        // Delegamos a definição do tempo inicial ao TimeManager se necessário futuramente
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
        
        // Só processa a lógica a cada 2 segundos para otimizar performance
        if (m_TimerAccumulator >= m_UpdateFrequency)
        {
            m_TimerAccumulator = 0;
            
            switch (m_MissionState)
            {
                case 0: // IDLE / COOLDOWN
                    m_CooldownTimer -= 2; 
                    if (m_CooldownTimer <= 0) StartRandomMission(); 
                    break;

                case 1: // DISPONÍVEL (Aguardando !aceitar no NPC)
                    break;

                case 2: // MATERIALIZANDO (Spawn físico ocorrendo)
                    m_ActiveMission.m_MissionTime += 2;
                    // O TimeManager agora valida se o tempo de materialização (15s) passou
                    if (PNH_TimeManager.HasTimeElapsed(m_ActiveMission.m_MissionTime, 15)) 
                    {
                        m_MissionState = 3; 
                        PNH_AuditManager.LogMissionEvent("Sistema", m_ActiveMission.m_MissionType, "Vigilancia Ativada");
                    }
                    break;

                case 3: // ATIVA (Monitorização de jogadores e alvos)
                    m_ActiveMission.m_MissionTime += 2;
                    
                    // Valida timeout da missão via TimeManager
                    if (PNH_TimeManager.HasTimeElapsed(m_ActiveMission.m_MissionTime, m_ActiveMission.m_MissionTimeout)) 
                    {
                        PNH_AuditManager.LogMissionEvent(m_ActiveMission.m_MissionOwnerName, m_ActiveMission.m_MissionType, "Falhou por Tempo");
                        EndMission();
                    }
                    else 
                    {
                        // Monitorização delegada às verificações da própria missão
                        array<Man> players = new array<Man>; 
                        GetGame().GetPlayers(players);
                        for (int i = 0; i < players.Count(); i++) 
                        {
                            PlayerBase p = PlayerBase.Cast(players.Get(i));
                            if (p) m_ActiveMission.PlayerChecks(p);
                        }
                    }
                    break;
            }
        }
    }

    void StartRandomMission()
    {
        // PNH 2.0: Focado apenas na Horda para estabilização
        m_ActiveMission = new HordeMission(); 
        m_ActiveMission.m_MissionTier = 1;
        m_ActiveMission.m_MissionType = "Horde";
        
        // Coordenadas agora tratadas como dados puros vindos do EventsWorldData
        m_ActiveMission.m_MissionLocation = "Balota_Teste";
        m_ActiveMission.m_MissionPosition = "4400.5 7.3 2517.7".ToVector();

        m_MissionState = 1; 
        
        // Auditoria regista o sorteio
        PNH_AuditManager.LogMissionEvent("Sistema", "Horde", "Sorteada em Balota_Teste");
        
        PNH_BroadcastManager.GetInstance().AnnounceMissionAvailable("Balota_Teste");
    }

    void EndMission() 
    { 
        if (m_ActiveMission) m_ActiveMission.CleanUp(); 
        m_ActiveMission = null; 
        m_MissionState = 0; 
        
        // Cooldown padrão de 5 minutos (300s)
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