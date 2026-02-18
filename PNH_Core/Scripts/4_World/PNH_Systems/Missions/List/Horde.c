class HordeMission extends PNH_MissionBase
{
	override bool DeployMission()
	{
		if (!m_MissionAccepted) return false; 
		
		m_MissionInformant = "COMANDO PNH";
		m_MissionMessage1 = "ATENÇÃO MERCENÁRIO: Horda militar detectada em " + m_MissionLocation;
		m_MissionMessage2 = "Os alvos estao armados e sao extremamente agressivos.";
		m_MissionMessage3 = "Elimine todos para limpar o setor e receber o seu XP.";
		m_MissionMessage4 = "Mantenha a radio ligada para novas atualizacoes.";
		
		for (int i = 0; i < 15; i++)
		{
			vector spawnPos = m_MissionPosition + Vector(Math.RandomFloat(-10, 10), 0, Math.RandomFloat(-10, 10));
			spawnPos[1] = GetGame().SurfaceY(spawnPos[0], spawnPos[2]); 
			Object npc = GetGame().CreateObjectEx("ZmbM_SoldierNormal_Heavy", spawnPos, ECE_PLACE_ON_SURFACE);
			if (npc) m_MissionAIs.Insert(npc);
		}

		// PNH 2.0: Removemos os envios daqui. O Agente de Mídia cuida disso.
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
        // PNH 2.0: Chama o Agente para anunciar a vitória
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