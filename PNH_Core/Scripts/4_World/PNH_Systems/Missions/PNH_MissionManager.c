/// --- Documentação PNH_Core: PNH_MissionManager.c ---
/// Versão do Sistema: 1.0.0 (Ref: PNH_Consts)
/// Função do arquivo: Atuar como o motor central do sistema de missões, gerindo o ciclo de vida completo (IDLE, Disponível, Materializando e Ativa), controlando os timers de cooldown e realizando o sorteio aleatório de novas missões e coordenadas.
/// Comunicação com outros arquivos: Recebe batimento cardíaco (OnUpdate) do PNH_Init.c, utiliza o PNH_EventsWorldData.c para obter coordenadas, instancia classes de missão (como HordeMission), reporta eventos ao PNH_AuditManager.c e solicita anúncios ao PNH_BroadcastManager.c.
/// Motivo da existência do arquivo no sistema: Centralizar a máquina de estados das missões do servidor, garantindo que o fluxo de sorteio, execução e limpeza ocorra de forma automática e organizada.
/// Dependências internas: PNH_MissionSettings.c (carregamento de configs), PNH_EventsWorldData.c (base de dados de locais), PNH_AuditManager.c (registos) e PNH_TimeManager.c (controlo de tempo).
/// Última atualização: 2026-02-18
/// IMPORTANTE: Ao alterar este arquivo, documente no CHANGELOG_PNH.md

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
        
        PNH_EventsWorldData.Init();
        
        // CORREÇÃO: Filtra para escolher APENAS missões da "Horde"
        array<int> validIndexes = new array<int>;
        for (int i = 0; i < PNH_EventsWorldData.MissionEvents.Count(); i++)
        {
            string eventName = PNH_EventsWorldData.MissionEvents.Get(i);
            if (eventName.Contains("Horde")) // Apenas palavras contendo Horde
            {
                validIndexes.Insert(i);
            }
        }

        if (validIndexes.Count() > 0)
        {
            // Sorteia um índice válido do nosso filtro
            int randPos = Math.RandomInt(0, validIndexes.Count());
            int randIndex = validIndexes.Get(randPos);
            
            m_ActiveMission.m_MissionLocation = PNH_EventsWorldData.MissionEvents.Get(randIndex);
            m_ActiveMission.m_MissionPosition = PNH_EventsWorldData.MissionPositions.Get(randIndex);
        }
        else
        {
            m_ActiveMission.m_MissionLocation = "Horde Balota_Teste"; 
            m_ActiveMission.m_MissionPosition = "4400.5 7.3 2517.7".ToVector();
        }

        m_MissionState = 1; 
        
        PNH_AuditManager.LogMissionEvent("Sistema", "Horde", "Sorteada em " + m_ActiveMission.m_MissionLocation);
        PNH_BroadcastManager.GetInstance().AnnounceMissionAvailable(m_ActiveMission.m_MissionLocation);
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