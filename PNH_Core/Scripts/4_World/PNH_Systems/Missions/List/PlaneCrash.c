class PlaneCrashMission extends PNH_MissionBase
{
    override bool DeployMission()
    {
        if (!m_MissionAccepted) return false; 
        
        m_MissionInformant = "INTELIGÊNCIA PNH";
        m_MissionMessage1 = "MERCENÁRIO: Um avião tático aliado foi abatido em " + m_MissionLocation + ".";
        m_MissionMessage2 = "A carga é de altíssimo valor e as forças inimigas já estão a cercar a zona.";
        m_MissionMessage3 = "Elimine toda a resistência pesada e recupere o setor.";
        m_MissionMessage4 = "O Comando confia nas suas habilidades para esta operação de alto risco. Câmbio desligo.";
        
        // Spawn do Objeto Central (O Avião)
        Object crash = GetGame().CreateObjectEx("Land_Wreck_C130J", m_MissionPosition, ECE_PLACE_ON_SURFACE);
        if (crash) m_MissionObjects.Insert(crash);

        // Spawn dos Inimigos (Tier 3 = Mais quantidade/dificuldade)
        for (int i = 0; i < 20; i++)
        {
            vector spawnPos = m_MissionPosition + Vector(Math.RandomFloat(-15, 15), 0, Math.RandomFloat(-15, 15));
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
                // Atribui XP Tier 3
                PNH_ProfileManager.AddXP(PNH_Utils.GetPlayerByName(m_MissionOwnerName), settings.TabelaXP.XP_Tier_3);
            }
        }
    }
}