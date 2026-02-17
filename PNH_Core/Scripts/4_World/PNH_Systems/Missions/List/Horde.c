class HordeMission extends PNH_MissionBase
{
	override bool DeployMission()
	{
		if (!m_MissionAccepted) return false; 
		m_MissionInformant = "Comando PNH";
		
		for (int i = 0; i < 15; i++)
		{
			vector spawnPos = m_MissionPosition + Vector(Math.RandomFloat(-10, 10), 0, Math.RandomFloat(-10, 10));
			spawnPos[1] = GetGame().SurfaceY(spawnPos[0], spawnPos[2]); 
			Object npc = GetGame().CreateObjectEx("ZmbM_SoldierNormal_Heavy", spawnPos, ECE_PLACE_ON_SURFACE);
			if (npc) m_MissionAIs.Insert(npc);
		}

		PNH_Discord.Send("SISTEMA DE MISSÕES PNH", "[PNH_CORE] MISSÃO_INICIADA: Horde em " + m_MissionLocation);
		return true;
	}

	override void PlayerChecks(PlayerBase player)
	{
		if (!m_MissionAccepted || !IsContractOwner(player)) return; 
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
		PNH_Utils.SendMessageToAll("[RÁDIO PNH] A horda foi desmantelada por " + m_MissionOwnerName + "!");
		PNH_Discord.Send("SISTEMA DE MISSÕES PNH", "[PNH_CORE] MISSÃO_CONCLUÍDA: " + m_MissionOwnerName);

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