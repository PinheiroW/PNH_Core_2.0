/// --- Documentação PNH_Core: PNH_ChatHook.c ---
/// Versão do Sistema: 1.0.0 (Ref: PNH_Consts)
///[cite_start]/// Função do arquivo: Intercetar todos os eventos de chat do servidor para detetar comandos iniciados por "!" ou "/" e reencaminhá-los para processamento, além de espelhar o chat global no Discord[cite: 31].
///[cite_start]/// Comunicação com outros arquivos: Envia o texto dos comandos para o PNH_ChatManager.c e utiliza o PNH_Discord.c para enviar mensagens de chat global para o webhook configurado[cite: 31].
/// Motivo da existência no sistema: Servir como o "escuta" oficial do servidor, permitindo que o sistema PNH reaja a inputs de texto dos jogadores de forma dinâmica.
///[cite_start]/// Dependências internas: PNH_ChatManager.c (processamento), PNH_Discord.c (saída externa), PNH_CoreConfig.c (URLs de webhooks) e PNH_Utils.c[cite: 31].
/// Última atualização: 2026-02-18
/// IMPORTANTE: Ao alterar este arquivo, documente no CHANGELOG_PNH.md

modded class MissionServer
{
    override void OnEvent(EventType eventTypeId, Param params)
    {
        super.OnEvent(eventTypeId, params);

        if (eventTypeId == ChatMessageEventTypeID)
        {
            ChatMessageEventParams chatParams = ChatMessageEventParams.Cast(params);
            
            if (chatParams)
            {
                string senderName = chatParams.param2;
                string text = chatParams.param3;
                
                if (senderName == "" || text == "") return;

                PlayerBase player = GetPlayerByName(senderName);

                // --- 1. NOVA ROTA: Envia para o Gerenciador Universal ---
                // Se o comando for reconhecido (!reload_mission, etc), ele retorna 'true' e encerra a leitura.
                if (player && PNH_ChatManager.HandleCommand(player, text))
                {
                    return;
                }

                // --- 2. LOG DE CHAT GLOBAL (Ignora se começar com ! ou /) ---
                string prefixo = text.Substring(0, 1);
                if (prefixo != "!" && prefixo != "/")
                {
                    string msgChat = "**" + senderName + "**: " + text;
                    PNH_Discord.Send("Global Chat", msgChat, 3447003, PNH_CoreConfig.GetChatURL());
                    return; 
                }

                // --- 3. COMANDOS LEGADOS DE ADMIN ---
                string textLower = text;
                textLower.ToLower();

                // COMANDO: !coretest
                if (textLower == "!coretest")
                {
                    if (player)
                    {
                        string sID = PNH_Utils.GetSteamID(player);
                        if (PNH_CoreConfig.IsSuperAdmin(sID))
                        {
                            PNH_Utils.SendMessage(player, "Teste Core 2.2 Iniciado...");
                            if (PNH_CoreConfig.GetSystemURL() != "") PNH_Discord.Send("Teste", "Canal SISTEMA OK", 65280, PNH_CoreConfig.GetSystemURL());
                            if (PNH_CoreConfig.GetAdminURL() != "") PNH_Discord.Send("Teste", "Canal ADMIN OK", 16711680, PNH_CoreConfig.GetAdminURL());
                            if (PNH_CoreConfig.GetChatURL() != "") PNH_Discord.Send("Teste", "Canal CHAT OK", 3447003, PNH_CoreConfig.GetChatURL());
                            if (PNH_CoreConfig.GetLoginURL() != "") PNH_Discord.Send("Teste", "Canal LOGIN OK", 16753920, PNH_CoreConfig.GetLoginURL());
                            PNH_Utils.SendMessage(player, "Testes enviados para o Discord!");
                        }
                    }
                    return;
                }

                // COMANDO: !pnhreload (Configurações do Core)
                if (textLower == "!pnhreload")
                {
                    if (player)
                    {
                        string sIDReload = PNH_Utils.GetSteamID(player);
                        if (PNH_CoreConfig.IsSuperAdmin(sIDReload))
                        {
                            PNH_CoreConfig.Reload(); 
                            PNH_Utils.SendMessage(player, "Configuracoes Recarregadas com Sucesso!");
                            PNH_Discord.Send("System", "Configuracoes recarregadas manualmente por Admin via !pnhreload", 65280, PNH_CoreConfig.GetSystemURL());
                        }
                    }
                    return;
                }
            }
        }
    }

    // Função auxiliar local
    PlayerBase GetPlayerByName(string name)
    {
        array<Man> players = new array<Man>;
        GetGame().GetPlayers(players);
        for (int i = 0; i < players.Count(); i++)
        {
            PlayerBase pb = PlayerBase.Cast(players.Get(i));
            if (pb && pb.GetIdentity() && pb.GetIdentity().GetName() == name) return pb;
        }
        return null;
    }
}