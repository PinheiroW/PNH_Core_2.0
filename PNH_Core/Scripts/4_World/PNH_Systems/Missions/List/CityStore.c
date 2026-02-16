class CityStoreMission extends PNH_MissionBase
{
    Object m_NPC;
    ItemBase m_RewardBarrel;
    ItemBase m_BagObject;
    
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

        // --- FASE A: BUSCA PELA MOCHILA ---
        if (!m_PhaseB_Active)
        {
            float distA = vector.Distance(playerPos, m_MissionPosition);
            
            if (!m_A_OuterWarned && distA <= m_MissionZoneOuterRadius) { 
                m_A_OuterWarned = true; 
                EnviarAviso(m_MissionInformant, "Aproximando-se do setor de risco biológico. O mercado barricado está logo à frente."); 
            }
            if (!m_A_InnerWarned && distA <= m_MissionZoneInnerRadius) { 
                m_A_InnerWarned = true; 
                EnviarAviso(m_MissionInformant, "Você chegou. Entre no mercado e localize a Mochila Improvisada com os cogumelos e antibióticos."); 
            }

            // GATILHO: Pegou a mochila
            if (m_BagObject && player.GetInventory().HasEntityInInventory(m_BagObject))
            {
                m_PhaseB_Active = true;
                string msgB = "**Fase B: Extração**. Mochila recuperada! Leve-a imediatamente para a equipe NBC na delegacia de " + m_Config.CidadeEntrega + ".";
                EnviarAviso(m_MissionInformant, msgB);
            }
        }
        // --- FASE B: ENTREGA AO NPC NBC ---
        else if (!m_IsVictory && m_NPC)
        {
            float distB = vector.Distance(playerPos, m_NPC.GetPosition());
            
            if (!m_B_OuterWarned && distB <= m_MissionZoneOuterRadius) { 
                m_B_OuterWarned = true; 
                EnviarAviso(m_MissionInformant, "O contato da bio-extração está na área da delegacia."); 
            }
            if (!m_B_InnerWarned && distB <= m_MissionZoneInnerRadius) { 
                m_B_InnerWarned = true; 
                EnviarAviso(m_MissionInformant, "Entregue a mochila improvisada diretamente ao contato para receber o seu pagamento."); 
            }
            
            // Vitória (Distância de 3m + Mochila no inventário)
            if (distB < 3.0 && m_BagObject && player.GetInventory().HasEntityInInventory(m_BagObject))
            {
                FinalizarMissao(player);
            }
        }
    }

    void FinalizarMissao(PlayerBase player)
    {
        m_IsVictory = true;
        
        // Exclui a mochila inteira (junto com tudo que está dentro)
        if (m_BagObject) GetGame().ObjectDelete(m_BagObject);
        
        SpawnRewards();
        
        string winMsg = "[PNH_CORE] MISSÃO_CONCLUÍDA: " + player.GetIdentity().GetName() + " entregou as amostras biológicas em " + m_Config.CidadeEntrega + "!";
        PNH_Logger.Log("Missões", winMsg);
        EnviarAviso("Comando PNH", "Material recebido com sucesso! O barril de recompensa foi liberado no local.");
        
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

        // 1. SPAWN NPC NBC
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

        // 2. ENCONTRAR O MERCADO (Raio expandido para 20 metros para garantir que acha)
        Object missionBuilding = null;
        GetGame().GetObjectsAtPosition(m_MissionPosition, 20.0, m_ObjectList, m_ObjectCargoList);
        for (int b = 0; b < m_ObjectList.Count(); b++) {
            if (m_ObjectList[b].GetType().Contains("Land_City_Store")) {
                missionBuilding = m_ObjectList[b];
                break;
            }
        }

        if (missionBuilding) 
        {
            // 3. CONSTRUIR BARRICADAS
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
            
            // 4. SPAWN DA MOCHILA E ITENS DENTRO DELA
            vector bagWorldPos = missionBuilding.ModelToWorld(m_Config.PosicaoBolsaLocal.ToVector());
            m_BagObject = ItemBase.Cast(GetGame().CreateObject(m_Config.ClasseBolsa, bagWorldPos, false, false, true));
            if (m_BagObject)
            {
                vector bOri = missionBuilding.GetOrientation();
                vector bagOri = m_Config.OrientacaoBolsaLocal.ToVector();
                m_BagObject.SetOrientation(Vector(bOri[0] + bagOri[0], bOri[1] + bagOri[1], bOri[2] + bagOri[2]));
                
                for (int j = 0; j < m_Config.QtdItemCientifico; j++) m_BagObject.GetInventory().CreateInInventory(m_Config.ItemCientifico);
                for (int k = 0; k < m_Config.QtdItemEstabilizador; k++) m_BagObject.GetInventory().CreateInInventory(m_Config.ItemEstabilizador);
                m_MissionObjects.Insert(m_BagObject);
            }
        }
        else 
        {
            // FALLBACK: Caso ocorra alguma anomalia grave e o prédio não seja encontrado, 
            // a mochila ainda vai nascer no centro do marcador para a missão não quebrar.
            PNH_Logger.Log("Missões", "[PNH_CORE] AVISO: Prédio Land_City_Store não detectado! Usando ponto zero.");
            m_BagObject = ItemBase.Cast(GetGame().CreateObject(m_Config.ClasseBolsa, m_MissionPosition, false, false, true));
            if (m_BagObject) {
                for (int m = 0; m < m_Config.QtdItemCientifico; m++) m_BagObject.GetInventory().CreateInInventory(m_Config.ItemCientifico);
                for (int n = 0; n < m_Config.QtdItemEstabilizador; n++) m_BagObject.GetInventory().CreateInInventory(m_Config.ItemEstabilizador);
                m_MissionObjects.Insert(m_BagObject);
            }
        }

        PNH_Logger.Log("Missões", "[PNH_CORE] MISSÃO_INICIADA: Operação Micélio iniciada em " + m_MissionLocation);
        return true;
    }
}