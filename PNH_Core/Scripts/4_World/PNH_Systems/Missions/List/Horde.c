class HordeMission extends PNH_MissionBase
{
    override bool DeployMission()
    {
        if (!m_MissionAccepted) return false;
        m_MissionAIs.Clear();

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
        if (!m_MissionAccepted || !IsContractOwner(player) || m_MissionAIs.Count() == 0) return;

        bool zombiesAlive = false;
        foreach (Object ai : m_MissionAIs) {
            if (ai && EntityAI.Cast(ai).IsAlive()) { zombiesAlive = true; break; }
        }

        if (!zombiesAlive) {
            MissionFinal();
            PNH_MissionManager.GetInstance().EndMission();
        }
    }

    override void MissionFinal()
    {
        // Anúncio de rádio/global
        PNH_BroadcastManager.GetInstance().AnnounceMissionEnded(m_MissionOwnerName);

        if (m_MissionAccepted && m_MissionOwnerID != "")
        {
            PNH_PlayerProfileData pData = PNH_ProfileManager.LoadProfile(m_MissionOwnerID, m_MissionOwnerName);
            PNH_MissionSettingsData settings = PNH_MissionSettings.GetData();
            
            if (pData && settings) 
            {
                pData.TemMissaoAtiva = false; 
                pData.ClasseMissaoAtiva = ""; 
                
                // PNH 2.0 FIX: Adiciona o XP diretamente ao perfil antes de salvar
                pData.XP += settings.TabelaXP.XP_Tier_1; 
                PNH_ProfileManager.SaveProfile(pData);
                
                // Feedback imediato no chat para o jogador
                PlayerBase player = PNH_Utils.GetPlayerBySteamID(m_MissionOwnerID);
                if (player) PNH_Utils.SendMessage(player, "[PNH] Contrato Concluido! +" + settings.TabelaXP.XP_Tier_1 + " XP obtido.");
            }
        }
    }
}