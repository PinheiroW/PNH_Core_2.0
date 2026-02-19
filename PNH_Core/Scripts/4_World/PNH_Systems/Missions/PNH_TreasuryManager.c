/// --- Documentação PNH_Core: PNH_TreasuryManager.c ---
/// Versão do Sistema: 2.1.6 (Correção de Assinatura do Motor Enforce)
/// Função: Único ponto de distribuição de recompensas (XP + Loot Físico).

class PNH_TreasuryManager
{
    // CORREÇÃO CRÍTICA: Removidos os valores padrão ("0 0 0") que quebravam a leitura do compilador
    static void ProcessMissionReward(string steamID, string playerName, int tier, vector pos, vector ori)
    {
        PNH_PlayerProfileData pData = PNH_ProfileManager.LoadProfile(steamID, playerName);
        PNH_MissionSettingsData settings = PNH_MissionSettings.GetData();
        
        if (!pData || !settings) return;

        // 1. Processamento de XP baseado no Tier configurado no JSON
        int rewardXP = 0;
        if (tier == 1)      rewardXP = settings.TabelaXP.XP_Tier_1;
        else if (tier == 2) rewardXP = settings.TabelaXP.XP_Tier_2;
        else if (tier == 3) rewardXP = settings.TabelaXP.XP_Tier_3;
        else                rewardXP = settings.TabelaXP.XP_Tier_4;

        pData.XP += rewardXP;
        pData.TemMissaoAtiva = false;
        pData.ClasseMissaoAtiva = "";
        PNH_ProfileManager.SaveProfile(pData);
        
        // 2. Materialização do Barril (Usando a constante vector.Zero nativa do DayZ)
        if (pos != vector.Zero)
        {
            GeneratePhysicalReward(pos, ori, tier, settings);
        }

        // 3. Notificação ao Jogador
        PlayerBase player = PNH_Utils.GetPlayerByName(playerName);
        if (player)
        {
            string msg = settings.Textos.Msg_ContratoLiquidado;
            msg.Replace("%1", rewardXP.ToString());
            PNH_Utils.SendMessage(player, msg);
        }
    }

    protected static void GeneratePhysicalReward(vector pos, vector ori, int tier, PNH_MissionSettingsData settings)
    {
        EntityAI container = EntityAI.Cast(GetGame().CreateObject("Barrel_Blue", pos));
        if (!container) return;

        container.SetOrientation(ori);

        array<ref PNH_LootItem> pool;
        if (tier == 1)      pool = settings.Loot_Tier1;
        else if (tier == 2) pool = settings.Loot_Tier2;
        else if (tier == 3) pool = settings.Loot_Tier3;
        else                pool = settings.Loot_Tier4;

        if (!pool) return;

        foreach (PNH_LootItem entry : pool)
        {
            // PROTEÇÃO CONTRA CRASH: Verifica se o item existe na config do jogo antes de o criar
            if (entry.Item != "" && (GetGame().ConfigIsExisting("CfgVehicles " + entry.Item) || GetGame().ConfigIsExisting("CfgWeapons " + entry.Item) || GetGame().ConfigIsExisting("CfgMagazines " + entry.Item)))
            {
                if (Math.RandomInt(0, 100) <= entry.Chance)
                {
                    for (int i = 0; i < entry.Quantidade; i++) 
                    {
                        container.GetInventory().CreateInInventory(entry.Item);
                    }
                }
            }
            else if (entry.Item != "")
            {
                PNH_Logger.Log("Treasury", "[ERRO] Nome de item invalido no JSON: " + entry.Item);
            }
        }
    }
}