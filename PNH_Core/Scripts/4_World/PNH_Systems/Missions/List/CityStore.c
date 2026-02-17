class CityStoreMission extends PNH_MissionBase
{
	override bool DeployMission()
	{
		if (!m_MissionAccepted) return false; // TRAVA DE INICIALIZAÇÃO

		m_MissionInformant = "Logística PNH";
		m_MissionLocation = m_MissionLocation; 
		m_MissionMessage1 = "Saqueadores em " + m_MissionLocation;
		m_MissionMessage2 = "Uma loja de suprimentos foi dominada.";
		m_MissionMessage3 = "Elimine os alvos e garanta a segurança.";

		m_MissionTimeout = 2100;
		m_MissionZoneOuterRadius = 150;
		m_MissionZoneInnerRadius = 20;

		for (int i = 0; i < 8; i++)
		{
			vector spawnPos = m_MissionPosition + "1.5 0 1.5".ToVector(); // CORREÇÃO VETOR
			Object looter = GetGame().CreateObjectEx("ZmbM_VillagerOld_Blue", spawnPos, ECE_PLACE_ON_SURFACE);
			if (looter) m_MissionAIs.Insert(looter);
		}

		PNH_DiscordWebhook.SendMissionMessage("SISTEMA DE MISSÕES PNH", "[PNH_CORE] MISSÃO_INICIADA: CityStore em " + m_MissionLocation);
		return true;
	}

	override void PlayerChecks(PlayerBase player)
	{
		if (!m_MissionAccepted || !IsContractOwner(player)) return; // BLINDAGEM
		if (!player || !player.IsAlive()) return;

		if (vector.Distance(player.GetPosition(), m_MissionPosition) <= m_MissionZoneInnerRadius)
		{
			bool inimigosVivos = false;
			foreach (Object ai : m_MissionAIs) { if (ai && EntityAI.Cast(ai).IsAlive()) { inimigosVivos = true; break; } }
			if (!inimigosVivos) { MissionFinal(); PNH_MissionManager.GetInstance().EndMission(); }
		}
	}

	override void MissionFinal()
	{
		PNH_Utils.SendMessageToAll("[RÁDIO PNH] A loja em " + m_MissionLocation + " foi recuperada por " + m_MissionOwnerName + ".");
		PNH_DiscordWebhook.SendMissionMessage("SISTEMA DE MISSÕES PNH", "[PNH_CORE] MISSÃO_CONCLUÍDA: " + m_MissionOwnerName);

		if (m_MissionAccepted && m_MissionOwnerID != "")
		{
			PNH_PlayerProfileData pData = PNH_ProfileManager.LoadProfile(m_MissionOwnerID, m_MissionOwnerName);
			PNH_MissionSettingsData settings = PNH_MissionSettings.GetData();
			if (pData && settings) 
			{
				pData.TemMissaoAtiva = false; pData.ClasseMissaoAtiva = ""; PNH_ProfileManager.SaveProfile(pData);
				PNH_ProfileManager.AddXP(PNH_Utils.GetPlayerByName(m_MissionOwnerName), settings.TabelaXP.XP_Tier_2); // RECOMPENSA
			}
		}
	}
}