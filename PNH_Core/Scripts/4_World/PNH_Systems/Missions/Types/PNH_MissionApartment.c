/// --- Documentação PNH_Core: PNH_MissionApartment.c ---
/// Versão do Sistema: 2.2.0 (Integração com Agente Treasury: XP + Loot Centralizado)
/// Função: Gerir a narrativa de infiltração e extração. A entrega final é agora processada pelo TreasuryManager.

class PNH_MissionApartment : PNH_MissionBase
{
    protected ref array<Object> m_CenarioObjetos = new array<Object>;
    protected Object m_MissionBuilding; 
    protected EntityAI m_CorpoTraidor;
    protected EntityAI m_NPCEntrega;
    protected EntityAI m_ItemObjetivo; 
    protected bool m_FaseEntregaAtiva = false;

    // Flags de rádio para a Fase B
    protected bool m_FaseB_MsgInicioSent = false;
    protected bool m_FaseB_Msg90mSent = false;
    protected bool m_FaseB_Msg20mSent = false;

    // 1. MONTAGEM DA MISSÃO (Após aceite no Tablet)
    override bool DeployMission()
    {
        super.DeployMission(); 

        array<Object> objList = new array<Object>;
        array<CargoBase> cargoList = new array<CargoBase>;
        GetGame().GetObjectsAtPosition(m_MissionPosition, 1.0, objList, cargoList);
        
        for (int i = 0; i < objList.Count(); i++)
        {
            if (objList.Get(i).GetType() == "Land_Tenement_Small")
            {
                m_MissionBuilding = objList.Get(i);
                break;
            }
        }

        if (m_MissionBuilding)
        {
            // Tranca o prédio e atualiza navegação
            Building Tenement = Building.Cast(m_MissionBuilding);
            for (int k = 0; k < 32; k++) { if (Tenement.IsDoorOpen(k)) Tenement.CloseDoor(k); }
            GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(GetGame().UpdatePathgraphRegionByObject, 1000, false, Tenement);

            // Spawn Barricadas (ModelToWorld)
            foreach (auto barricada : m_Config.Cenario.Barricadas)
            {
                vector bPos = m_MissionBuilding.ModelToWorld(barricada.PosicaoLocal.ToVector());
                Object obj = GetGame().CreateObjectEx(barricada.Classe, bPos, ECE_CREATEPHYSICS);
                if (obj) 
                {
                    obj.SetPosition(bPos);
                    obj.SetDirection(m_MissionBuilding.GetDirection());
                    obj.SetOrientation(obj.GetOrientation() + barricada.OrientacaoLocal.ToVector());
                    m_CenarioObjetos.Insert(obj);
                }
            }

            // Spawn Traidor e Item
            vector posCorpo = m_MissionBuilding.ModelToWorld(m_Config.PosicaoCorpoLocal.ToVector());
            m_CorpoTraidor = EntityAI.Cast(GetGame().CreateObject(m_Config.ClasseCorpo, posCorpo));
            if (m_CorpoTraidor)
            {
                m_CorpoTraidor.SetDirection(m_MissionBuilding.GetDirection());
                m_CorpoTraidor.SetOrientation(m_CorpoTraidor.GetOrientation() + m_Config.OrientacaoCorpoLocal.ToVector());
                foreach (string roupaNPC : m_Config.RoupasNPC) m_CorpoTraidor.GetInventory().CreateInInventory(roupaNPC);
                foreach (string itemNPC : m_Config.InventarioNPC) m_CorpoTraidor.GetInventory().CreateInInventory(itemNPC);
                m_CenarioObjetos.Insert(m_CorpoTraidor);
                m_ItemObjetivo = m_CorpoTraidor.GetInventory().CreateInInventory(m_Config.ItemMissao);
            }

            // Spawn Horda Interna
            foreach (string posZmb : m_Config.SpawnsZumbisInternos)
            {
                vector posZmbMundo = m_MissionBuilding.ModelToWorld(posZmb.ToVector());
                GetGame().CreateObject(m_Config.Dificuldade.ClassesZumbis.GetRandomElement(), posZmbMundo);
            }

            PNH_Logger.Log("Mission", "[PNH] Infiltração Montada no Predio: " + m_MissionBuilding.GetType());
            return true; 
        }
        return false;
    }

