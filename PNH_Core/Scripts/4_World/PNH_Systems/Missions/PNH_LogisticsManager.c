class PNH_LogisticsManager
{
    static vector GetRandomSpawnPos(vector center, float radius)
    {
        float x = center[0] + Math.RandomFloat(-radius, radius);
        float z = center[2] + Math.RandomFloat(-radius, radius);
        float y = GetGame().SurfaceY(x, z);
        return Vector(x, y, z);
    }

    static Object SpawnNPC(string type, vector center, float radius)
    {
        vector spawnPos = GetRandomSpawnPos(center, radius);
        Object npc = GetGame().CreateObject(type, spawnPos, false, true, true);
        if (npc) { 
            npc.SetPosition(spawnPos); 
            return npc; 
        }
        return null;
    }

    // --- BARRIL COM LIMITE MÁXIMO (SORTEIO INTELIGENTE) ---
    static Object SpawnRewardContainer(string type, vector pos)
    {
        EntityAI container = EntityAI.Cast(GetGame().CreateObject(type, pos, false, true, true));
        if (container)
        {
            container.SetPosition(pos);
            PNH_MissionSettingsData settings = PNH_MissionSettings.GetData();
            PNH_MissionManager mngr = PNH_MissionManager.GetInstance();
            
            // Descobre o Tier da missão atual (Se falhar, assume Tier 1 por segurança)
            int tierAtual = 1; 
            if (mngr && mngr.m_ActiveMission) tierAtual = mngr.m_ActiveMission.m_MissionTier;

            if (settings)
            {
                array<ref PNH_LootItem> listaAtiva;

                // Seleciona a lista de loot correta baseada no Tier
                switch (tierAtual)
                {
                    case 1: listaAtiva = settings.Loot_Tier1; break;
                    case 2: listaAtiva = settings.Loot_Tier2; break;
                    case 3: listaAtiva = settings.Loot_Tier3; break;
                    case 4: listaAtiva = settings.Loot_Tier4; break;
                    default: listaAtiva = settings.Loot_Tier1; break;
                }

                if (listaAtiva && listaAtiva.Count() > 0)
                {
                    // Pega o limite do JSON
                    int maxItens = settings.ConfiguracoesGerais.MaxItensNoBarril;
                    if (maxItens <= 0) maxItens = 5; // Proteção caso o JSON não tenha a variável preenchida ou esteja a 0

                    int itensGerados = 0;
                    int tentativas = 0; // Evita loop infinito se os itens tiverem 0% de chance

                    // Fica a sortear itens até encher a quantidade estipulada no JSON
                    while (itensGerados < maxItens && tentativas < 200)
                    {
                        tentativas++;
                        
                        // 1. Escolhe um item da lista aleatoriamente
                        int index = Math.RandomInt(0, listaAtiva.Count());
                        PNH_LootItem loot = listaAtiva.Get(index);
                        
                        // 2. Rola a probabilidade desse item (ex: 15% para a SKS)
                        int sorteio = Math.RandomIntInclusive(1, 100);
                        
                        // 3. Se o número sorteado for menor ou igual à chance, o item spawna
                        if (sorteio <= loot.Chance)
                        {
                            int qtd = loot.Quantidade;
                            if (qtd <= 0) qtd = 1;

                            // Adiciona a quantidade ao barril, respeitando o limite máximo definido
                            for (int i = 0; i < qtd; i++)
                            {
                                if (itensGerados < maxItens) 
                                {
                                    container.GetInventory().CreateInInventory(loot.Item);
                                    itensGerados++;
                                }
                            }
                        }
                    }
                    PNH_Logger.Log("Logistics", "[PNH_CORE] Recompensa gerada para Tier " + tierAtual + ". Total: " + itensGerados + " itens colocados no barril.");
                }
                else
                {
                    // Proteção caso o JSON esteja vazio
                    container.GetInventory().CreateInInventory("BandageDressing");
                    PNH_Logger.Log("Logistics", "[PNH_CORE] Recompensa de emergencia gerada.");
                }
            }
        }
        return container;
    }
}