/// --- Documentação PNH_Core: PNH_ChatManager.c ---
/// Versão do Sistema: 1.2.0 (Fase de Localização Total)
/// Função do arquivo: Processar comandos de texto. Agora 100% localizado, utilizando o ficheiro JSON para todas as respostas de status e informações de missão ao jogador.
/// Comunicação com outros arquivos: Interage com PNH_ProfileManager para dados de perfil e PNH_MissionSettings.c para recuperar as strings traduzíveis.
/// Motivo da existência no sistema: Servir como interface de utilizador via chat, permitindo personalização total das mensagens sem alteração de código.
/// Dependências internas: PNH_Utils.c, PNH_MissionSettings.c e PNH_MissionManager.c.
/// Última atualização: 2026-02-18
/// IMPORTANTE: Ao alterar este arquivo, documente no CHANGELOG_PNH.md

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

        PNH_MissionSettingsData settings = PNH_MissionSettings.GetData();
        if (!settings) return false;

        // =======================================================
        // --- COMANDO: STATUS / PERFIL DO JOGADOR ---
        // =======================================================
        if (command == "status" || command == "perfil")
        {
            if (!player || !player.GetIdentity()) return true;
            
            string plainId = player.GetIdentity().GetPlainId();
            string pName = player.GetIdentity().GetName();
            
            PNH_PlayerProfileData pData = PNH_ProfileManager.LoadProfile(plainId, pName);
            
            if (pData)
            {
                string rankName = "Recruta";
                int nextXP = settings.TabelaXP.XP_Mercenario;
                string nextRankName = "Mercenario";
                
                if (pData.Patente == 2) { rankName = "Mercenario"; nextXP = settings.TabelaXP.XP_Especialista; nextRankName = "Especialista"; }
                else if (pData.Patente == 3) { rankName = "Especialista"; nextXP = settings.TabelaXP.XP_Lenda; nextRankName = "Lenda"; }
                else if (pData.Patente == 4) { rankName = "Lenda"; nextXP = 0; nextRankName = "N/A"; }

                string statusMissao = "NAO";
                if (pData.TemMissaoAtiva) statusMissao = "SIM";

                // Formatação Dinâmica via JSON
                string msgOp = settings.Textos.Msg_StatusOperador; msgOp.Replace("%1", pName);
                string msgPat = settings.Textos.Msg_StatusPatente; msgPat.Replace("%1", rankName);
                string msgXP = settings.Textos.Msg_StatusXP; msgXP.Replace("%1", pData.XP.ToString());
                string msgMiss = settings.Textos.Msg_StatusMissaoAtiva; msgMiss.Replace("%1", statusMissao);

                PNH_Utils.SendMessage(player, "------- PERFIL PNH 2.0 -------");
                PNH_Utils.SendMessage(player, msgOp);
                PNH_Utils.SendMessage(player, msgPat);
                PNH_Utils.SendMessage(player, msgXP);
                
                if (nextXP > 0) 
                {
                    string msgNext = settings.Textos.Msg_StatusProximoNivel;
                    msgNext.Replace("%1", (nextXP - pData.XP).ToString());
                    msgNext.Replace("%2", nextRankName);
                    PNH_Utils.SendMessage(player, msgNext);
                }

                PNH_Utils.SendMessage(player, msgMiss);
                PNH_Utils.SendMessage(player, "------------------------------");
            }
            return true;
        }

        // =======================================================
        // --- COMANDO: CONSULTA DE MISSÃO GERAL ---
        // =======================================================
        if (command == "missao")
        {
            PNH_MissionManager managerMissao = PNH_MissionManager.GetInstance();
            if (managerMissao)
            {
                if (managerMissao.m_MissionState == 1 && managerMissao.m_ActiveMission)
                {
                    string msgDisp = settings.Textos.Msg_MissaoDisponivel;
                    msgDisp.Replace("%1", managerMissao.m_ActiveMission.m_MissionLocation);
                    PNH_Utils.SendMessage(player, msgDisp);
                }
                else if (managerMissao.m_MissionState >= 2)
                {
                    PNH_Utils.SendMessage(player, settings.Textos.Msg_MissaoEmOperacao);
                }
                else
                {
                    PNH_Utils.SendMessage(player, settings.Textos.Msg_SemOperacoes);
                }
            }
            return true;
        }

        // =======================================================
        // --- COMANDO: RESUMO DA MISSÃO (DONO) ---
        // =======================================================
        if (command == "resumo")
        {
            PNH_MissionManager mngrResumo = PNH_MissionManager.GetInstance();
            if (mngrResumo && mngrResumo.m_MissionState >= 2 && mngrResumo.m_ActiveMission)
            {
                if (mngrResumo.m_ActiveMission.IsContractOwner(player))
                {
                    int timeLeftInt = (int)(mngrResumo.m_ActiveMission.m_MissionTimeout - mngrResumo.m_ActiveMission.m_MissionTime);
                    string timeStr = PNH_TimeManager.FormatTime(timeLeftInt);

                    string loc = mngrResumo.m_ActiveMission.m_MissionLocation;

                    PNH_BroadcastManager.GetInstance().SendNotificationToPlayer(player, "OBJETIVO ATUAL", "Alvo: " + loc + "\nTempo: " + timeStr, 10.0);

                    PNH_Utils.SendMessage(player, "=== INTEL DA MISSÃO ===");
                    PNH_Utils.SendMessage(player, "Alvo: " + loc);
                    PNH_Utils.SendMessage(player, "Informante: " + mngrResumo.m_ActiveMission.m_MissionInformant);
                    
                    // A Lore agora vem das Etapas configuradas
                    PNH_Utils.SendMessage(player, "- " + mngrResumo.m_ActiveMission.m_LoreEtapas.Aceitou);
                    PNH_Utils.SendMessage(player, "=======================");
                }
                else
                {
                    PNH_Utils.SendMessage(player, "[ERRO] Voce nao possui o contrato ativo.");
                }
            }
            return true;
        }

        if (command == "aceitar")
        {
            PNH_ContractBroker.GetInstance().TryAcceptContract(player);
            return true;
        }

        if (command == "reload_mission")
        {
            string adminId = player.GetIdentity().GetPlainId();
            if (PNH_CoreConfig.IsSuperAdmin(adminId))
            {
                PNH_MissionManager managerReload = PNH_MissionManager.GetInstance();
                if (managerReload)
                {
                    managerReload.ReloadMissions();
                    managerReload.ForceMissionCycle();
                    PNH_Utils.SendMessage(player, "[PNH CORE] JSON recarregado. Ciclo reiniciado.");
                }
            }
            return true;
        }
        return false;
    }
}