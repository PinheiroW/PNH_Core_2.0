/// --- Documentação PNH_Core: PNH_MissionApartment.c ---
/// Versão do Sistema: 1.3.1 (Correção de Variável Inexistente)
/// Função: Gerir a narrativa de infiltração, rastreio dinâmico do item e entrega final ao contato Boris.

class PNH_MissionApartment : PNH_MissionBase
{
    protected ref array<Object> m_CenarioObjetos = new array<Object>;
    protected EntityAI m_CorpoTraidor;
    protected EntityAI m_NPCEntrega;
    protected EntityAI m_ItemObjetivo; // Rastreia fisicamente o Livro da Missão
    protected bool m_FaseEntregaAtiva = false;

    // 1. MONTAGEM DA MISSÃO (Barricadas, Corpo e Inimigos)
    override bool DeployMission()
    {
        super.DeployMission(); 

        // --- SPAWN DO CENÁRIO (BARRICADAS) ---
        foreach (auto barricada : m_Config.Cenario.Barricadas)
        {
            vector posMundo = m_MissionPosition + barricada.PosicaoLocal.ToVector();
            Object obj = GetGame().CreateObjectEx(barricada.Classe, posMundo, ECE_CREATEPHYSICS);
            if (obj) 
            {
                obj.SetOrientation(barricada.OrientacaoLocal.ToVector());
                m_CenarioObjetos.Insert(obj);
            }
        }

        // --- SPAWN DO TRAIDOR E DO ITEM ---
        vector posCorpo = m_MissionPosition + m_Config.PosicaoCorpoLocal.ToVector();
        m_CorpoTraidor = EntityAI.Cast(GetGame().CreateObject(m_Config.ClasseCorpo, posCorpo));
        if (m_CorpoTraidor)
        {
            m_CorpoTraidor.SetOrientation(m_Config.OrientacaoCorpoLocal.ToVector());
            m_CenarioObjetos.Insert(m_CorpoTraidor);
            
            // Cria e GRAVA a referência exata do item da missão no mundo
            m_ItemObjetivo = m_CorpoTraidor.GetInventory().CreateInInventory(m_Config.ItemMissao);
        }

        // --- SPAWN DO ASSASSINO (ZUMBI ALVO) ---
        vector posZmbAlvo = m_MissionPosition + m_Config.PosicaoZumbiAssassinoLocal.ToVector();
        EntityAI assassin = EntityAI.Cast(GetGame().CreateObject(m_Config.ClasseZumbiAssassino, posZmbAlvo));
        if (assassin)
        {
            foreach (string roupaNPC : m_Config.RoupasNPC) assassin.GetInventory().CreateInInventory(roupaNPC);
            foreach (string itemNPC : m_Config.InventarioNPC) assassin.GetInventory().CreateInInventory(itemNPC);
        }

        // --- SPAWN DA HORDA INTERNA ---
        foreach (string posZmb : m_Config.SpawnsZumbisInternos)
        {
            vector posZmbMundo = m_MissionPosition + posZmb.ToVector();
            GetGame().CreateObject(m_Config.Dificuldade.ClassesZumbis.GetRandomElement(), posZmbMundo);
        }
        
        PNH_Logger.Log("Mission", "[PNH] Infiltração Montada. Itens gerados: " + m_CenarioObjetos.Count());
        
        return true; 
    }

    override void MissionChecks()
    {
        super.MissionChecks();

        if (!m_FaseEntregaAtiva) CheckItemDiscovery();
        else CheckDeliveryPoint();
    }

    // Verifica se o livro foi roubado/retirado do corpo
    void CheckItemDiscovery()
    {
        // Se o item existe, mas a sua "raiz" (quem o está a carregar) já não é o corpo do traidor
        if (m_ItemObjetivo && m_CorpoTraidor)
        {
            if (m_ItemObjetivo.GetHierarchyRoot() != m_CorpoTraidor)
            {
                // Apenas ativamos a Fase de Entrega através deste booleano
                m_FaseEntregaAtiva = true;
                
                PNH_BroadcastManager.GetInstance().SendGlobalMessage(m_Config.Lore.MensagemFaseB + m_Config.CidadeEntrega);
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

    // Rastreia o livro em si até ao ponto de entrega, ignorando em qual mochila está
    void CheckDeliveryPoint()
    {
        if (m_ItemObjetivo)
        {
            // Se o livro físico (e quem o carrega) estiver a menos de 3 metros do ponto do Boris
            float distToBoris = vector.Distance(m_ItemObjetivo.GetPosition(), m_Config.PosicaoEntrega.ToVector());
            
            if (distToBoris < 3.0)
            {
                // Deleta o livro para simular a entrega
                GetGame().ObjectDelete(m_ItemObjetivo); 
                
                // Encontra qual jogador estava perto para lhe passar a glória e pagar
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
        PNH_BroadcastManager.GetInstance().SendGlobalMessage(m_Config.Lore.MensagemVitoria);
        
        EntityAI container = EntityAI.Cast(GetGame().CreateObject(m_Config.RecompensasHorda.Container, m_Config.PosicaoBarrilEntrega.ToVector()));
        if (container) container.SetOrientation(m_Config.OrientacaoBarrilEntrega.ToVector());
        
        PNH_MissionManager.GetInstance().EndMission();
    }

    override void CleanUp()
    {
        if (m_ItemObjetivo && !m_FaseEntregaAtiva) GetGame().ObjectDelete(m_ItemObjetivo); // Deleta o livro se a missão falhar antes de ser pego
        foreach (Object obj : m_CenarioObjetos) if (obj) GetGame().ObjectDelete(obj);
        super.CleanUp();
    }
}