/// --- Documentação PNH_Core: PNH_Discord.c ---
/// Versão do Sistema: 1.0.0 (Ref: PNH_Consts)
/// Função do arquivo: Responsável pela integração externa com o Discord, enviando mensagens formatadas (Embeds) via Webhooks através da API Rest do framework Dabs.
/// Comunicação com outros arquivos: É utilizado pelo PNH_Logger.c para logs críticos, pelo PNH_BroadcastManager.c para anúncios de missões e pelos ficheiros de Hook (Chat e Conexão) na camada 5_Mission.
/// Motivo da existência: Permitir a monitorização em tempo real das atividades do servidor (logs, chat e missões) fora do ambiente de jogo.
/// Dependências internas: PNH_CoreConfig.c (para obtenção das URLs padrão) e o módulo JM_CF_Scripts no config.cpp.
/// Última atualização: 2026-02-18
/// IMPORTANTE: Ao alterar este arquivo, documente no CHANGELOG_PNH.md

class PNH_Discord
{
    // Agora aceita URL opcional.
    static void Send(string moduleName, string message, int color = 16777215, string webhookURL = "")
    {
        if (GetGame().IsClient()) return;

        string targetURL = webhookURL;
        
        // Se não passou URL, usa a de SISTEMA como padrão (Fallback)
        if (targetURL == "") 
        {
            targetURL = PNH_CoreConfig.GetSystemURL();
        }

        // Se ainda estiver vazia ou for curta demais, cancela
        if (targetURL == "" || targetURL.Length() < 10) return;

        // --- CORREÇÃO AQUI ---
        // Removido o GetGame() antes de GetRestApi(). 
        // Com o JM_CF_Scripts no config.cpp, usamos a função global do Framework.
        RestContext ctx = GetRestApi().GetRestContext(targetURL);
        
        if (!ctx) return; // Segurança extra

        ctx.SetHeader("application/json");

        string json = "{\"username\": \"PNH System\", \"embeds\": [{ \"title\": \"" + moduleName + "\", \"description\": \"" + message + "\", \"color\": " + color.ToString() + " }]}";

        ctx.POST(null, "", json);
    }
}