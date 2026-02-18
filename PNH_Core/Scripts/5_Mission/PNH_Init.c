/// --- Documentação PNH_Core: PNH_Init.c ---
/// Versão do Sistema: 1.0.0 (Ref: PNH_Consts)
/// Função do arquivo: Orquestrar o arranque completo do mod no lado do servidor, carregando configurações, inicializando as instâncias dos Managers e spawnando os NPCs Oficiais no mapa.
/// Comunicação com outros arquivos: Invoca o carregamento do PNH_CoreConfig.c e PNH_MissionSettings.c, ativa o batimento cardíaco do PNH_MissionManager.c via OnUpdate e coordena o spawn inicial com o PNH_NPCManager.c.
/// Motivo da existência no sistema: Garantir que todos os componentes do mod sejam inicializados na ordem correta (Config -> Managers -> Mundo) assim que a missão do servidor começa.
/// Dependências internas: Praticamente todos os Managers e arquivos de configuração do Core (Logger, CoreConfig, MissionSettings, MissionManager, BroadcastManager, ContractBroker, NPCManager, AuditManager e ProfileManager).
/// Última atualização: 2026-02-18
/// IMPORTANTE: Ao alterar este arquivo, documente no CHANGELOG_PNH.md

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