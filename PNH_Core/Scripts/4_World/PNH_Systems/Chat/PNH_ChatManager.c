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
        if (args.Count() == 0) return false;
        
        string command = args.Get(0);
        command.ToLower();

        PNH_MissionManager manager = PNH_MissionManager.GetInstance();

        // =======================================================
        // --- COMANDO: STATUS / PERFIL DO JOGADOR ---
        // =======================================================
        if (command == "status" || command == "perfil")
        {
            if (!player || !player.GetIdentity()) return true;
            
            string plainId = player.GetIdentity().GetPlainId();
            string pName = player.GetIdentity().GetName();
            
            PNH_PlayerProfileData pData = PNH_ProfileManager.LoadProfile(plainId, pName);
            PNH_MissionSettingsData settings = PNH_MissionSettings.GetData();
            
            if (pData && settings)
            {
                string rankName = "Recruta";
                int nextXP = settings.TabelaXP.XP_Mercenario;
                string nextRankName = "Mercenario";
                
                if (pData.Patente == 2) { rankName = "Mercenario"; nextXP = settings.TabelaXP.XP_Especialista; nextRankName = "Especialista"; }
                else if (pData.Patente == 3) { rankName = "Especialista"; nextXP = settings.TabelaXP.XP_Lenda; nextRankName = "Lenda"; }
                else if (pData.Patente == 4) { rankName = "Lenda"; nextXP = 0; nextRankName = "N/A"; }

                string statusMissao = "NAO";
                if (pData.TemMissaoAtiva) statusMissao = "SIM";

                PNH_Utils.SendMessage(player, "------- PERFIL PNH 2.0 -------");
                PNH_Utils.SendMessage(player, "Operador: " + pName);
                PNH_Utils.SendMessage(player, "Patente: " + rankName);
                PNH_Utils.SendMessage(player, "XP Atual: " + pData.XP);
                if (nextXP > 0) PNH_Utils.SendMessage(player, "Proximo Nivel: " + (nextXP - pData.XP) + " XP para " + nextRankName);
                PNH_Utils.SendMessage(player, "Missao Ativa: " + statusMissao);
                PNH_Utils.SendMessage(player, "------------------------------");
            }
            return true;
        }

        // =======================================================
        // --- COMANDO: ACEITAR CONTRATO ---
        // =======================================================
        if (command == "aceitar")
        {
            if (!manager || !manager.m_ActiveMission)
            {
                PNH_Utils.SendMessage(player, "PNH: Nenhuma operacao disponivel no momento. Aguarde o radio.");
                return true;
            }

            if (manager.m_ActiveMission.m_MissionAccepted)
            {
                PNH_Utils.SendMessage(player, "PNH: Este contrato ja foi assinado por outro mercenario.");
                return true;
            }

            PNH_MissionSettingsData config = PNH_MissionSettings.GetData();
            bool pertoDeNPC = false;
            string npcName = "Informante";

            foreach (PNH_MissionSettings_NPC npc : config.NPCsQuestGivers)
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
                PNH_Utils.SendMessage(player, "PNH: Voce precisa estar diante de um oficial PNH para assinar o contrato.");
                return true;
            }

            manager.m_ActiveMission.AcceptContract(player, manager.m_ActiveMission.m_MissionTier, manager.m_ActiveMission.m_MissionType);
            
            // PNH 2.0: O Agente de Transmissão assume o rádio e o Discord!
            if (manager.m_ActiveMission.DeployMission())
            {
                PNH_BroadcastManager bManager = PNH_BroadcastManager.GetInstance();
                
                bManager.SendToPlayer(player, "=== CONTRATO ASSINADO COM: " + npcName + " ===");
                bManager.SendToPlayer(player, "O seu alvo esta em: " + manager.m_ActiveMission.m_MissionLocation);
                
                bManager.AnnounceMissionStarted(manager.m_ActiveMission.m_MissionType, manager.m_ActiveMission.m_MissionLocation, player.GetIdentity().GetName());
                
                bManager.SendLoreMessages(manager.m_ActiveMission);
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
                    PNH_Utils.SendMessage(player, "[PNH CORE] JSON recarregado. Ciclo reiniciado.");
                }
            }
            return true;
        }

        return false;
    }
}