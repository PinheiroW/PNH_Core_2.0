class GraveyardMission extends PNH_MissionBase
{
    override bool DeployMission()
    {
        if (!m_MissionAccepted) return false; 
        
        m_MissionInformant = "COMANDO PNH";
        m_MissionMessage1 = "CONTRATO BÁSICO: Atividade anómala reportada no cemitério em " + m_MissionLocation + ".";
        m_MissionMessage2 = "Temos errantes a agrupar-se na zona de sepultamento.";
        m_MissionMessage3 = "A sua missão é simples: limpar a área de qualquer presença hostil.";
        m_MissionMessage4 = "Trabalho fácil para alguém da sua patente. Câmbio desligo.";
        
        // Spawn de 6 Inimigos básicos
        for (int i = 0; i < 6; i++)
        {
            vector spawnPos = m_MissionPosition + Vector(Math.RandomFloat(-7, 7), 0, Math.RandomFloat(-7, 7));
            spawnPos[1] = GetGame().SurfaceY(spawnPos[0], spawnPos[2]); 
            Object npc = GetGame().CreateObjectEx("ZmbM_CitizenASkinny", spawnPos, ECE_PLACE_ON_SURFACE);
            if (npc) m_MissionAIs.Insert(npc);
        }

        return true;
    }

    override void PlayerChecks(PlayerBase player)
    {
        if (!m_MissionAccepted || !IsContractOwner(player)) return; 
        if (!player || !player.IsAlive()) return;

        if (vector.Distance(player.GetPosition(), m_MissionPosition) <= m_MissionZoneOuterRadius)
        {
            bool enemiesAlive = false;
            foreach (Object ai : m_MissionAIs) { if (ai && EntityAI.Cast(ai).IsAlive()) { enemiesAlive = true; break; } }
            if (!enemiesAlive) { MissionFinal(); PNH_MissionManager.GetInstance().EndMission(); }
        }
    }

    override void MissionFinal()
    {
        PNH_BroadcastManager.GetInstance().AnnounceMissionEnded(m_MissionOwnerName);

        if (m_MissionAccepted && m_MissionOwnerID != "")
        {
            PNH_PlayerProfileData pData = PNH_ProfileManager.LoadProfile(m_MissionOwnerID, m_MissionOwnerName);
            PNH_MissionSettingsData settings = PNH_MissionSettings.GetData();
            if (pData && settings) 
            {
                pData.TemMissaoAtiva = false; pData.ClasseMissaoAtiva = ""; PNH_ProfileManager.SaveProfile(pData);
                PNH_ProfileManager.AddXP(PNH_Utils.GetPlayerByName(m_MissionOwnerName), settings.TabelaXP.XP_Tier_1);
            }
        }
    }
}