/// --- Documentação PNH_Core: Horde.c ---
/// Versão do Sistema: 1.1.0 (Implementação de Narrativa Dinâmica)
/// Função do arquivo: Define a lógica específica da missão de "Horda", gerindo o spawn de infectados, monitorização de proximidade para rádio e entrega de recompensas.
/// Comunicação com outros arquivos: Estende PNH_MissionBase.c. Interage com PNH_LogisticsManager.c para spawns e PNH_BroadcastManager.c para alertas de rádio.
/// Motivo da existência: Providenciar um tipo de missão focado em combate PVE com progressão narrativa baseada na posição do jogador.
/// Dependências internas: PNH_LogisticsManager.c, PNH_BroadcastManager.c, PNH_TreasuryManager.c e PNH_MissionSettings.c.
/// Última atualização: 2026-02-18
/// IMPORTANTE: Ao alterar este arquivo, documente no CHANGELOG_PNH.md

class HordeMission extends PNH_MissionBase
{
    void HordeMission()
    {
        m_MissionType = "Horde";
        m_MissionInformant = "SGT. BASTOS";
        
        // A Lore agora é injetada dinamicamente pelo MissionManager via JSON.
        // Removemos a lógica legada que preenchia m_MissionMessages manualmente.
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
        if (!m_MissionAccepted) return;

        // --- 1. GATILHOS DE PROXIMIDADE (Narrativa Dinâmica) ---
        PlayerBase owner = PNH_Utils.GetPlayerByName(m_MissionOwnerName);
        if (owner)
        {
            float dist = vector.Distance(owner.GetPosition(), m_MissionPosition);

            // Gatilho de Rádio: 90 metros
            if (dist <= 90.0 && !m_Msg90mSent && m_LoreEtapas.Chegou_90m != "")
            {
                PNH_BroadcastManager.GetInstance().SendNotificationToPlayer(owner, m_MissionInformant, m_LoreEtapas.Chegou_90m, 5.0, "Notifications/gui/data/info.edds");
                m_Msg90mSent = true;
                PNH_Logger.Log("Missão", "[PNH] Rádio 90m disparado para " + m_MissionOwnerName);
            }

            // Gatilho de Rádio: 20 metros (Contato Iminente)
            if (dist <= 20.0 && !m_Msg20mSent && m_LoreEtapas.Chegou_20m != "")
            {
                PNH_BroadcastManager.GetInstance().SendNotificationToPlayer(owner, m_MissionInformant, m_LoreEtapas.Chegou_20m, 6.0, "Notifications/gui/data/warning.edds");
                m_Msg20mSent = true;
                PNH_Logger.Log("Missão", "[PNH] Rádio 20m disparado para " + m_MissionOwnerName);
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
        PNH_LogisticsManager.SpawnRewardContainer("Barrel_Blue", m_MissionPosition);
        PNH_BroadcastManager.GetInstance().AnnounceMissionEnded(m_MissionOwnerName, m_MissionOwnerID);

        // --- 3. MENSAGEM FINAL DE RÁDIO ---
        PlayerBase owner = PNH_Utils.GetPlayerByName(m_MissionOwnerName);
        if (owner && m_LoreEtapas.Concluiu != "")
        {
            PNH_BroadcastManager.GetInstance().SendNotificationToPlayer(owner, m_MissionInformant, m_LoreEtapas.Concluiu, 7.0, "Notifications/gui/data/info.edds");
        }

        if (m_MissionAccepted && m_MissionOwnerID != "")
        {
            PNH_MissionSettingsData settings = PNH_MissionSettings.GetData();
            int rewardXP = 100;
            if (settings && settings.TabelaXP) rewardXP = settings.TabelaXP.XP_Tier_2;
            
            PNH_TreasuryManager.ProcessMissionReward(m_MissionOwnerID, m_MissionOwnerName, rewardXP);
        }
    }
}