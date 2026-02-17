class PNH_MissionBase
{
    // ==========================================
    // --- NOVAS VARIÁVEIS PNH 2.0 (CONTRATO) ---
    // ==========================================
    string m_MissionOwnerID;
    string m_MissionOwnerName;
    bool m_MissionAccepted;
    int m_MissionTier;
    string m_MissionType; 
    
    // ==========================================
    // --- VARIÁVEIS ORIGINAIS DA MISSÃO ---
    // ==========================================
    string m_MissionInformant;
    string m_MissionLocation;
    string m_MissionMessage1;
    string m_MissionMessage2;
    string m_MissionMessage3;
    string m_MissionMessage4;
    string m_MissionMessage5;

    vector m_MissionPosition;
    float m_MissionTimeout;
    float m_MissionTime;
    float m_MissionZoneOuterRadius;
    float m_MissionZoneInnerRadius;
    bool m_RewardsSpawned;

    ref array<Object> m_MissionObjects = new array<Object>;
    ref array<Object> m_MissionAIs = new array<Object>;
    ref array<Object> m_ObjectList = new array<Object>;
    
    // CORREÇÃO: O tipo deve ser CargoBase para ser compatível com o GetObjectsAtPosition
    ref array<CargoBase> m_ObjectCargoList = new array<CargoBase>; 

    void PNH_MissionBase()
    {
        m_MissionTime = 0;
        m_RewardsSpawned = false;
        
        // PNH 2.0: Inicializa o contrato vazio (A aguardar dono)
        m_MissionOwnerID = "";
        m_MissionOwnerName = "";
        m_MissionAccepted = false;
        m_MissionTier = 1;
        m_MissionType = "Desconhecida";
    }

    // ==========================================
    // --- FUNÇÕES DE CONTRATO (PNH 2.0) --------
    // ==========================================
    
    // Regista o jogador como o Dono Oficial desta Missão
    void AcceptContract(PlayerBase player, int tier, string missionName)
    {
        if (!player || !player.GetIdentity()) return;
        m_MissionOwnerID = player.GetIdentity().GetPlainId();
        m_MissionOwnerName = player.GetIdentity().GetName();
        m_MissionAccepted = true;
        m_MissionTier = tier;
        m_MissionType = missionName;
    }

    // Verifica se o jogador que se aproxima é o verdadeiro Dono do Contrato
    bool IsContractOwner(PlayerBase player)
    {
        if (!m_MissionAccepted || !player || !player.GetIdentity()) return false;
        return (m_MissionOwnerID == player.GetIdentity().GetPlainId());
    }

    // ==========================================

    bool IsExtended() { return false; }
    bool DeployMission() { return true; }
    void MissionFinal() {}
    void PlayerChecks(PlayerBase player) {}

    void CleanUp()
    {
        int i;
        // 1. Apaga todos os objetos físicos da missão
        for (i = 0; i < m_MissionObjects.Count(); i++)
        {
            if (m_MissionObjects[i]) GetGame().ObjectDelete(m_MissionObjects[i]);
        }
        m_MissionObjects.Clear(); // ADIÇÃO PNH 2.0: Limpa o array da memória do servidor

        // 2. Apaga todos os zumbis e NPCs da missão
        for (i = 0; i < m_MissionAIs.Count(); i++)
        {
            if (m_MissionAIs[i]) GetGame().ObjectDelete(m_MissionAIs[i]);
        }
        m_MissionAIs.Clear(); // ADIÇÃO PNH 2.0: Limpa o array da memória do servidor
        
        // 3. PNH 2.0: Reseta os dados do Contrato na memória
        m_MissionOwnerID = "";
        m_MissionOwnerName = "";
        m_MissionAccepted = false;
    }
}