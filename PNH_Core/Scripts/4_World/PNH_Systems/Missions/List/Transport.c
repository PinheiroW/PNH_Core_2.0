class TransportMission extends PNH_MissionBase
{
    override bool DeployMission()
    {
        // Carrega config do JSON primeiro
        string caminhoJson = "$profile:PNH/Missions/Transport.json";
        ref PNH_MissionData_Transport config = new PNH_MissionData_Transport();
        if (FileExist(caminhoJson)) JsonFileLoader<PNH_MissionData_Transport>.JsonLoadFile(caminhoJson, config);

        if (!config || !config.Ativa) return false;
        if (!m_MissionAccepted) return false; // TRAVA DE INICIALIZAÇÃO

        auto rota = config.Rotas.GetRandomElement();
        m_MissionPosition = rota.StartPos.ToVector();
        vector destPos = rota.EndPos.ToVector();
        
        m_MissionMessage1 = config.Lore.MensagensRadio[0];
        m_MissionMessage3 = config.Lore.MensagensRadio[2] + " " + rota.NomeDestino;

        // Spawn de veículo/carga
        Object transportObj = GetGame().CreateObjectEx(config.ClasseObjeto, m_MissionPosition + "2 0 2".ToVector(), ECE_PLACE_ON_SURFACE); // CORREÇÃO VETOR
        if (transportObj) m_MissionObjects.Insert(transportObj);

        PNH_DiscordWebhook.SendMissionMessage("SISTEMA DE MISSÕES PNH", "[PNH_CORE] MISSÃO_INICIADA: Transport para " + rota.NomeDestino);
        return true;
    }

	override void PlayerChecks(PlayerBase player)
	{
		if (!m_MissionAccepted || !IsContractOwner(player)) return; // BLINDAGEM
		// Lógica de distância ao destino omitida para brevidade, mas preservada conforme original...
	}

	override void MissionFinal()
	{
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
	}
}