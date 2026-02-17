// Local: PNH_Core/Scripts/4_World/PNH_Systems/Missions/Tipos/Apartment.c

class ApartmentMission extends PNH_MissionBase
{
	override bool DeployMission()
	{
		// ==========================================================
		// --- TRAVA DE INICIALIZAÇÃO PNH 2.0 (SEU PLANO ORIGINAL) ---
		// A missão só nasce no mapa se houver um contrato assinado.
		if (!m_MissionAccepted) return false; 
		// ==========================================================

		m_MissionInformant = "Agente S";
		m_MissionLocation = m_MissionLocation; 
		m_MissionMessage1 = "Apartamento em " + m_MissionLocation + " está sob investigação.";
		m_MissionMessage2 = "Relatos de atividades estranhas nos andares superiores.";
		m_MissionMessage3 = "Neutralize as ameaças e garanta a segurança da área.";
		m_MissionMessage4 = "";
		m_MissionMessage5 = "";

		m_MissionTimeout = 2400;
		m_MissionZoneOuterRadius = 100;
		m_MissionZoneInnerRadius = 15;

		// --- SPAWN DE NPCs (Corrigido para evitar crash) ---
		// CORREÇÃO TÉCNICA: Somando vetores corretamente com .ToVector()
		Object zumbie = GetGame().CreateObjectEx("ZmbM_CommercialPilot_Blue", m_MissionPosition + "2 0 2".ToVector(), ECE_PLACE_ON_SURFACE);
		if (zumbie) m_MissionAIs.Insert(zumbie);

		// Integração Discord (Só dispara se a missão for realmente aceita)
		PNH_DiscordWebhook.SendMissionMessage("SISTEMA DE MISSÕES PNH", "[PNH_CORE] MISSÃO_INICIADA: Apartment em " + m_MissionLocation);

		return true;
	}

	override void PlayerChecks(PlayerBase player)
	{
		// --- BLINDAGEM DE CONTRATO ---
		if (!m_MissionAccepted) return; 
		if (!IsContractOwner(player)) return; 

		if (!player || !player.IsAlive()) return;

		float distance = vector.Distance(player.GetPosition(), m_MissionPosition);

		if (distance <= m_MissionZoneInnerRadius)
		{
			MissionFinal();
			PNH_MissionManager.GetInstance().EndMission();
		}
	}

	override void MissionFinal()
	{
		// Mensagens e Logs originais preservados
		PNH_Utils.SendMessageToAll("[RÁDIO PNH] Os apartamentos em " + m_MissionLocation + " foram limpos por " + m_MissionOwnerName + ".");
		PNH_Logger.Log("Missões", "[PNH_CORE] MISSÃO_CONCLUÍDA: Apartment por " + m_MissionOwnerName);
		
		PNH_DiscordWebhook.SendMissionMessage("SISTEMA DE MISSÕES PNH", "[PNH_CORE] MISSÃO_CONCLUÍDA: " + m_MissionOwnerName + " limpou os apartamentos.");

		for (int i = 0; i < m_MissionAIs.Count(); i++)
		{
			if (m_MissionAIs[i]) GetGame().ObjectDelete(m_MissionAIs[i]);
		}

		// ==========================================================
		// --- RECOMPENSA E LIMPEZA DE PERFIL (TIER 3) ---
		// ==========================================================
		if (m_MissionAccepted && m_MissionOwnerID != "")
		{
			PNH_PlayerProfileData pData = PNH_ProfileManager.LoadProfile(m_MissionOwnerID, m_MissionOwnerName);
			PNH_MissionSettingsData settings = PNH_MissionSettings.GetData();

			if (pData && settings) 
			{
				pData.TemMissaoAtiva = false;
				pData.ClasseMissaoAtiva = "";
				PNH_ProfileManager.SaveProfile(pData);
				
				int xpGanho = settings.TabelaXP.XP_Tier_3; 

				PlayerBase dono = PNH_Utils.GetPlayerByName(m_MissionOwnerName);
				if (dono) {
					PNH_ProfileManager.AddXP(dono, xpGanho); 
					PNH_Utils.SendMessage(dono, "OPERAÇÃO CONCLUÍDA: +" + xpGanho + " XP creditados.");
				}
			}
		}
	}
}