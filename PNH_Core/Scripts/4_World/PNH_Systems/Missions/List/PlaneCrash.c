class PlaneCrashMission extends PNH_MissionBase
{
	override bool DeployMission()
	{
		if (!m_MissionAccepted) return false; // TRAVA DE INICIALIZAÇÃO

		m_MissionInformant = "Operador PNH";
		m_MissionLocation = m_MissionLocation; 
		m_MissionMessage1 = "Queda de aeronave detectada em " + m_MissionLocation;
		m_MissionMessage2 = "A área está hostil.";
		m_MissionMessage3 = "Recupere a caixa preta.";
		
		m_MissionTimeout = 2700;
		m_MissionZoneOuterRadius = 200;
		m_MissionZoneInnerRadius = 10;

		Object crashSite = GetGame().CreateObjectEx("Plane_Crash_Model", m_MissionPosition, ECE_PLACE_ON_SURFACE);
		if (crashSite) m_MissionObjects.Insert(crashSite);

		for (int i = 0; i < 10; i++)
		{
			vector spawnPos = m_MissionPosition + "5 0 5".ToVector(); // CORREÇÃO VETOR
			Object guard = GetGame().CreateObjectEx("ZmbM_SoldierNormal_Heavy", spawnPos, ECE_PLACE_ON_SURFACE);
			if (guard) m_MissionAIs.Insert(guard);
		}

		PNH_Discord.Send("SISTEMA DE MISSÕES PNH", "[PNH_CORE] MISSÃO_INICIADA: PlaneCrash em " + m_MissionLocation);
		return true;
	}

	override void PlayerChecks(PlayerBase player)
	{
		if (!m_MissionAccepted || !IsContractOwner(player)) return; // BLINDAGEM
		if (!player || !player.IsAlive()) return;

		if (vector.Distance(player.GetPosition(), m_MissionPosition) <= m_MissionZoneInnerRadius)
		{
			bool enemiesLeft = false;
			foreach (Object ai : m_MissionAIs) { if (ai && EntityAI.Cast(ai).IsAlive()) { enemiesLeft = true; break; } }
			if (!enemiesLeft) { MissionFinal(); PNH_MissionManager.GetInstance().EndMission(); }
		}
	}

	override void MissionFinal()
	{
		PNH_Utils.SendMessageToAll("[RÁDIO PNH] " + m_MissionOwnerName + " garantiu os destroços!");

		if (m_MissionAccepted && m_MissionOwnerID != "")
		{
			PNH_PlayerProfileData pData = PNH_ProfileManager.LoadProfile(m_MissionOwnerID, m_MissionOwnerName);
			PNH_MissionSettingsData settings = PNH_MissionSettings.GetData();
			if (pData && settings) 
			{
				pData.TemMissaoAtiva = false; pData.ClasseMissaoAtiva = ""; PNH_ProfileManager.SaveProfile(pData);
				PNH_ProfileManager.AddXP(PNH_Utils.GetPlayerByName(m_MissionOwnerName), settings.TabelaXP.XP_Tier_3); // RECOMPENSA
			}
		}
	}
}