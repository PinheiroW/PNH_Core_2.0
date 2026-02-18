class PNH_ContractBroker
{
    static ref PNH_ContractBroker m_Instance;

    void PNH_ContractBroker() {}

    static PNH_ContractBroker GetInstance()
    {
        if (!m_Instance) m_Instance = new PNH_ContractBroker();
        return m_Instance;
    }

    // --- LÓGICA DE NEGÓCIO: TENTAR ASSINAR CONTRATO ---
    void TryAcceptContract(PlayerBase player)
    {
        PNH_MissionManager manager = PNH_MissionManager.GetInstance();
        PNH_BroadcastManager bManager = PNH_BroadcastManager.GetInstance();

        // 1. Verifica se há missão a decorrer
        if (!manager || !manager.m_ActiveMission)
        {
            bManager.SendToPlayer(player, "PNH: Nenhuma operacao disponivel no momento. Aguarde o radio.");
            return;
        }

        // 2. Verifica se alguém já roubou o contrato
        if (manager.m_ActiveMission.m_MissionAccepted)
        {
            bManager.SendToPlayer(player, "PNH: Este contrato ja foi assinado por outro mercenario.");
            return;
        }

        // 3. Valida a distância do jogador ao NPC Oficial
        PNH_MissionSettingsData config = PNH_MissionSettings.GetData();
        bool pertoDeNPC = false;
        string npcName = "Informante";

        if (config && config.NPCsQuestGivers)
        {
            foreach (PNH_MissionSettings_NPC npc : config.NPCsQuestGivers)
            {
                if (vector.Distance(player.GetPosition(), npc.Posicao.ToVector()) < 5.0)
                {
                    pertoDeNPC = true;
                    npcName = npc.Nome;
                    break;
                }
            }
        }

        if (!pertoDeNPC)
        {
            bManager.SendToPlayer(player, "PNH: Voce precisa estar diante de um oficial PNH para assinar o contrato.");
            return;
        }

        // 4. Oficializa o Contrato
        manager.m_ActiveMission.AcceptContract(player, manager.m_ActiveMission.m_MissionTier, manager.m_ActiveMission.m_MissionType);
        
        // 5. Faz o Deploy Físico (Spawn dos Zombies/Itens) e Anuncia
        if (manager.m_ActiveMission.DeployMission())
        {
            bManager.SendToPlayer(player, "=== CONTRATO ASSINADO COM: " + npcName + " ===");
            bManager.SendToPlayer(player, "O seu alvo esta em: " + manager.m_ActiveMission.m_MissionLocation);
            
            bManager.AnnounceMissionStarted(manager.m_ActiveMission.m_MissionType, manager.m_ActiveMission.m_MissionLocation, player.GetIdentity().GetName());
            bManager.SendLoreMessages(manager.m_ActiveMission);
        }
    }
}