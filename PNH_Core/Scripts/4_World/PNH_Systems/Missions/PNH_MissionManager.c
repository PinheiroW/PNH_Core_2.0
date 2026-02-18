/// --- Documentação PNH_Core: PNH_MissionManager.c ---
/// Versão do Sistema: 1.1.0 (Integração de Narrativa Dinâmica)
/// Função do arquivo: Atuar como o motor central das missões. Agora, além de gerir estados e timers, é responsável por injetar a Lore correta do dicionário JSON na missão ativa.
/// Comunicação com outros arquivos: Recebe batimento do PNH_Init.c. Consulta PNH_MissionSettings.c para obter o dicionário de lore e alimenta o objeto m_ActiveMission com as etapas de rádio.
/// Motivo da existência no sistema: Centralizar a inteligência de execução e garantir que cada missão sorteada tenha acesso aos seus textos de narrativa específicos.
/// Dependências internas: PNH_MissionSettings.c (dados), PNH_EventsWorldData.c (locais), PNH_AuditManager.c e PNH_TimeManager.c.
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
        // 1. Instanciação Base
        m_ActiveMission = new HordeMission(); 
        m_ActiveMission.m_MissionTier = 1;
        m_ActiveMission.m_MissionType = "Horde";
        
        // --- NOVO: INJEÇÃO DE NARRATIVA DINÂMICA ---
        // Procuramos no dicionário de configurações os diálogos para este tipo de missão
        PNH_MissionSettingsData settings = PNH_MissionSettings.GetData();
        if (settings && settings.DicionarioMissoes)
        {
            foreach (PNH_DicionarioMissao dic : settings.DicionarioMissoes)
            {
                if (dic.TipoMissao == m_ActiveMission.m_MissionType)
                {
                    // Entrega os textos das 4 etapas (Aceitou, 90m, 20m, Final) à missão
                    m_ActiveMission.m_LoreEtapas = dic.Etapas;
                    PNH_Logger.Log("Manager", "[PNH] Narrativa Dinâmica vinculada: " + dic.TipoMissao);
                    break;
                }
            }
        }
        
        PNH_EventsWorldData.Init();
        
        array<int> validIndexes = new array<int>;
        for (int i = 0; i < PNH_EventsWorldData.MissionEvents.Count(); i++)
        {
            string eventName = PNH_EventsWorldData.MissionEvents.Get(i);
            if (eventName.Contains("Horde")) 
            {
                validIndexes.Insert(i);
            }
        }

        if (validIndexes.Count() > 0)
        {
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