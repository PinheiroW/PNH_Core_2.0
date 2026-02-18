class PNH_BroadcastManager
{
    static ref PNH_BroadcastManager m_Instance;

    void PNH_BroadcastManager() {}

    static PNH_BroadcastManager GetInstance()
    {
        if (!m_Instance) m_Instance = new PNH_BroadcastManager();
        return m_Instance;
    }

    // --- CANAIS DE SAÍDA PUROS ---
    void BroadcastGlobal(string message) { PNH_Utils.SendMessageToAll(message); }
    void SendToPlayer(PlayerBase player, string message) { if (player) PNH_Utils.SendMessage(player, message); }

    // --- ANÚNCIOS TÁTICOS (Rádio + Discord + Audit) ---
    void AnnounceMissionAvailable(string location)
    {
        string msg = "[ALERTA PNH] Nova operacao disponivel em " + location + ". Va ate ao local para assinar o contrato!";
        BroadcastGlobal(msg);
        
        PNH_Logger.Log("Missões", "[PNH_CORE] Sorteio realizado: " + location);
    }

    void AnnounceMissionStarted(string missionType, string location, string coords, string ownerName, string ownerID)
    {
        string globalMsg = "[RADIO PNH] Um mercenario fechou o contrato de " + location + "!";
        
        // Formato com Coordenadas e IDêntico ao teu sistema de Login
        string discordMsg = "Jogador " + ownerName + " aceitou o contrato " + missionType + " em " + location + " [" + coords + "]. (ID: " + ownerID + ")";
        
        BroadcastGlobal(globalMsg);
        PNH_Discord.Send("SISTEMA DE MISSÕES PNH", discordMsg, 3066993, PNH_CoreConfig.GetMissionsURL());
    }

    void AnnounceMissionEnded(string ownerName, string ownerID)
    {
        string globalMsg = "[RÁDIO PNH] O contrato foi concluido com sucesso por " + ownerName + "!";
        
        // Formato IDêntico ao teu sistema de Login
        string discordMsg = "Jogador " + ownerName + " concluiu o contrato. (ID: " + ownerID + ")";
        
        BroadcastGlobal(globalMsg);
        PNH_Discord.Send("SISTEMA DE MISSÕES PNH", discordMsg, 3447003, PNH_CoreConfig.GetMissionsURL());
    }

    // --- NOVO SISTEMA DE LORE (MENSAGEIRO PURO) ---
    void DeliverMissionBriefing(string informant, array<string> messages)
    {
        if (!messages || messages.Count() == 0) return;

        string tag = "[" + informant + "] ";
        
        for (int i = 0; i < messages.Count(); i++)
        {
            string msg = messages.Get(i);
            if (msg == "") continue;

            // Escalonamento de mensagens: 0s, 3s, 6s, 9s...
            GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(this.DelayedMessage, (i * 3000), false, tag + msg);
        }
    }

    private void DelayedMessage(string msg) { BroadcastGlobal(msg); }
}