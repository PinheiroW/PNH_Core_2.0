class PNH_IntelManager
{
    static bool IsPlayerInZone(PlayerBase player, vector zonePos, float radius)
    {
        if (!player) return false;
        float distance = vector.Distance(player.GetPosition(), zonePos);
        return (distance <= radius);
    }

    // Verifica se o jogador está a menos de 3 metros de um Oficial PNH
    static bool IsPlayerNearQuestGiver(PlayerBase player, float maxDistance = 3.0)
    {
        if (!player) return false;
        
        PNH_MissionSettingsData settings = PNH_MissionSettings.GetData();
        if (settings && settings.NPCsQuestGivers)
        {
            vector playerPos = player.GetPosition();
            for (int i = 0; i < settings.NPCsQuestGivers.Count(); i++)
            {
                // CORREÇÃO: Atualizado para o novo nome da classe (PNH_NPCQuestGiver)
                PNH_NPCQuestGiver npc = settings.NPCsQuestGivers.Get(i);
                
                // CORREÇÃO: Transformar o texto (string) num vector antes de calcular
                if (vector.Distance(playerPos, npc.Posicao.ToVector()) <= maxDistance)
                {
                    return true;
                }
            }
        }
        return false;
    }

    static array<PlayerBase> GetPlayersInRadius(vector center, float radius)
    {
        array<PlayerBase> playersNear = new array<PlayerBase>;
        array<Man> allPlayers = new array<Man>;
        GetGame().GetPlayers(allPlayers);

        foreach (Man man : allPlayers)
        {
            PlayerBase p = PlayerBase.Cast(man);
            if (p && vector.Distance(p.GetPosition(), center) <= radius)
            {
                playersNear.Insert(p);
            }
        }
        return playersNear;
    }
}