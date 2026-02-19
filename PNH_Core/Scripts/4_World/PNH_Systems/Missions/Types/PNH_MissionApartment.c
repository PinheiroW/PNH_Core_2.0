/// --- Documentação PNH_Core: PNH_MissionApartment.c ---
/// Versão do Sistema: 1.1.0 (Correção de Vetores e Equipamento Dinâmico)
/// Função: Gerir a narrativa de infiltração, recuperação de item e entrega final ao contato Boris.

class PNH_MissionApartment : PNH_MissionBase
{
    protected ref array<Object> m_CenarioObjetos = new array<Object>;
    protected EntityAI m_CorpoTraidor;
    protected EntityAI m_NPCEntrega;
    protected bool m_FaseEntregaAtiva = false;

    // 1. MONTAGEM DA MISSÃO (Barricadas, Corpo e Inimigos)
    override void Deploy()
    {
        super.Deploy(); // Ativa os gatilhos base de distância

        // --- SPAWN DO CENÁRIO (BARRICADAS) ---
        // As posições no JSON são locais, somamos à posição global da missão
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

        // --- SPAWN DO TRAIDOR (CORPO) ---
        vector posCorpo = m_MissionPosition + m_Config.PosicaoCorpoLocal.ToVector();
        m_CorpoTraidor = EntityAI.Cast(GetGame().CreateObject(m_Config.ClasseCorpo, posCorpo));
        if (m_CorpoTraidor)
        {
            m_CorpoTraidor.SetOrientation(m_Config.OrientacaoCorpoLocal.ToVector());
            // Injeta o item da missão (Livro) no inventário do corpo
            m_CorpoTraidor.GetInventory().CreateInInventory(m_Config.ItemMissao);
            m_CenarioObjetos.Insert(m_CorpoTraidor);
        }

        // --- SPAWN DO ASSASSINO (ZUMBI ALVO) ---
        vector posZmbAlvo = m_MissionPosition + m_Config.PosicaoZumbiAssassinoLocal.ToVector();
        EntityAI assassin = EntityAI.Cast(GetGame().CreateObject(m_Config.ClasseZumbiAssassino, posZmbAlvo));
        if (assassin)
        {
            // Equipa o Assassino com as roupas e itens do JSON
            foreach (string roupaNPC : m_Config.RoupasNPC) 
            {
                assassin.GetInventory().CreateInInventory(roupaNPC);
            }
            foreach (string itemNPC : m_Config.InventarioNPC) 
            {
                assassin.GetInventory().CreateInInventory(itemNPC);
            }
        }

        // --- SPAWN DA HORDA INTERNA ---
        foreach (string posZmb : m_Config.SpawnsZumbisInternos)
        {
            vector posZmbMundo = m_MissionPosition + posZmb.ToVector();
            GetGame().CreateObject(m_Config.Dificuldade.ClassesZumbis.GetRandomElement(), posZmbMundo);
        }
        
        PNH_Logger.Log("Mission", "[PNH] Infiltração Montada. Itens gerados: " + m_CenarioObjetos.Count());
    }

    // 2. VERIFICAÇÕES DE PROGRESSO (ESTADO DA MISSÃO)
    override void MissionChecks()
    {
        super.MissionChecks();

        if (!m_FaseEntregaAtiva)
        {
            CheckItemDiscovery();
        }
        else
        {
            CheckDeliveryPoint();
        }
    }

    // Verifica se o jogador já recuperou o item do corpo
    void CheckItemDiscovery()
    {
        if (m_CorpoTraidor && !m_CorpoTraidor.GetInventory().FindAttachmentByName(m_Config.ItemMissao))
        {
            m_FaseEntregaAtiva = true;
            m_MissionState = 2; // Estado de Entrega
            
            // Notifica os jogadores da Fase B
            PNH_BroadcastManager.GetInstance().SendGlobalMessage(m_Config.Lore.MensagemFaseB + m_Config.CidadeEntrega);
            
            // Materializa o Boris no ponto de extração
            SpawnNPCEntrega();
        }
    }

    void SpawnNPCEntrega()
    {
        m_NPCEntrega = EntityAI.Cast(GetGame().CreateObject(m_Config.ClasseNPCEntrega, m_Config.PosicaoEntrega.ToVector()));
        if (m_NPCEntrega)
        {
            // Veste o Boris conforme o contrato
            foreach (string roupa : m_Config.RoupasNPCEntrega)
            {
                m_NPCEntrega.GetInventory().CreateInInventory(roupa);
            }
            m_CenarioObjetos.Insert(m_NPCEntrega);
        }
    }

    // Verifica se o jogador com o item está perto do Boris para concluir
    void CheckDeliveryPoint()
    {
        array<Man> players = new array<Man>;
        GetGame().GetPlayers(players);

        foreach (Man p : players)
        {
            float dist = vector.Distance(p.GetPosition(), m_Config.PosicaoEntrega.ToVector());
            if (dist < 3.0)
            {
                InventoryItem item = InventoryItem.Cast(p.GetInventory().FindAttachmentByName(m_Config.ItemMissao));
                if (item)
                {
                    // Transação Concluída!
                    GetGame().ObjectDelete(item); // Boris recolhe o livro
                    FinalizeMission(PlayerBase.Cast(p));
                    break;
                }
            }
        }
    }

    void FinalizeMission(PlayerBase p)
    {
        PNH_BroadcastManager.GetInstance().SendGlobalMessage(m_Config.Lore.MensagemVitoria);
        
        // Gera o barril de pagamento
        EntityAI container = EntityAI.Cast(GetGame().CreateObject(m_Config.RecompensasHorda.Container, m_Config.PosicaoBarrilEntrega.ToVector()));
        if (container)
        {
            container.SetOrientation(m_Config.OrientacaoBarrilEntrega.ToVector());
            // Aqui podes injetar o loadout de armas se desejares
        }
        
        // Limpeza agendada via CallLater ou Cleanup imediato
        PNH_MissionManager.GetInstance().EndMission();
    }

    // 3. LIMPEZA TOTAL (Previne lag e objetos órfãos)
    override void CleanUp()
    {
        foreach (Object obj : m_CenarioObjetos)
        {
            if (obj) GetGame().ObjectDelete(obj);
        }
        super.CleanUp();
    }
}