/// --- Documentação PNH_Core: PNH_MissionData.c ---
/// Versão do Sistema: 1.0.0 (Ref: PNH_Consts)
/// Função do arquivo: Definir as classes de dados e modelos (Data Models) que estruturam o ficheiro de configuração PNH_MissionSettings.json, incluindo as definições para Tiers, Loot, NPCs, Tabela de XP e os Textos de Interface customizáveis.
/// Comunicação com outros arquivos: Serve como o "esqueleto" de dados utilizado pelo PNH_MissionSettings.c para carregar as configurações e por todos os Managers que precisam de ler regras ou tabelas de loot do JSON.
/// Motivo da existência: Isolar a definição da estrutura de dados da lógica de processamento, permitindo expandir o mod (como adicionar novos parâmetros no JSON) de forma organizada e sem quebrar a lógica dos scripts.
/// Dependências internas: Nenhuma.
/// Última atualização: 2026-02-18
/// IMPORTANTE: Ao alterar este arquivo, documente no CHANGELOG_PNH.md

class PNH_DebugSettings {
    bool DebugMode;
    string DebugMission;
    bool DebugShowInfo;
}

class PNH_ConfiguracoesGerais {
    int TempoEntreMissoesMinutos;
    int TempoLimpezaSegundos;
    int MaxItensNoBarril; // NOVO: Quantidade limite de itens a sortear para o barril
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