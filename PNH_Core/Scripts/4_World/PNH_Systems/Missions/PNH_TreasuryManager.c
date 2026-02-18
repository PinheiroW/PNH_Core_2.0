/// --- Documentação PNH_Core: PNH_TreasuryManager.c ---
/// Versão do Sistema: 1.2.0 (Fase de Localização Total)
/// Função do arquivo: Gerir a atribuição de recompensas (XP) aos jogadores. Agora atualizado para utilizar mensagens de recompensa dinâmicas vindas do ficheiro de configuração JSON.
/// Comunicação com outros arquivos: Chamado pelas classes de missão (ex: Horde.c). Consulta PNH_MissionSettings.c para obter valores de XP e textos de interface.
/// Motivo da existência no sistema: Centralizar a lógica de progressão e garantir que as notificações de pagamento sejam totalmente customizáveis.
/// Dependências internas: PNH_ProfileManager (Utils.c) e PNH_MissionSettings.c.
/// Última atualização: 2026-02-18
/// IMPORTANTE: Ao alterar este arquivo, documente no CHANGELOG_PNH.md

class PNH_TreasuryManager
{
    static void ProcessMissionReward(string steamID, string playerName, int tier)
    {
        // 1. Carrega o perfil e as configurações de forma segura
        PNH_PlayerProfileData pData = PNH_ProfileManager.LoadProfile(steamID, playerName);
        PNH_MissionSettingsData settings = PNH_MissionSettings.GetData();
        
        if (pData && settings)
        {
            int rewardXP = 0;
            
            // 2. Define o valor baseado no Tier
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
            
            // 5. Notifica o jogador usando os textos localizados do JSON
            PlayerBase player = PNH_Utils.GetPlayerByName(playerName);
            if (player)
            {
                // Recupera a string do JSON e substitui o marcador %1 pelo valor do XP
                string msgLiquidado = settings.Textos.Msg_ContratoLiquidado;
                msgLiquidado.Replace("%1", rewardXP.ToString());

                PNH_Utils.SendMessage(player, "------------------------------------------");
                PNH_Utils.SendMessage(player, msgLiquidado);
                PNH_Utils.SendMessage(player, "------------------------------------------");
            }
            
            PNH_Logger.Log("Treasury", "[PNH] Recompensa de " + rewardXP + " XP processada para " + playerName);
        }
    }
}