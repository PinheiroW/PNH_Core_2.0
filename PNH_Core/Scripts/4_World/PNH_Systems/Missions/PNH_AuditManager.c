/// --- Documentação PNH_Core: PNH_AuditManager.c ---
/// Versão do Sistema: 1.0.0 (Ref: PNH_Consts)
/// Função do arquivo: Responsável por registar eventos críticos de jogabilidade e ações administrativas, formatando mensagens com carimbos de data/hora (timestamps) detalhados para auditoria interna.
/// Comunicação com outros arquivos: Recebe solicitações de log do PNH_MissionManager.c (ciclos de missão), PNH_ContractBroker.c (assinaturas de contratos) e PNH_ChatManager.c (comandos administrativos).
/// Motivo da existência no sistema: Garantir a transparência e rastreabilidade de todas as atividades importantes do mod, permitindo aos administradores verificar o histórico de missões e o uso de comandos.
/// Dependências internas: PNH_Logger.c (para gravação física dos logs) e funções nativas de tempo do DayZ (GetYearMonthDay/GetHourMinuteSecond).
/// Última atualização: 2026-02-18
/// IMPORTANTE: Ao alterar este arquivo, documente no CHANGELOG_PNH.md

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