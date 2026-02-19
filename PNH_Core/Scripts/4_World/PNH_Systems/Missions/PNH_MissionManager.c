/// --- Documentação PNH_Core: PNH_MissionManager.c ---
/// Versão do Sistema: 2.0.0 (Suporte a Múltiplas Narrativas)
/// Funções atualizadas: StartRandomMission agora suporta instanciar 'Apartment' de forma dinâmica.

class PNH_MissionManager
{
    static ref PNH_MissionManager m_Instance; 
    ref PNH_MissionBase m_ActiveMission;
    int m_MissionState;     
    int m_CooldownTimer;
    float m_UpdateFrequency = 2.0; //
    float m_TimerAccumulator;

    void PNH_MissionManager() 
    { 
        m_MissionState = 0; 
        m_TimerAccumulator = 0;
        PNH_MissionSettings.Load(); 
        m_CooldownTimer = 30; //
    }

    static PNH_MissionManager GetInstance() 
    { 
        if (!m_Instance) m_Instance = new PNH_MissionManager(); 
        return m_Instance; 
    }

    void OnUpdate(float timeslice)
    {
        m_TimerAccumulator += timeslice;
        
        if (m_TimerAccumulator >= m_UpdateFrequency) //
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

    // --- CORREÇÃO: LÓGICA DE SORTEIO DINÂMICO ---
    void StartRandomMission()
    {
        // 1. Decisão do Tipo de Missão (50% de chance para cada)
        float randType = Math.RandomFloat(0, 1);
        
        if (randType > 0.5)
        {
            m_ActiveMission = new PNH_MissionApartment(); // Nova classe para Apartment.json
            m_ActiveMission.m_MissionType = "Apartment";
            m_ActiveMission.m_MissionTier = 2; // Infiltração é mais difícil
        }
        else
        {
            m_ActiveMission = new HordeMission(); 
            m_ActiveMission.m_MissionType = "Horde";
            m_ActiveMission.m_MissionTier = 1;
        }

        // 2. INJEÇÃO DE NARRATIVA DINÂMICA (JSON)
        PNH_MissionSettingsData settings = PNH_MissionSettings.GetData();
        if (settings && settings.DicionarioMissoes)
        {
            foreach (PNH_DicionarioMissao dic : settings.DicionarioMissoes)
            {
                if (dic.TipoMissao == m_ActiveMission.m_MissionType)
                {
                    m_ActiveMission.m_LoreEtapas = dic.Etapas;
                    PNH_Logger.Log("Manager", "[PNH] Narrativa Dinâmica vinculada: " + dic.TipoMissao);
                    break;
                }
            }
        }
        
        // 3. DEFINIÇÃO DE LOCALIZAÇÃO
        PNH_EventsWorldData.Init();
        array<int> validIndexes = new array<int>;
        for (int i = 0; i < PNH_EventsWorldData.MissionEvents.Count(); i++)
        {
            string eventName = PNH_EventsWorldData.MissionEvents.Get(i);
            // Filtra locais que suportam o tipo de missão atual
            if (eventName.Contains(m_ActiveMission.m_MissionType)) 
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
            // Fallback caso não encontre locais específicos no WorldData
            m_ActiveMission.m_MissionLocation = "Setor Residencial"; 
            m_ActiveMission.m_MissionPosition = "4400.5 7.3 2517.7".ToVector();
        }

        m_MissionState = 1; 
        
        PNH_AuditManager.LogMissionEvent("Sistema", m_ActiveMission.m_MissionType, "Sorteada em " + m_ActiveMission.m_MissionLocation);
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