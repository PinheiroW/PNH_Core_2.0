class TransportMission extends PNH_MissionBase
{
    override bool DeployMission()
    {
        if (!m_MissionAccepted) return false; 
        
        m_MissionInformant = "ALTO COMANDO PNH";
        m_MissionMessage1 = "CÓDIGO VERMELHO: Uma coluna de transporte PNH foi emboscada perto de " + m_MissionLocation + ".";
        m_MissionMessage2 = "A carga é material militar classificado. As forças inimigas assumiram o controlo dos destroços.";
        m_MissionMessage3 = "Recupere o veículo e elimine toda a força de ocupação sem excepção.";
        m_MissionMessage4 = "Espere resistência máxima (Tier 4). Não falhe connosco. Câmbio desligo.";
        
        // Spawn do Veículo Destruído
        Object wreck = GetGame().CreateObjectEx("Land_Wreck_V3S", m_MissionPosition, ECE_PLACE_ON_SURFACE);
        if (wreck) m_MissionObjects.Insert(wreck);

        // Força de Ocupação Máxima (25 Mercenários/Zumbis Pesados)
        for (int i = 0; i < 25; i++)
        {
            vector spawnPos = m_MissionPosition + Vector(Math.RandomFloat(-20, 20), 0, Math.RandomFloat(-20, 20));
            spawnPos[1] = GetGame().SurfaceY(spawnPos[0], spawnPos[2]); 
            Object npc = GetGame().CreateObjectEx("ZmbM_SoldierNormal_Heavy", spawnPos, ECE_PLACE_ON_SURFACE);
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
                PNH_ProfileManager.AddXP(PNH_Utils.GetPlayerByName(m_MissionOwnerName), settings.TabelaXP.XP_Tier_4);
            }
        }
    }
}