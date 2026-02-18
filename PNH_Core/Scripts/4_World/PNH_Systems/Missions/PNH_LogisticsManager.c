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
        if (npc)
        {
            npc.SetPosition(spawnPos);
            return npc;
        }
        return null;
    }

    // Agente especializado em criar contentores de loot (Prémio)
    static Object SpawnRewardContainer(string type, vector pos)
    {
        vector groundPos = pos;
        groundPos[1] = GetGame().SurfaceY(pos[0], pos[2]);
        
        Object container = GetGame().CreateObject(type, groundPos, false, true, true);
        if (container)
        {
            container.SetPosition(groundPos);
            PNH_Logger.Log("Logistics", "[PNH_CORE] Recompensa materializada: " + type);
        }
        return container;
    }
}