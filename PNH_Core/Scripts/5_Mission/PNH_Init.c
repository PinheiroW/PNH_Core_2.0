modded class MissionServer
{
    // 1. Declara o nosso Cérebro de Missões na memória do servidor
    ref PNH_MissionManager m_PNH_MissionManager;

    override void OnInit()
    {
        super.OnInit();
        
        // Carrega as Configurações do Core
        PNH_CoreConfig.Load();
        
        // Log de inicialização do sistema base
        PNH_Logger.Log("Core", "Sistema PNH Utils e Config carregados.");
        
        // 2. INICIA O SISTEMA DE MISSÕES (O motor liga aqui!)
        m_PNH_MissionManager = new PNH_MissionManager();
    }

    // 3. O "Coração" do Servidor - Bate a cada Frame do jogo
    override void OnUpdate(float timeslice)
    {
        super.OnUpdate(timeslice);

        // Envia o pulso de tempo para o nosso Cérebro de Missões
        if (m_PNH_MissionManager)
        {
            m_PNH_MissionManager.OnUpdate(timeslice);
        }
    }

    // ---> 4. NOVA FUNÇÃO PNH 2.0: GATILHO DE LOGIN <---
    override void InvokeOnConnect(PlayerBase player, PlayerIdentity identity)
    {
        super.InvokeOnConnect(player, identity);
        
        // CORREÇÃO: Usando GetPlainId() para pegar o Steam64 (Numérico)
        if (identity) {
            PNH_ProfileManager.LoadProfile(identity.GetPlainId(), identity.GetName());
            Print("[PNH_2.0] Perfil verificado/criado com sucesso para: " + identity.GetName());
        }
    }
}