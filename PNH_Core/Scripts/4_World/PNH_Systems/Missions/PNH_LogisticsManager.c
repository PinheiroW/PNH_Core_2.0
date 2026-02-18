class PNH_LogisticsManager
{
    // A Horda continua a usar o SurfaceY para nascer na rua em redor do prédio
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
        if (npc) { npc.SetPosition(spawnPos); return npc; }
        return null;
    }

    // CORREÇÃO: O Barril agora respeita a coordenada 3D exata (X, Altura, Z)
    static Object SpawnRewardContainer(string type, vector pos)
    {
        // Removemos o groundPos[1] = SurfaceY. Agora usamos o 'pos' puro!
        EntityAI container = EntityAI.Cast(GetGame().CreateObject(type, pos, false, true, true));
        if (container)
        {
            // Coloca o barril na posição e altura perfeitas definidas por ti
            container.SetPosition(pos);
            
            // --- GERAÇÃO DO LOOT INTERNO ---
            container.GetInventory().CreateInInventory("M4A1");
            container.GetInventory().CreateInInventory("Mag_STANAG_30Rnd");
            container.GetInventory().CreateInInventory("Mag_STANAG_30Rnd");
            container.GetInventory().CreateInInventory("AmmoBox_556x45_20Rnd");
            container.GetInventory().CreateInInventory("BandageDressing");
            
            PNH_Logger.Log("Logistics", "[PNH_CORE] Recompensa materializada na altura exata: " + pos[1].ToString());
        }
        return container;
    }
}