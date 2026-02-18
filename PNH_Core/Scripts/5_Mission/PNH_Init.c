modded class MissionServer
{
    override void OnInit()
    {
        super.OnInit();
        
        // 1. Auditoria e Logs (Primeiro passo para registar o arranque)
        PNH_Logger.Log("Core", "[PNH_CORE] Sistema Central a arrancar...");
        
        // 2. Configurações Base (Webhooks, Admins, Tabela de XP)
        PNH_CoreConfig.Load(); 
        PNH_MissionSettings.Load();
        
        // 3. Inicialização dos Agentes Estáticos (Apenas carregar instâncias se necessário)
        PNH_MissionManager.GetInstance(); 
        PNH_BroadcastManager.GetInstance();
        PNH_ContractBroker.GetInstance();
        
        // 4. Logística e NPCs (O mundo físico)
        PNH_NPCManager.GetInstance().SpawnAllNPCs(); 
        
        // 5. Auditoria Final de Sucesso
        PNH_AuditManager.LogAdminAction("Sistema", "Todos os Agentes Inicializados com Sucesso.");
    }

    override void OnUpdate(float timeslice)
    {
        super.OnUpdate(timeslice);
        
        // O Único agente que precisa de batimento cardíaco constante é o Manager
        PNH_MissionManager manager = PNH_MissionManager.GetInstance();
        if (manager) manager.OnUpdate(timeslice);
    }

    override void InvokeOnConnect(PlayerBase player, PlayerIdentity identity)
    {
        super.InvokeOnConnect(player, identity);
        
        if (player && identity)
        {
            string plainId = identity.GetPlainId();
            string pName = identity.GetName();
            
            // Verifica/Cria perfil do mercenário ao entrar
            PNH_PlayerProfileData profile = PNH_ProfileManager.LoadProfile(plainId, pName);
            
            if (profile) 
            {
                PNH_AuditManager.LogMissionEvent(pName, "Conexao", "Perfil de Mercenario carregado");
            }
        }
    }
}