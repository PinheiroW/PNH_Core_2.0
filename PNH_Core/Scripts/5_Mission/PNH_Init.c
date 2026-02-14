modded class MissionServer
{
    override void OnInit()
    {
        super.OnInit();
        
        // 1. Carrega as Configurações do Core (CRUCIAL)
        PNH_CoreConfig.Load();
        
        // 2. Log de inicialização
        PNH_Logger.Log("Core", "Sistema PNH Utils e Config carregados.");
        
        // 3. Teste Rápido (Opcional - Pode comentar depois)
        // PNH_Discord.Send("Core Init", "Servidor Online! PNH Core 2.0 Carregado.", 65280);
    }
}