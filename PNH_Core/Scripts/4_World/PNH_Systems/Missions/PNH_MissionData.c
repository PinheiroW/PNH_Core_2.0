/// --- Documentação PNH_Core: PNH_MissionData.c ---
/// Versão do Sistema: 2.3.0 (Estrutura de Diálogos por Fases A e B)
/// Função do arquivo: Definir as classes de dados e modelos (Data Models) para o ficheiro PNH_MissionSettings.json e configurações individuais de missões (Ex: Apartment.json).

// =========================================================================
// 1. CLASSES PARA MISSÕES DE INFILTRAÇÃO E NARRATIVAS (Apartment.json)
// =========================================================================

class PNH_CenarioBarricada 
{
    string Classe;
    string PosicaoLocal;
    string OrientacaoLocal;
}

class PNH_CenarioConfig 
{
    ref array<ref PNH_CenarioBarricada> Barricadas;
    
    void PNH_CenarioConfig() 
    {
        Barricadas = new array<ref PNH_CenarioBarricada>;
    }
}

class PNH_DificuldadeConfig
{
    int QuantidadeHordaFinal;
    ref array<string> ClassesZumbis;

    void PNH_DificuldadeConfig()
    {
        ClassesZumbis = new array<string>;
    }
}

class PNH_RecompensaLoadout
{
    ref array<string> Itens;

    void PNH_RecompensaLoadout()
    {
        Itens = new array<string>;
    }
}

class PNH_RecompensasConfig
{
    string Container;
    ref array<ref PNH_RecompensaLoadout> Loadouts;

    void PNH_RecompensasConfig()
    {
        Loadouts = new array<ref PNH_RecompensaLoadout>;
    }
}

class PNH_LoreApartment 
{
    string Informante;
    ref array<string> MensagensFaseA; // Os 3 textos da infiltração
    ref array<string> MensagensFaseB; // Os 3 textos da extração
    string MensagemVitoria;

    void PNH_LoreApartment()
    {
        MensagensFaseA = new array<string>;
        MensagensFaseB = new array<string>;
    }
}

// Modelo mestre para carregar ficheiros de missão individuais
class PNH_MissionConfigData 
{
    bool Ativa;
    float TempoLimiteSegundos;
    float RaioAvisoExterno;
    float RaioAvisoInterno;
    string ClasseCorpo;
    string PosicaoCorpoLocal;
    string OrientacaoCorpoLocal;
    string ClasseZumbiAssassino;
    string PosicaoZumbiAssassinoLocal;
    ref array<string> RoupasNPC;
    ref array<string> InventarioNPC;
    ref array<string> SpawnsZumbisInternos;
    string ItemMissao;
    string PosicaoEntrega;
    string PosicaoBarrilEntrega;
    string OrientacaoBarrilEntrega;
    string CidadeEntrega;
    string ClasseNPCEntrega;
    ref array<string> RoupasNPCEntrega;
    ref PNH_LoreApartment Lore;
    ref PNH_DificuldadeConfig Dificuldade;
    ref PNH_CenarioConfig Cenario;
    ref PNH_RecompensasConfig RecompensasHorda;

    void PNH_MissionConfigData()
    {
        RoupasNPC = new array<string>;
        InventarioNPC = new array<string>;
        SpawnsZumbisInternos = new array<string>;
        RoupasNPCEntrega = new array<string>;
        Lore = new PNH_LoreApartment();
        Dificuldade = new PNH_DificuldadeConfig();
        Cenario = new PNH_CenarioConfig();
        RecompensasHorda = new PNH_RecompensasConfig();
    }
}

// =========================================================================
// 2. CLASSES DO SISTEMA DE CONFIGURAÇÃO GLOBAL (PNH_MissionSettings.json)
// =========================================================================

class PNH_DebugSettings {
    bool DebugMode;
    string DebugMission;
    bool DebugShowInfo;
}

class PNH_ConfiguracoesGerais {
    int TempoEntreMissoesMinutos;
    int TempoLimpezaSegundos;
    int MaxItensNoBarril; 
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

class PNH_TextosInterface {
    string Erro_LongeNPC;
    string Erro_JaAssinado;
    string Erro_PatenteBaixa;
    string Sucesso_Assinatura;
    string Msg_ContratoLiquidado;
    string Msg_StatusOperador;
    string Msg_StatusPatente;
    string Msg_StatusXP;
    string Msg_StatusProximoNivel;
    string Msg_StatusMissaoAtiva;
    string Msg_MissaoDisponivel;
    string Msg_MissaoEmOperacao;
    string Msg_SemOperacoes;
}

class PNH_LoreEtapas {
    string Aceitou;
    string Chegou_90m;
    string Chegou_20m;
    string Concluiu;
}

class PNH_DicionarioMissao {
    string TipoMissao;
    ref PNH_LoreEtapas Etapas;

    void PNH_DicionarioMissao(string tipo) {
        TipoMissao = tipo;
        Etapas = new PNH_LoreEtapas();
    }
}

class PNH_MissionSettingsData {
    ref PNH_DebugSettings DebugSettings;
    ref PNH_ConfiguracoesGerais ConfiguracoesGerais;
    ref PNH_CatalogoMissoes CatalogoMissoes;
    ref PNH_RegrasContratos RegrasContratos;
    ref PNH_TabelaXP TabelaXP;
    ref array<ref PNH_NPCQuestGiver> NPCsQuestGivers;
    ref PNH_TextosInterface Textos;
    ref array<ref PNH_DicionarioMissao> DicionarioMissoes; 
    
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
        DicionarioMissoes = new array<ref PNH_DicionarioMissao>;
        
        Loot_Tier1 = new array<ref PNH_LootItem>;
        Loot_Tier2 = new array<ref PNH_LootItem>;
        Loot_Tier3 = new array<ref PNH_LootItem>;
        Loot_Tier4 = new array<ref PNH_LootItem>;
    }
}