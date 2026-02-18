class PNH_AuditManager
{
    // Regista ações importantes (Aceitação, Conclusão, Morte em missão)
    static void LogMissionEvent(string playerName, string missionType, string action)
    {
        string timestamp = PNH_Utils.GetTimestamp(); // Assume que tens uma função de tempo em Utils
        string logEntry = "[PNH AUDIT] [" + timestamp + "] Mercenario: " + playerName + " | Missao: " + missionType + " | Evento: " + action;
        
        // Escreve no log do sistema PNH
        PNH_Logger.Log("Audit", logEntry);
    }

    // Regista alterações administrativas (como !reload_mission)
    static void LogAdminAction(string adminName, string command)
    {
        PNH_Logger.Log("Admin", "[PNH ADMIN] " + adminName + " executou o comando: " + command);
    }
}