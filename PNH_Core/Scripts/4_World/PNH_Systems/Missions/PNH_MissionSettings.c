/// --- Documentação PNH_Core: PNH_MissionSettings.c ---
/// Versão do Sistema: 2.5.1 (Geração Automática: 4 Tiers de Loot e 8 Fases de Lore)
/// Função: Gerir o ciclo de vida do JSON. Cria a estrutura padrão pronta para uso no primeiro boot.
/// Correção: Booleanos configurados como true/false para evitar Access Violation.

class PNH_MissionSettings
{
    protected static string m_ConfigRoot = "$profile:\\PNH\\";
    protected static string m_ConfigPath = m_ConfigRoot + "PNH_MissionSettings.json";
    protected static ref PNH_MissionSettingsData m_ConfigData;

    // Carrega as configurações da pasta profile
    static void Load()
    {
        if (!FileExist(m_ConfigRoot))
        {
            MakeDirectory(m_ConfigRoot);
            PNH_Logger.Log("Settings", "[PNH_CORE] Pasta de configuracao criada em: " + m_ConfigRoot);
        }

        if (!FileExist(m_ConfigPath))
        {
            PNH_Logger.Log("Settings", "[PNH_CORE] Gerando configuracao inicial completa...");
            CreateDefaultConfig();
        }

        if (!m_ConfigData) m_ConfigData = new PNH_MissionSettingsData();
        JsonFileLoader<PNH_MissionSettingsData>.JsonLoadFile(m_ConfigPath, m_ConfigData);
    }

    // Retorna os dados carregados para os outros Managers
    static PNH_MissionSettingsData GetData()
    {
        if (!m_ConfigData) Load();
        return m_ConfigData;
    }

    // Cria o JSON inicial com exemplos de todos os novos sistemas
    static void CreateDefaultConfig()
    {
        m_ConfigData = new PNH_MissionSettingsData();

        // 1. Configurações Gerais (Booleanos Reais)
        m_ConfigData.DebugSettings.DebugMode = true;
        m_ConfigData.DebugSettings.DebugShowInfo = false;
        m_ConfigData.ConfiguracoesGerais.TempoEntreMissoesMinutos = 10;
        m_ConfigData.ConfiguracoesGerais.MaxItensNoBarril = 15;
        m_ConfigData.ConfiguracoesGerais.UsarPDA = false;
        m_ConfigData.ConfiguracoesGerais.ModoRP = true;

        // 2. Tabela de XP (4 Tiers)
        m_ConfigData.TabelaXP.XP_Tier_1 = 100;
        m_ConfigData.TabelaXP.XP_Tier_2 = 250;
        m_ConfigData.TabelaXP.XP_Tier_3 = 500;
        m_ConfigData.TabelaXP.XP_Tier_4 = 1000;

        // 3. Dicionário de Narrativa (EXEMPLO 8 FASES: Fase A + Fase B)
        PNH_DicionarioMissao dicApartment = new PNH_DicionarioMissao("Apartment");
        
        // Fase A: Infiltração
        dicApartment.Etapas.Aceitou = "Infiltracao autorizada. O traidor trancou-se num apartamento barricado.";
        dicApartment.Etapas.Chegou_90m = "Aviso: Sensores detetaram forte presenca de infetados no predio.";
        dicApartment.Etapas.Chegou_20m = "O seu objetivo e localizar o corpo e recuperar o Diario.";
        dicApartment.Etapas.Concluiu = "Diario recuperado! Saia daí imediatamente.";
        
        // Fase B: Extração (Boris)
        dicApartment.Etapas.FaseB_Inicio = "Fase B: Siga para Green Mountain para encontrar o contato Boris.";
        dicApartment.Etapas.FaseB_Chegou_90m = "Aproximacao: Voce esta chegando no ponto de entrega.";
        dicApartment.Etapas.FaseB_Chegou_20m = "Boris avistado. Entregue o livro para liberar o seu barril.";
        dicApartment.Etapas.FaseB_Concluiu = "Missao Concluida! O pagamento foi liberado.";
        
        m_ConfigData.DicionarioMissoes.Insert(dicApartment);

        // 4. Exemplos de Loot por Tier
        PNH_LootItem t1 = new PNH_LootItem(); t1.Item = "SKS"; t1.Chance = 50; t1.Quantidade = 1;
        m_ConfigData.Loot_Tier1.Insert(t1);

        PNH_LootItem t2 = new PNH_LootItem(); t2.Item = "M4A1"; t2.Chance = 40; t2.Quantidade = 1;
        m_ConfigData.Loot_Tier2.Insert(t2);

        // 5. Salva o ficheiro físico
        JsonFileLoader<PNH_MissionSettingsData>.JsonSaveFile(m_ConfigPath, m_ConfigData);
        PNH_Logger.Log("Settings", "[PNH_CORE] PNH_MissionSettings.json gerado com sucesso.");
    }
}