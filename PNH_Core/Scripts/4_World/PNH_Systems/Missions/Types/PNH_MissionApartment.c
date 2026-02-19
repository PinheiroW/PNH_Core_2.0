/// --- Documentação PNH_Core: PNH_MissionApartment.c ---
/// Versão do Sistema: 2.1.0 (Gatilhos de Distância 8 Fases e Injeção de Loot no Barril)
/// Função: Gerir a narrativa completa de infiltração e extração com rádio tático baseado na posição do jogador.

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

    // 1. MONTAGEM DA MISSÃO (Apenas após aceite no Tablet)
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
            // Tranca o prédio
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

    // 2. VERIFICAÇÕES DE DISTÂNCIA E ESTADO (Rádio Realista)
    override void MissionChecks()
    {
        super.MissionChecks();
        
        PlayerBase owner = PlayerBase.Cast(GetGame().GetPlayerByPlainId(m_MissionOwnerID));
        if (!owner) return;

        float dist;

        if (!m_FaseEntregaAtiva)
        {
            // --- LÓGICA FASE A (INFILTRAÇÃO) ---
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
            // --- LÓGICA FASE B (EXTRAÇÃO) ---
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

    // 3. FINALIZAÇÃO E INJEÇÃO DE LOOT NO BARRIL
    void FinalizeMission(PlayerBase p)
    {
        PNH_BroadcastManager.GetInstance().BroadcastGlobal(m_LoreEtapas.FaseB_Concluiu);
        
        EntityAI container = EntityAI.Cast(GetGame().CreateObject(m_Config.RecompensasHorda.Container, m_Config.PosicaoBarrilEntrega.ToVector()));
        if (container) 
        {
            container.SetOrientation(m_Config.OrientacaoBarrilEntrega.ToVector());
            m_CenarioObjetos.Insert(container); 

            // --- INJEÇÃO DE LOOT (Sorteia Loadout do JSON) ---
            if (m_Config.RecompensasHorda.Loadouts.Count() > 0)
            {
                PNH_RecompensaLoadout loadout = m_Config.RecompensasHorda.Loadouts.GetRandomElement();
                foreach (string itemClass : loadout.Itens)
                {
                    container.GetInventory().CreateInInventory(itemClass);
                }
            }
        }
        
        PNH_MissionManager.GetInstance().EndMission();
    }

    override void CleanUp()
    {
        if (m_ItemObjetivo && !m_FaseEntregaAtiva) GetGame().ObjectDelete(m_ItemObjetivo); 
        foreach (Object obj : m_CenarioObjetos) if (obj) GetGame().ObjectDelete(obj);
        super.CleanUp();
    }
}