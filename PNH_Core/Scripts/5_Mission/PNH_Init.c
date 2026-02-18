modded class MissionServer
{
    override void OnInit()
    {
        super.OnInit();
        
        // --- INICIALIZAÇÃO PNH CORE 2.0 (ARQUITETURA DE AGENTES) ---
        PNH_Logger.Log("Core", "[PNH_CORE] Sistema Central a arrancar...");
        
        PNH_ProfileManager.Init();
        PNH_MissionManager.GetInstance(); 
        
        // PNH 2.0: O Agente de População entra em cena
        PNH_NPCManager.GetInstance().SpawnAllNPCs(); 
        
        PNH_Logger.Log("Core", "[PNH_CORE] Todos os Agentes Inicializados.");
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