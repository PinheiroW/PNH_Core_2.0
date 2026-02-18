class HordeMission extends PNH_MissionBase
{
    override bool DeployMission()
    {
        if (!m_MissionAccepted) return false;
        m_MissionAIs.Clear();

        // LOGÍSTICA: Delegamos a criação física dos infectados para o agente responsável
        for (int i = 0; i < 15; i++)
        {
            Object npc = PNH_LogisticsManager.SpawnNPC("ZmbM_SoldierNormal", m_MissionPosition, 15.0);
            if (npc) m_MissionAIs.Insert(npc);
        }
        
        // AUDITORIA: Registamos o início tático da missão
        PNH_AuditManager.LogMissionEvent(m_MissionOwnerName, "Horde", "Infectados Materializados");
        return true;
    }

    override void PlayerChecks(PlayerBase player)
    {
        if (!m_MissionAccepted || !IsContractOwner(player)) return;

        // INTELIGÊNCIA: Validamos se o jogador está na zona de combate antes de processar lógica
        if (!PNH_IntelManager.IsOwnerInObjectiveZone(m_MissionOwnerID, m_MissionPosition, m_MissionZoneOuterRadius)) return;

        // Verificação de alvos vivos (Lógica interna da missão)
        if (m_MissionAIs.Count() > 0)
        {
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
    }

    override void MissionFinal()
    {
        // COMUNICAÇÃO: O rádio anuncia a vitória
        PNH_BroadcastManager.GetInstance().AnnounceMissionEnded(m_MissionOwnerName);

        // TESOURARIA: Processa o pagamento de XP de forma atómica
        if (m_MissionAccepted && m_MissionOwnerID != "")
        {
            PNH_TreasuryManager.ProcessMissionReward(m_MissionOwnerID, m_MissionOwnerName, m_MissionTier);
        }

        // AUDITORIA: Registo final da operação bem-sucedida
        PNH_AuditManager.LogMissionEvent(m_MissionOwnerName, "Horde", "Contrato Finalizado");
    }
}