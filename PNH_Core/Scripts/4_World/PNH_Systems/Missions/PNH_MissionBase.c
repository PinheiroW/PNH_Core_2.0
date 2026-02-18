class PNH_MissionBase
{
    string m_MissionOwnerID;
    string m_MissionOwnerName;
    bool m_MissionAccepted;
    int m_MissionTier;
    string m_MissionType; 
    string m_MissionLocation;

    vector m_MissionPosition;
    float m_MissionTimeout; 
    float m_MissionTime;
    float m_MissionZoneOuterRadius;
    
    ref array<Object> m_MissionAIs = new array<Object>;

    // Sistema de Lore
    string m_MissionInformant;
    ref array<string> m_MissionMessages;

    void PNH_MissionBase()
    {
        m_MissionTime = 0;
        m_MissionTimeout = 3600; 
        m_MissionZoneOuterRadius = 200.0;
        m_MissionAccepted = false;
        m_MissionOwnerID = "";
        
        m_MissionInformant = "COMANDO PNH";
        m_MissionMessages = new array<string>;
    }

    void AcceptContract(PlayerBase player, int tier, string missionName)
    {
        if (!player || !player.GetIdentity()) return;
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
    void MissionFinal() {}
    void MissionChecks() {}

    void CleanUp()
    {
        foreach (Object ai : m_MissionAIs) { if (ai) GetGame().ObjectDelete(ai); }
        m_MissionAIs.Clear();
        m_MissionAccepted = false;
    }
}