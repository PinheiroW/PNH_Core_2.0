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