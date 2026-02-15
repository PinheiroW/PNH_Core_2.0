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

        // COMANDO: reload_mission
        if (command == "reload_mission")
        {
            string steamID = PNH_Utils.GetSteamID(player);
            
            if (PNH_CoreConfig.IsSuperAdmin(steamID))
            {
                // CORREÇÃO: Busca a instância global do Manager diretamente
                PNH_MissionManager manager = PNH_MissionManager.GetInstance();
                if (manager)
                {
                    manager.ReloadMissions();
                    PNH_Utils.SendMessage(player, "[PNH CORE] Sucesso: Missões JSON recarregadas.");
                    PNH_Logger.Log("Admin", "Admin " + player.GetIdentity().GetName() + " executou reload_mission.");
                }
                else
                {
                    PNH_Utils.SendMessage(player, "[PNH CORE] Erro: Mission Manager não está online.");
                }
            }
            else
            {
                PNH_Utils.SendMessage(player, "[PNH CORE] Acesso Negado: Você não é administrador.");
            }
            return true;
        }

        return false;
    }
}