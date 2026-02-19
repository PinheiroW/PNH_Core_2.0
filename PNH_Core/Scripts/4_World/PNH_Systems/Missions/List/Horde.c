/// --- Documentação PNH_Core: Horde.c ---
/// Versão do Sistema: 2.0.0 (Integração com Agente Treasury: XP + Loot Centralizado)
/// Função do arquivo: Define a lógica da missão de "Horda". A recompensa física e XP são agora delegados ao TreasuryManager.

class HordeMission extends PNH_MissionBase
{
    void HordeMission()
    {
        m_MissionType = "Horde";
        m_MissionInformant = "SGT. BASTOS";
        m_MissionTier = 1; // Horda é definida como Tier 1
    }

    override bool DeployMission()
    {
        if (!m_MissionAccepted) return false;
        m_MissionAIs.Clear();

        // Spawn de 15 Infectados ao redor da posição da missão
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
        if (!m_MissionAccepted) return;

        // --- 1. GATILHOS DE PROXIMIDADE (Narrativa Dinâmica) ---
        PlayerBase owner = PNH_Utils.GetPlayerByName(m_MissionOwnerName);
        if (owner)
        {
            float dist = vector.Distance(owner.GetPosition(), m_MissionPosition);

            if (dist <= 90.0 && !m_Msg90mSent)
            {
                PNH_BroadcastManager.GetInstance().SendNotificationToPlayer(owner, m_MissionInformant, m_LoreEtapas.Chegou_90m, 5.0, "Notifications/gui/data/info.edds");
                m_Msg90mSent = true;
            }

            if (dist <= 20.0 && !m_Msg20mSent)
            {
                PNH_BroadcastManager.GetInstance().SendNotificationToPlayer(owner, m_MissionInformant, m_LoreEtapas.Chegou_20m, 6.0, "Notifications/gui/data/warning.edds");
                m_Msg20mSent = true;
            }
        }

        // --- 2. VERIFICAÇÃO DE ALVOS ---
        if (m_MissionAIs.Count() == 0) return;

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
        // Notificação Global de Encerramento
        PNH_BroadcastManager.GetInstance().AnnounceMissionEnded(m_MissionOwnerName, m_MissionOwnerID);

        // Mensagem Final de Rádio
        PlayerBase owner = PNH_Utils.GetPlayerByName(m_MissionOwnerName);
        if (owner)
        {
            PNH_BroadcastManager.GetInstance().SendNotificationToPlayer(owner, m_MissionInformant, m_LoreEtapas.Concluiu, 7.0, "Notifications/gui/data/info.edds");
        }

        // DELEGAÇÃO AO TREASURY: Processa XP e gera barril de loot Tier 1
        if (m_MissionAccepted && m_MissionOwnerID != "")
        {
            PNH_TreasuryManager.ProcessMissionReward(
                m_MissionOwnerID, 
                m_MissionOwnerName, 
                m_MissionTier, 
                m_MissionPosition, 
                "0 0 0" // Orientação padrão para o barril na horda
            );
        }
    }
}