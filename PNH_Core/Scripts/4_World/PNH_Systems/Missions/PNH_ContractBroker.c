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

        if (manager.m_ActiveMission.m_MissionAccepted) {
            bManager.SendToPlayer(player, "PNH: Este contrato ja foi assinado por outro mercenario.");
            return;
        }

        // Regista o contrato
        manager.m_ActiveMission.AcceptContract(player, manager.m_ActiveMission.m_MissionTier, manager.m_ActiveMission.m_MissionType);
        
        if (manager.m_ActiveMission.DeployMission()) {
            // PNH 2.0: Define o estado de materialização (15s) no Manager
            manager.m_MissionState = 2; 
            manager.m_ActiveMission.m_MissionTime = 0;
            
            bManager.SendToPlayer(player, "=== CONTRATO ASSINADO COM SUCESSO ===");
            
            // PNH 2.0 FIX: O BroadcastManager envia as mensagens de Lore, evitando duplicação com o log de início
            bManager.SendLoreMessages(manager.m_ActiveMission);
        }
    }
}