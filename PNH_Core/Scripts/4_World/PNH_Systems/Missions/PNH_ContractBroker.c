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

        if (!player || !player.GetIdentity() || !manager || !manager.m_ActiveMission) return;

        // CORREÇÃO: Bloqueia se o jogador estiver longe do Oficial (NPC)
        if (!PNH_IntelManager.IsPlayerNearQuestGiver(player, 3.0)) {
            bManager.SendToPlayer(player, "[ERRO PNH] Voce precisa estar perto de um Oficial PNH para assinar o contrato!");
            return;
        }

        if (manager.m_ActiveMission.m_MissionAccepted) {
            bManager.SendToPlayer(player, "PNH: Este contrato ja foi assinado por outro mercenario.");
            return;
        }

        manager.m_ActiveMission.AcceptContract(player, manager.m_ActiveMission.m_MissionTier, manager.m_ActiveMission.m_MissionType);
        
        if (manager.m_ActiveMission.DeployMission()) {
            manager.m_MissionState = 2; 
            manager.m_ActiveMission.m_MissionTime = 0;
            
            bManager.SendToPlayer(player, "=== CONTRATO ASSINADO COM SUCESSO ===");
            
            vector pos = manager.m_ActiveMission.m_MissionPosition;
            string strCoords = "X: " + Math.Round(pos[0]).ToString() + ", Z: " + Math.Round(pos[2]).ToString();

            bManager.AnnounceMissionStarted(manager.m_ActiveMission.m_MissionType, manager.m_ActiveMission.m_MissionLocation, strCoords, manager.m_ActiveMission.m_MissionOwnerName, manager.m_ActiveMission.m_MissionOwnerID);
            
            PNH_AuditManager.LogMissionEvent(manager.m_ActiveMission.m_MissionOwnerName, manager.m_ActiveMission.m_MissionType, "Contrato Assinado em [" + strCoords + "]");
            
            bManager.DeliverMissionBriefing(manager.m_ActiveMission.m_MissionInformant, manager.m_ActiveMission.m_MissionMessages);
        }
    }
}