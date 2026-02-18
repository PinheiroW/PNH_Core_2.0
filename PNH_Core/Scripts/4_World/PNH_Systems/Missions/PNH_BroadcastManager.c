class PNH_BroadcastManager
{
    static ref PNH_BroadcastManager m_Instance;

    void PNH_BroadcastManager() {}

    static PNH_BroadcastManager GetInstance()
    {
        if (!m_Instance) m_Instance = new PNH_BroadcastManager();
        return m_Instance;
    }

    // --- ENVIOS BÁSICOS ---
    void BroadcastGlobal(string message) { PNH_Utils.SendMessageToAll(message); }
    void SendToPlayer(PlayerBase player, string message) { if (player) PNH_Utils.SendMessage(player, message); }

    // --- ANÚNCIOS OFICIAIS DE SISTEMA (Sincronizados com Rádio, Discord e Logs) ---
    void AnnounceMissionAvailable(string npcLocal)
    {
        string msg = "[ALERTA PNH] Nova operacao disponivel em " + npcLocal + ". Va ate ao local para assinar o contrato!";
        BroadcastGlobal(msg);
        PNH_Logger.Log("Missões", "[PNH_CORE] Sorteio realizado: " + npcLocal);
    }

    void AnnounceMissionStarted(string missionType, string location, string ownerName)
    {
        string globalMsg = "[RADIO PNH] Um mercenario fechou o contrato de " + location + "!";
        string discordMsg = "[PNH_CORE] MISSÃO INICIADA: " + missionType + " em " + location + " por " + ownerName;
        
        BroadcastGlobal(globalMsg);
        PNH_Discord.Send("SISTEMA DE MISSÕES PNH", discordMsg);
        PNH_Logger.Log("Missões", discordMsg);
    }

    void AnnounceMissionEnded(string ownerName)
    {
        string globalMsg = "[RÁDIO PNH] O contrato foi concluido com sucesso por " + ownerName + "!";
        string discordMsg = "[PNH_CORE] MISSÃO CONCLUÍDA por " + ownerName;
        
        BroadcastGlobal(globalMsg);
        PNH_Discord.Send("SISTEMA DE MISSÕES PNH", discordMsg);
        PNH_Logger.Log("Missões", discordMsg);
    }

    // --- SISTEMA DE INSTRUÇÕES COM ATRASO (Lore) ---
    void SendLoreMessages(PNH_MissionBase mission)
    {
        if (!mission) return;
        string tag = "[" + mission.m_MissionInformant + "] ";
        BroadcastGlobal(tag + mission.m_MissionMessage1);
        
        if (mission.m_MissionMessage2 != "")
            GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(this.DelayedMessage, 3000, false, tag + mission.m_MissionMessage2);
            
        if (mission.m_MissionMessage3 != "")
            GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(this.DelayedMessage, 6000, false, tag + mission.m_MissionMessage3);
            
        if (mission.m_MissionMessage4 != "")
            GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(this.DelayedMessage, 9000, false, tag + mission.m_MissionMessage4);
    }

    void DelayedMessage(string msg) { BroadcastGlobal(msg); }
}