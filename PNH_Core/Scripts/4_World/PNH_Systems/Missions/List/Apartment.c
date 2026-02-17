class ApartmentMission extends PNH_MissionBase
{
    ItemBase m_RewardContainer;
    Object m_SurvivorNPC;
    Object m_NPCEntrega;
    
    bool m_IsVictory = false;
    bool m_Deployed = false; 
    
    // Controles de Fase B
    bool m_PhaseB_Active = false;
    bool m_ExchangeDone = false;
    
    string m_SurvivorName;

    bool m_A_OuterWarned = false; bool m_A_InnerWarned = false;
    bool m_B_OuterWarned = false; bool m_B_InnerWarned = false;

    ref PNH_MissionData_Apartment m_Config;

    override bool IsExtended() { return true; } 

    void ApartmentMission()
    {
        string caminhoJson = "$profile:PNH/Missions/Apartment.json";
        m_Config = new PNH_MissionData_Apartment();
        if (FileExist(caminhoJson)) JsonFileLoader<PNH_MissionData_Apartment>.JsonLoadFile(caminhoJson, m_Config);

        if (m_Config.Lore && m_Config.Lore.NomesSobreviventes && m_Config.Lore.NomesSobreviventes.Count() > 0)
            m_SurvivorName = m_Config.Lore.NomesSobreviventes.GetRandomElement();
        else
            m_SurvivorName = "Alvo";

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

    // =========================================================
    // FUNÇÃO ROBUSTA DE BUSCA (Verifica Mãos e Bolsos Profundos)
    // =========================================================
    EntityAI ProcurarItemNoJogador(PlayerBase player, string itemClass)
    {
        if (!player || itemClass == "") return null;

        // 1. Verifica primeiro nas Mãos
        EntityAI itemMaos = player.GetHumanInventory().GetEntityInHands();
        if (itemMaos && itemMaos.GetType() == itemClass) return itemMaos;

        // 2. Faz um Raio-X a todo o inventário (mochilas, calças, casacos)
        array<EntityAI> itemsArray = new array<EntityAI>;
        player.GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER, itemsArray);
        
        for (int i = 0; i < itemsArray.Count(); i++)
        {
            if (itemsArray[i] && itemsArray[i].GetType() == itemClass)
            {
                return itemsArray[i];
            }
        }
        return null;
    }

    override void PlayerChecks(PlayerBase player)
    {
        vector playerPos = player.GetPosition();
        
        // Usa a nova função para saber se o jogador está com o livro
        EntityAI livroNoJogador = ProcurarItemNoJogador(player, m_Config.ItemMissao);

        // --- FASE A: BUSCA PELO LIVRO NO PRÉDIO ---
        if (!m_PhaseB_Active)
        {
            float distA = vector.Distance(playerPos, m_MissionPosition);

            if (!m_A_OuterWarned && distA <= m_MissionZoneOuterRadius) {
                m_A_OuterWarned = true;
                EnviarAviso(m_MissionInformant, m_Config.Lore.MensagemAproximacao + m_SurvivorName + ".");
            }

            if (!m_A_InnerWarned && distA <= m_MissionZoneInnerRadius) {
                m_A_InnerWarned = true;
                EnviarAviso(m_MissionInformant, m_Config.Lore.MensagemNoObjetivo);
            }

            // GATILHO: Jogador pegou o livro
            if (livroNoJogador)
            {
                m_PhaseB_Active = true;
                string msgB = m_Config.Lore.MensagemFaseB + m_Config.CidadeEntrega + ".";
                EnviarAviso(m_MissionInformant, msgB);
                PNH_Logger.Log("Missões", "[PNH_CORE] FASE B INICIADA: Transporte do livro para " + m_Config.CidadeEntrega);
            }
        }
        // --- FASE B: ENTREGA DO LIVRO EM GREEN MOUNTAIN ---
        else if (m_NPCEntrega)
        {
            float distB = vector.Distance(playerPos, m_NPCEntrega.GetPosition());

            if (!m_B_OuterWarned && distB <= m_MissionZoneOuterRadius) {
                m_B_OuterWarned = true;
                EnviarAviso(m_MissionInformant, m_Config.Lore.MensagemAproximacaoEntrega);
            }

            if (!m_B_InnerWarned && distB <= 15.0) { 
                m_B_InnerWarned = true;
                EnviarAviso(m_MissionInformant, m_Config.Lore.MensagemNoObjetivoEntrega);
            }

            // GATILHO DE ENTREGA: Distância aumentada para 5 metros para evitar bugs de colisão
            if (!m_ExchangeDone && distB <= 5.0 && livroNoJogador)
            {
                m_ExchangeDone = true;
                
                // DELETA O LIVRO DO INVENTÁRIO
                GetGame().ObjectDelete(livroNoJogador);
                
                SpawnRewards(m_NPCEntrega.GetPosition());
                EnviarAviso("Comando PNH", "Livro validado e entregue. O seu barril de pagamento surgiu!");
            }

            // VITÓRIA
            if (m_ExchangeDone && !m_IsVictory && m_RewardContainer && m_RewardContainer.IsOpen())
            {
                FinalizarMissao(player);
            }
        }
    }

    void FinalizarMissao(PlayerBase player)
    {
        m_IsVictory = true;
        PNH_Logger.Log("Missões", "[PNH_CORE] MISSÃO_CONCLUÍDA: " + player.GetIdentity().GetName() + " em Green Mountain.");
        EnviarAviso("Comando PNH", m_Config.Lore.MensagemVitoria);
        PNH_MissionSettingsData config = PNH_MissionSettings.GetData();
        m_MissionTimeout = m_MissionTime + config.ConfiguracoesGerais.TempoLimpezaSegundos;
    }

    override bool DeployMission()
    {
        if (!m_Config || !m_Config.Ativa || m_Deployed) return false;
        m_Deployed = true;

        if (m_Config.Lore && m_Config.Lore.MensagensRadio && m_Config.Lore.MensagensRadio.Count() >= 3)
            m_MissionMessage3 = m_Config.Lore.MensagensRadio[2] + "** " + m_MissionLocation + " **.";

        Object missionBuilding = null;
        GetGame().GetObjectsAtPosition(m_MissionPosition, 30.0, m_ObjectList, m_ObjectCargoList);
        for (int b = 0; b < m_ObjectList.Count(); b++) {
            if (m_ObjectList[b].GetType().Contains("Tenement") || m_ObjectList[b].GetType().Contains("Apartment")) {
                missionBuilding = m_ObjectList[b];
                break;
            }
        }

        if (missionBuilding)
        {
            Building tenement = Building.Cast(missionBuilding);
            for (int k = 0; k < 32; k++) {
                if (tenement.IsDoorOpen(k)) tenement.CloseDoor(k);
            }

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

            vector cPos = missionBuilding.ModelToWorld(m_Config.PosicaoCorpoLocal.ToVector());
            m_SurvivorNPC = GetGame().CreateObject(m_Config.ClasseCorpo, cPos, false, false, true);
            if (m_SurvivorNPC) {
                EntityAI ent = EntityAI.Cast(m_SurvivorNPC);
                ent.SetOrientation(missionBuilding.GetOrientation() + m_Config.OrientacaoCorpoLocal.ToVector());
                for (int r = 0; r < m_Config.RoupasNPC.Count(); r++) ent.GetInventory().CreateInInventory(m_Config.RoupasNPC[r]);
                for (int it = 0; it < m_Config.InventarioNPC.Count(); it++) ent.GetInventory().CreateInInventory(m_Config.InventarioNPC[it]);
                
                // CRIA O LIVRO NO INVENTÁRIO DO NPC
                ent.GetInventory().CreateInInventory(m_Config.ItemMissao);
                
                ent.SetHealth("", "", 10); 
                m_MissionObjects.Insert(m_SurvivorNPC);

                vector zKillPos = missionBuilding.ModelToWorld(m_Config.PosicaoZumbiAssassinoLocal.ToVector());
                m_MissionAIs.Insert(GetGame().CreateObject(m_Config.ClasseZumbiAssassino, zKillPos, false, true, true));
            }

            for (int j = 0; j < m_Config.SpawnsZumbisInternos.Count(); j++) {
                vector zIPos = missionBuilding.ModelToWorld(m_Config.SpawnsZumbisInternos[j].ToVector());
                m_MissionAIs.Insert(GetGame().CreateObject(m_Config.Dificuldade.ClassesZumbis.GetRandomElement(), zIPos, false, true, true));
            }
        }

        // --- SPAWN DA FASE B (BORIS EM GREEN MOUNTAIN) ---
        vector deliveryPos = m_Config.PosicaoEntrega.ToVector();
        deliveryPos[1] = GetGame().SurfaceY(deliveryPos[0], deliveryPos[2]);
        m_NPCEntrega = GetGame().CreateObject(m_Config.ClasseNPCEntrega, deliveryPos, false, false, true);
        if (m_NPCEntrega) {
            EntityAI npcBoris = EntityAI.Cast(m_NPCEntrega);
            npcBoris.SetOrientation("0 0 0"); 
            for (int rBoris = 0; rBoris < m_Config.RoupasNPCEntrega.Count(); rBoris++) {
                npcBoris.GetInventory().CreateInInventory(m_Config.RoupasNPCEntrega[rBoris]);
            }
            m_MissionObjects.Insert(m_NPCEntrega);
        }

        PNH_Logger.Log("Missões", "[PNH SYSTEM] MISSÃO_INICIADA: Eliminação e Recuperação");
        return true;
    }

    void SpawnRewards(vector pos)
    {
        vector rPos = pos + "1.5 0 1.5";
        rPos[1] = GetGame().SurfaceY(rPos[0], rPos[2]);
        
        m_RewardContainer = ItemBase.Cast(GetGame().CreateObject(m_Config.RecompensasHorda.Container, rPos, false, false, true));
        if (m_RewardContainer) {
            auto loadout = m_Config.RecompensasHorda.Loadouts.GetRandomElement();
            for (int l = 0; l < loadout.Itens.Count(); l++) m_RewardContainer.GetInventory().CreateInInventory(loadout.Itens[l]);
            m_RewardContainer.Close();
            m_MissionObjects.Insert(m_RewardContainer);
        }
    }

    void EnviarAviso(string emissor, string msg)
    {
        PNH_MissionSettingsData config = PNH_MissionSettings.GetData();
        if (config.ConfiguracoesGerais.UsarPDA) GetRPCManager().SendRPC("[GearPDA] ", "SendGlobalMessage", new Param2<string, string>(emissor, msg), true);
        else PNH_Utils.SendMessageToAll("[" + emissor + "] " + msg);
    }
}