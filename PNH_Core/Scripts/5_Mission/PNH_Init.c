modded class MissionServer
{
    override void OnInit()
    {
        super.OnInit();
        
        // --- INICIALIZAÇÃO PNH CORE 2.0 (ARQUITETURA DE AGENTES) ---
        PNH_Logger.Log("Core", "[PNH_CORE] Sistema Central a arrancar...");
        
        // 1. Inicia o Diretor de Tempo e Sorteio (Manager Base)
        PNH_MissionManager.GetInstance(); 
        
        // 2. Inicia o Agente de População de NPCs (Zelador)
        PNH_NPCManager.GetInstance().SpawnAllNPCs(); 
        
        // 3. Inicia o Agente de Comunicação (Rádio e Discord)
        PNH_BroadcastManager.GetInstance();
        
        // 4. Inicia o Agente Corretor (Processamento de Contratos)
        PNH_ContractBroker.GetInstance();
        
        PNH_Logger.Log("Core", "[PNH_CORE] Todos os Agentes Inicializados com Sucesso.");
    }

    // ====================================================================
    // --- PNH 2.0: O MOTOR DO TEMPO (Faz os timers das missões andarem) ---
    // ====================================================================
    override void OnUpdate(float timeslice)
    {
        super.OnUpdate(timeslice); // Garante que o servidor vanilla continua a funcionar
        
        PNH_MissionManager manager = PNH_MissionManager.GetInstance();
        if (manager)
        {
            manager.OnUpdate(timeslice); // Passa os segundos para o teu Manager
        }
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