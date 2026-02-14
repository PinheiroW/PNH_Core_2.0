modded class MissionServer
{
    // Jogador Conectou
    override void InvokeOnConnect(PlayerBase player, PlayerIdentity identity)
    {
        super.InvokeOnConnect(player, identity);
        
        if (identity)
        {
            string nome = identity.GetName();
            string id = identity.GetPlainId();
            string msg = "Jogador **" + nome + "** conectou ao servidor. (ID: " + id + ")";
            
            // Envia para o canal de LOGIN (Cor Verde Claro)
            PNH_Discord.Send("Conexão", msg, 65280, PNH_CoreConfig.GetLoginURL());
        }
    }

    // Jogador Desconectou (CORREÇÃO DO ERRO AQUI)
    override void PlayerDisconnected(PlayerBase player, PlayerIdentity identity, string uid)
    {
        super.PlayerDisconnected(player, identity, uid);
        
        string nome = "Desconhecido";
        
        // BLINDAGEM CONTRA CRASH:
        // Verifica se 'identity' existe antes de tentar pegar o nome.
        if (identity) 
        {
            nome = identity.GetName();
        }
        else if (player && player.GetIdentity()) 
        {
            nome = player.GetIdentity().GetName();
        }
        
        string msg = "Jogador **" + nome + "** saiu do servidor.";
        
        // Envia para o canal de LOGIN (Cor Laranja)
        PNH_Discord.Send("Desconexão", msg, 16753920, PNH_CoreConfig.GetLoginURL());
    }
}