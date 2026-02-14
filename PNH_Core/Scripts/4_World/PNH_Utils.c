class PNH_Utils
{
    // --- NOVA FUNÇÃO: Enviar para TODOS ---
    static void SendMessageToAll(string message)
    {
        array<Man> players = new array<Man>;
        GetGame().GetPlayers(players);

        foreach(Man p : players)
        {
            PlayerBase pb = PlayerBase.Cast(p);
            if(pb)
            {
                SendMessage(pb, message);
            }
        }
    }

    // Enviar Individual
    static void SendMessage(PlayerBase player, string message)
    {
        if (player && player.GetIdentity())
        {
            Param1<string> m = new Param1<string>("[PNH] " + message);
            GetGame().RPCSingleParam(player, ERPCs.RPC_USER_ACTION_MESSAGE, m, true, player.GetIdentity());
        }
    }
    
    static string GetSteamID(PlayerBase player)
    {
        if (player && player.GetIdentity()) return player.GetIdentity().GetPlainId();
        return "";
    }
    
    static string GetName(PlayerBase player)
    {
        if (player && player.GetIdentity()) return player.GetIdentity().GetName();
        return "";
    }

    static PlayerBase GetPlayerByName(string name)
    {
        if (name == "") return null;
        
        name.ToLower();
        array<Man> players = new array<Man>;
        GetGame().GetPlayers(players);
        
        for (int i = 0; i < players.Count(); i++)
        {
            PlayerBase pb = PlayerBase.Cast(players.Get(i));
            if (pb && pb.GetIdentity())
            {
                string pName = pb.GetIdentity().GetName();
                pName.ToLower();
                if (pName.Contains(name)) return pb;
            }
        }
        return null;
    }
}