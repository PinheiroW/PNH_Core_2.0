class CityStoreMission extends PNH_MissionBase
{
    override bool DeployMission()
    {
        if (!m_MissionAccepted) return false; 
        
        m_MissionInformant = "COMANDO LOGÍSTICO";
        m_MissionMessage1 = "MERCENÁRIO: Um ponto de abastecimento civil foi localizado em " + m_MissionLocation + ".";
        m_MissionMessage2 = "A área está bloqueada por infectados. Precisamos de assegurar a zona para extração de mantimentos.";
        m_MissionMessage3 = "Neutralize os alvos no perímetro da loja.";
        m_MissionMessage4 = "Autorização para força letal total. Câmbio.";
        
        // Spawn de 8 Inimigos
        for (int i = 0; i < 8; i++)
        {
            vector spawnPos = m_MissionPosition + Vector(Math.RandomFloat(-10, 10), 0, Math.RandomFloat(-10, 10));
            spawnPos[1] = GetGame().SurfaceY(spawnPos[0], spawnPos[2]); 
            Object npc = GetGame().CreateObjectEx("ZmbM_Commercial", spawnPos, ECE_PLACE_ON_SURFACE);
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
                PNH_ProfileManager.AddXP(PNH_Utils.GetPlayerByName(m_MissionOwnerName), settings.TabelaXP.XP_Tier_2);
            }
        }
    }
}