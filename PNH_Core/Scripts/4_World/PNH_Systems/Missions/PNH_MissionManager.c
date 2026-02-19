/// --- Documentação PNH_Core: PNH_MissionManager.c ---
/// Versão do Sistema: 2.1.0 (Suporte a Narrativas Dinâmicas e Carregamento de JSON Externo)
/// Funções atualizadas: StartRandomMission agora carrega configurações específicas de cada tipo de missão.

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

    // --- CORREÇÃO: LÓGICA DE SORTEIO E CARREGAMENTO DE CONFIGURAÇÃO ---
    void StartRandomMission()
    {
        float randType = Math.RandomFloat(0, 1);
        string jsonPath = "";

        // 1. Decisão do Tipo de Missão e Definição do Caminho do JSON
        if (randType > 0.5)
        {
            m_ActiveMission = new PNH_MissionApartment();
            m_ActiveMission.m_MissionType = "Apartment";
            m_ActiveMission.m_MissionTier = 2;
            jsonPath = "$profile:PNH_Settings/Missions/Apartment.json"; // Caminho para o JSON de Infiltração
        }
        else
        {
            m_ActiveMission = new HordeMission(); 
            m_ActiveMission.m_MissionType = "Horde";
            m_ActiveMission.m_MissionTier = 1;
            jsonPath = "$profile:PNH_Settings/Missions/Horde.json"; // Caminho para o JSON de Horda
        }

        // 2. CARREGAMENTO DA CONFIGURAÇÃO ESPECÍFICA DO JSON
        PNH_MissionConfigData config = LoadMissionConfig(jsonPath);
        if (config) 
        {
            m_ActiveMission.m_Config = config;
            PNH_Logger.Log("Manager", "[PNH] Configuração de missão carregada de: " + jsonPath);
        }
        else 
        {
            PNH_Logger.Log("Manager", "[PNH] Erro Crítico: Arquivo de configuração não encontrado em " + jsonPath);
            return; 
        }

        // 3. INJEÇÃO DE NARRATIVA DINÂMICA (PNH_MissionSettings)
        PNH_MissionSettingsData settings = PNH_MissionSettings.GetData();
        if (settings && settings.DicionarioMissoes)
        {
            foreach (PNH_DicionarioMissao dic : settings.DicionarioMissoes)
            {
                if (dic.TipoMissao == m_ActiveMission.m_MissionType)
                {
                    m_ActiveMission.m_LoreEtapas = dic.Etapas;
                    break;
                }
            }
        }
        
        // 4. DEFINIÇÃO DE LOCALIZAÇÃO
        PNH_EventsWorldData.Init();
        array<int> validIndexes = new array<int>;
        for (int i = 0; i < PNH_EventsWorldData.MissionEvents.Count(); i++)
        {
            string eventName = PNH_EventsWorldData.MissionEvents.Get(i);
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
            m_ActiveMission.m_MissionLocation = "Setor Residencial"; 
            m_ActiveMission.m_MissionPosition = "4400.5 7.3 2517.7".ToVector();
        }

        m_MissionState = 1; 
        
        PNH_AuditManager.LogMissionEvent("Sistema", m_ActiveMission.m_MissionType, "Sorteada em " + m_ActiveMission.m_MissionLocation);
        PNH_BroadcastManager.GetInstance().AnnounceMissionAvailable(m_ActiveMission.m_MissionLocation);
        
        // Inicia a execução do cenário
        m_ActiveMission.Deploy();
    }

    // Função auxiliar para carregar o arquivo JSON de configuração da missão
    PNH_MissionConfigData LoadMissionConfig(string path)
    {
        PNH_MissionConfigData data = new PNH_MissionConfigData();
        if (FileExist(path))
        {
            JsonFileLoader<PNH_MissionConfigData>.JsonLoadFile(path, data);
            return data;
        }
        return null;
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