// =======================================================
// --- ESTRUTURA DE DADOS PNH CORE 2.0 (MODULAR) ---
// =======================================================

class PNH_DebugSettings {
    bool DebugMode;
    string DebugMission;
    bool DebugShowInfo;
}

class PNH_ConfiguracoesGerais {
    int TempoEntreMissoesMinutos;
    int TempoLimpezaSegundos;
    bool UsarPDA;
    bool ModoRP;
}

class PNH_CatalogoMissoes {
    ref array<string> Guia_MissoesDisponiveis;
    ref array<string> Tier1_Recruta;
    ref array<string> Tier2_Mercenario;
    ref array<string> Tier3_Especialista;
    ref array<string> Tier4_Lenda;

    void PNH_CatalogoMissoes() {
        Guia_MissoesDisponiveis = new array<string>;
        Tier1_Recruta = new array<string>;
        Tier2_Mercenario = new array<string>;
        Tier3_Especialista = new array<string>;
        Tier4_Lenda = new array<string>;
    }
}

class PNH_RegrasContratos {
    int CooldownMinutos;
    int RaioAbandonoMetros;
    int TempoRecuperacaoPosMorteSegundos;
    int TempoAvisoAbandonoSegundos;
    int MaxSimultaneasBaixas;
    int MaxSimultaneasAltas;
}

class PNH_TabelaXP {
    int XP_Tier_1;
    int XP_Tier_2;
    int XP_Tier_3;
    int XP_Tier_4;
    int PerdaCombatLog;
    int PerdaTempo;
    int XP_Recruta;
    int XP_Mercenario;
    int XP_Especialista;
    int XP_Lenda;
}

class PNH_NPCQuestGiver {
    string Nome;
    string Posicao;
    string Orientacao;
    ref array<int> TiersDisponiveis;
    ref array<string> Roupas;

    void PNH_NPCQuestGiver() {
        TiersDisponiveis = new array<int>;
        Roupas = new array<string>;
    }
}

class PNH_LootItem {
    string Item;
    int Chance;
    int Quantidade;
}

// --- NOVA CLASSE PARA OS TEXTOS EDITÁVEIS ---
class PNH_TextosInterface {
    string Erro_LongeNPC;
    string Erro_JaAssinado;
    string Erro_PatenteBaixa;
    string Sucesso_Assinatura;
}

class PNH_MissionSettingsData {
    ref PNH_DebugSettings DebugSettings;
    ref PNH_ConfiguracoesGerais ConfiguracoesGerais;
    ref PNH_CatalogoMissoes CatalogoMissoes;
    ref PNH_RegrasContratos RegrasContratos;
    ref PNH_TabelaXP TabelaXP;
    ref array<ref PNH_NPCQuestGiver> NPCsQuestGivers;
    
    // Adicionamos a variável dos Textos aqui
    ref PNH_TextosInterface Textos;
    
    ref array<string> MissaoLore;
    
    ref array<ref PNH_LootItem> Loot_Tier1;
    ref array<ref PNH_LootItem> Loot_Tier2;
    ref array<ref PNH_LootItem> Loot_Tier3;
    ref array<ref PNH_LootItem> Loot_Tier4;

    void PNH_MissionSettingsData() {
        DebugSettings = new PNH_DebugSettings();
        ConfiguracoesGerais = new PNH_ConfiguracoesGerais();
        CatalogoMissoes = new PNH_CatalogoMissoes();
        RegrasContratos = new PNH_RegrasContratos();
        TabelaXP = new PNH_TabelaXP();
        NPCsQuestGivers = new array<ref PNH_NPCQuestGiver>;
        
        Textos = new PNH_TextosInterface();
        
        MissaoLore = new array<string>;
        Loot_Tier1 = new array<ref PNH_LootItem>;
        Loot_Tier2 = new array<ref PNH_LootItem>;
        Loot_Tier3 = new array<ref PNH_LootItem>;
        Loot_Tier4 = new array<ref PNH_LootItem>;
    }
}