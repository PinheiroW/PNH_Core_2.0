class CityStoreMission extends PNH_MissionBase
{
    Object m_NPC;
    ItemBase m_RewardBarrel;
    bool m_IsVictory = false;
    bool m_PhaseB_Active = false;
    
    // Controles de Perímetro
    bool m_A_OuterWarned = false; bool m_A_InnerWarned = false;
    bool m_B_OuterWarned = false; bool m_B_InnerWarned = false;
    
    ref PNH_MissionData_CityStore m_Config;

    override bool IsExtended() { return true; }
    
    void CityStoreMission()
    {
        string caminhoJson = "$profile:PNH/Missions/CityStore.json";
        m_Config = new PNH_MissionData_CityStore();
        if (FileExist(caminhoJson)) JsonFileLoader<PNH_MissionData_CityStore>.JsonLoadFile(caminhoJson, m_Config);

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
        PNH_MissionSettingsData config = PNH_MissionSettings.GetData();
        vector playerPos = player.GetPosition();

        // --- FASE A: COLETA NO MERCADO ---
        if (!m_PhaseB_Active)
        {
            float distA = vector.Distance(playerPos, m_MissionPosition);
            if (!m_A_OuterWarned && distA <= m_MissionZoneOuterRadius) { m_A_OuterWarned = true; EnviarAviso(m_MissionInformant, "Aproximando-se do setor de risco biológico."); }
            if (!m_A_InnerWarned && distA <= m_MissionZoneInnerRadius) { m_A_InnerWarned = true; EnviarAviso(m_MissionInformant, "Você entrou na área. Colete as amostras biológicas e os estabilizadores."); }

            // Verifica se tem todos os itens
            if (VerificarItens(player))
            {
                m_PhaseB_Active = true;
                string msgB = "**Fase B: Extração**. Amostras coletadas! Leve-as para o contato em NBC na delegacia de " + m_Config.CidadeEntrega + ".";
                EnviarAviso(m_MissionInformant, msgB);
            }
        }
        // --- FASE B: ENTREGA AO NPC NBC ---
        else if (!m_IsVictory && m_NPC)
        {
            float distB = vector.Distance(playerPos, m_NPC.GetPosition());
            if (!m_B_OuterWarned && distB <= m_MissionZoneOuterRadius) { m_B_OuterWarned = true; EnviarAviso(m_MissionInformant, "O contato da bio-extração está na área."); }
            
            // Chegou a menos de 3 metros do NPC com os itens
            if (distB < 3.0 && VerificarItens(player))
            {
                FinalizarMissao(player);
            }
        }
    }

    // LÓGICA DE CONTAGEM CORRIGIDA
    bool VerificarItens(PlayerBase player)
    {
        if (!m_Config) return false;
        
        array<EntityAI> items = new array<EntityAI>;
        player.GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER, items);
        
        int mush = 0;
        int meds = 0;
        
        for (int i = 0; i < items.Count(); i++)
        {
            if (items[i])
            {
                if (items[i].GetType() == m_Config.ItemCientifico) mush++;
                else if (items[i].GetType() == m_Config.ItemEstabilizador) meds++;
            }
        }
        
        return (mush >= m_Config.QtdItemCientifico && meds >= m_Config.QtdItemEstabilizador);
    }

    void FinalizarMissao(PlayerBase player)
    {
        // Deleta os itens de quest do inventário do jogador
        array<EntityAI> items = new array<EntityAI>;
        player.GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER, items);
        int dM = 0; int dE = 0;
        for (int i = 0; i < items.Count(); i++)
        {
            if (items[i])
            {
                if (dM < m_Config.QtdItemCientifico && items[i].GetType() == m_Config.ItemCientifico) { GetGame().ObjectDelete(items[i]); dM++; }
                else if (dE < m_Config.QtdItemEstabilizador && items[i].GetType() == m_Config.ItemEstabilizador) { GetGame().ObjectDelete(items[i]); dE++; }
            }
        }

        m_IsVictory = true;
        SpawnRewards();
        
        string winMsg = "[PNH_CORE] MISSÃO_CONCLUÍDA: " + player.GetIdentity().GetName() + " entregou as amostras biológicas em " + m_Config.CidadeEntrega + "!";
        PNH_Logger.Log("Missões", winMsg);
        EnviarAviso("Comando PNH", "Material entregue com sucesso! Acesse o barril de recompensa.");
        
        PNH_MissionSettingsData config = PNH_MissionSettings.GetData();
        m_MissionTimeout = m_MissionTime + config.ConfiguracoesGerais.TempoLimpezaSegundos;
    }

    void SpawnRewards()
    {
        if (!m_NPC || !m_Config || !m_Config.Recompensas) return;
        
        vector rPos = m_NPC.GetPosition() + "1.5 0 1.5";
        rPos[1] = GetGame().SurfaceY(rPos[0], rPos[2]);
        
        m_RewardBarrel = ItemBase.Cast(GetGame().CreateObject(m_Config.Recompensas.Container, rPos, false, false, false));
        if (m_RewardBarrel && m_Config.Recompensas.Loadouts && m_Config.Recompensas.Loadouts.Count() > 0)
        {
            auto loadout = m_Config.Recompensas.Loadouts.GetRandomElement();
            for (int i = 0; i < loadout.Itens.Count(); i++) m_RewardBarrel.GetInventory().CreateInInventory(loadout.Itens[i]);
            m_RewardBarrel.Close();
            m_MissionObjects.Insert(m_RewardBarrel);
        }
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
            m_MissionMessage1 = m_MissionMessage1 + m_MissionLocation; 
            m_MissionMessage3 = m_Config.Lore.MensagensRadio[2] + "** " + m_Config.CidadeEntrega + " **.";
        }

        // 1. SPAWN NPC NBC NO DESTINO
        vector deliveryPos = m_Config.PosicaoEntrega.ToVector();
        deliveryPos[1] = GetGame().SurfaceY(deliveryPos[0], deliveryPos[2]);
        m_NPC = GetGame().CreateObject(m_Config.ClasseNPC, deliveryPos, false, false, true);
        if (m_NPC) {
            EntityAI ent = EntityAI.Cast(m_NPC);
            ent.SetOrientation("180 0 0");
            if (m_Config.RoupaNPC) {
                for (int i = 0; i < m_Config.RoupaNPC.Count(); i++) ent.GetInventory().CreateInInventory(m_Config.RoupaNPC[i]);
            }
            m_MissionObjects.Insert(m_NPC);
        }

        // 2. ENCONTRAR O MERCADO PARA REFERÊNCIA DE SPAWN
        Object missionBuilding;
        array<Object> objects = new array<Object>;
        GetGame().GetObjectsAtPosition(m_MissionPosition, 5.0, objects, null);
        for (int b = 0; b < objects.Count(); b++) {
            if (objects[b].GetType().Contains("Land_City_Store")) {
                missionBuilding = objects[b];
                break;
            }
        }

        vector baseSpawnPos = m_MissionPosition;
        if (missionBuilding) 
        {
            baseSpawnPos = missionBuilding.ModelToWorld("0 -4.5 0"); // Altura do chão do mercado interno

            // 3. CONSTRUIR AS BARRICADAS
            if (m_Config.Cenario && m_Config.Cenario.Barricadas) 
            {
                for (int barIdx = 0; barIdx < m_Config.Cenario.Barricadas.Count(); barIdx++) 
                {
                    PNH_MissionSettings_Barricada barricada = m_Config.Cenario.Barricadas[barIdx];
                    vector localPos = barricada.PosicaoLocal.ToVector();
                    vector localOri = barricada.OrientacaoLocal.ToVector();
                    
                    // Transforma a coordenada local para o mundo real respeitando a rotação da loja
                    vector worldPos = missionBuilding.ModelToWorld(localPos);
                    Object barObj = GetGame().CreateObject(barricada.Classe, worldPos, false, false, true);
                    
                    if (barObj) {
                        vector buildingOri = missionBuilding.GetOrientation();
                        // Ajusta a rotação do objeto barricada relativo à loja
                        barObj.SetOrientation(Vector(buildingOri[0] + localOri[0], buildingOri[1] + localOri[1], buildingOri[2] + localOri[2]));
                        m_MissionObjects.Insert(barObj);
                    }
                }
            }
        }

        // 4. SPAWN DE ITENS BIOLÓGICOS NO CHÃO DO MERCADO
        for (int j = 0; j < m_Config.QtdItemCientifico; j++) {
            vector mushPos = baseSpawnPos + Vector(Math.RandomFloat(-4, 4), 0.1, Math.RandomFloat(-4, 4));
            m_MissionObjects.Insert(GetGame().CreateObject(m_Config.ItemCientifico, mushPos, false, false, true));
        }
        
        for (int k = 0; k < m_Config.QtdItemEstabilizador; k++) {
            vector medPos = baseSpawnPos + Vector(Math.RandomFloat(-3, 3), 0.1, Math.RandomFloat(-3, 3));
            m_MissionObjects.Insert(GetGame().CreateObject(m_Config.ItemEstabilizador, medPos, false, false, true));
        }

        PNH_Logger.Log("Missões", "[PNH_CORE] MISSÃO_INICIADA: Operação Micélio iniciada em " + m_MissionLocation);
        return true;
    }