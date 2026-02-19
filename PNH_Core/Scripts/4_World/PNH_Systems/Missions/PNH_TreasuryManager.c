/// --- Documentação PNH_Core: PNH_TreasuryManager.c ---
/// Versão do Sistema: 2.0.0 (Agente de Pagamento Centralizado: XP + Loot Físico por Tier)
/// Função: Único ponto de distribuição de recompensas. Gere o XP no perfil e materializa o barril de loot sorteado.

class PNH_TreasuryManager
{
    // Função principal chamada ao concluir qualquer missão
    static void ProcessMissionReward(string steamID, string playerName, int tier, vector pos = "0 0 0", vector ori = "0 0 0")
    {
        PNH_PlayerProfileData pData = PNH_ProfileManager.LoadProfile(steamID, playerName);
        PNH_MissionSettingsData settings = PNH_MissionSettings.GetData();
        
        if (!pData || !settings) return;

        // 1. PROCESSAMENTO DE XP (Baseado no Tier)
        int rewardXP = 0;
        if (tier == 1)      rewardXP = settings.TabelaXP.XP_Tier_1;
        else if (tier == 2) rewardXP = settings.TabelaXP.XP_Tier_2;
        else if (tier == 3) rewardXP = settings.TabelaXP.XP_Tier_3;
        else                rewardXP = settings.TabelaXP.XP_Tier_4;

        pData.XP += rewardXP;
        pData.TemMissaoAtiva = false;
        pData.ClasseMissaoAtiva = "";
        PNH_ProfileManager.SaveProfile(pData);
        
        // 2. MATERIALIZAÇÃO DO BARRIL DE RECOMPENSA (Se houver posição válida)
        if (pos != "0 0 0")
        {
            GeneratePhysicalReward(pos, ori, tier, settings);
        }

        // 3. NOTIFICAÇÃO DO JOGADOR
        PlayerBase player = PNH_Utils.GetPlayerByName(playerName);
        if (player)
        {
            string msgLiquidado = settings.Textos.Msg_ContratoLiquidado;
            msgLiquidado.Replace("%1", rewardXP.ToString());

            PNH_Utils.SendMessage(player, "------------------------------------------");
            PNH_Utils.SendMessage(player, msgLiquidado);
            PNH_Utils.SendMessage(player, "------------------------------------------");
        }
        
        PNH_Logger.Log("Treasury", "[PNH] Contrato Tier " + tier + " liquidado para " + playerName + ". XP: +" + rewardXP);
    }

    // Lógica interna para criar o barril e sortear o loot
    protected static void GeneratePhysicalReward(vector pos, vector ori, int tier, PNH_MissionSettingsData settings)
    {
        // Cria o Barril Azul (Padrão do Projeto)
        EntityAI container = EntityAI.Cast(GetGame().CreateObject("Barrel_Blue", pos));
        if (!container) return;

        container.SetOrientation(ori);

        // Seleciona a lista de loot correta baseada no Tier
        array<ref PNH_LootItem> selectedLootPool;
        if (tier == 1)      selectedLootPool = settings.Loot_Tier1;
        else if (tier == 2) selectedLootPool = settings.Loot_Tier2;
        else if (tier == 3) selectedLootPool = settings.Loot_Tier3;
        else                selectedLootPool = settings.Loot_Tier4;

        if (!selectedLootPool || selectedLootPool.Count() == 0) return;

        int itemsSpawned = 0;
        int maxItems = settings.ConfiguracoesGerais.MaxItensNoBarril;

        // Sorteia os itens da pool correspondente
        foreach (PNH_LootItem lootEntry : selectedLootPool)
        {
            if (itemsSpawned >= maxItems) break;

            int roll = Math.RandomInt(0, 100);
            if (roll <= lootEntry.Chance)
            {
                for (int i = 0; i < lootEntry.Quantidade; i++)
                {
                    container.GetInventory().CreateInInventory(lootEntry.Item);
                    itemsSpawned++;
                    if (itemsSpawned >= maxItems) break;
                }
            }
        }
        
        PNH_Logger.Log("Treasury", "[PNH] Barril de Recompensa gerado no Tier " + tier + " com " + itemsSpawned + " itens.");
    }
}