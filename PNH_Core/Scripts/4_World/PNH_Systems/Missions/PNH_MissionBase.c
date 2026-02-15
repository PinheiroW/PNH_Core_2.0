class PNH_MissionBase
{
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
    }

    bool IsExtended() { return false; }
    bool DeployMission() { return true; }
    void MissionFinal() {}
    void PlayerChecks(PlayerBase player) {}

    void CleanUp()
    {
        int i;
        for (i = 0; i < m_MissionObjects.Count(); i++)
        {
            if (m_MissionObjects[i]) GetGame().ObjectDelete(m_MissionObjects[i]);
        }
        for (i = 0; i < m_MissionAIs.Count(); i++)
        {
            if (m_MissionAIs[i]) GetGame().ObjectDelete(m_MissionAIs[i]);
        }
    }
}