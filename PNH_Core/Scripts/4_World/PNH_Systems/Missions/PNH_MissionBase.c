class PNH_MissionBase
{
    ref PNH_MissionConfig m_Config; // <-- A "Ponte" para o JSON

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

    void PNH_MissionBase()
    {
        m_MissionObjects = new array<Object>;
        m_MissionAIs = new array<Object>;
        m_MissionPosition = "0 0 0";
        m_MissionTime = 0;
        m_RewardsSpawned = false;
        m_MsgNum = 0;
    }

    bool IsExtended() { return false; }
    bool DeployMission() { return false; }
    void PlayerChecks(PlayerBase player) {}

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