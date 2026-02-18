class HordeMission extends PNH_MissionBase
{
    override bool DeployMission()
    {
        if (!m_MissionAccepted) return false;
        m_MissionAIs.Clear();

        // Spawn de 15 soldados infectados
        for (int i = 0; i < 15; i++)
        {
            vector spawnPos = m_MissionPosition + Vector(Math.RandomFloat(-10, 10), 0, Math.RandomFloat(-10, 10));
            spawnPos[1] = GetGame().SurfaceY(spawnPos[0], spawnPos[2]); 
            
            Object npc = GetGame().CreateObject("ZmbM_SoldierNormal", spawnPos, false, true, true);
            if (npc) {
                npc.SetPosition(spawnPos);
                m_MissionAIs.Insert(npc);
            }
        }
        PNH_Logger.Log("Missões", "[PNH_CORE] Spawn físico da horda concluído: " + m_MissionAIs.Count() + " alvos.");
        return true;
    }

    override void PlayerChecks(PlayerBase player)
    {
        // O Manager só permite esta verificação após 15s de materialização
        if (!m_MissionAccepted || !IsContractOwner(player) || m_MissionAIs.Count() == 0) return;

        bool zombiesAlive = false;
        foreach (Object ai : m_MissionAIs) {
            if (ai && EntityAI.Cast(ai).IsAlive()) { 
                zombiesAlive = true; 
                break; 
            }
        }

        if (!zombiesAlive) {
            MissionFinal();
            PNH_MissionManager.GetInstance().EndMission();
        }
    }

    override void MissionFinal()
    {
        // 1. O rádio/Discord anuncia a conclusão
        PNH_BroadcastManager.GetInstance().AnnounceMissionEnded(m_MissionOwnerName);

        // 2. A Tesouraria processa o pagamento usando os dados do contrato
        if (m_MissionAccepted && m_MissionOwnerID != "")
        {
            PNH_TreasuryManager.ProcessMissionReward(m_MissionOwnerID, m_MissionOwnerName, m_MissionTier);
        }
    }
}