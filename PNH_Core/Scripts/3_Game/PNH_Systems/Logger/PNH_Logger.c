/// --- Documentação PNH_Core: PNH_Logger.c ---
/// Versão do Sistema: 1.0.0 (Ref: PNH_Consts)
/// Função do arquivo: Centralizar a emissão de mensagens de registo no servidor, diferenciando entre logs informativos, erros críticos e mensagens de depuração (debug) com prefixos visuais.
/// Comunicação com outros arquivos: Chamado por praticamente todos os Managers e classes do sistema (Audit, Settings, Logistics) para reportar o estado de carregamento e execução de rotinas.
/// Motivo da existência: Facilitar a identificação de falhas no ficheiro script.log do servidor e automatizar o envio de alertas de erro para o canal administrativo do Discord.
/// Dependências internas: PNH_Consts.c (prefixo de log) e PNH_Discord.c (para reencaminhamento de alertas).
/// Última atualização: 2026-02-18
/// IMPORTANTE: Ao alterar este arquivo, documente no CHANGELOG_PNH.md

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