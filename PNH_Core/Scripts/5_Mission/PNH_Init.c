modded class MissionServer
{
    override void OnInit()
    {
        super.OnInit();
        
        // --- INICIALIZAÇÃO PNH CORE 2.0 (ARQUITETURA DE AGENTES) ---
        PNH_Logger.Log("Core", "[PNH_CORE] Sistema Central a arrancar...");
        
        // 1. Inicia Banco de Dados (Perfis e XP)
        PNH_ProfileManager.Init();
        
        // 2. Inicia o Diretor de Tempo e Sorteio (Manager Base)
        PNH_MissionManager.GetInstance(); 
        
        // 3. Inicia o Agente de População de NPCs (Zelador)
        PNH_NPCManager.GetInstance().SpawnAllNPCs(); 
        
        // 4. Inicia o Agente de Comunicação (Rádio e Discord)
        PNH_BroadcastManager.GetInstance();
        
        PNH_Logger.Log("Core", "[PNH_CORE] Todos os Agentes Inicializados com Sucesso.");
    }

    override void InvokeOnConnect(PlayerBase player, PlayerIdentity identity)
    {
        super.InvokeOnConnect(player, identity);
        
        if (player && identity)
        {
            string plainId = identity.GetPlainId();
            string pName = identity.GetName();
            PNH_PlayerProfileData profile = PNH_ProfileManager.LoadProfile(plainId, pName);
            
            if (profile)
            {
                PNH_Logger.Log("Profiles", "[PNH_CORE] Perfil verificado para: " + pName + " (XP: " + profile.XP + ")");
            }
        }
    }
}