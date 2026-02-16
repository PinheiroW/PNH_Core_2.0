class TransportMission extends PNH_MissionBase
{
    ItemBase m_BagObject;
    ItemBase m_RewardBarrel;
    Object m_NPCColeta;
    Object m_NPCEntrega;
    
    bool m_IsVictory = false;
    bool m_ExchangeDone = false; 
    bool m_PickedUpMessageSent = false; 
    string m_WinnerName = "Desconhecido";

    bool m_StartZoneWarned = false;    
    bool m_StartArrivedWarned = false; 
    bool m_EndZoneWarned = false;      
    bool m_EndArrivedWarned = false;   
    
    ref PNH_MissionData_Transport m_Config;

    override bool IsExtended() { return true; }
    
    void TransportMission()
    {
        string caminhoJson = "$profile:PNH/Missions/Transport.json";
        m_Config = new PNH_MissionData_Transport();
        if (FileExist(caminhoJson)) JsonFileLoader<PNH_MissionData_Transport>.JsonLoadFile(caminhoJson, m_Config);

        m_MissionTimeout = m_Config.TempoLimiteSegundos;
        m_MissionZoneOuterRadius = m_Config.RaioAvisoExterno;
        m_MissionZoneInnerRadius = m_Config.RaioAvisoInterno;
        m_MissionInformant = m_Config.Lore.Informante;
        m_MissionMessage1 = m_Config.Lore.MensagensRadio[0];
        m_MissionMessage2 = m_Config.Lore.MensagensRadio[1];
    }

    override void PlayerChecks(PlayerBase player)
    {
        PNH_MissionSettingsData config = PNH_MissionSettings.GetData();
        vector playerPos = player.GetPosition();
        
        // --- FASE 1: COLETA (GARAGEM A) ---
        if (!m_PickedUpMessageSent)
        {
            float distToStart = vector.Distance(playerPos, m_MissionPosition);
            if (!m_StartZoneWarned && distToStart <= m_MissionZoneOuterRadius) { m_StartZoneWarned = true; EnviarAviso(m_MissionInformant, "Aproximando-se da garagem de coleta."); }
            if (!m_StartArrivedWarned && distToStart <= m_MissionZoneInnerRadius) { m_StartArrivedWarned = true; EnviarAviso(m_MissionInformant, "O contato está na garagem. Pegue a mochila com ele."); }

            if (player.GetInventory().HasEntityInInventory(m_BagObject))
            {
                m_PickedUpMessageSent = true;
                EnviarAviso(m_MissionInformant, "Carga coletada! Leve-a agora para a garagem em " + m_Config.CidadeEntrega + ".");
            }
        }

        // --- FASE 2: ENTREGA (GARAGEM B) ---
        if (m_PickedUpMessageSent && !m_ExchangeDone && m_NPCEntrega)
        {
            float distToEnd = vector.Distance(playerPos, m_NPCEntrega.GetPosition());
            if (!m_EndZoneWarned && distToEnd <= m_MissionZoneOuterRadius) { m_EndZoneWarned = true; EnviarAviso(m_MissionInformant, "Chegando na garagem de entrega em " + m_Config.CidadeEntrega + "."); }
            
            if (distToEnd < 3.0 && player.GetInventory().HasEntityInInventory(m_BagObject))
            {
                m_ExchangeDone = true;
                m_WinnerName = player.GetIdentity().GetName();
                
                GetGame().ObjectDelete(m_BagObject); 
                SpawnRewards(); 
                
                string msgEntrega = "[PNH_CORE] MISSÃO_CONCLUÍDA: " + m_WinnerName + " entregou as sementes na garagem de " + m_Config.CidadeEntrega + "!";
                EnviarAviso("Comando PNH", "Mercadoria entregue! O barril de pagamento surgiu.");
                
                // LOG PARA O DISCORD (Acionado na entrega ao NPC)
                PNH_Logger.Log("Missões", msgEntrega); 
            }
        }

        // --- FINALIZAÇÃO: ABRIR BARRIL ---
        if (m_ExchangeDone && !m_IsVictory && m_RewardBarrel)
        {
            if (m_RewardBarrel.IsOpen())
            {
                m_IsVictory = true;
                // Inicia a limpeza da área
                m_MissionTimeout = m_MissionTime + config.ConfiguracoesGerais.TempoLimpezaSegundos;
            }
        }
    }

    void EnviarAviso(string emissor, string msg)
    {
        PNH_MissionSettingsData config = PNH_MissionSettings.GetData();
        if (config.ConfiguracoesGerais.UsarPDA) GetRPCManager().SendRPC("[GearPDA] ", "SendGlobalMessage", new Param2<string, string>(emissor, msg), true);
        else PNH_Utils.SendMessageToAll("[" + emissor + "] " + msg);
    }

    void VesterNPC(Object npc, array<string> roupas)
    {
        EntityAI ent = EntityAI.Cast(npc);
        if (!ent) return;
        for (int i = 0; i < roupas.Count(); i++) 
        {
            ent.GetInventory().CreateInInventory(roupas[i]);
        }
    }

    void SpawnRewards()
    {
        if (m_NPCEntrega)
        {
            vector rewardPos = m_NPCEntrega.GetPosition() + "1.5 0 1.5";
            rewardPos[1] = GetGame().SurfaceY(rewardPos[0], rewardPos[2]);
            m_RewardBarrel = ItemBase.Cast(GetGame().CreateObject(m_Config.Recompensas.Container, rewardPos, false, false, false));
            if (m_RewardBarrel)
            {
                for (int i = 0; i < m_Config.Recompensas.Itens.Count(); i++)
                    m_RewardBarrel.GetInventory().CreateInInventory(m_Config.Recompensas.Itens[i]);
                m_RewardBarrel.Close(); 
                m_MissionObjects.Insert(m_RewardBarrel);
            }
        }
    }

    override bool DeployMission()
    {
        if (!m_Config || !m_Config.Ativa) return false;

        m_MissionMessage3 = m_Config.Lore.MensagensRadio[2] + "** " + m_MissionLocation + " **.";

        // NPC 1: COLETA (Dressed)
        m_NPCColeta = GetGame().CreateObject(m_Config.ClasseNPCColeta, m_MissionPosition, false, false, true);
        if (m_NPCColeta) { 
            m_NPCColeta.SetOrientation("0 0 0"); 
            VesterNPC(m_NPCColeta, m_Config.RoupasNPCColeta);
            m_MissionObjects.Insert(m_NPCColeta); 
        }

        // NPC 2: ENTREGA (Dressed)
        vector deliveryPos = m_Config.PosicaoEntrega.ToVector();
        deliveryPos[1] = GetGame().SurfaceY(deliveryPos[0], deliveryPos[2]);
        m_NPCEntrega = GetGame().CreateObject(m_Config.ClasseNPCEntrega, deliveryPos, false, false, true);
        if (m_NPCEntrega) { 
            m_NPCEntrega.SetOrientation("180 0 0"); 
            VesterNPC(m_NPCEntrega, m_Config.RoupasNPCEntrega);
            m_MissionObjects.Insert(m_NPCEntrega); 
        }

        // MOCHILA (Com as sementes)
        m_BagObject = ItemBase.Cast(GetGame().CreateObject(m_Config.ClasseVeiculo, m_MissionPosition, false, false, true));
        if (m_BagObject)
        {
            for (int k = 0; k < m_Config.QuantidadeCargaNecessaria; k++)
                m_BagObject.GetInventory().CreateInInventory(m_Config.ItemCarga);
            m_MissionObjects.Insert(m_BagObject);
        }

        PNH_Logger.Log("Missões", "[PNH_CORE] MISSÃO_INICIADA: Transporte iniciado para garagem em " + m_Config.CidadeEntrega);
        return true;
    }
}