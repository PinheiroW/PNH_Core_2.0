/// --- Documentação PNH_Core: PNH_BroadcastManager.c ---
/// Versão do Sistema: 1.0.0 (Ref: PNH_Consts)
/// Função do arquivo: Centralizar e gerir o envio de notificações globais e individuais, utilizando o sistema nativo de pop-ups do DayZ (com ícones customizáveis), mensagens de chat e anúncios táticos de rádio para o início e fim de missões.
/// Comunicação com outros arquivos: É invocado pelo PNH_MissionManager.c para anunciar a disponibilidade de missões, pelo PNH_ContractBroker.c para confirmar assinaturas de contratos e pelas missões específicas (como Horde.c) para entregar briefings de lore via rádio.
/// Motivo da existência do arquivo no sistema: Desacoplar a lógica de interface (UI/Notificações) da lógica de jogo, permitindo que as mensagens sejam enviadas de forma assíncrona (com atrasos controlados para simular rádio) sem bloquear a execução de outros sistemas.
/// Dependências internas: PNH_Utils.c (para mensagens de chat), PNH_Discord.c (para espelhamento de anúncios no Discord) e PNH_CoreConfig.c (para obtenção de URLs de webhooks).
/// Última atualização: 2026-02-18
/// IMPORTANTE: Ao alterar este arquivo, documente no CHANGELOG_PNH.md

class PNH_BroadcastManager
{
    static ref PNH_BroadcastManager m_Instance;

    void PNH_BroadcastManager() {}

    static PNH_BroadcastManager GetInstance()
    {
        if (!m_Instance) m_Instance = new PNH_BroadcastManager();
        return m_Instance;
    }

    // --- CANAIS DE SAÍDA BÁSICOS ---
    void BroadcastGlobal(string message) { PNH_Utils.SendMessageToAll(message); }
    void SendToPlayer(PlayerBase player, string message) { if (player) PNH_Utils.SendMessage(player, message); }

    // =======================================================
    // --- NOTIFICAÇÕES NATIVAS DO DAYZ (COM ÍCONES) ---
    // =======================================================
    
    // NOVO: Adicionei o parâmetro "icon" para podermos escolher a imagem!
    void BroadcastNotification(string title, string text, float time = 5.0, string icon = "Notifications/gui/data/info.edds")
    {
        array<Man> players = new array<Man>;
        GetGame().GetPlayers(players);
        
        for (int i = 0; i < players.Count(); i++)
        {
            PlayerBase p = PlayerBase.Cast(players.Get(i));
            if (p && p.GetIdentity())
            {
                // Agora o último parâmetro recebe a variável 'icon'
                NotificationSystem.SendNotificationToPlayerIdentityExtended(p.GetIdentity(), time, title, text, icon);
            }
        }
    }

    void SendNotificationToPlayer(PlayerBase player, string title, string text, float time = 5.0, string icon = "Notifications/gui/data/info.edds")
    {
        if (player && player.GetIdentity())
        {
            // Agora o último parâmetro recebe a variável 'icon'
            NotificationSystem.SendNotificationToPlayerIdentityExtended(player.GetIdentity(), time, title, text, icon);
        }
    }

    // --- ANÚNCIOS TÁTICOS ---
    void AnnounceMissionAvailable(string location)
    {
        string msg = "Nova operacao disponivel em " + location + ". Va ate ao local assinar o contrato!";
        
        // Usa o ícone de AVISO (Warning) para chamar a atenção
        BroadcastNotification("ALERTA PNH", msg, 7.0, "Notifications/gui/data/warning.edds"); 
        
        BroadcastGlobal("[ALERTA PNH] " + msg);        
        PNH_Logger.Log("Missões", "[PNH_CORE] Sorteio realizado: " + location);
    }

    void AnnounceMissionStarted(string missionType, string location, string coords, string ownerName, string ownerID)
    {
        string telaMsg = "Um mercenario fechou o contrato de " + location + "!";
        string discordMsg = "Jogador " + ownerName + " aceitou o contrato " + missionType + " em " + location + " [" + coords + "]. (ID: " + ownerID + ")";
        
        // Usa o ícone de INFORMAÇÃO padrão
        BroadcastNotification("RADIO PNH", telaMsg, 5.0); 
        
        BroadcastGlobal("[RADIO PNH] " + telaMsg);
        PNH_Discord.Send("SISTEMA DE MISSÕES PNH", discordMsg, 3066993, PNH_CoreConfig.GetMissionsURL());
    }

    void AnnounceMissionEnded(string ownerName, string ownerID)
    {
        string telaMsg = "O contrato foi concluido com sucesso por " + ownerName + "!";
        string discordMsg = "Jogador " + ownerName + " concluiu o contrato. (ID: " + ownerID + ")";
        
        // Usa o ícone de INFORMAÇÃO padrão
        BroadcastNotification("RADIO PNH", telaMsg, 5.0); 
        
        BroadcastGlobal("[RADIO PNH] " + telaMsg);
        PNH_Discord.Send("SISTEMA DE MISSÕES PNH", discordMsg, 3447003, PNH_CoreConfig.GetMissionsURL());
    }

    // --- SISTEMA DE LORE ---
    void DeliverMissionBriefing(string informant, array<string> messages)
    {
        if (!messages || messages.Count() == 0) return;
        for (int i = 0; i < messages.Count(); i++)
        {
            string msg = messages.Get(i);
            if (msg == "") continue;
            GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(this.DelayedNotification, (i * 5500), false, informant, msg);
        }
    }

    private void DelayedNotification(string title, string msg) 
    { 
        // Usa o ícone de INFORMAÇÃO padrão
        BroadcastNotification(title, msg, 5.0); 
        BroadcastGlobal("[" + title + "] " + msg); 
    }
}