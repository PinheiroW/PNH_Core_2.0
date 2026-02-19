/// --- Documentação PNH_Core: PNH_ChatManager.c ---
/// Versão do Sistema: 1.2.1 (Correção de Compilação - Comando Reload)
/// Função: Processar comandos de chat e interface administrativa. Agora sincronizado com o MissionManager v2.1.6.

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

        // --- COMANDO !status (Verifica o perfil e missão ativa) ---
        if (command == "status")
        {
            PNH_PlayerProfileData pData = PNH_ProfileManager.LoadProfile(player.GetIdentity().GetPlainId(), player.GetIdentity().GetName());
            PNH_MissionSettingsData settings = PNH_MissionSettings.GetData();
            PNH_MissionManager mngr = PNH_MissionManager.GetInstance();

            if (pData && settings && mngr)
            {
                PNH_Utils.SendMessage(player, "=== STATUS DO OPERADOR ===");
                
                string msgOp = settings.Textos.Msg_StatusOperador;
                msgOp.Replace("%1", pData.Nome);
                PNH_Utils.SendMessage(player, msgOp);

                string msgPat = settings.Textos.Msg_StatusPatente;
                msgPat.Replace("%1", pData.Patente);
                PNH_Utils.SendMessage(player, msgPat);

                string msgXP = settings.Textos.Msg_StatusXP;
                msgXP.Replace("%1", pData.XP.ToString());
                PNH_Utils.SendMessage(player, msgXP);

                if (pData.TemMissaoAtiva && mngr.m_ActiveMission)
                {
                    string loc = mngr.m_ActiveMission.m_MissionLocation;
                    PNH_Utils.SendMessage(player, "=== CONTRATO EM CURSO ===");
                    PNH_Utils.SendMessage(player, "Local: " + loc);
                    PNH_Utils.SendMessage(player, "Informante: " + mngr.m_ActiveMission.m_MissionInformant);
                    PNH_Utils.SendMessage(player, "- " + mngr.m_ActiveMission.m_LoreEtapas.Aceitou);
                }
                else
                {
                    PNH_Utils.SendMessage(player, "Nenhum contrato assinado no momento.");
                }
                PNH_Utils.SendMessage(player, "==========================");
            }
            return true;
        }

        // --- COMANDO !aceitar (Aceita o contrato disponível) ---
        if (command == "aceitar")
        {
            PNH_ContractBroker.GetInstance().TryAcceptContract(player);
            return true;
        }

        // --- COMANDO !reload_mission (ADMIN: Recarrega JSON e Reinicia Ciclo) ---
        if (command == "reload_mission")
        {
            string adminId = player.GetIdentity().GetPlainId();
            
            // Verifica se o jogador é Super Admin
            if (PNH_CoreConfig.IsSuperAdmin(adminId))
            {
                // Recarrega o ficheiro físico JSON para a memória
                PNH_MissionSettings.Load();
                
                PNH_MissionManager manager = PNH_MissionManager.GetInstance();
                if (manager)
                {
                    // Encerra a missão atual (se houver) e reseta os timers do gestor
                    manager.EndMission();
                    PNH_Utils.SendMessage(player, "[PNH CORE] JSON Recarregado e Ciclo de Missoes Reiniciado.");
                }
            }
            else
            {
                PNH_Utils.SendMessage(player, "[ERRO] Voce nao possui permissao de Admin para isto.");
            }
            return true;
        }

        return false;
    }
}