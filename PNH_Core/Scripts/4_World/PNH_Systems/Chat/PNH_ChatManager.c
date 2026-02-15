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

        if (command == "reload_mission")
        {
            string steamID = PNH_Utils.GetSteamID(player);
            
            if (PNH_CoreConfig.IsSuperAdmin(steamID))
            {
                PNH_MissionManager manager = PNH_MissionManager.GetInstance();
                if (manager)
                {
                    // Recarrega as configurações do arquivo JSON
                    manager.ReloadMissions();
                    
                    // Força o reset do ciclo (Nova missão em 1 minuto)
                    manager.ForceMissionCycle();

                    PNH_Utils.SendMessage(player, "[PNH CORE] Sucesso: JSON recarregado. Nova missão em 60 segundos.");
                    PNH_Logger.Log("Admin", "Admin " + player.GetIdentity().GetName() + " executou reload_mission e resetou o ciclo.");
                }
                else
                {
                    PNH_Utils.SendMessage(player, "[PNH CORE] Erro: Mission Manager offline.");
                }
            }
            else
            {
                PNH_Utils.SendMessage(player, "[PNH CORE] Acesso Negado.");
            }
            return true;
        }

        return false;
    }
}