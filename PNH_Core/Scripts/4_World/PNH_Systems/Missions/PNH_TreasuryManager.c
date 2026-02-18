/// --- Documentação PNH_Core: PNH_TreasuryManager.c ---
/// Versão do Sistema: 1.0.0 (Ref: PNH_Consts)
/// Função do arquivo: Gerir a atribuição de recompensas intangíveis (XP) aos jogadores, processando a liquidação de contratos e atualizando o estado de missão no perfil do mercenário.
/// Comunicação com outros arquivos: É chamado pelas classes de missão (ex: Horde.c) no momento da conclusão da operação para pagar o mercenário.
/// Motivo da existência no sistema: Isolar a lógica financeira e de progressão, garantindo que a recompensa de XP seja aplicada de forma segura e registada nos logs.
/// Dependências internas: PNH_ProfileManager (Utils.c) para manipulação de dados do jogador e PNH_MissionSettings.c para consulta da tabela de XP por Tier.
/// Última atualização: 2026-02-18
/// IMPORTANTE: Ao alterar este arquivo, documente no CHANGELOG_PNH.md

class PNH_TreasuryManager
{
    // Função estática: pode ser chamada de qualquer lugar sem precisar instanciar
    static void ProcessMissionReward(string steamID, string playerName, int tier)
    {
        // 1. Carrega o perfil de forma segura
        PNH_PlayerProfileData pData = PNH_ProfileManager.LoadProfile(steamID, playerName);
        PNH_MissionSettingsData settings = PNH_MissionSettings.GetData();
        
        if (pData && settings)
        {
            int rewardXP = 0;
            
            // 2. Define o valor baseado no Tier (atualmente focado no Tier 1)
            if (tier == 1) rewardXP = settings.TabelaXP.XP_Tier_1;
            else if (tier == 2) rewardXP = settings.TabelaXP.XP_Tier_2;
            else if (tier == 3) rewardXP = settings.TabelaXP.XP_Tier_3;
            else rewardXP = settings.TabelaXP.XP_Tier_4;

            // 3. Aplica a recompensa e reseta o status de missão no perfil
            pData.XP += rewardXP;
            pData.TemMissaoAtiva = false;
            pData.ClasseMissaoAtiva = "";
            
            // 4. Salva o perfil atualizado
            PNH_ProfileManager.SaveProfile(pData);
            
            // 5. Notifica o jogador (Busca o objeto PlayerBase apenas para a mensagem)
            PlayerBase player = PNH_Utils.GetPlayerByName(playerName);
            if (player)
            {
                PNH_Utils.SendMessage(player, "------------------------------------------");
                PNH_Utils.SendMessage(player, "[PNH] CONTRATO LIQUIDADO: +" + rewardXP + " XP");
                PNH_Utils.SendMessage(player, "------------------------------------------");
            }
            
            PNH_Logger.Log("Treasury", "[PNH] Recompensa de " + rewardXP + " XP processada para " + playerName);
        }
    }
}