modded class MissionServer
{
    // Declara o nosso Cérebro de Missões
    ref PNH_MissionManager m_PNH_MissionManager;

    override void OnInit()
    {
        super.OnInit();
        
        // 1. Carrega as Configurações do Core (CRUCIAL)
        PNH_CoreConfig.Load();
        
        // 2. Log de inicialização
        PNH_Logger.Log("Core", "Sistema PNH Utils e Config carregados.");
        
        // 3. Inicia o Sistema de Missões PNH
        m_PNH_MissionManager = new PNH_MissionManager();
    }

    // O "Coração" do Servidor - Bate a cada Frame
    override void OnUpdate(float timeslice)
    {
        super.OnUpdate(timeslice);

        // Envia o pulso de tempo para o nosso Cérebro de Missões
        if (m_PNH_MissionManager)
        {
            m_PNH_MissionManager.OnUpdate(timeslice);
        }
    }
}