    // 2. VERIFICAÇÕES DE DISTÂNCIA E ESTADO
    override void MissionChecks()
    {
        super.MissionChecks();
        
        PlayerBase owner = PlayerBase.Cast(GetGame().GetPlayerByPlainId(m_MissionOwnerID));
        if (!owner) return;

        float dist;
        if (!m_FaseEntregaAtiva)
        {
            dist = vector.Distance(owner.GetPosition(), m_MissionPosition);
            if (dist < 90.0 && !m_Msg90mSent) {
                PNH_BroadcastManager.GetInstance().SendNotificationToPlayer(owner, m_MissionInformant, m_LoreEtapas.Chegou_90m, 8.0, "Notifications/gui/data/info.edds");
                m_Msg90mSent = true;
            }
            if (dist < 20.0 && !m_Msg20mSent) {
                PNH_BroadcastManager.GetInstance().SendNotificationToPlayer(owner, m_MissionInformant, m_LoreEtapas.Chegou_20m, 8.0, "Notifications/gui/data/info.edds");
                m_Msg20mSent = true;
            }
            CheckItemDiscovery(owner);
        }
        else
        {
            dist = vector.Distance(owner.GetPosition(), m_Config.PosicaoEntrega.ToVector());
            if (!m_FaseB_MsgInicioSent) {
                PNH_BroadcastManager.GetInstance().SendNotificationToPlayer(owner, m_MissionInformant, m_LoreEtapas.FaseB_Inicio, 8.0, "Notifications/gui/data/info.edds");
                m_FaseB_MsgInicioSent = true;
            }
            if (dist < 90.0 && !m_FaseB_Msg90mSent) {
                PNH_BroadcastManager.GetInstance().SendNotificationToPlayer(owner, m_MissionInformant, m_LoreEtapas.FaseB_Chegou_90m, 8.0, "Notifications/gui/data/info.edds");
                m_FaseB_Msg90mSent = true;
            }
            if (dist < 20.0 && !m_FaseB_Msg20mSent) {
                PNH_BroadcastManager.GetInstance().SendNotificationToPlayer(owner, m_MissionInformant, m_LoreEtapas.FaseB_Chegou_20m, 8.0, "Notifications/gui/data/info.edds");
                m_FaseB_Msg20mSent = true;
            }
            CheckDeliveryPoint();
        }
    }

    void CheckItemDiscovery(PlayerBase owner)
    {
        if (m_ItemObjetivo && m_CorpoTraidor)
        {
            if (m_ItemObjetivo.GetHierarchyRoot() != m_CorpoTraidor)
            {
                m_FaseEntregaAtiva = true;
                PNH_BroadcastManager.GetInstance().SendNotificationToPlayer(owner, m_MissionInformant, m_LoreEtapas.Concluiu, 8.0, "Notifications/gui/data/info.edds");
                SpawnNPCEntrega();
            }
        }
    }

    void SpawnNPCEntrega()
    {
        m_NPCEntrega = EntityAI.Cast(GetGame().CreateObject(m_Config.ClasseNPCEntrega, m_Config.PosicaoEntrega.ToVector()));
        if (m_NPCEntrega)
        {
            foreach (string roupa : m_Config.RoupasNPCEntrega) m_NPCEntrega.GetInventory().CreateInInventory(roupa);
            m_CenarioObjetos.Insert(m_NPCEntrega);
        }
    }

    void CheckDeliveryPoint()
    {
        if (m_ItemObjetivo)
        {
            float distToBoris = vector.Distance(m_ItemObjetivo.GetPosition(), m_Config.PosicaoEntrega.ToVector());
            if (distToBoris < 3.0)
            {
                GetGame().ObjectDelete(m_ItemObjetivo); 
                array<Man> players = new array<Man>;
                GetGame().GetPlayers(players);
                foreach (Man p : players)
                {
                    if (vector.Distance(p.GetPosition(), m_Config.PosicaoEntrega.ToVector()) < 5.0)
                    {
                        FinalizeMission(PlayerBase.Cast(p));
                        break;
                    }
                }
            }
        }
    }

    // 3. FINALIZAÇÃO DELEGADA AO TREASURY
    void FinalizeMission(PlayerBase p)
    {
        if (!p) return;

        // Notificação Global de Conclusão
        PNH_BroadcastManager.GetInstance().BroadcastGlobal(m_LoreEtapas.FaseB_Concluiu);
        
        // DELEGAÇÃO: O Treasury processa o XP e gera o barril físico com o loot sorteado do Tier 2
        PNH_TreasuryManager.ProcessMissionReward(
            p.GetIdentity().GetPlainId(), 
            p.GetIdentity().GetName(), 
            m_MissionTier, 
            m_Config.PosicaoBarrilEntrega.ToVector(), 
            m_Config.OrientacaoBarrilEntrega.ToVector()
        );
        
        PNH_MissionManager.GetInstance().EndMission();
    }

    override void CleanUp()
    {
        if (m_ItemObjetivo && !m_FaseEntregaAtiva) GetGame().ObjectDelete(m_ItemObjetivo); 
        foreach (Object obj : m_CenarioObjetos) if (obj) GetGame().ObjectDelete(obj);
        super.CleanUp();
    }
}