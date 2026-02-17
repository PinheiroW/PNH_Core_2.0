// Local: PNH_Core/Scripts/4_World/PNH_Systems/Missions/Tipos/Graveyard.c

class GraveyardMission extends PNH_MissionBase
{
    // ... (Variáveis originais preservadas) ...
    UndergroundStash m_Stash;
    ItemBase m_Chest;
    string m_SurvivorName;
    bool m_IsVictory = false;
    bool m_OuterWarned = false;
    bool m_InnerWarned = false;
    ref PNH_MissionData_Graveyard m_Config;

    // GraveyardMission() e IsExtended() permanecem conforme o original

    override bool DeployMission()
    {
        // ==========================================================
        // --- TRAVA DE INICIALIZAÇÃO PNH 2.0 (SEU PLANO ORIGINAL) ---
        // O cemitério e os zumbis só aparecem se o contrato for aceito.
        if (!m_MissionAccepted) return false; 
        // ==========================================================

        if (!m_Config || !m_Config.Ativa) return false;

        // ... (Lógica de mensagens de rádio preservada) ...

        m_Stash = UndergroundStash.Cast(GetGame().CreateObject("UndergroundStash", m_MissionPosition, false, false, true));
        if (m_Stash)
        {
            m_Stash.PlaceOnGround(); 
            m_Chest = ItemBase.Cast(m_Stash.GetInventory().CreateInInventory(m_Config.Recompensas.Container));
            
            // ... (Lógica de Loadouts preservada) ...
            m_MissionObjects.Insert(m_Stash);
        }

        // Spawn de Zumbis da Horda Final
        if (m_Config.Dificuldade && m_Config.Dificuldade.ClassesZumbis)
        {
            for (int i = 0; i < m_Config.Dificuldade.QuantidadeHordaFinal; i++)
            {
                vector zPos = m_MissionPosition + Vector(Math.RandomFloat(-20, 20), 0, Math.RandomFloat(-20, 20));
                zPos[1] = GetGame().SurfaceY(zPos[0], zPos[2]);
                m_MissionAIs.Insert(GetGame().CreateObject(m_Config.Dificuldade.ClassesZumbis.GetRandomElement(), zPos, false, true, true));
            }
        }

        PNH_Logger.Log("Missões", "[PNH_CORE] MISSÃO_INICIADA: Graveyard em " + m_MissionLocation);
        return true;
    }

    override void PlayerChecks(PlayerBase player)
    {
        if (!m_MissionAccepted) return; 
        if (!IsContractOwner(player)) return; 

        // ... (Lógica de avisos 90m/20m e gatilho de vitória preservada) ...
        
        if (!m_IsVictory && m_Chest)
        {
            Object parent = m_Chest.GetHierarchyParent();
            if (!parent || parent.GetType() != "UndergroundStash")
            {
                m_IsVictory = true;
                MissionFinal();
                PNH_MissionManager.GetInstance().EndMission();
            }
        }
    }

    override void MissionFinal()
    {
        PNH_MissionSettingsData globalConfig = PNH_MissionSettings.GetData();
        // ... (Lógica de Mensagem de sucesso preservada) ...

        if (m_MissionAccepted && m_MissionOwnerID != "")
        {
            PNH_PlayerProfileData pData = PNH_ProfileManager.LoadProfile(m_MissionOwnerID, m_MissionOwnerName);
            if (pData && globalConfig) 
            {
                pData.TemMissaoAtiva = false;
                pData.ClasseMissaoAtiva = "";
                PNH_ProfileManager.SaveProfile(pData);
                
                int xpGanho = globalConfig.TabelaXP.XP_Tier_2; 
                PlayerBase dono = PNH_Utils.GetPlayerByName(m_MissionOwnerName);
                if (dono) {
                    PNH_ProfileManager.AddXP(dono, xpGanho); 
                    PNH_Utils.SendMessage(dono, "CONTRATO CONCLUÍDO: +" + xpGanho + " XP creditados.");
                }
            }
        }
        m_MissionTimeout = m_MissionTime + globalConfig.ConfiguracoesGerais.TempoLimpezaSegundos;
    }
}