class PNH_MissionSettings
{
    protected static string m_ConfigRoot = "$profile:\\PNH\\";
    protected static string m_ConfigPath = m_ConfigRoot + "PNH_MissionSettings.json";
    protected static ref PNH_MissionSettingsData m_ConfigData;

    static void Load()
    {
        if (!FileExist(m_ConfigRoot))
        {
            MakeDirectory(m_ConfigRoot);
            PNH_Logger.Log("Settings", "[PNH_CORE] Pasta de configuracao criada em: " + m_ConfigRoot);
        }

        if (!FileExist(m_ConfigPath))
        {
            PNH_Logger.Log("Settings", "[PNH_CORE] Ficheiro JSON nao encontrado. Gerando configuracao inicial completa...");
            CreateDefaultConfig();
        }

        if (!m_ConfigData) m_ConfigData = new PNH_MissionSettingsData();
        JsonFileLoader<PNH_MissionSettingsData>.JsonLoadFile(m_ConfigPath, m_ConfigData);
        
        if (m_ConfigData)
        {
            PNH_Logger.Log("Settings", "[PNH_CORE] Configuracoes carregadas com sucesso.");
        }
        else
        {
            PNH_Logger.Log("Settings", "[ERRO] Falha critica ao processar o JSON!");
        }
    }

    static PNH_MissionSettingsData GetData()
    {
        if (!m_ConfigData) Load();
        return m_ConfigData;
    }

