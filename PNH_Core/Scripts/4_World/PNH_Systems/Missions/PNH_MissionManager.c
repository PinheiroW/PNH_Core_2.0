/// --- PNH_MissionManager.c (v2.1.6 - CORRIGIDO) ---
/// Função: Centralizar o sorteio, estados e limpeza das missões no servidor.

class PNH_MissionManager
{
    static ref PNH_MissionManager m_Instance; 
    ref PNH_MissionBase m_ActiveMission;
    int m_MissionState;     // 0: Idle, 1: Disponível, 2: Materializando, 3: Ativa
    int m_CooldownTimer;
    float m_UpdateFrequency = 2.0; 
    float m_TimerAccumulator;

    // Construtor do Gestor
    void PNH_MissionManager() 
    { 
        m_MissionState = 0; 
        m_TimerAccumulator = 0;
        PNH_MissionSettings.Load(); // Carrega as configurações globais do JSON
        m_CooldownTimer = 30; 
    }

    // Singleton para acesso global
    static PNH_MissionManager GetInstance() 
    { 
        if (!m_Instance) m_Instance = new PNH_MissionManager(); 
        return m_Instance; 
    }

    // Ciclo de atualização do Gestor
    void OnUpdate(float timeslice)
    {
        m_TimerAccumulator += timeslice;
        
        if (m_TimerAccumulator >= m_UpdateFrequency) 
        {
            m_TimerAccumulator = 0;
            
            switch (m_MissionState)
            {
                case 0: // IDLE - Aguarda o tempo de Cooldown para sortear
                    if (m_CooldownTimer <= 0)
                    {
                        StartRandomMission();
                    }
                    else
                    {
                        m_CooldownTimer -= 2;
                    }
                    break;

                case 3: // ATIVA - A missão está em curso, monitoriza progresso e rádio
                    if (m_ActiveMission)
                    {
                        m_ActiveMission.MissionChecks();
                    }
                    break;
            }
        }
    }

    // Sorteia e prepara a próxima missão (Sem materializar ainda)
    void StartRandomMission()
    {
        if (m_ActiveMission) return;

        // Sorteio Simples (Exemplo: 50% de chance para cada tipo disponível)
        int dice = Math.RandomInt(0, 100);
        
        if (dice < 50)
        {
            m_ActiveMission = new HordeMission();
            m_ActiveMission.m_MissionTier = 1; // Horda é Tier 1
        }
        else
        {
            m_ActiveMission = new PNH_MissionApartment();
            m_ActiveMission.m_MissionTier = 2; // Apartment é Tier 2
            
            // Carrega as coordenadas específicas do Apartment.json
            PNH_MissionConfigData config = LoadMissionConfig("$profile:\\PNH\\Missions\\Apartment.json");
            if (config) m_ActiveMission.m_Config = config;
        }

        // Define a localização (Exemplo fixo para testes ou puxado de banco de dados)
        m_ActiveMission.m_MissionLocation = "Setor Residencial"; 
        m_ActiveMission.m_MissionPosition = "4400.5 7.3 2517.7".ToVector();

        // ESTADO 1: Fica disponível no Tablet, mas NÃO nasce nada no mapa ainda
        m_MissionState = 1; 
        
        PNH_Logger.Log("Mission", "[PNH] Proxima missao pronta: " + m_ActiveMission.m_MissionType);
        PNH_BroadcastManager.GetInstance().AnnounceMissionAvailable(m_ActiveMission.m_MissionLocation);
    }

    // Carregador auxiliar de JSONs de missão
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

    // Finaliza e limpa a missão atual
    void EndMission() 
    { 
        if (m_ActiveMission) m_ActiveMission.CleanUp(); 
        m_ActiveMission = null; 
        m_MissionState = 0;
        
        // Define o tempo de espera para a próxima sorteada
        PNH_MissionSettingsData settings = PNH_MissionSettings.GetData();
        if (settings)
            m_CooldownTimer = settings.ConfiguracoesGerais.TempoEntreMissoesMinutos * 60;
        else
            m_CooldownTimer = 300;
    }
}