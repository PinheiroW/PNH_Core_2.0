class HordeMission extends PNH_MissionBase
{
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

    // CORREÇÃO: Foco absoluto no estado dos alvos, ignorando a distância do jogador
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
        PNH_BroadcastManager.GetInstance().AnnounceMissionEnded(m_MissionOwnerName);

        if (m_MissionAccepted && m_MissionOwnerID != "")
        {
            PNH_TreasuryManager.ProcessMissionReward(m_MissionOwnerID, m_MissionOwnerName, m_MissionTier);
        }

        PNH_AuditManager.LogMissionEvent(m_MissionOwnerName, "Horde", "Contrato Finalizado");
    }
}