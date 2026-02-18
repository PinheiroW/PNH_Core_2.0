class PNH_LogisticsManager
{
    // Calcula uma posição aleatória segura ao redor de um ponto central
    static vector GetRandomSpawnPos(vector center, float radius)
    {
        float x = center[0] + Math.RandomFloat(-radius, radius);
        float z = center[2] + Math.RandomFloat(-radius, radius);
        float y = GetGame().SurfaceY(x, z);
        
        return Vector(x, y, z);
    }

    // Agente especializado em criar NPCs de forma segura
    static Object SpawnNPC(string type, vector center, float radius)
    {
        vector spawnPos = GetRandomSpawnPos(center, radius);
        
        // PNH 2.0: Tenta criar o objeto com colisão verificada
        Object npc = GetGame().CreateObject(type, spawnPos, false, true, true);
        if (npc)
        {
            npc.SetPosition(spawnPos);
            return npc;
        }
        return null;
    }
}