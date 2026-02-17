class HordeMission extends PNH_MissionBase
{
	override bool DeployMission()
	{
		if (!m_MissionAccepted) return false; // TRAVA DE INICIALIZAÇÃO

		m_MissionInformant = "Comando PNH";
		m_MissionLocation = m_MissionLocation; 
		m_MissionMessage1 = "Horda ativa em " + m_MissionLocation;
		m_MissionMessage2 = "Um piloto caiu e a carga atraiu dezenas de infectados.";
		m_MissionMessage3 = "Elimine todos os alvos na zona para recuperar os suprimentos.";
		
		m_MissionTimeout = 1800;
		m_MissionZoneOuterRadius = 150;
		m_MissionZoneInnerRadius = 15;

		for (int i = 0; i < 15; i++)
		{
			vector spawnPos = m_MissionPosition + "1.0 0 1.0".ToVector(); // CORREÇÃO VETOR
			Object npc = GetGame().CreateObjectEx("ZmbM_SoldierNormal_Base", spawnPos, ECE_PLACE_ON_SURFACE);
			if (npc) m_MissionAIs.Insert(npc);
		}

		PNH_DiscordWebhook.SendMissionMessage("SISTEMA DE MISSÕES PNH", "[PNH_CORE] MISSÃO_INICIADA: Horde em " + m_MissionLocation);
		return true;
	}

	override void PlayerChecks(PlayerBase player)
	{
		if (!m_MissionAccepted || !IsContractOwner(player)) return; // BLINDAGEM
		if (!player || !player.IsAlive()) return;

		if (vector.Distance(player.GetPosition(), m_MissionPosition) <= m_MissionZoneOuterRadius)
		{
			bool zombiesAlive = false;
			foreach (Object ai : m_MissionAIs) { if (ai && EntityAI.Cast(ai).IsAlive()) { zombiesAlive = true; break; } }

			if (!zombiesAlive) { MissionFinal(); PNH_MissionManager.GetInstance().EndMission(); }
		}
	}

	override void MissionFinal()
	{
		PNH_Utils.SendMessageToAll("[RÁDIO PNH] " + m_MissionOwnerName + " desmantelou a horda!");
		PNH_DiscordWebhook.SendMissionMessage("SISTEMA DE MISSÕES PNH", "[PNH_CORE] MISSÃO_CONCLUÍDA: " + m_MissionOwnerName);

		if (m_MissionAccepted && m_MissionOwnerID != "")
		{
			PNH_PlayerProfileData pData = PNH_ProfileManager.LoadProfile(m_MissionOwnerID, m_MissionOwnerName);
			PNH_MissionSettingsData settings = PNH_MissionSettings.GetData();
			if (pData && settings) 
			{
				pData.TemMissaoAtiva = false; pData.ClasseMissaoAtiva = ""; PNH_ProfileManager.SaveProfile(pData);
				PNH_ProfileManager.AddXP(PNH_Utils.GetPlayerByName(m_MissionOwnerName), settings.TabelaXP.XP_Tier_1); // RECOMPENSA
			}
		}
		// Rascunhos de loot preservados abaixo...
	}
}