class HordeMission extends PNH_MissionBase
{
    ItemBase m_RewardContainer;
    Object m_Pilot;
    bool m_IsVictory = false;

    bool m_OuterWarned = false;
    bool m_InnerWarned = false;

    ref PNH_MissionData_Horde m_Config;

    override bool IsExtended() { return false; }

    void HordeMission()
    {
        string caminhoJson = "$profile:PNH/Missions/Horde.json";
        m_Config = new PNH_MissionData_Horde();
        if (FileExist(caminhoJson)) JsonFileLoader<PNH_MissionData_Horde>.JsonLoadFile(caminhoJson, m_Config);

        m_MissionTimeout = m_Config.TempoLimiteSegundos;
        m_MissionZoneOuterRadius = m_Config.RaioAvisoExterno;
        m_MissionZoneInnerRadius = m_Config.RaioAvisoInterno;

        if (m_Config.Lore) {
            m_MissionInformant = m_Config.Lore.Informante;
            if (m_Config.Lore.MensagensRadio && m_Config.Lore.MensagensRadio.Count() >= 2) {
                m_MissionMessage1 = m_Config.Lore.MensagensRadio[0];
                m_MissionMessage2 = m_Config.Lore.MensagensRadio[1];
            }
        }
    }

    override void PlayerChecks(PlayerBase player)
    {
        float distance = vector.Distance(player.GetPosition(), m_MissionPosition);

        if (!m_OuterWarned && distance <= m_MissionZoneOuterRadius) {
            m_OuterWarned = true;
            EnviarAviso(m_MissionInformant, "Aviso: Zona de alta densidade de infectados à frente. Prepare-se.");
        }

        if (!m_InnerWarned && distance <= m_MissionZoneInnerRadius) {
            m_InnerWarned = true;
            EnviarAviso(m_MissionInformant, "Você localizou a horda. O corpo do piloto e a mochila devem estar por perto. Abra a mochila para finalizar.");
        }

        // GATILHO: Abrir a mochila
        if (!m_IsVictory && m_RewardContainer && m_RewardContainer.IsOpen())
        {
            FinalizarMissao(player);
        }
    }

    void FinalizarMissao(PlayerBase player)
    {
        m_IsVictory = true;
        
        string winMsg = "[PNH_CORE] MISSÃO_CONCLUÍDA: " + player.GetIdentity().GetName() + " desmantelou a horda e resgatou a mochila do piloto!";
        PNH_Logger.Log("Missões", winMsg);

        string mensagemFinal = "Equipamento do piloto recuperado com sucesso!";
        if (m_Config.Lore && m_Config.Lore.MensagemVitoria != "") {
            mensagemFinal = m_Config.Lore.MensagemVitoria;
        }

        EnviarAviso("Comando PNH", mensagemFinal);

        PNH_MissionSettingsData config = PNH_MissionSettings.GetData();
        m_MissionTimeout = m_MissionTime + config.ConfiguracoesGerais.TempoLimpezaSegundos;
    }

    void EnviarAviso(string emissor, string msg)
    {
        PNH_MissionSettingsData config = PNH_MissionSettings.GetData();
        if (config.ConfiguracoesGerais.UsarPDA) GetRPCManager().SendRPC("[GearPDA] ", "SendGlobalMessage", new Param2<string, string>(emissor, msg), true);
        else PNH_Utils.SendMessageToAll("[" + emissor + "] " + msg);
    }

    override bool DeployMission()
    {
        if (!m_Config || !m_Config.Ativa) return false;

        if (m_Config.Lore && m_Config.Lore.MensagensRadio && m_Config.Lore.MensagensRadio.Count() >= 3) {
            m_MissionMessage3 = m_Config.Lore.MensagensRadio[2] + "** " + m_MissionLocation + " **.";
        }

        vector basePos = m_MissionPosition;
        basePos[1] = GetGame().SurfaceY(basePos[0], basePos[2]);

        // 1. SPAWN DA MOCHILA RECOMPENSA
        if (m_Config.RecompensasHorda)
        {
            m_RewardContainer = ItemBase.Cast(GetGame().CreateObject(m_Config.RecompensasHorda.Container, basePos, false, false, true));
            if (m_RewardContainer && m_Config.RecompensasHorda.Loadouts && m_Config.RecompensasHorda.Loadouts.Count() > 0)
            {
                auto loadout = m_Config.RecompensasHorda.Loadouts.GetRandomElement();
                for (int i = 0; i < loadout.Itens.Count(); i++) m_RewardContainer.GetInventory().CreateInInventory(loadout.Itens[i]);
                m_RewardContainer.Close();
                m_MissionObjects.Insert(m_RewardContainer);
            }
        }

        // 2. SPAWN DO PILOTO MORTO
        if (m_Config.Piloto && m_Config.Piloto.Classe != "")
        {
            vector pilotPos = basePos + Vector(Math.RandomFloat(-2, 2), 0, Math.RandomFloat(-2, 2));
            pilotPos[1] = GetGame().SurfaceY(pilotPos[0], pilotPos[2]);
            m_Pilot = GetGame().CreateObject(m_Config.Piloto.Classe, pilotPos, false, false, true);
            if (m_Pilot)
            {
                EntityAI pilotEnt = EntityAI.Cast(m_Pilot);
                pilotEnt.SetHealth("", "", 0); // O piloto já nasce morto
                if (m_Config.Piloto.Itens) {
                    for (int j = 0; j < m_Config.Piloto.Itens.Count(); j++) pilotEnt.GetInventory().CreateInInventory(m_Config.Piloto.Itens[j]);
                }
                m_MissionObjects.Insert(m_Pilot);
            }
        }

        // 3. SPAWN DA HORDA
        if (m_Config.Dificuldade && m_Config.Dificuldade.ClassesZumbis.Count() > 0)
        {
            for (int z = 0; z < m_Config.Dificuldade.QuantidadeHordaFinal; z++)
            {
                vector zPos = basePos + Vector(Math.RandomFloat(-15, 15), 0, Math.RandomFloat(-15, 15));
                zPos[1] = GetGame().SurfaceY(zPos[0], zPos[2]);
                m_MissionAIs.Insert(GetGame().CreateObject(m_Config.Dificuldade.ClassesZumbis.GetRandomElement(), zPos, false, true, true));
            }
        }

        PNH_Logger.Log("Missões", "[PNH_CORE] MISSÃO_INICIADA: Horda ativa em " + m_MissionLocation);
        return true;
    }
}