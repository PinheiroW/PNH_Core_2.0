class PNH_ChatManager
{
    static bool HandleCommand(PlayerBase player, string message)
    {
        if (message.Length() < 1) return false;
        string prefix = message.Substring(0, 1);
        if (prefix != "!" && prefix != "/") return false;

        string commandLine = message.Substring(1, message.Length() - 1);
        TStringArray args = new TStringArray;
        commandLine.Split(" ", args);
        string command = args.Get(0);
        command.ToLower();

        PNH_MissionManager manager = PNH_MissionManager.GetInstance();

        // =======================================================
        // --- COMANDO: ACEITAR CONTRATO ---
        // =======================================================
        if (command == "aceitar")
        {
            if (!manager || !manager.m_ActiveMission)
            {
                PNH_Utils.SendMessage(player, "PNH: Nenhuma operação disponível no momento. Aguarde o rádio.");
                return true;
            }

            if (manager.m_ActiveMission.m_MissionAccepted)
            {
                PNH_Utils.SendMessage(player, "PNH: Este contrato já foi assinado por outro mercenário.");
                return true;
            }

            // Verifica proximidade com os NPCs QuestGivers
            PNH_MissionSettingsData settings = PNH_MissionSettings.GetData();
            bool pertoDeNPC = false;
            string npcName = "Informante";

            foreach (PNH_MissionSettings_NPC npc : settings.NPCsQuestGivers)
            {
                if (vector.Distance(player.GetPosition(), npc.Posicao.ToVector()) < 5.0)
                {
                    pertoDeNPC = true;
                    npcName = npc.Nome;
                    break;
                }
            }

            if (!pertoDeNPC)
            {
                PNH_Utils.SendMessage(player, "PNH: Você precisa estar diante de um oficial PNH para assinar o contrato.");
                return true;
            }

            // --- EXECUÇÃO DO CONTRATO ---
            manager.m_ActiveMission.AcceptContract(player, manager.m_ActiveMission.m_MissionTier, manager.m_ActiveMission.m_MissionType);
            
            // ATIVA O SPAWN FÍSICO (Zumbis/Itens) NO MAPA
            if (manager.m_ActiveMission.DeployMission())
            {
                PNH_Utils.SendMessage(player, "===================================");
                PNH_Utils.SendMessage(player, "CONTRATO ASSINADO COM: " + npcName);
                PNH_Utils.SendMessage(player, "Sua missão foi marcada no setor. Boa sorte!");
                PNH_Utils.SendMessage(player, "===================================");
                PNH_Utils.SendMessageToAll("[RÁDIO PNH] Um mercenário fechou o contrato. Operação em curso!");
            }

            return true;
        }

        // =======================================================
        // --- COMANDO ADMIN: RELOAD MISSION ---
        // =======================================================
        if (command == "reload_mission")
        {
            if (PNH_CoreConfig.IsSuperAdmin(player.GetIdentity().GetPlainId()))
            {
                if (manager)
                {
                    manager.ReloadMissions();
                    manager.ForceMissionCycle();
                    PNH_Utils.SendMessage(player, "[PNH CORE] JSON recarregado. Nova missão em instantes.");
                }
            }
            return true;
        }

        return false;
    }
}