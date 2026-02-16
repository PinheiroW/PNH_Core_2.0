class ApartmentMission extends PNH_MissionBase
{
    ItemBase m_RewardContainer;
    Object m_SurvivorNPC;
    bool m_IsVictory = false;
    bool m_Deployed = false; 
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
                m_MissionMessage1 = m_SurvivorName + m_Config.Lore.MensagensRadio[0];
                m_MissionMessage2 = m_Config.Lore.MensagensRadio[1];
            }
        }
    }

    override void PlayerChecks(PlayerBase player)
    {
        float distance = vector.Distance(player.GetPosition(), m_MissionPosition);

        if (!m_OuterWarned && distance <= m_MissionZoneOuterRadius) {
            m_OuterWarned = true;
            EnviarAviso(m_MissionInformant, m_Config.Lore.MensagemAproximacao + m_SurvivorName + ".");
        }

        if (!m_InnerWarned && distance <= m_MissionZoneInnerRadius) {
            m_InnerWarned = true;
            EnviarAviso(m_MissionInformant, m_Config.Lore.MensagemNoObjetivo);
        }

        if (!m_IsVictory && m_RewardContainer && m_RewardContainer.IsOpen())
        {
            FinalizarMissao(player);
        }
    }

    void FinalizarMissao(PlayerBase player)
    {
        m_IsVictory = true;
        PNH_Logger.Log("Missões", "[PNH_CORE] MISSÃO_CONCLUÍDA: " + player.GetIdentity().GetName());

        // SPAWN DA SEGUNDA HORDA (10 Zumbis Fora do Prédio)
        SpawnZumbis(10, m_MissionPosition, 30.0);

        string mensagemFinal = m_Config.Lore.MensagemVitoria;
        EnviarAviso("Comando PNH", mensagemFinal);

        PNH_MissionSettingsData config = PNH_MissionSettings.GetData();
        m_MissionTimeout = m_MissionTime + config.ConfiguracoesGerais.TempoLimpezaSegundos;
    }

    override bool DeployMission()
    {
        if (!m_Config || !m_Config.Ativa || m_Deployed) return false;
        m_Deployed = true;

        if (m_Config.Lore && m_Config.Lore.MensagensRadio && m_Config.Lore.MensagensRadio.Count() >= 3) {
            m_MissionMessage3 = m_Config.Lore.MensagensRadio[2] + "** " + m_MissionLocation + " **.";
        }

        Object missionBuilding = null;
        // Aumentado o raio de busca para garantir a detecção do prédio e spawn das barricadas
        GetGame().GetObjectsAtPosition(m_MissionPosition, 30.0, m_ObjectList, m_ObjectCargoList);
        for (int b = 0; b < m_ObjectList.Count(); b++) {
            string type = m_ObjectList[b].GetType();
            if (type.Contains("Land_Tenement_Small") || type.Contains("Apartment") || type.Contains("Tenement")) {
                missionBuilding = m_ObjectList[b];
                break;
            }
        }

        if (missionBuilding)
        {
            // 1. SPAWN BARRICADAS
            if (m_Config.Cenario && m_Config.Cenario.Barricadas) {
                for (int i = 0; i < m_Config.Cenario.Barricadas.Count(); i++) {
                    PNH_MissionSettings_Barricada bar = m_Config.Cenario.Barricadas[i];
                    Object bObj = GetGame().CreateObject(bar.Classe, missionBuilding.ModelToWorld(bar.PosicaoLocal.ToVector()), false, false, true);
                    if (bObj) {
                        bObj.SetOrientation(missionBuilding.GetOrientation() + bar.OrientacaoLocal.ToVector());
                        m_MissionObjects.Insert(bObj);
                    }
                }
            }

            // 2. SPAWN NPC (VIVO E EQUIPADO)
            vector cPos = missionBuilding.ModelToWorld(m_Config.PosicaoCorpoLocal.ToVector());
            m_SurvivorNPC = GetGame().CreateObject(m_Config.ClasseCorpo, cPos, false, false, true);
            if (m_SurvivorNPC) {
                EntityAI npcEnt = EntityAI.Cast(m_SurvivorNPC);
                npcEnt.SetOrientation(missionBuilding.GetOrientation() + m_Config.OrientacaoCorpoLocal.ToVector());
                
                // Veste o NPC
                for (int r = 0; r < m_Config.RoupasNPC.Count(); r++) npcEnt.GetInventory().CreateInInventory(m_Config.RoupasNPC[r]);
                // Adiciona itens ao inventário
                for (int it = 0; it < m_Config.InventarioNPC.Count(); it++) npcEnt.GetInventory().CreateInInventory(m_Config.InventarioNPC[it]);
                
                m_MissionObjects.Insert(m_SurvivorNPC);
            }

            // 3. SPAWN BARRIL
            vector rPos = missionBuilding.ModelToWorld(m_Config.PosicaoRecompensaLocal.ToVector());
            m_RewardContainer = ItemBase.Cast(GetGame().CreateObject(m_Config.RecompensasHorda.Container, rPos, false, false, true));
            if (m_RewardContainer) {
                auto loadout = m_Config.RecompensasHorda.Loadouts.GetRandomElement();
                for (int j = 0; j < loadout.Itens.Count(); j++) m_RewardContainer.GetInventory().CreateInInventory(loadout.Itens[j]);
                m_RewardContainer.Close();
                m_MissionObjects.Insert(m_RewardContainer);
            }
        }

        // 4. HORDA INICIAL (10 Zumbis protegendo o prédio)
        SpawnZumbis(10, m_MissionPosition, 15.0);

        PNH_Logger.Log("Missões", "[PNH SYSTEM] MISSÃO_INICIADA: Apartment em " + m_MissionLocation);
        return true;
    }

    void SpawnZumbis(int qtd, vector pos, float raio)
    {
        if (m_Config.Dificuldade && m_Config.Dificuldade.ClassesZumbis.Count() > 0) {
            for (int z = 0; z < qtd; z++) {
                vector zPos = pos + Vector(Math.RandomFloat(-raio, raio), 0, Math.RandomFloat(-raio, raio));
                zPos[1] = GetGame().SurfaceY(zPos[0], zPos[2]);
                m_MissionAIs.Insert(GetGame().CreateObject(m_Config.Dificuldade.ClassesZumbis.GetRandomElement(), zPos, false, true, true));
            }
        }
    }

    void EnviarAviso(string emissor, string msg)
    {
        PNH_MissionSettingsData config = PNH_MissionSettings.GetData();
        if (config.ConfiguracoesGerais.UsarPDA) GetRPCManager().SendRPC("[GearPDA] ", "SendGlobalMessage", new Param2<string, string>(emissor, msg), true);
        else PNH_Utils.SendMessageToAll("[" + emissor + "] " + msg);
    }
}