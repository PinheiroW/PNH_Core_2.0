/*
    MOD: PNH_Core
    SCRIPT: PNH_ChatManager.c (4_World)
    DESC: Processa comandos de chat iniciados por "/"
*/

class PNH_ChatManager
{
    // Verifica se a mensagem é um comando (começa com /)
    static bool IsCommand(string text)
    {
        return (text.IndexOf("/") == 0); // Retorna TRUE se o primeiro caractere for /
    }

    // A Mágica acontece aqui
    static void HandleCommand(PlayerBase player, string text)
    {
        if (!player || !player.GetIdentity()) return;

        // 1. Remove o "/" e transforma em minúsculo para facilitar (Ex: "/BemVindo" vira "bemvindo")
        string cleanText = text.Substring(1, text.Length() - 1);
        cleanText.ToLower();
        
        // 2. Separa o comando dos argumentos (caso tenhamos algo como "/banir Fulano")
        TStringArray args = new TStringArray;
        cleanText.Split(" ", args);
        string command = args[0]; // O primeiro item é o comando principal

        // 3. Loga no console para sabermos que funcionou
        PNH_Logger.Log("Chat", "Jogador " + player.GetIdentity().GetName() + " usou o comando: " + command);

        // --- AQUI ENTRA O ROTEADOR DE COMANDOS ---
        // No futuro, os outros mods vão se registrar aqui. 
        // Por enquanto, vamos fazer um teste manual direto ("Hardcoded").
        
        if (command == "pnh")
        {
            // Teste de conexão total: Chat -> Core -> Discord
            PNH_Discord.Send("Comando /pnh", "O jogador **" + player.GetIdentity().GetName() + "** testou o chat com sucesso!", 16753920);
            
            // Avisa o jogador (Mensagem no chat local apenas para ele)
            NotificationSystem.SendNotificationToPlayerExtended(player, 5, "PNH Core", "Comando recebido com sucesso!", "set:dayz_gui image:tutorials");
        }
    }
}