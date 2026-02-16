class ApartmentMission extends PNH_MissionBase
{
    ItemBase m_RewardContainer;
    Object m_SurvivorNPC;
    bool m_IsVictory = false;
    bool m_Deployed = false; 
    bool m_BarrelSpawned = false; // Gatilho para o barril
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

        // GATILHO PARA O BARRIL: Jogador perto do corpo do NPC morto
        if (!m_BarrelSpawned && m_SurvivorNPC && !m_SurvivorNPC.IsAlive())
        {
            float distToCorpse = vector.Distance(player.GetPosition(), m_SurvivorNPC.GetPosition());
            if (distToCorpse <= 2.0) {
                m_BarrelSpawned = true;
                SpawnRewards();
                EnviarAviso("Comando PNH", "O sobrevivente não resistiu. O barril de suprimentos foi localizado no local.");
            }
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

        // SPAWN DA HORDA EXTERNA (Ao abrir o barril)
        SpawnZumbis(10, m_MissionPosition, 35.0);

        string mensagemFinal = m_Config.Lore.MensagemVitoria;
        EnviarAviso("Comando PNH", mensagemFinal);

        PNH_MissionSettingsData config = PNH_MissionSettings.GetData();
        m_MissionTimeout = m_MissionTime + config.ConfiguracoesGerais.TempoLimpezaSegundos;
    }

    override bool DeployMission()
    {
        if (!m_Config || !m_Config.Ativa || m_Deployed) return false;
        m_Deployed = true;

        Object missionBuilding = null;
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
            // 1. BARRICADAS
            SpawnBarricadas(missionBuilding);

            // 2. NPC VIVO (Com pouca vida para o zumbi matar)
            SpawnSurvivor(missionBuilding);

            // 3. ZUMBIS INTERNOS (Dinamica do Mod Original)
            SpawnZumbisInternos(missionBuilding);
        }

        PNH_Logger.Log("Missões", "[PNH SYSTEM] MISSÃO_INICIADA: Apartment em " + m_MissionLocation);
        return true;
    }

    void SpawnSurvivor(Object missionBuilding)
    {
        vector cPos = missionBuilding.ModelToWorld(m_Config.PosicaoCorpoLocal.ToVector());
        m_SurvivorNPC = GetGame().CreateObject(m_Config.ClasseCorpo, cPos, false, false, true);
        if (m_SurvivorNPC) {
            EntityAI ent = EntityAI.Cast(m_SurvivorNPC);
            ent.SetOrientation(missionBuilding.GetOrientation() + m_Config.OrientacaoCorpoLocal.ToVector());
            for (int r = 0; r < m_Config.RoupasNPC.Count(); r++) ent.GetInventory().CreateInInventory(m_Config.RoupasNPC[r]);
            for (int it = 0; it < m_Config.InventarioNPC.Count(); it++) ent.GetInventory().CreateInInventory(m_Config.InventarioNPC[it]);
            
            ent.SetHealth("", "", 10); // Nasce vivo, mas muito fraco
            m_MissionObjects.Insert(m_SurvivorNPC);

            // SPAWN DO ZUMBI ASSASSINO AO LADO
            vector zKillPos = missionBuilding.ModelToWorld(m_Config.PosicaoZumbiAssassinoLocal.ToVector());
            Object killer = GetGame().CreateObject(m_Config.ClasseZumbiAssassino, zKillPos, false, true, true);
            if (killer) m_MissionAIs.Insert(killer);
        }
    }

    void SpawnRewards()
    {
        // Barril nasce somente no gatilho de proximidade
        Object missionBuilding = null;
        GetGame().GetObjectsAtPosition(m_MissionPosition, 30.0, m_ObjectList, m_ObjectCargoList);
        for (int b = 0; b < m_ObjectList.Count(); b++) {
            if (m_ObjectList[b].GetType().Contains("Land_Tenement_Small") || m_ObjectList[b].GetType().Contains("Apartment")) {
                missionBuilding = m_ObjectList[b];
                break;
            }
        }

        if (missionBuilding) {
            vector rPos = missionBuilding.ModelToWorld(m_Config.PosicaoRecompensaLocal.ToVector());
            m_RewardContainer = ItemBase.Cast(GetGame().CreateObject(m_Config.RecompensasHorda.Container, rPos, false, false, true));
            if (m_RewardContainer) {
                auto loadout = m_Config.RecompensasHorda.Loadouts.GetRandomElement();
                for (int j = 0; j < loadout.Itens.Count(); j++) m_RewardContainer.GetInventory().CreateInInventory(loadout.Itens[j]);
                m_RewardContainer.Close();
                m_MissionObjects.Insert(m_RewardContainer);
            }
        }
    }

    void SpawnZumbisInternos(Object missionBuilding)
    {
        for (int i = 0; i < m_Config.SpawnsZumbisInternos.Count(); i++) {
            vector zPos = missionBuilding.ModelToWorld(m_Config.SpawnsZumbisInternos[i].ToVector());
            m_MissionAIs.Insert(GetGame().CreateObject(m_Config.Dificuldade.ClassesZumbis.GetRandomElement(), zPos, false, true, true));
        }
    }

    void SpawnBarricadas(Object missionBuilding)
    {
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