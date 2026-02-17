// Local: PNH_Core/Scripts/4_World/PNH_Systems/Missions/Tipos/BearHunt.c

class BearHuntMission extends PNH_MissionBase
{
	override bool DeployMission()
	{
		// ==========================================================
		// --- TRAVA DE INICIALIZAÇÃO PNH 2.0 (SEU PLANO ORIGINAL) ---
		// Se o contrato não foi aceito, o urso NÃO nasce e nada é avisado.
		if (!m_MissionAccepted) return false; 
		// ==========================================================

		m_MissionInformant = "Ranger Local";
		m_MissionLocation = m_MissionLocation; 
		m_MissionMessage1 = "Predador avistado nos arredores de " + m_MissionLocation;
		m_MissionMessage2 = "Um urso ferido e agressivo está atacando quem passa.";
		m_MissionMessage3 = "Encontre o animal e neutralize-o antes que haja mais vítimas.";

		m_MissionTimeout = 1800;
		m_MissionZoneOuterRadius = 300;
		m_MissionZoneInnerRadius = 20;

		// --- SPAWN DO URSO (Com correção técnica de vetor) ---
		vector spawnPos = m_MissionPosition + "1.0 0 1.0".ToVector(); 
		Object bear = GetGame().CreateObjectEx("Animal_UrsusArctos", spawnPos, ECE_PLACE_ON_SURFACE);
		if (bear) m_MissionAIs.Insert(bear);

		PNH_Discord.Send("SISTEMA DE MISSÕES PNH", "[PNH_CORE] MISSÃO_INICIADA: BearHunt em " + m_MissionLocation);

		return true;
	}

	override void PlayerChecks(PlayerBase player)
	{
		if (!m_MissionAccepted) return; 
		if (!IsContractOwner(player)) return; 

		if (!player || !player.IsAlive()) return;

		float distance = vector.Distance(player.GetPosition(), m_MissionPosition);

		if (distance <= m_MissionZoneOuterRadius)
		{
			bool bearAlive = false;
			foreach (Object ai : m_MissionAIs)
			{
				if (ai && EntityAI.Cast(ai).IsAlive()) { bearAlive = true; break; }
			}

			if (!bearAlive)
			{
				MissionFinal();
				PNH_MissionManager.GetInstance().EndMission();
			}
		}
	}

	override void MissionFinal()
	{
		PNH_Utils.SendMessageToAll("[RÁDIO PNH] A ameaça em " + m_MissionLocation + " foi eliminada por " + m_MissionOwnerName + ".");
		PNH_Logger.Log("Missões", "[PNH_CORE] MISSÃO_CONCLUÍDA: BearHunt por " + m_MissionOwnerName);
		PNH_Discord.Send("SISTEMA DE MISSÕES PNH", "[PNH_CORE] MISSÃO_CONCLUÍDA: " + m_MissionOwnerName + " abateu o urso.");

		for (int i = 0; i < m_MissionAIs.Count(); i++)
		{
			if (m_MissionAIs[i]) GetGame().ObjectDelete(m_MissionAIs[i]);
		}

		if (m_MissionAccepted && m_MissionOwnerID != "")
		{
			PNH_PlayerProfileData pData = PNH_ProfileManager.LoadProfile(m_MissionOwnerID, m_MissionOwnerName);
			PNH_MissionSettingsData settings = PNH_MissionSettings.GetData();

			if (pData && settings) 
			{
				pData.TemMissaoAtiva = false;
				pData.ClasseMissaoAtiva = "";
				PNH_ProfileManager.SaveProfile(pData);
				
				int xpGanho = settings.TabelaXP.XP_Tier_2; 
				PlayerBase dono = PNH_Utils.GetPlayerByName(m_MissionOwnerName);
				if (dono) {
					PNH_ProfileManager.AddXP(dono, xpGanho); 
					PNH_Utils.SendMessage(dono, "CONTRATO CONCLUÍDO: +" + xpGanho + " XP creditados.");
				}
			}
		}
	}
}