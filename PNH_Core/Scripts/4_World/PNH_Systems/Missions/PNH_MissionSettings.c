// --- SUB-CLASSES DE ORGANIZAÇÃO DO JSON ---
class PNH_MissionSettings_Debug {
    bool DebugMode;
    string DebugMission;
    bool DebugShowInfo;
}

class PNH_MissionSettings_Geral {
    int TempoEntreMissoesMinutos;
    int TempoLimpezaSegundos;
    bool UsarPDA;
    bool ModoRP;
}

class PNH_MissionSettings_Contratos {
    int CooldownMinutos;
    int RaioAbandonoMetros;
    int TempoRecuperacaoPosMorteSegundos;
    int TempoAvisoAbandonoSegundos;
    int MaxSimultaneasBaixas; // Tiers 1 e 2
    int MaxSimultaneasAltas;  // Tiers 3 e 4
}

class PNH_MissionSettings_TabelaXP {
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

class PNH_MissionSettings_NPC {
    string Nome;
    string Posicao;
    string Orientacao;
    ref array<int> TiersDisponiveis;
    ref array<string> Roupas;
    
    void PNH_MissionSettings_NPC() {
        TiersDisponiveis = new array<int>;
        Roupas = new array<string>;
    }
}

// NOVO: A sua ideia de separar por Tiers
class PNH_MissionSettings_CatalogoMissoes {
    ref array<string> Guia_MissoesDisponiveis; // Apenas informativo
    ref array<string> Tier1_Recruta;
    ref array<string> Tier2_Mercenario;
    ref array<string> Tier3_Especialista;
    ref array<string> Tier4_Lenda;

    void PNH_MissionSettings_CatalogoMissoes() {
        Guia_MissoesDisponiveis = new array<string>;
        Tier1_Recruta = new array<string>;
        Tier2_Mercenario = new array<string>;
        Tier3_Especialista = new array<string>;
        Tier4_Lenda = new array<string>;
    }
}

// --- CLASSE PRINCIPAL DOS DADOS MESTRES ---
class PNH_MissionSettingsData {
    ref PNH_MissionSettings_Debug DebugSettings; 
    ref PNH_MissionSettings_Geral ConfiguracoesGerais;
    
    // --- NOVOS DADOS PNH 2.0 ---
    ref PNH_MissionSettings_CatalogoMissoes CatalogoMissoes; // Substitui o antigo MissoesAtivas
    ref PNH_MissionSettings_Contratos RegrasContratos;
    ref PNH_MissionSettings_TabelaXP TabelaXP;
    ref array<ref PNH_MissionSettings_NPC> NPCsQuestGivers;

    void PNH_MissionSettingsData() {
        DebugSettings = new PNH_MissionSettings_Debug();
        ConfiguracoesGerais = new PNH_MissionSettings_Geral();
        
        CatalogoMissoes = new PNH_MissionSettings_CatalogoMissoes();
        RegrasContratos = new PNH_MissionSettings_Contratos();
        TabelaXP = new PNH_MissionSettings_TabelaXP();
        NPCsQuestGivers = new array<ref PNH_MissionSettings_NPC>;
    }
}

// --- GERENCIADOR DO ARQUIVO ---
class PNH_MissionSettings {
    static ref PNH_MissionSettingsData m_Data;

