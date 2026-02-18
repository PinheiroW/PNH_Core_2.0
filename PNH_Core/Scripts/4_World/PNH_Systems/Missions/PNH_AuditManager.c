class PNH_AuditManager
{
    // Regista ações importantes (Aceitação, Conclusão, Morte em missão)
    static void LogMissionEvent(string playerName, string missionType, string action)
    {
        // Obtém a data e hora nativa do servidor DayZ
        int year, month, day, hour, minute, second;
        GetYearMonthDay(year, month, day);
        GetHourMinuteSecond(hour, minute, second);
        
        // Formata para o padrão: YYYY-MM-DD HH:MM:SS
        string timestamp = year.ToString() + "-" + month.ToStringLen(2) + "-" + day.ToStringLen(2) + " " + hour.ToStringLen(2) + ":" + minute.ToStringLen(2) + ":" + second.ToStringLen(2);

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