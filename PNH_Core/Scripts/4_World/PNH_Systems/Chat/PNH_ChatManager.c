class PNH_ChatManager
{
    static bool HandleCommand(PlayerBase player, string message)
    {
        if (message.Length() < 1) return false;

        string prefix = message.Substring(0, 1);
        if (prefix != "/" && prefix != "!") return false;

        string commandLine = message.Substring(1, message.Length() - 1);
        TStringArray args = new TStringArray;
        commandLine.Split(" ", args);

        if (args.Count() == 0) return false;

        string command = args.Get(0);
        command.ToLower();

        // =======================================================
        // --- COMANDO: STATUS DO JOGADOR ---
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
                string rankName = "Desconhecido";
                int nextXP = 0;
                string nextRankName = "N/A";
                
                if (pData.Patente == 1) { rankName = "Recruta"; nextXP = settings.TabelaXP.XP_Mercenario; nextRankName = "Mercenário"; }
                else if (pData.Patente == 2) { rankName = "Mercenário"; nextXP = settings.TabelaXP.XP_Especialista; nextRankName = "Especialista"; }
                else if (pData.Patente == 3) { rankName = "Especialista"; nextXP = settings.TabelaXP.XP_Lenda; nextRankName = "Lenda"; }
                else if (pData.Patente == 4) { rankName = "Lenda"; nextXP = pData.XP; nextRankName = "Nível Máximo"; }

                PNH_Utils.SendMessage(player, "=== SEU PERFIL MERCENÁRIO ===");
                PNH_Utils.SendMessage(player, "Nome: " + pData.PlayerName + " | Patente: " + rankName + " (Nível " + pData.Patente + ")");
                
                if (pData.Patente < 4) {
                    int xpFaltante = nextXP - pData.XP;
                    PNH_Utils.SendMessage(player, "XP Atual: " + pData.XP + " | Faltam " + xpFaltante + " XP para " + nextRankName);
                } else {
                    PNH_Utils.SendMessage(player, "XP Atual: " + pData.XP + " | Você atingiu o nível máximo!");
                }
                
                if (pData.TemMissaoAtiva) PNH_Utils.SendMessage(player, "Status: EM OPERAÇÃO (" + pData.ClasseMissaoAtiva + ").");
                else PNH_Utils.SendMessage(player, "Status: AGUARDANDO CONTRATO.");
            }
            return true; 
        }

        // =======================================================
        // --- NOVO COMANDO PNH 2.0: ACEITAR CONTRATO ---
        // =======================================================
        if (command == "aceitar")
        {
            if (!player || !player.GetIdentity()) return true;

            PNH_MissionManager manager = PNH_MissionManager.GetInstance();
            
            // 1. Verifica se existe uma missão a decorrer
            if (!manager || !manager.m_ActiveMission) {
                PNH_Utils.SendMessage(player, "Nenhuma operação disponível no momento. Aguarde o rádio.");
                return true;
            }

            // 2. Verifica se a missão já foi aceite por outro jogador
            if (manager.m_ActiveMission.m_MissionAccepted) {
                PNH_Utils.SendMessage(player, "Este contrato já foi assumido por outro mercenário.");
                return true;
            }

            // 3. Verifica o Perfil do Jogador
            string plainId2 = player.GetIdentity().GetPlainId();
            string pName2 = player.GetIdentity().GetName();
            PNH_PlayerProfileData pData2 = PNH_ProfileManager.LoadProfile(plainId2, pName2);

            if (pData2.TemMissaoAtiva) {
                PNH_Utils.SendMessage(player, "Você já tem uma operação em andamento! Cumpra-a ou morra a tentar.");
                return true;
            }

            // 4. Verifica a distância para o NPC
            PNH_MissionSettingsData settingsNPC = PNH_MissionSettings.GetData();
            bool pertoDeNPC = false;
            string npcName = "";
            vector playerPos = player.GetPosition();

            if (settingsNPC && settingsNPC.NPCsQuestGivers) {
                for (int i = 0; i < settingsNPC.NPCsQuestGivers.Count(); i++) {
                    PNH_MissionSettings_NPC npc = settingsNPC.NPCsQuestGivers.Get(i);
                    vector npcPos = npc.Posicao.ToVector();
                    
                    if (vector.Distance(playerPos, npcPos) <= 5.0) {
                        pertoDeNPC = true;
                        npcName = npc.Nome;
                        break;
                    }
                }
            }

            if (!pertoDeNPC) {
                PNH_Utils.SendMessage(player, "[NEGADO] Você precisa estar fisicamente próximo a um Contratante para assinar.");
                return true;
            }

            // 5. SUCESSO! Assina o contrato
            string localDaMissao = manager.m_ActiveMission.m_MissionLocation;
            manager.m_ActiveMission.AcceptContract(player, 1, localDaMissao);
            
            pData2.TemMissaoAtiva = true;
            pData2.ClasseMissaoAtiva = "Operação em " + localDaMissao;
            PNH_ProfileManager.SaveProfile(pData2);

            PNH_Utils.SendMessage(player, "===================================");
            PNH_Utils.SendMessage(player, "CONTRATO ASSINADO COM: " + npcName);
            PNH_Utils.SendMessage(player, "O seu alvo está em: " + localDaMissao);
            PNH_Utils.SendMessage(player, "===================================");
            
            PNH_Utils.SendMessageToAll("[RÁDIO PNH] Atenção: Um mercenário acabou de fechar o contrato de " + localDaMissao + "!");

            return true;
        }

        // =======================================================
        // --- COMANDO DE ADMIN: RELOAD MISSION ---
        // =======================================================
        if (command == "reload_mission")
        {
            string steamID = PNH_Utils.GetSteamID(player);
            
            if (PNH_CoreConfig.IsSuperAdmin(steamID))
            {
                // CORREÇÃO AQUI: Mudámos de 'manager' para 'adminManager'
                PNH_MissionManager adminManager = PNH_MissionManager.GetInstance();
                if (adminManager)
                {
                    adminManager.ReloadMissions();
                    adminManager.ForceMissionCycle();
                    PNH_Utils.SendMessage(player, "[PNH CORE] Sucesso: JSON recarregado. Nova missão em 60 segundos.");
                }
            }
            return true;
        }

        return false;
    }
}