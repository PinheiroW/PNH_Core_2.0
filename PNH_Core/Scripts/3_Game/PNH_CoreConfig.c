/// --- Documentação PNH_Core: PNH_CoreConfig.c ---
/// Função do arquivo: Gere o carregamento, salvamento e acesso às configurações globais do Core, como URLs de Webhooks do Discord, mensagens de broadcast automático e a lista de Super Admins através de um ficheiro JSON.
//[cite_start]/// Comunicação com outros arquivos: Fornece as URLs necessárias para o PNH_Discord.c e PNH_Logger.c, valida permissões administrativas para o PNH_ChatHook.c e é invocado para inicialização no PNH_Init.c[cite: 13, 14, 15, 23, 24].
/// Motivo da existência do arquivo no sistema: Centralizar dados sensíveis e variáveis de infraestrutura num ficheiro externo, permitindo que donos de servidores configurem o mod sem necessidade de alterar o código-fonte.
/// Dependências internas: PNH_Consts.c (para definição de prefixos e caminhos de diretórios).
/// Última atualização: 2026-02-18
/// Sempre que alterar ou documentar um arquivo, atualize também o arquivo CHANGELOG_PNH

class PNH_CoreSettings
{
    // Webhooks
    string SystemWebhookURL;   
    string AdminWebhookURL;    
    string ChatWebhookURL;     
    string LoginWebhookURL;    
    string WelcomeWebhookURL;
    string MissionsWebhookURL; // Canal Exclusivo de Missões
    
    // Auto Broadcast
    float BroadcastIntervalMin;          
    ref array<string> BroadcastMessages; 
    
    ref array<string> SuperAdmins;

    void PNH_CoreSettings()
    {
        SystemWebhookURL = "";
        AdminWebhookURL = "";
        ChatWebhookURL = "";
        LoginWebhookURL = "";
        WelcomeWebhookURL = "";
        MissionsWebhookURL = "";
        
        BroadcastIntervalMin = 10.0;
        BroadcastMessages = new array<string>;
        SuperAdmins = new array<string>;
    }
}

class PNH_CoreConfig
{
    private static const string CONFIG_PATH = "$profile:PNH/Core_Config.json";
    static ref PNH_CoreSettings m_Settings;

    static void Load()
    {
        m_Settings = new PNH_CoreSettings();
        
        if (!FileExist("$profile:PNH")) 
        {
            MakeDirectory("$profile:PNH");
        }

        if (FileExist(CONFIG_PATH))
        {
            JsonFileLoader<PNH_CoreSettings>.JsonLoadFile(CONFIG_PATH, m_Settings);
        }
        else
        {
            m_Settings.BroadcastMessages.Insert("Bem-vindo ao Servidor PNH!");
            Save();
        }
    }

    static void Save()
    {
        JsonFileLoader<PNH_CoreSettings>.JsonSaveFile(CONFIG_PATH, m_Settings);
    }
    
    static void Reload()
    {
        if (FileExist(CONFIG_PATH))
        {
            JsonFileLoader<PNH_CoreSettings>.JsonLoadFile(CONFIG_PATH, m_Settings);
        }
    }
    
    // --- GETTERS BLINDADOS (Safe Mode) ---
    // Usamos uma variável local 'url' para garantir que o compilador
    // entenda que SEMPRE haverá um retorno no final da função.

    static string GetSystemURL()  
    { 
        string url = "";
        if (m_Settings) url = m_Settings.SystemWebhookURL;
        return url; 
    }

    static string GetAdminURL()   
    { 
        string url = "";
        if (m_Settings) url = m_Settings.AdminWebhookURL;
        return url; 
    }

    static string GetChatURL()    
    { 
        string url = "";
        if (m_Settings) url = m_Settings.ChatWebhookURL;
        return url; 
    }

    static string GetLoginURL()   
    { 
        string url = "";
        if (m_Settings) url = m_Settings.LoginWebhookURL;
        return url; 
    }

    static string GetWelcomeURL() 
    { 
        string url = "";
        if (m_Settings) url = m_Settings.WelcomeWebhookURL;
        return url; 
    }
    
    static string GetMissionsURL()
    { 
        string url = "";
        if (m_Settings) url = m_Settings.MissionsWebhookURL;
        return url; 
    }
    
    static bool IsSuperAdmin(string steamID)
    {
        if (m_Settings && m_Settings.SuperAdmins)
        {
            return (m_Settings.SuperAdmins.Find(steamID) != -1);
        }
        return false;
    }
}