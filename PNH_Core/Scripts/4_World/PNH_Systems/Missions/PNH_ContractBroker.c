/// --- Documentação PNH_Core: PNH_ContractBroker.c ---
/// Versão do Sistema: 1.2.0 (Fase de Localização e Narrativa)
/// Função do arquivo: Gerir toda a lógica de validação e assinatura física de contratos.
/// Comunicação com outros arquivos: Comunica com o PNH_IntelManager.c, PNH_ProfileManager, PNH_MissionManager.c e PNH_BroadcastManager.c.
/// Motivo da existência do arquivo no sistema: Isolar a lógica complexa de "venda" e validação de contratos.
/// Dependências internas: PNH_MissionManager.c, PNH_BroadcastManager.c, PNH_MissionSettings.c, PNH_IntelManager.c, PNH_ProfileManager e PNH_AuditManager.c.
/// Última atualização: 2026-02-18
/// IMPORTANTE: Ao alterar este arquivo, documente no CHANGELOG_PNH.md

class PNH_ContractBroker
{
    static ref PNH_ContractBroker m_Instance;

    void PNH_ContractBroker() {}

    static PNH_ContractBroker GetInstance() {
        if (!m_Instance) m_Instance = new PNH_ContractBroker();
        return m_Instance;
    }

    void TryAcceptContract(PlayerBase player)
    {
        PNH_MissionManager manager = PNH_MissionManager.GetInstance();
        PNH_BroadcastManager bManager = PNH_BroadcastManager.GetInstance();
        
        // Puxamos as configurações para aceder aos Textos
        PNH_MissionSettingsData settings = PNH_MissionSettings.GetData();

        if (!player || !player.GetIdentity() || !manager || !manager.m_ActiveMission) return;

        // VERIFICAÇÃO 1: Distância (Lê o texto de erro do JSON)
        if (!PNH_IntelManager.IsPlayerNearQuestGiver(player, 10.0)) {
            bManager.SendToPlayer(player, settings.Textos.Erro_LongeNPC);
            return;
        }

        // VERIFICAÇÃO 2: Missão já tem dono (Lê o texto de erro do JSON)
        if (manager.m_ActiveMission.m_MissionAccepted) {
            bManager.SendToPlayer(player, settings.Textos.Erro_JaAssinado);
            return;
        }

        // VERIFICAÇÃO 3: Patente vs Tier
        string plainId = player.GetIdentity().GetPlainId();
        string pName = player.GetIdentity().GetName();
        PNH_PlayerProfileData pData = PNH_ProfileManager.LoadProfile(plainId, pName);
        
        string missionType = manager.m_ActiveMission.m_MissionType;
        bool podeAceitar = false;

        if (pData.Patente >= 1 && settings.CatalogoMissoes.Tier1_Recruta.Find(missionType) != -1) podeAceitar = true;
        else if (pData.Patente >= 2 && settings.CatalogoMissoes.Tier2_Mercenario.Find(missionType) != -1) podeAceitar = true;
        else if (pData.Patente >= 3 && settings.CatalogoMissoes.Tier3_Especialista.Find(missionType) != -1) podeAceitar = true;
        else if (pData.Patente >= 4 && settings.CatalogoMissoes.Tier4_Lenda.Find(missionType) != -1) podeAceitar = true;

        if (!podeAceitar) {
            // Usa o texto do JSON, mas adicionamos a patente do jogador dinamicamente para ele saber o motivo
            bManager.SendToPlayer(player, settings.Textos.Erro_PatenteBaixa + " (Patente Atual: " + pData.Patente + ")");
            return;
        }

        // --- EXECUÇÃO DO CONTRATO ---
        manager.m_ActiveMission.AcceptContract(player, manager.m_ActiveMission.m_MissionTier, manager.m_ActiveMission.m_MissionType);
        
        if (manager.m_ActiveMission.DeployMission()) {
            manager.m_MissionState = 2; 
            manager.m_ActiveMission.m_MissionTime = 0;
            
            // Sucesso! Lê a frase de vitória do JSON
            bManager.SendToPlayer(player, settings.Textos.Sucesso_Assinatura);
            
            vector pos = manager.m_ActiveMission.m_MissionPosition;
            
            // CORREÇÃO PARA O DISCORD: Adicionamos os parênteses retos aqui
            string strCoords = "[X: " + Math.Round(pos[0]).ToString() + ", Z: " + Math.Round(pos[2]).ToString() + "]";

            // Formatação exata para o Webhook do Discord
            string discordMsg = "Jogador " + pName + " aceitou o contrato " + missionType + " em " + manager.m_ActiveMission.m_MissionLocation + " " + strCoords + ". (ID: " + plainId + ")";

            bManager.AnnounceMissionStarted(manager.m_ActiveMission.m_MissionType, manager.m_ActiveMission.m_MissionLocation, strCoords, manager.m_ActiveMission.m_MissionOwnerName, manager.m_ActiveMission.m_MissionOwnerID);
            
            // Dispara a mensagem formatada para o log/Discord
            PNH_AuditManager.LogMissionEvent("SISTEMA DE MISSÕES PNH", "", discordMsg);
            
            // --- CORREÇÃO DO CRASH: USO DO NOVO SISTEMA DE LORE (FASE 1) ---
            if (manager.m_ActiveMission.m_LoreEtapas && manager.m_ActiveMission.m_LoreEtapas.Aceitou != "")
            {
                bManager.SendNotificationToPlayer(player, manager.m_ActiveMission.m_MissionInformant, manager.m_ActiveMission.m_LoreEtapas.Aceitou, 8.0, "Notifications/gui/data/info.edds");
            }
        }
    }
}