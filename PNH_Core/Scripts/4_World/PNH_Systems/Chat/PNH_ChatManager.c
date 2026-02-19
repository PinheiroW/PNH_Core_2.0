/// --- Documentação PNH_Core: PNH_ChatManager.c ---
/// Versão do Sistema: 1.2.2 (Correção de Variáveis e Sincronização de Métodos)
/// Função: Processar comandos de chat e interface administrativa.
/// Correção: Sincronizado com PNH_Utils.c (PlayerName) e PNH_MissionManager.c (ReloadMissions/ForceMissionCycle).

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
                // CORREÇÃO: Utiliza PlayerName para coincidir com a classe em PNH_Utils.c
                msgOp.Replace("%1", pData.PlayerName); 
                PNH_Utils.SendMessage(player, msgOp);

                string msgPat = settings.Textos.Msg_StatusPatente;
                msgPat.Replace("%1", pData.Patente.ToString());
                PNH_Utils.SendMessage(player, msgPat);

                string msgXP = settings.Textos.Msg_StatusXP;
                msgXP.Replace("%1", pData.XP.ToString());
                PNH_Utils.SendMessage(player, msgXP);

                if (pData.TemMissaoAtiva && mngr.m_ActiveMission)
                {
                    PNH_Utils.SendMessage(player, "=== CONTRATO ATIVO ===");
                    PNH_Utils.SendMessage(player, "Alvo: " + mngr.m_ActiveMission.m_MissionLocation);
                    PNH_Utils.SendMessage(player, "Informante: " + mngr.m_ActiveMission.m_MissionInformant);
                    
                    // A Lore é puxada das Etapas configuradas no JSON
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
            if (PNH_CoreConfig.IsSuperAdmin(player.GetIdentity().GetPlainId()))
            {
                PNH_MissionManager manager = PNH_MissionManager.GetInstance();
                if (manager)
                {
                    // Sincronizado com os métodos de suporte do PNH_MissionManager.c v2.1.7
                    manager.ReloadMissions();
                    manager.ForceMissionCycle();
                    PNH_Utils.SendMessage(player, "[PNH CORE] Configuracoes recarregadas e ciclo reiniciado.");
                }
            }
            else
            {
                PNH_Utils.SendMessage(player, "[ERRO] Sem permissao de Admin.");
            }
            return true;
        }

        return false;
    }
}