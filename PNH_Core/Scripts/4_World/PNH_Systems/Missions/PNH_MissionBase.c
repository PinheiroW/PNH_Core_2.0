class PNH_MissionBase
{
    vector m_MissionPosition;
    string m_MissionLocation;
    string m_MissionInformant;
    ref TStringArray m_MissionDescription;

    string m_MissionMessage1;
    string m_MissionMessage2;
    string m_MissionMessage3;

    int m_MissionTimeout;
    int m_MissionTime;
    float m_MissionZoneOuterRadius;
    float m_MissionZoneInnerRadius;

    ref array<Object> m_MissionObjects;
    ref array<Object> m_MissionAIs;

    bool m_RewardsSpawned;
    int m_MsgNum;
    
    // Variável de controle de tempo de mensagem usada por algumas missões portadas
    int m_MsgChkTime; 

    // --- VARIÁVEIS ADICIONADAS PARA SUPORTE ÀS MISSÕES PORTADAS ---
    bool m_MissionExtended;
    ref array<Object> m_ObjectList;
    ref array<CargoBase> m_ObjectCargoList;

    void PNH_MissionBase()
    {
        // Instancia as arrays padrão do Core
        m_MissionObjects = new array<Object>;
        m_MissionAIs = new array<Object>;
        
        // Instancia as arrays de busca utilizadas pelos scripts portados
        m_ObjectList = new array<Object>;           
        m_ObjectCargoList = new array<CargoBase>;   
        
        // Valores padrão
        m_MissionPosition = "0 0 0";
        m_MissionTime = 0;
        m_RewardsSpawned = false;
        m_MsgNum = 0;
        m_MsgChkTime = 0;
        m_MissionExtended = false;
    }

    // Métodos virtuais que serão sobrescritos (override) dentro de cada missão
    bool IsExtended() { return false; }
    bool DeployMission() { return false; }
    void PlayerChecks(PlayerBase player) {}
    
    // <-- CORREÇÃO: Função adicionada para satisfazer o 'override' das missões -->
    void MissionFinal() {} 

    // Limpeza padrão chamada pelo Manager ao encerrar a missão
    void CleanUp()
    {
        if (m_MissionObjects)
        {
            foreach (Object obj : m_MissionObjects)
            {
                if (obj) GetGame().ObjectDelete(obj);
            }
        }
        
        if (m_MissionAIs)
        {
            foreach (Object ai : m_MissionAIs)
            {
                if (ai) GetGame().ObjectDelete(ai);
            }
        }
    }
}