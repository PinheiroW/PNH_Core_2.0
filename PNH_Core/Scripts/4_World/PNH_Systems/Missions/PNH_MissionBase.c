class PNH_MissionBase
{
    string m_MissionOwnerID;
    string m_MissionOwnerName;
    bool m_MissionAccepted;
    int m_MissionTier;
    string m_MissionType; 
    string m_MissionLocation;
    
    // Variáveis que o BroadcastManager exige
    string m_MissionInformant;
    string m_MissionMessage1, m_MissionMessage2, m_MissionMessage3, m_MissionMessage4;

    vector m_MissionPosition;
    float m_MissionTimeout; 
    float m_MissionTime;
    float m_MissionZoneOuterRadius;
    ref array<Object> m_MissionAIs = new array<Object>;

    void PNH_MissionBase()
    {
        m_MissionTime = 0;
        m_MissionTimeout = 3600; 
        m_MissionZoneOuterRadius = 200.0;
        m_MissionAccepted = false;
        m_MissionInformant = "COMANDO PNH";
    }

    void AcceptContract(PlayerBase player, int tier, string missionName)
    {
        m_MissionOwnerID = player.GetIdentity().GetPlainId();
        m_MissionOwnerName = player.GetIdentity().GetName();
        m_MissionAccepted = true;
        m_MissionTier = tier;
        m_MissionType = missionName;
    }

    bool IsContractOwner(PlayerBase player)
    {
        if (!m_MissionAccepted || !player || !player.GetIdentity()) return false;
        return (m_MissionOwnerID == player.GetIdentity().GetPlainId());
    }

    bool DeployMission() { return true; }
    void PlayerChecks(PlayerBase player) {}
    void MissionFinal() {}
    void CleanUp() { m_MissionAccepted = false; m_MissionAIs.Clear(); }
}