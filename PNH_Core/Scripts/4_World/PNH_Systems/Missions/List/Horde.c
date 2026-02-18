class HordeMission extends PNH_MissionBase
{
    void HordeMission()
    {
        // Puxamos os dados carregados do JSON
        PNH_MissionSettingsData settings = PNH_MissionSettings.GetData();
        
        m_MissionType = "Horde";
        m_MissionInformant = "SGT. BASTOS";
        
        // Ativação da Lore via JSON:
        // Se houver mensagens no JSON, usamos essas. Caso contrário, usamos um backup.
        if (settings && settings.MissaoLore && settings.MissaoLore.Count() > 0)
        {
            m_MissionMessages = settings.MissaoLore;
        }
        else
        {
            m_MissionMessages.Insert("Mercenario, detectamos uma concentracao perigosa de infectados.");
            m_MissionMessages.Insert("Limpe o perimetro estabelecido para receber os suprimentos.");
        }
    }

    override bool DeployMission()
    {
        if (!m_MissionAccepted) return false;
        m_MissionAIs.Clear();

        // Spawn de 15 Infectados (Zumbis) ao redor da posição da missão
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

        // Se todos os alvos foram eliminados, finaliza a missão
        if (!zombiesAlive) 
        {
            MissionFinal();
            PNH_MissionManager.GetInstance().EndMission();
        }
    }

    override void MissionFinal()
    {
        // Materializa o prémio físico (Loot dinâmico via JSON dentro do barril)
        PNH_LogisticsManager.SpawnRewardContainer("Barrel_Blue", m_MissionPosition);

        PNH_BroadcastManager.GetInstance().AnnounceMissionEnded(m_MissionOwnerName, m_MissionOwnerID);

        if (m_MissionAccepted && m_MissionOwnerID != "")
        {
            // Atribui o XP base configurado para Tier 2 (Horda)
            PNH_MissionSettingsData settings = PNH_MissionSettings.GetData();
            int rewardXP = 100;
            if (settings && settings.TabelaXP) rewardXP = settings.TabelaXP.XP_Tier_2;
            
            PNH_TreasuryManager.ProcessMissionReward(m_MissionOwnerID, m_MissionOwnerName, rewardXP);
        }
    }
}