/// --- Documentação PNH_Core: PNH_MissionApartment.c ---
/// Versão do Sistema: 2.0.0 (Engenharia Reversa: ModelToWorld, Portas, 3 Diálogos de Rádio e Extração do Boris)
/// Função: A missão só nasce quando é aceita. O prédio tranca, o cenário é gerado perfeitamente e o jogador deve extrair o livro para Green Mountain.

class PNH_MissionApartment : PNH_MissionBase
{
    protected ref array<Object> m_CenarioObjetos = new array<Object>;
    protected Object m_MissionBuilding; // Variável que detém o prédio original no mapa
    protected EntityAI m_CorpoTraidor;
    protected EntityAI m_NPCEntrega;
    protected EntityAI m_ItemObjetivo; 
    protected bool m_FaseEntregaAtiva = false;

    // A missão AGORA SÓ NASCE quando o jogador aceita o contrato
    override bool DeployMission()
    {
        super.DeployMission(); 

        // 1. PROCURA PELO PRÉDIO EXATO NO MAPA (Engenharia do Mod Original)
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
            // 2. FECHA TODAS AS PORTAS E ATUALIZA NAVEGAÇÃO DOS ZUMBIS
            Building Tenement = Building.Cast(m_MissionBuilding);
            for (int k = 0; k < 32; k++)
            {
                if (Tenement.IsDoorOpen(k)) Tenement.CloseDoor(k);
            }
            GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(GetGame().UpdatePathgraphRegionByObject, 1000, false, Tenement);

            // 3. SPAWN DO CENÁRIO COM MODELTOWORLD (As barricadas agora ficam perfeitas nas janelas)
            foreach (auto barricada : m_Config.Cenario.Barricadas)
            {
                vector bPos = m_MissionBuilding.ModelToWorld(barricada.PosicaoLocal.ToVector());
                vector bOri = barricada.OrientacaoLocal.ToVector();
                vector bDir = m_MissionBuilding.GetDirection();
                
                Object obj = GetGame().CreateObjectEx(barricada.Classe, bPos, ECE_CREATEPHYSICS);
                if (obj) 
                {
                    obj.SetPosition(bPos);
                    obj.SetDirection(bDir);
                    obj.SetOrientation(obj.GetOrientation() + bOri);
                    m_CenarioObjetos.Insert(obj);
                }
            }

            // 4. SPAWN DO TRAIDOR E DO ITEM
            vector posCorpo = m_MissionBuilding.ModelToWorld(m_Config.PosicaoCorpoLocal.ToVector());
            m_CorpoTraidor = EntityAI.Cast(GetGame().CreateObject(m_Config.ClasseCorpo, posCorpo));
            if (m_CorpoTraidor)
            {
                m_CorpoTraidor.SetDirection(m_MissionBuilding.GetDirection());
                m_CorpoTraidor.SetOrientation(m_CorpoTraidor.GetOrientation() + m_Config.OrientacaoCorpoLocal.ToVector());
                
                // O Traidor recebe as roupas para ter onde guardar o livro
                foreach (string roupaNPC : m_Config.RoupasNPC) m_CorpoTraidor.GetInventory().CreateInInventory(roupaNPC);
                foreach (string itemNPC : m_Config.InventarioNPC) m_CorpoTraidor.GetInventory().CreateInInventory(itemNPC);

                m_CenarioObjetos.Insert(m_CorpoTraidor);
                
                // O servidor guarda a identidade do livro para não dar lag a procurar nas mochilas dos jogadores
                m_ItemObjetivo = m_CorpoTraidor.GetInventory().CreateInInventory(m_Config.ItemMissao);
            }

            // 5. SPAWN DA HORDA INTERNA COM MODELTOWORLD
            foreach (string posZmb : m_Config.SpawnsZumbisInternos)
            {
                vector posZmbMundo = m_MissionBuilding.ModelToWorld(posZmb.ToVector());
                GetGame().CreateObject(m_Config.Dificuldade.ClassesZumbis.GetRandomElement(), posZmbMundo);
            }

            // 6. DISPARA O RÁDIO DE INFILTRAÇÃO (3 MENSAGENS COMPASSADAS)
            PNH_BroadcastManager.GetInstance().DeliverMissionBriefing(m_Config.Lore.Informante, m_Config.Lore.MensagensFaseA);
            
            PNH_Logger.Log("Mission", "[PNH] Infiltração Montada. Itens gerados: " + m_CenarioObjetos.Count());
            return true; 
        }
        else 
        {
            PNH_Logger.Log("Mission", "[PNH] ERRO CRÍTICO: Prédio Land_Tenement_Small não encontrado nas coordenadas!");
            return false;
        }
    }

    override void MissionChecks()
    {
        super.MissionChecks();
        if (!m_FaseEntregaAtiva) CheckItemDiscovery();
        else CheckDeliveryPoint();
    }

    // Verifica se o livro foi retirado do corpo
    void CheckItemDiscovery()
    {
        if (m_ItemObjetivo && m_CorpoTraidor)
        {
            if (m_ItemObjetivo.GetHierarchyRoot() != m_CorpoTraidor)
            {
                m_FaseEntregaAtiva = true;
                
                // DISPARA O RÁDIO DE EXTRAÇÃO (3 MENSAGENS COMPASSADAS)
                PNH_BroadcastManager.GetInstance().DeliverMissionBriefing(m_Config.Lore.Informante, m_Config.Lore.MensagensFaseB);
                
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

    // Verifica se o jogador com o livro chegou ao pé do Boris
    void CheckDeliveryPoint()
    {
        if (m_ItemObjetivo)
        {
            float distToBoris = vector.Distance(m_ItemObjetivo.GetPosition(), m_Config.PosicaoEntrega.ToVector());
            if (distToBoris < 3.0)
            {
                GetGame().ObjectDelete(m_ItemObjetivo); // Entrega o livro ao Boris
                array<Man> players = new array<Man>;
                GetGame().GetPlayers(players);
                PlayerBase winner = null;
                
                foreach (Man p : players)
                {
                    if (vector.Distance(p.GetPosition(), m_Config.PosicaoEntrega.ToVector()) < 5.0)
                    {
                        winner = PlayerBase.Cast(p);
                        break;
                    }
                }
                FinalizeMission(winner);
            }
        }
    }

    void FinalizeMission(PlayerBase p)
    {
        PNH_BroadcastManager.GetInstance().BroadcastGlobal(m_Config.Lore.MensagemVitoria);
        
        EntityAI container = EntityAI.Cast(GetGame().CreateObject(m_Config.RecompensasHorda.Container, m_Config.PosicaoBarrilEntrega.ToVector()));
        if (container) 
        {
            container.SetOrientation(m_Config.OrientacaoBarrilEntrega.ToVector());
            m_CenarioObjetos.Insert(container); 
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