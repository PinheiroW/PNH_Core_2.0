modded class MissionServer
{
    override void OnInit()
    {
        super.OnInit();
        
        PNH_Logger.Log("Core", "[PNH_CORE] Sistema Central a arrancar...");
        
        // PNH 2.0 FIX: Carrega o JSON de Admins e Webhooks antes de tudo
        PNH_CoreConfig.Load(); 
        
        // PNH_ProfileManager não precisa de Init() - Removido para evitar o crash
        PNH_MissionManager.GetInstance(); 
        PNH_NPCManager.GetInstance().SpawnAllNPCs(); 
        PNH_BroadcastManager.GetInstance();
        PNH_ContractBroker.GetInstance();
        
        PNH_Logger.Log("Core", "[PNH_CORE] Todos os Agentes Inicializados com Sucesso.");
    }

    override void OnUpdate(float timeslice)
    {
        super.OnUpdate(timeslice);
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
            PNH_PlayerProfileData profile = PNH_ProfileManager.LoadProfile(plainId, pName);
            if (profile) PNH_Logger.Log("Profiles", "[PNH_CORE] Perfil verificado para: " + pName);
        }
    }
}