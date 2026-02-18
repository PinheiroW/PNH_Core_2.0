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

    // --- BARRIL COM LOOT EXCLUSIVO E PORCENTAGEM ---
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

                // Se houver itens na lista, rola os dados (RNG) para cada um
                if (listaAtiva && listaAtiva.Count() > 0)
                {
                    int itensGerados = 0;
                    foreach (PNH_LootItem loot : listaAtiva)
                    {
                        int sorteio = Math.RandomIntInclusive(1, 100); // Rola de 1 a 100
                        
                        // Se o número sorteado for menor ou igual à chance, o item spawna
                        if (sorteio <= loot.Chance)
                        {
                            // Proteção: se a quantidade estiver a 0 no JSON, assume 1
                            int qtd = loot.Quantidade;
                            if (qtd <= 0) qtd = 1; 

                            for (int i = 0; i < qtd; i++)
                            {
                                container.GetInventory().CreateInInventory(loot.Item);
                                itensGerados++;
                            }
                        }
                    }
                    PNH_Logger.Log("Logistics", "[PNH_CORE] Recompensa gerada para Tier " + tierAtual + " com " + itensGerados + " itens.");
                }
                else
                {
                    // Proteção caso o JSON esteja vazio
                    container.GetInventory().CreateInInventory("BandageDressing");
                }
            }
        }
        return container;
    }
}