    static void Load() {
        string dirPath = "$profile:PNH/";
        string filePath = dirPath + "PNH_MissionSettings.json";

        if (!FileExist(dirPath)) MakeDirectory(dirPath);

        m_Data = new PNH_MissionSettingsData();

        if (FileExist(filePath)) {
            JsonFileLoader<PNH_MissionSettingsData>.JsonLoadFile(filePath, m_Data);
        } else {
            // Valores Default se o JSON não existir
            m_Data.DebugSettings.DebugMode = false;
            m_Data.ConfiguracoesGerais.TempoEntreMissoesMinutos = 5;
            
            // --- POPULANDO O CATÁLOGO (SUA IDEIA) ---
            m_Data.CatalogoMissoes.Guia_MissoesDisponiveis.Insert("Copie os nomes abaixo para os Tiers:");
            m_Data.CatalogoMissoes.Guia_MissoesDisponiveis.Insert("Apartment");
            m_Data.CatalogoMissoes.Guia_MissoesDisponiveis.Insert("BearHunt");
            m_Data.CatalogoMissoes.Guia_MissoesDisponiveis.Insert("Horde");
            m_Data.CatalogoMissoes.Guia_MissoesDisponiveis.Insert("PlaneCrash");
            m_Data.CatalogoMissoes.Guia_MissoesDisponiveis.Insert("Transport");
            m_Data.CatalogoMissoes.Guia_MissoesDisponiveis.Insert("Graveyard");
            m_Data.CatalogoMissoes.Guia_MissoesDisponiveis.Insert("CityStore");

            // Distribuindo (Exemplo base)
            m_Data.CatalogoMissoes.Tier1_Recruta.Insert("BearHunt");
            m_Data.CatalogoMissoes.Tier1_Recruta.Insert("Graveyard");
            
            m_Data.CatalogoMissoes.Tier2_Mercenario.Insert("CityStore");
            m_Data.CatalogoMissoes.Tier2_Mercenario.Insert("Horde");
            
            m_Data.CatalogoMissoes.Tier3_Especialista.Insert("Apartment");
            m_Data.CatalogoMissoes.Tier3_Especialista.Insert("PlaneCrash");
            
            m_Data.CatalogoMissoes.Tier4_Lenda.Insert("Transport"); // Operação Prometeu

            // --- REGRAS DE CONTRATO ---
            m_Data.RegrasContratos.CooldownMinutos = 30;
            m_Data.RegrasContratos.RaioAbandonoMetros = 2000;
            m_Data.RegrasContratos.TempoRecuperacaoPosMorteSegundos = 900;
            m_Data.RegrasContratos.TempoAvisoAbandonoSegundos = 300;
            m_Data.RegrasContratos.MaxSimultaneasBaixas = 3;
            m_Data.RegrasContratos.MaxSimultaneasAltas = 2;
            
            // --- TABELA DE XP ---
            m_Data.TabelaXP.XP_Tier_1 = 100;
            m_Data.TabelaXP.XP_Tier_2 = 200;
            m_Data.TabelaXP.XP_Tier_3 = 300;
            m_Data.TabelaXP.XP_Tier_4 = 400;
            m_Data.TabelaXP.PerdaCombatLog = 50;
            m_Data.TabelaXP.PerdaTempo = 25;
            m_Data.TabelaXP.XP_Recruta = 0;
            m_Data.TabelaXP.XP_Mercenario = 200;
            m_Data.TabelaXP.XP_Especialista = 600;
            m_Data.TabelaXP.XP_Lenda = 1500;
            
            // --- PREENCHENDO OS 3 NPCS ---
            // NPC 1: Green Mountain (Tier 1)
            PNH_MissionSettings_NPC npc1 = new PNH_MissionSettings_NPC();
            npc1.Nome = "Ivan O Coletor";
            npc1.Posicao = "4308.203125 321.532928 5414.665039";
            npc1.Orientacao = "-50.409973 0.000000 0.000000";
            npc1.TiersDisponiveis.Insert(1);
            npc1.Roupas.Insert("TTsKO_Jacket_Cammo");
            npc1.Roupas.Insert("TTsKO_Pants_Cammo");
            m_Data.NPCsQuestGivers.Insert(npc1);

            // NPC 2: Green Mountain (Tier 2)
            PNH_MissionSettings_NPC npc2 = new PNH_MissionSettings_NPC();
            npc2.Nome = "Vlado O Velho";
            npc2.Posicao = "4311.329102 321.532928 5417.958008"; // A sua segunda coordenada
            npc2.Orientacao = "-50.409969 0.000000 0.000000";
            npc2.TiersDisponiveis.Insert(2);
            npc2.Roupas.Insert("HuntingJacket_Autumn");
            npc2.Roupas.Insert("HunterPants_Autumn");
            m_Data.NPCsQuestGivers.Insert(npc2);

            // NPC 3: Radio Zenit (Tier 3 e 4)
            PNH_MissionSettings_NPC npc3 = new PNH_MissionSettings_NPC();
            npc3.Nome = "Dr. Legasov";
            npc3.Posicao = "8078.318848 459.208160 9311.061523"; // Zenit
            npc3.Orientacao = "133.602814 0.000000 -0.000000";
            npc3.TiersDisponiveis.Insert(3);
            npc3.TiersDisponiveis.Insert(4);
            npc3.Roupas.Insert("LabCoat");
            npc3.Roupas.Insert("NBC_Pants_Grey");
            m_Data.NPCsQuestGivers.Insert(npc3);
            
            JsonFileLoader<PNH_MissionSettingsData>.JsonSaveFile(filePath, m_Data);
        }
    }

    static PNH_MissionSettingsData GetData() {
        if (!m_Data) Load();
        return m_Data;
    }
}