/*
    MOD: PNH_Core
    FILE: PNH_Logger.c
    DESC: Sistema centralizado de logs com gatilho para Discord Webhooks.
*/

class PNH_Logger
{
    static void Log(string moduleName, string message)
    {
        // 1. Formata a mensagem para o log interno (script.log)
        string finalMsg = string.Format("%1 [%2] :: %3", PNH_Consts.LOG_PREFIX, moduleName, message);
        Print(finalMsg);

        // 2. VERIFICAÇÃO DE GATILHO [PNH_CORE]
        if (message.Contains("[PNH_CORE]"))
        {
            int color = 16777215; // Branco

            if (message.Contains("MISSÃO_INICIADA")) 
                color = 3066993;  // Verde
            else if (message.Contains("CORPO_ENCONTRADO")) 
                color = 15105570; // Laranja
            else if (message.Contains("MISSÃO_CONCLUÍDA")) 
                color = 3447003;  // Azul

            // CORREÇÃO: Agora envia para o GetMissionsURL() do seu JSON
            PNH_Discord.Send("SISTEMA DE MISSÕES PNH", message, color, PNH_CoreConfig.GetMissionsURL());
        }
    }

    static void Error(string moduleName, string message)
    {
        string finalMsg = string.Format("%1 [ERROR] [%2] !!!! %3 !!!!", PNH_Consts.LOG_PREFIX, moduleName, message);
        Print(finalMsg);
        PNH_Discord.Send("ERRO CRÍTICO CORE", message, 15158332, PNH_CoreConfig.GetAdminURL());
    }
    
    static void Debug(string moduleName, string message)
    {
        #ifdef PNH_DEBUG
        string finalMsg = string.Format("%1 [DEBUG] [%2] :: %3", PNH_Consts.LOG_PREFIX, moduleName, message);
        Print(finalMsg);
        #endif
    }
}