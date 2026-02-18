class BearHuntMission extends PNH_MissionBase
{
    override bool DeployMission()
    {
        if (!m_MissionAccepted) return false; 
        
        // 1. Define o Lore da Missão (O Agente Broadcast envia por nós)
        m_MissionInformant = "GUILDA DE CAÇADORES";
        m_MissionMessage1 = "ATENÇÃO CAÇADOR: Recebemos relatos de ursos agressivos em " + m_MissionLocation + ".";
        m_MissionMessage2 = "Os animais atacaram batedores da PNH e estão extremamente letais.";
        m_MissionMessage3 = "Rastreie e abata os alvos. O pagamento será feito na confirmação da morte.";
        m_MissionMessage4 = "Boa caçada, mercenário. Não vire presa.";
        
        // 2. Lógica de Spawn Física
        for (int i = 0; i < 2; i++)
        {
            vector spawnPos = m_MissionPosition + Vector(Math.RandomFloat(-5, 5), 0, Math.RandomFloat(-5, 5));
            spawnPos[1] = GetGame().SurfaceY(spawnPos[0], spawnPos[2]); 
            Object bear = GetGame().CreateObjectEx("Animal_UrsusArctos", spawnPos, ECE_PLACE_ON_SURFACE);
            if (bear) m_MissionAIs.Insert(bear);
        }

        return true;
    }

    override void PlayerChecks(PlayerBase player)
    {
        if (!m_MissionAccepted || !IsContractOwner(player)) return; 
        if (!player || !player.IsAlive()) return;

        if (vector.Distance(player.GetPosition(), m_MissionPosition) <= m_MissionZoneOuterRadius)
        {
            bool bearsAlive = false;
            foreach (Object ai : m_MissionAIs) { if (ai && EntityAI.Cast(ai).IsAlive()) { bearsAlive = true; break; } }
            if (!bearsAlive) { MissionFinal(); PNH_MissionManager.GetInstance().EndMission(); }
        }
    }

    override void MissionFinal()
    {
        // 3. O Agente Broadcast assume o anúncio final
        PNH_BroadcastManager.GetInstance().AnnounceMissionEnded(m_MissionOwnerName);

        if (m_MissionAccepted && m_MissionOwnerID != "")
        {
            PNH_PlayerProfileData pData = PNH_ProfileManager.LoadProfile(m_MissionOwnerID, m_MissionOwnerName);
            PNH_MissionSettingsData settings = PNH_MissionSettings.GetData();
            if (pData && settings) 
            {
                pData.TemMissaoAtiva = false; pData.ClasseMissaoAtiva = ""; PNH_ProfileManager.SaveProfile(pData);
                // Atribui XP Tier 1
                PNH_ProfileManager.AddXP(PNH_Utils.GetPlayerByName(m_MissionOwnerName), settings.TabelaXP.XP_Tier_1);
            }
        }
    }
}