    // --- GERAÇÃO DO CONTEÚDO PADRÃO COM TODAS AS FUNÇÕES ATIVAS ---
    static void CreateDefaultConfig()
    {
        m_ConfigData = new PNH_MissionSettingsData();

        // 1. Debug e Configurações Gerais
        m_ConfigData.DebugSettings.DebugMode = true;
        m_ConfigData.DebugSettings.DebugMission = "Horde";
        m_ConfigData.DebugSettings.DebugShowInfo = false;
        m_ConfigData.ConfiguracoesGerais.TempoEntreMissoesMinutos = 1;
        m_ConfigData.ConfiguracoesGerais.TempoLimpezaSegundos = 5;

        // 2. Catálogo de Missões e Tiers
        m_ConfigData.CatalogoMissoes.Guia_MissoesDisponiveis.Insert("Apartment");
        m_ConfigData.CatalogoMissoes.Guia_MissoesDisponiveis.Insert("BearHunt");
        m_ConfigData.CatalogoMissoes.Guia_MissoesDisponiveis.Insert("Horde");
        m_ConfigData.CatalogoMissoes.Guia_MissoesDisponiveis.Insert("Graveyard");
        m_ConfigData.CatalogoMissoes.Guia_MissoesDisponiveis.Insert("CityStore");
        
        m_ConfigData.CatalogoMissoes.Tier1_Recruta.Insert("BearHunt");
        m_ConfigData.CatalogoMissoes.Tier1_Recruta.Insert("Graveyard");
        m_ConfigData.CatalogoMissoes.Tier2_Mercenario.Insert("CityStore");
        m_ConfigData.CatalogoMissoes.Tier2_Mercenario.Insert("Horde");

        // 3. Regras de Contratos (Distância, Tempo, etc)
        m_ConfigData.RegrasContratos.CooldownMinutos = 10;
        m_ConfigData.RegrasContratos.RaioAbandonoMetros = 2000;
        m_ConfigData.RegrasContratos.TempoRecuperacaoPosMorteSegundos = 900;
        m_ConfigData.RegrasContratos.TempoAvisoAbandonoSegundos = 300;
        m_ConfigData.RegrasContratos.MaxSimultaneasBaixas = 3;
        m_ConfigData.RegrasContratos.MaxSimultaneasAltas = 2;

        // 4. Progressão e Tabela de XP
        m_ConfigData.TabelaXP.XP_Tier_1 = 100;
        m_ConfigData.TabelaXP.XP_Tier_2 = 200;
        m_ConfigData.TabelaXP.XP_Tier_3 = 400;
        m_ConfigData.TabelaXP.XP_Tier_4 = 800;
        m_ConfigData.TabelaXP.PerdaCombatLog = 50;
        m_ConfigData.TabelaXP.PerdaTempo = 25;
        m_ConfigData.TabelaXP.XP_Mercenario = 200;
        m_ConfigData.TabelaXP.XP_Especialista = 600;
        m_ConfigData.TabelaXP.XP_Lenda = 1500;

        // 5. Oficiais (NPCs Quest Givers)
        PNH_NPCQuestGiver npc1 = new PNH_NPCQuestGiver();
        npc1.Nome = "Ivan O Coletor";
        npc1.Posicao = "4308.203125 321.532928 5414.665039";
        npc1.Orientacao = "-50.409973 0.000000 0.000000";
        npc1.TiersDisponiveis.Insert(1);
        npc1.Roupas.Insert("TTsKO_Jacket_Cammo");
        npc1.Roupas.Insert("TTsKO_Pants_Cammo");
        m_ConfigData.NPCsQuestGivers.Insert(npc1);

        PNH_NPCQuestGiver npc2 = new PNH_NPCQuestGiver();
        npc2.Nome = "Vlado O Velho";
        npc2.Posicao = "4311.329102 321.532928 5417.958008";
        npc2.Orientacao = "-50.409969 0.000000 0.000000";
        npc2.TiersDisponiveis.Insert(2);
        npc2.Roupas.Insert("HuntingJacket_Autumn");
        npc2.Roupas.Insert("HunterPants_Autumn");
        m_ConfigData.NPCsQuestGivers.Insert(npc2);

        // =======================================================
        // 6. TEXTOS EDITÁVEIS DE INTERFACE (NOVO)
        // =======================================================
        m_ConfigData.Textos.Erro_LongeNPC = "[ERRO PNH] Voce precisa estar perto do Oficial PNH para assinar o contrato!";
        m_ConfigData.Textos.Erro_JaAssinado = "[ERRO PNH] Este contrato ja foi assinado por outro mercenario.";
        m_ConfigData.Textos.Erro_PatenteBaixa = "[ERRO PNH] A tua patente e insuficiente para este contrato!";
        m_ConfigData.Textos.Sucesso_Assinatura = "=== CONTRATO ASSINADO COM SUCESSO ===";

        // 7. Lore Dinâmica
        m_ConfigData.MissaoLore.Insert("Detectamos atividade hostil intensa nesta localidade.");
        m_ConfigData.MissaoLore.Insert("Sua missao e neutralizar as ameacas e garantir o perimetro.");

        // =======================================================
        // 8. LOOT EXCLUSIVO POR TIER + PORCENTAGEM (RNG) E QUANTIDADE
        // =======================================================
        
        // --- Loot Tier 1 (Recruta) ---
        PNH_LootItem loot1_1 = new PNH_LootItem(); loot1_1.Item = "SKS"; loot1_1.Chance = 15; loot1_1.Quantidade = 1;
        PNH_LootItem loot1_2 = new PNH_LootItem(); loot1_2.Item = "Mosin9130"; loot1_2.Chance = 15; loot1_2.Quantidade = 1;
        PNH_LootItem loot1_3 = new PNH_LootItem(); loot1_3.Item = "AmmoBox_762x39_20Rnd"; loot1_3.Chance = 50; loot1_3.Quantidade = 2;
        PNH_LootItem loot1_4 = new PNH_LootItem(); loot1_4.Item = "BandageDressing"; loot1_4.Chance = 100; loot1_4.Quantidade = 3;
        m_ConfigData.Loot_Tier1.Insert(loot1_1);
        m_ConfigData.Loot_Tier1.Insert(loot1_2);
        m_ConfigData.Loot_Tier1.Insert(loot1_3);
        m_ConfigData.Loot_Tier1.Insert(loot1_4);

        // --- Loot Tier 2 (Mercenário) ---
        PNH_LootItem loot2_1 = new PNH_LootItem(); loot2_1.Item = "M4A1"; loot2_1.Chance = 10; loot2_1.Quantidade = 1;
        PNH_LootItem loot2_2 = new PNH_LootItem(); loot2_2.Item = "AKM"; loot2_2.Chance = 10; loot2_2.Quantidade = 1;
        PNH_LootItem loot2_3 = new PNH_LootItem(); loot2_3.Item = "Mag_STANAG_30Rnd"; loot2_3.Chance = 40; loot2_3.Quantidade = 2;
        PNH_LootItem loot2_4 = new PNH_LootItem(); loot2_4.Item = "TacticalBaconCan"; loot2_4.Chance = 100; loot2_4.Quantidade = 4;
        m_ConfigData.Loot_Tier2.Insert(loot2_1);
        m_ConfigData.Loot_Tier2.Insert(loot2_2);
        m_ConfigData.Loot_Tier2.Insert(loot2_3);
        m_ConfigData.Loot_Tier2.Insert(loot2_4);

        // Salva o arquivo fisicamente
        JsonFileLoader<PNH_MissionSettingsData>.JsonSaveFile(m_ConfigPath, m_ConfigData);
        PNH_Logger.Log("Settings", "[PNH_CORE] PNH_MissionSettings.json criado com sucesso (Textos Customizaveis e Loot Avancado Ativos).");
    }

    static void Reload()
    {
        m_ConfigData = null;
        Load();
    }
}