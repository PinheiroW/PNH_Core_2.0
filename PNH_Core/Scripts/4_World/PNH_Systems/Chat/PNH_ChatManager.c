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
            PNH_ContractBroker.GetInstance().TryAcceptContract(player);
            return true;
        }

        // =======================================================
        // --- COMANDO ADMIN: RELOAD MISSION ---
        // =======================================================
        if (command == "reload_mission")
        {
            // PNH 2.0 FIX: Mudámos para 'adminId' para evitar o erro de Multiple Declaration
            string adminId = player.GetIdentity().GetPlainId();
            
            if (PNH_CoreConfig.IsSuperAdmin(adminId))
            {
                PNH_MissionManager manager = PNH_MissionManager.GetInstance();
                if (manager)
                {
                    manager.ReloadMissions();
                    manager.ForceMissionCycle();
                    PNH_Utils.SendMessage(player, "[PNH CORE] JSON recarregado. Ciclo de missoes reiniciado.");
                }
            }
            else
            {
                PNH_Utils.SendMessage(player, "[ERRO] Sem permissao PNH. Adicione o seu ID (" + adminId + ") no ficheiro de configuracao!");
            }
            return true;
        }
        return false;
    }
}