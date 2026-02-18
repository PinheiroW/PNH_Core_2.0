/// --- Documentação PNH_Core: PNH_ConnectionLogger.c ---
/// Versão do Sistema: 1.0.0 (Ref: PNH_Consts)
//[cite_start]/// Função do arquivo: Monitorizar as entradas e saídas de jogadores no servidor, extraindo dados como Nome e SteamID para registo[cite: 31].
//[cite_start]/// Comunicação com outros arquivos: Utiliza o PNH_Discord.c para enviar alertas de login e logout em tempo real para o canal de logs administrativos[cite: 31].
/// Motivo da existência no sistema: Facilitar a administração do servidor, permitindo que os donos saibam quem está a entrar ou sair sem precisarem de consultar os arquivos RPT nativos.
//[cite_start]/// Dependências internas: PNH_Discord.c e PNH_CoreConfig.c (para obter o LoginWebhookURL)[cite: 31].
/// Última atualização: 2026-02-18
/// IMPORTANTE: Ao alterar este arquivo, documente no CHANGELOG_PNH.md

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