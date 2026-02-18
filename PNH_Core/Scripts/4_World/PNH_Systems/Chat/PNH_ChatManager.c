/// --- Documentação PNH_Core: PNH_ChatManager.c ---
/// Versão do Sistema: 1.0.0 (Ref: PNH_Consts)
/// Função do arquivo: Processar, validar e executar comandos de texto enviados pelos jogadores no chat (como !status, !missao, !resumo, !aceitar e !reload_mission).
/// Comunicação com outros arquivos: Interage com o PNH_ProfileManager (Utils.c) para exibir perfis, PNH_MissionManager.c para consultar o estado das missões, PNH_ContractBroker.c para assinatura de contratos e PNH_BroadcastManager.c para disparar notificações visuais na tela.
/// Motivo da existência do arquivo no sistema: Atuar como a interface de comando e controlo (UI via Chat) entre o utilizador e o ecossistema de missões do PNH_Core.
/// Dependências internas: PNH_Utils.c (mensagens e perfis), PNH_MissionSettings.c (dados de XP/Patente), PNH_MissionManager.c (estado global) e PNH_CoreConfig.c (validação de SuperAdmins).
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
        // --- COMANDO: CONSULTA DE MISSÃO GERAL ---
        // =======================================================
        if (command == "missao")
        {
            PNH_MissionManager managerMissao = PNH_MissionManager.GetInstance();
            if (managerMissao)
            {
                if (managerMissao.m_MissionState == 1 && managerMissao.m_ActiveMission)
                {
                    PNH_Utils.SendMessage(player, "[COMANDO PNH] Temos um contrato disponivel na regiao de: " + managerMissao.m_ActiveMission.m_MissionLocation);
                    PNH_Utils.SendMessage(player, "Va ate ao local e use !aceitar perto do oficial.");
                }
                else if (managerMissao.m_MissionState >= 2)
                {
                    PNH_Utils.SendMessage(player, "[COMANDO PNH] Ja existe um esquadrao em operacao no momento. Aguarde nova janela.");
                }
                else
                {
                    PNH_Utils.SendMessage(player, "[COMANDO PNH] Sem operacoes no momento. Aguardando Intel...");
                }
            }
            return true;
        }

        // =======================================================
        // --- NOVO COMANDO: RESUMO DA MISSÃO (EXCLUSIVO DO DONO) ---
        // =======================================================
        if (command == "resumo")
        {
            PNH_MissionManager mngrResumo = PNH_MissionManager.GetInstance();
            if (mngrResumo && mngrResumo.m_MissionState >= 2 && mngrResumo.m_ActiveMission)
            {
                if (mngrResumo.m_ActiveMission.IsContractOwner(player))
                {
                    // CORREÇÃO: Converte o tempo para Inteiro (int) antes de usar o Módulo (%)
                    int timeLeftInt = (int)(mngrResumo.m_ActiveMission.m_MissionTimeout - mngrResumo.m_ActiveMission.m_MissionTime);
                    int minutes = timeLeftInt / 60;
                    int seconds = timeLeftInt % 60;
                    string timeStr = minutes.ToStringLen(2) + ":" + seconds.ToStringLen(2);

                    string loc = mngrResumo.m_ActiveMission.m_MissionLocation;

                    // Envia um Popup na Tela usando a nossa nova função
                    PNH_BroadcastManager.GetInstance().SendNotificationToPlayer(player, "OBJETIVO ATUAL", "Alvo: " + loc + "\nTempo Restante: " + timeStr, 10.0);

                    // Imprime a História no Chat
                    PNH_Utils.SendMessage(player, "=== INTEL DA MISSÃO ===");
                    PNH_Utils.SendMessage(player, "Alvo: " + loc);
                    PNH_Utils.SendMessage(player, "Informante: " + mngrResumo.m_ActiveMission.m_MissionInformant);
                    for (int i = 0; i < mngrResumo.m_ActiveMission.m_MissionMessages.Count(); i++)
                    {
                        PNH_Utils.SendMessage(player, "- " + mngrResumo.m_ActiveMission.m_MissionMessages.Get(i));
                    }
                    PNH_Utils.SendMessage(player, "=======================");
                }
                else
                {
                    PNH_Utils.SendMessage(player, "[ERRO] Voce nao possui o contrato ativo desta missao.");
                }
            }
            else
            {
                PNH_Utils.SendMessage(player, "[ERRO] Nenhuma missao ativa no momento.");
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
            string adminId = player.GetIdentity().GetPlainId();
            if (PNH_CoreConfig.IsSuperAdmin(adminId))
            {
                PNH_MissionManager managerReload = PNH_MissionManager.GetInstance();
                if (managerReload)
                {
                    managerReload.ReloadMissions();
                    managerReload.ForceMissionCycle();
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