modded class MissionServer
{
    ref PNH_MissionManager m_PNH_MissionManager;

    override void OnInit()
    {
        super.OnInit();
        
        PNH_CoreConfig.Load();
        PNH_Logger.Log("Core", "Sistema PNH Utils e Config carregados.");
        
        m_PNH_MissionManager = new PNH_MissionManager();

        // --- ADIÇÃO PNH 2.0: INVOCA O SPAWN DOS NPCs NO INÍCIO DO SERVIDOR ---
        if (m_PNH_MissionManager)
        {
            m_PNH_MissionManager.SpawnNPCs();
        }
    }

    override void OnUpdate(float timeslice)
    {
        super.OnUpdate(timeslice);

        if (m_PNH_MissionManager)
        {
            m_PNH_MissionManager.OnUpdate(timeslice);
        }
    }

    override void InvokeOnConnect(PlayerBase player, PlayerIdentity identity)
    {
        super.InvokeOnConnect(player, identity);
        
        if (player && identity)
        {
            string steamID = identity.GetPlainId();
            string name = identity.GetName();
            
            // Regista o perfil do jogador no banco de dados PNH
            PNH_ProfileManager.LoadProfile(steamID, name);
            PNH_Logger.Log("Profiles", "[PNH_2.0] Perfil verificado/criado com sucesso para: " + name);
        }
    }
}