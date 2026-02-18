class HordeMission extends PNH_MissionBase
{
    void HordeMission()
    {
        m_MissionInformant = "SGT. BASTOS";
        m_MissionMessages.Insert("Mercenario, detectamos uma concentracao perigosa de infectados.");
        m_MissionMessages.Insert("Limpe o perimetro estabelecido. Se tiver sucesso, um barril de suprimentos sera deixado no local.");
    }

    override bool DeployMission()
    {
        if (!m_MissionAccepted) return false;
        m_MissionAIs.Clear();

        for (int i = 0; i < 15; i++)
        {
            Object npc = PNH_LogisticsManager.SpawnNPC("ZmbM_SoldierNormal", m_MissionPosition, 15.0);
            if (npc) m_MissionAIs.Insert(npc);
        }
        
        PNH_AuditManager.LogMissionEvent(m_MissionOwnerName, "Horde", "Infectados Materializados");
        return true;
    }

    override void MissionChecks()
    {
        if (!m_MissionAccepted || m_MissionAIs.Count() == 0) return;

        bool zombiesAlive = false;
        foreach (Object ai : m_MissionAIs) 
        {
            if (ai && EntityAI.Cast(ai).IsAlive()) 
            { 
                zombiesAlive = true; 
                break; 
            }
        }

        if (!zombiesAlive) 
        {
            MissionFinal();
            PNH_MissionManager.GetInstance().EndMission();
        }
    }

    override void MissionFinal()
    {
        // Materializa o prémio físico
        PNH_LogisticsManager.SpawnRewardContainer("Barrel_Blue", m_MissionPosition);

        PNH_BroadcastManager.GetInstance().AnnounceMissionEnded(m_MissionOwnerName, m_MissionOwnerID);

        if (m_MissionAccepted && m_MissionOwnerID != "")
        {
            PNH_TreasuryManager.ProcessMissionReward(m_MissionOwnerID, m_MissionOwnerName, m_MissionTier);
        }

        PNH_AuditManager.LogMissionEvent(m_MissionOwnerName, "Horde", "Contrato Finalizado");
    }
}