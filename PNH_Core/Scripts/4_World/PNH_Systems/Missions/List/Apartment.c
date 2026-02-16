class ApartmentMission extends PNH_MissionBase
{
    ItemBase m_RewardContainer;
    bool m_IsVictory = false;
    string m_SurvivorName;

    bool m_OuterWarned = false;
    bool m_InnerWarned = false;

    ref PNH_MissionData_Apartment m_Config;

    override bool IsExtended() { return false; }

    void ApartmentMission()
    {
        string caminhoJson = "$profile:PNH/Missions/Apartment.json";
        m_Config = new PNH_MissionData_Apartment();
        if (FileExist(caminhoJson)) JsonFileLoader<PNH_MissionData_Apartment>.JsonLoadFile(caminhoJson, m_Config);

        // Sorteia o nome do sobrevivente dono do apartamento
        if (m_Config.Lore && m_Config.Lore.NomesSobreviventes && m_Config.Lore.NomesSobreviventes.Count() > 0)
            m_SurvivorName = m_Config.Lore.NomesSobreviventes.GetRandomElement();
        else
            m_SurvivorName = "Sobrevivente";

        m_MissionTimeout = m_Config.TempoLimiteSegundos;
        m_MissionZoneOuterRadius = m_Config.RaioAvisoExterno;
        m_MissionZoneInnerRadius = m_Config.RaioAvisoInterno;

        if (m_Config.Lore) {
            m_MissionInformant = m_Config.Lore.Informante;
            if (m_Config.Lore.MensagensRadio && m_Config.Lore.MensagensRadio.Count() >= 2) {
                // Junta o nome sorteado com a frase do JSON (Ex: "Yuri foi um dos meus melhores alunos...")
                m_MissionMessage1 = m_SurvivorName + m_Config.Lore.MensagensRadio[0];
                m_MissionMessage2 = m_Config.Lore.MensagensRadio[1];
            }
        }
    }

    override void PlayerChecks(PlayerBase player)
    {
        float distance = vector.Distance(player.GetPosition(), m_MissionPosition);

        // 1. AVISOS DE PERÍMETRO
        if (!m_OuterWarned && distance <= m_MissionZoneOuterRadius) {
            m_OuterWarned = true;
            EnviarAviso(m_MissionInformant, "Você está se aproximando do prédio de " + m_SurvivorName + ". Cuidado, a área está infestada!");
        }

        if (!m_InnerWarned && distance <= m_MissionZoneInnerRadius) {
            m_InnerWarned = true;
            EnviarAviso(m_MissionInformant, "Você chegou ao apartamento. Encontre o barril com o equipamento barricado e abra-o!");
        }

        // 2. GATILHO DE VITÓRIA: ABRIR O BARRIL DENTRO DO PRÉDIO
        if (!m_IsVictory && m_RewardContainer && m_RewardContainer.IsOpen())
        {
            FinalizarMissao(player);
        }
    }

    void FinalizarMissao(PlayerBase player)
    {
        m_IsVictory = true;
        
        string winMsg = "[PNH_CORE] MISSÃO_CONCLUÍDA: " + player.GetIdentity().GetName() + " recuperou o equipamento no apartamento de " + m_SurvivorName + "!";
        PNH_Logger.Log("Missões", winMsg);

        string mensagemFinal = "Equipamento recuperado com sucesso!";
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

        // 1. ENCONTRAR O PRÉDIO DO APARTAMENTO
        Object missionBuilding = null;
        GetGame().GetObjectsAtPosition(m_MissionPosition, 20.0, m_ObjectList, m_ObjectCargoList);
        for (int b = 0; b < m_ObjectList.Count(); b++) {
            if (m_ObjectList[b].GetType().Contains("Land_Tenement_Small") || m_ObjectList[b].GetType().Contains("Apartment")) {
                missionBuilding = m_ObjectList[b];
                break;
            }
        }

        vector baseSpawnPos = m_MissionPosition;

        if (missionBuilding)
        {
            baseSpawnPos = missionBuilding.GetPosition();

            // 2. CONSTRUIR AS BARRICADAS FÍSICAS
            if (m_Config.Cenario && m_Config.Cenario.Barricadas)
            {
                for (int barIdx = 0; barIdx < m_Config.Cenario.Barricadas.Count(); barIdx++)
                {
                    PNH_MissionSettings_Barricada barricada = m_Config.Cenario.Barricadas[barIdx];
                    vector worldPosBar = missionBuilding.ModelToWorld(barricada.PosicaoLocal.ToVector());
                    Object barObj = GetGame().CreateObject(barricada.Classe, worldPosBar, false, false, true);

                    if (barObj) {
                        vector buildingOri = missionBuilding.GetOrientation();
                        vector localOri = barricada.OrientacaoLocal.ToVector();
                        barObj.SetOrientation(Vector(buildingOri[0] + localOri[0], buildingOri[1] + localOri[1], buildingOri[2] + localOri[2]));
                        m_MissionObjects.Insert(barObj);
                    }
                }
            }

            // 3. SPAWN DO BARRIL DENTRO DO QUARTO
            vector rewardWorldPos = missionBuilding.ModelToWorld(m_Config.PosicaoRecompensaLocal.ToVector());
            m_RewardContainer = ItemBase.Cast(GetGame().CreateObject(m_Config.RecompensasHorda.Container, rewardWorldPos, false, false, true));
        }
        else
        {
            // Fallback caso o prédio não seja encontrado
            PNH_Logger.Log("Missões", "[PNH_CORE] AVISO: Prédio Apartment não detectado! Usando ponto zero.");
            m_RewardContainer = ItemBase.Cast(GetGame().CreateObject(m_Config.RecompensasHorda.Container, m_MissionPosition, false, false, true));
        }

        // 4. PREENCHER RECOMPENSA (Lendo a lista de Loot do JSON)
        if (m_RewardContainer && m_Config.RecompensasHorda.Loadouts && m_Config.RecompensasHorda.Loadouts.Count() > 0)
        {
            auto loadout = m_Config.RecompensasHorda.Loadouts.GetRandomElement();
            for (int i = 0; i < loadout.Itens.Count(); i++) m_RewardContainer.GetInventory().CreateInInventory(loadout.Itens[i]);
            m_RewardContainer.Close();
            m_MissionObjects.Insert(m_RewardContainer);
        }

        // 5. SPAWN DA HORDA
        if (m_Config.Dificuldade && m_Config.Dificuldade.ClassesZumbis.Count() > 0)
        {
            for (int z = 0; z < m_Config.Dificuldade.QuantidadeHordaFinal; z++)
            {
                vector zPos = baseSpawnPos + Vector(Math.RandomFloat(-20, 20), 0, Math.RandomFloat(-20, 20));
                zPos[1] = GetGame().SurfaceY(zPos[0], zPos[2]);
                m_MissionAIs.Insert(GetGame().CreateObject(m_Config.Dificuldade.ClassesZumbis.GetRandomElement(), zPos, false, true, true));
            }
        }

        PNH_Logger.Log("Missões", "[PNH_CORE] MISSÃO_INICIADA: Apartamento de " + m_SurvivorName + " em " + m_MissionLocation);
        return true;
    }
}