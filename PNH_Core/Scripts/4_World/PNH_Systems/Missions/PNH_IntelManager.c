class PNH_IntelManager
{
    // Verifica se o dono da missão está dentro de um raio específico da zona
    static bool IsOwnerInObjectiveZone(string ownerID, vector zonePos, float radius)
    {
        PlayerBase player = PNH_Utils.GetPlayerBySteamID(ownerID);
        if (!player) return false;

        float distance = vector.Distance(player.GetPosition(), zonePos);
        return (distance <= radius);
    }

    // Função para detetar qualquer jogador próximo (útil para emboscadas ou eventos)
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