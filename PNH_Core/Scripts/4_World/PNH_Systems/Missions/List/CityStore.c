class CityStoreMission extends PNH_MissionBase
{
    Object m_NPC;
    ItemBase m_RewardBarrel;
    ItemBase m_BagObject;
    
    bool m_IsVictory = false;
    bool m_PhaseB_Active = false;
    bool m_ExchangeDone = false; 
    
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
                EnviarAviso(m_MissionInformant, "Você chegou. Cuidado com os infectados. Entre no mercado e localize a Mochila Improvisada."); 
            }

            // GATILHO: Pegou a mochila
            if (m_BagObject && player.GetInventory().HasEntityInInventory(m_BagObject))
            {
                m_PhaseB_Active = true;
                
                // Puxa a mensagem do JSON e adiciona a cidade no final
                string msgB = "**Fase B: Extração**. Mochila recuperada! Siga para a extração em ";
                if (m_Config.Lore && m_Config.Lore.MensagemFaseB != "") {
                    msgB = m_Config.Lore.MensagemFaseB;
                }
                msgB = msgB + m_Config.CidadeEntrega + ".";
                
                EnviarAviso(m_MissionInformant, msgB);
                
                // Aviso no Discord da Fase B
                if (m_NPC) {
                    string discordMsg = "[PNH_CORE] MISSÃO_ATUALIZADA: Fase B (Extração) iniciada no Terminal de Trem de " + m_Config.CidadeEntrega + " | Coordenadas: " + m_NPC.GetPosition().ToString();
                    PNH_Logger.Log("Missões", discordMsg);
                }

                // Spawna Zumbis da Fase B
                SpawnZumbisFaseB();
            }
        }
        // --- FASE B: ENTREGA E BARRIL ---
        else if (m_NPC)
        {
            float distB = vector.Distance(playerPos, m_NPC.GetPosition());
            
            if (!m_B_OuterWarned && distB <= m_MissionZoneOuterRadius) { 
                m_B_OuterWarned = true; 
                EnviarAviso(m_MissionInformant, "O contato da bio-extração está na área do Terminal de Trem."); 
            }
            if (!m_B_InnerWarned && distB <= m_MissionZoneInnerRadius) { 
                m_B_InnerWarned = true; 
                EnviarAviso(m_MissionInformant, "Entregue a mochila improvisada diretamente ao contato para liberar o barril de pagamento."); 
            }
            
            // Etapa 1: Entrega a Mochila (Aparece o Barril)
            if (!m_ExchangeDone && distB < 3.0 && m_BagObject && player.GetInventory().HasEntityInInventory(m_BagObject))
            {
                m_ExchangeDone = true;
                if (m_BagObject) GetGame().ObjectDelete(m_BagObject);
                SpawnRewards();
                EnviarAviso("Comando PNH", "Material biológico recebido com sucesso! O barril de recompensa surgiu, abra-o para finalizar a missão.");
            }
            
            // Etapa 2: Vitória ao Abrir o Barril
            if (m_ExchangeDone && !m_IsVictory && m_RewardBarrel)
            {
                if (m_RewardBarrel.IsOpen())
                {
                    FinalizarMissao(player);
                }
            }
        }
    }

    void SpawnZumbisFaseB()
    {
        if (m_NPC && m_Config && m_Config.Dificuldade && m_Config.Dificuldade.ClassesZumbis.Count() > 0)
        {
            vector center = m_NPC.GetPosition();
            for (int i = 0; i < m_Config.Dificuldade.QuantidadeHordaFinal; i++)
            {
                vector zPos = center + Vector(Math.RandomFloat(-15, 15), 0, Math.RandomFloat(-15, 15));
                zPos[1] = GetGame().SurfaceY(zPos[0], zPos[2]);
                m_MissionAIs.Insert(GetGame().CreateObject(m_Config.Dificuldade.ClassesZumbis.GetRandomElement(), zPos, false, true, true));
            }
        }
    }

    void FinalizarMissao(PlayerBase player)
    {
        m_IsVictory = true;
        
        string winMsg = "[PNH_CORE] MISSÃO_CONCLUÍDA: " + player.GetIdentity().GetName() + " finalizou a extração biológica no Terminal de Trem de " + m_Config.CidadeEntrega + "!";
        PNH_Logger.Log("Missões", winMsg);
        
        string mensagemFinal = "Missão concluída! Recompensa liberada no local.";
        if (m_Config.Lore && m_Config.Lore.MensagemVitoria != "") {
            mensagemFinal = m_Config.Lore.MensagemVitoria;
        }
        
        EnviarAviso("Comando PNH", mensagemFinal);
        
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

        // 1. SPAWN NPC NBC (Fase B)
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

        // 2. ENCONTRAR O MERCADO E SPAWN DA FASE A
        Object missionBuilding = null;
        GetGame().GetObjectsAtPosition(m_MissionPosition, 20.0, m_ObjectList, m_ObjectCargoList);
        for (int b = 0; b < m_ObjectList.Count(); b++) {
            if (m_ObjectList[b].GetType().Contains("Land_City_Store")) {
                missionBuilding = m_ObjectList[b];
                break;
            }
        }

        vector baseSpawnPos = m_MissionPosition;

        if (missionBuilding) 
        {
            baseSpawnPos = missionBuilding.GetPosition();

            // CONSTRUIR BARRICADAS
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
            
            // SPAWN DA MOCHILA E ITENS
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
            // FALLBACK DE SEGURANÇA
            PNH_Logger.Log("Missões", "[PNH_CORE] AVISO: Prédio não detectado! Usando ponto zero.");
            m_BagObject = ItemBase.Cast(GetGame().CreateObject(m_Config.ClasseBolsa, m_MissionPosition, false, false, true));
            if (m_BagObject) {
                for (int m = 0; m < m_Config.QtdItemCientifico; m++) m_BagObject.GetInventory().CreateInInventory(m_Config.ItemCientifico);
                for (int n = 0; n < m_Config.QtdItemEstabilizador; n++) m_BagObject.GetInventory().CreateInInventory(m_Config.ItemEstabilizador);
                m_MissionObjects.Insert(m_BagObject);
            }
        }

        // SPAWN DOS ZUMBIS DA FASE A (No Mercado)
        if (m_Config.Dificuldade && m_Config.Dificuldade.ClassesZumbis.Count() > 0)
        {
            for (int zA = 0; zA < m_Config.Dificuldade.QuantidadeHordaFinal; zA++)
            {
                vector zPosA = baseSpawnPos + Vector(Math.RandomFloat(-15, 15), 0, Math.RandomFloat(-15, 15));
                zPosA[1] = GetGame().SurfaceY(zPosA[0], zPosA[2]);
                m_MissionAIs.Insert(GetGame().CreateObject(m_Config.Dificuldade.ClassesZumbis.GetRandomElement(), zPosA, false, true, true));
            }
        }

        PNH_Logger.Log("Missões", "[PNH_CORE] MISSÃO_INICIADA: Operação Micélio iniciada em " + m_MissionLocation);
        return true;
    }
}