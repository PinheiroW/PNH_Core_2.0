class PNH_TreasuryManager
{
    // Agora aceita os 5 parâmetros necessários para as missões Apartment e Horde
    static void ProcessMissionReward(string steamID, string playerName, int tier, vector pos = "0 0 0", vector ori = "0 0 0")
    {
        PNH_PlayerProfileData pData = PNH_ProfileManager.LoadProfile(steamID, playerName);
        PNH_MissionSettingsData settings = PNH_MissionSettings.GetData();
        
        if (!pData || !settings) return;

        int rewardXP = 0;
        if (tier == 1)      rewardXP = settings.TabelaXP.XP_Tier_1;
        else if (tier == 2) rewardXP = settings.TabelaXP.XP_Tier_2;
        else if (tier == 3) rewardXP = settings.TabelaXP.XP_Tier_3;
        else                rewardXP = settings.TabelaXP.XP_Tier_4;

        pData.XP += rewardXP;
        pData.TemMissaoAtiva = false;
        pData.ClasseMissaoAtiva = "";
        PNH_ProfileManager.SaveProfile(pData);
        
        if (pos != "0 0 0") GeneratePhysicalReward(pos, ori, tier, settings);

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
            // PROTEÇÃO CONTRA CRASH: Se o nome do item no JSON estiver errado, o servidor ignora em vez de crashar
            if (entry.Item == "" || !GetGame().ConfigIsExisting("CfgVehicles " + entry.Item)) continue;

            if (Math.RandomInt(0, 100) <= entry.Chance)
            {
                for (int i = 0; i < entry.Quantidade; i++) 
                {
                    container.GetInventory().CreateInInventory(entry.Item);
                }
            }
        }
    }
}