class HordeMission : PNH_MissionBase
{
    void HordeMission()
    {
        m_MissionType = "Horde";
        m_MissionInformant = "SGT. BASTOS";
        m_MissionTier = 1;
        // Inicializa a lista para evitar o erro de NULL Pointer
        if (!m_MissionAIs) m_MissionAIs = new array<Object>;
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
        return true;
    }

    override void MissionChecks()
    {
        if (!m_MissionAccepted) return;

        PlayerBase owner = PNH_Utils.GetPlayerByName(m_MissionOwnerName);
        if (owner)
        {
            float dist = vector.Distance(owner.GetPosition(), m_MissionPosition);
            if (dist <= 90.0 && !m_Msg90mSent) {
                PNH_BroadcastManager.GetInstance().SendNotificationToPlayer(owner, m_MissionInformant, m_LoreEtapas.Chegou_90m, 5.0, "Notifications/gui/data/info.edds");
                m_Msg90mSent = true;
            }
            if (dist <= 20.0 && !m_Msg20mSent) {
                PNH_BroadcastManager.GetInstance().SendNotificationToPlayer(owner, m_MissionInformant, m_LoreEtapas.Chegou_20m, 6.0, "Notifications/gui/data/warning.edds");
                m_Msg20mSent = true;
            }
        }

        bool zombiesAlive = false;
        foreach (Object ai : m_MissionAIs) 
        {
            if (ai && EntityAI.Cast(ai).IsAlive()) { zombiesAlive = true; break; }
        }

        if (!zombiesAlive) 
        {
            MissionFinal();
            PNH_MissionManager.GetInstance().EndMission();
        }
    }

    override void MissionFinal()
    {
        PNH_BroadcastManager.GetInstance().AnnounceMissionEnded(m_MissionOwnerName, m_MissionOwnerID);
        
        PlayerBase owner = PNH_Utils.GetPlayerByName(m_MissionOwnerName);
        if (owner) PNH_BroadcastManager.GetInstance().SendNotificationToPlayer(owner, m_MissionInformant, m_LoreEtapas.Concluiu, 7.0, "Notifications/gui/data/info.edds");

        if (m_MissionAccepted && m_MissionOwnerID != "")
        {
            vector oriHorde = "0 0 0"; 
            PNH_TreasuryManager.ProcessMissionReward(m_MissionOwnerID, m_MissionOwnerName, m_MissionTier, m_MissionPosition, oriHorde);
        }
    }
}