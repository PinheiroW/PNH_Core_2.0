/*
    MOD: PNH_Core
    FILE: PNH_Logger.c
    DESC: Sistema centralizado de logs com gatilho para Discord Webhooks.
*/

class PNH_Logger
{
    // Log Informativo e Detector de Gatilhos para Missões
    static void Log(string moduleName, string message)
    {
        // 1. Formata a mensagem para o log interno do servidor (script.log)
        string finalMsg = string.Format("%1 [%2] :: %3", PNH_Consts.LOG_PREFIX, moduleName, message);
        Print(finalMsg);

        // 2. VERIFICAÇÃO DE GATILHO [PNH_CORE]
        // Se a mensagem contiver o prefixo que configuramos nas missões, envia para o Discord
        if (message.Contains("[PNH_CORE]"))
        {
            // Definimos as cores com base no status da missão
            int color = 16777215; // Branco (Padrão)

            if (message.Contains("MISSÃO_INICIADA")) 
                color = 3066993;  // Verde
            else if (message.Contains("JOGADOR_CHEGOU")) 
                color = 15105570; // Laranja
            else if (message.Contains("MISSÃO_CONCLUÍDA")) 
                color = 3447003;  // Azul

            // Envia para o Webhook de SISTEMA definido no seu Core_Config.json
            PNH_Discord.Send("SISTEMA DE MISSÕES PNH", message, color, PNH_CoreConfig.GetSystemURL());
        }
    }

    // Log de Erro - Para problemas críticos no Core
    static void Error(string moduleName, string message)
    {
        string finalMsg = string.Format("%1 [ERROR] [%2] !!!! %3 !!!!", PNH_Consts.LOG_PREFIX, moduleName, message);
        Print(finalMsg);
        
        // Opcional: Enviar erros críticos para o Webhook de ADMIN
        PNH_Discord.Send("ERRO CRÍTICO CORE", message, 15158332, PNH_CoreConfig.GetAdminURL());
    }
    
    // Log de Debug - Só aparece se o modo debug estiver ativo
    static void Debug(string moduleName, string message)
    {
        #ifdef PNH_DEBUG
        string finalMsg = string.Format("%1 [DEBUG] [%2] :: %3", PNH_Consts.LOG_PREFIX, moduleName, message);
        Print(finalMsg);
        #endif
    }
}