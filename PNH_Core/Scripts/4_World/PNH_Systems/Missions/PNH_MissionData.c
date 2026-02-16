// --- ESTRUTURAS DO CENÁRIO E BARRICADAS ---
class PNH_MissionSettings_Barricada {
    string Classe;
    string PosicaoLocal;
    string OrientacaoLocal;
}

class PNH_MissionSettings_Cenario {
    ref array<ref PNH_MissionSettings_Barricada> Barricadas;
    void PNH_MissionSettings_Cenario() { Barricadas = new array<ref PNH_MissionSettings_Barricada>; }
}

// --- ESTRUTURAS ESPECÍFICAS DA HORDA ---
class PNH_MissionSettings_Loadout {
    ref array<string> Itens;
}

class PNH_MissionSettings_RecompensasHorda {
    string Container;
    ref array<ref PNH_MissionSettings_Loadout> Loadouts;
    void PNH_MissionSettings_RecompensasHorda() { Loadouts = new array<ref PNH_MissionSettings_Loadout>; }
}

class PNH_MissionSettings_Piloto {
    string Classe;
    ref array<string> Itens;
}

// --- ESTRUTURA DO NOVO ALVO DA CAÇADA ---
class PNH_MissionSettings_Alvo {
    string ClasseAnimal;
    int Quantidade;
}

// --- ESTRUTURAS GERAIS DE LORE E DIFICULDADE ---
class PNH_MissionSettings_Lore {
    string Informante;
    ref array<string> NomesSobreviventes;
    ref array<string> MensagensRadio;
}

class PNH_MissionSettings_Dificuldade {
    int QuantidadeHordaFinal;
    ref array<string> ClassesZumbis;
}

class PNH_MissionSettings_Locais {
    ref array<string> SpawnsPredio;
    ref array<string> SpawnsBarril;
    ref array<string> SpawnsZumbis;
}

// --- ESTRUTURAS DE RECOMPENSA ---
class PNH_MissionSettings_RecompensasSimples {
    string Container;
    ref array<string> Itens;
}

// ======================================================
// CLASSES PRINCIPAIS DE CADA MISSÃO (LIGADAS AO JSON)
// ======================================================

// 1. DADOS DA MISSÃO APARTMENT
class PNH_MissionData_Apartment {
    bool Ativa;
    float TempoLimiteSegundos;
    float RaioAvisoExterno;
    float RaioAvisoInterno;
    
    ref PNH_MissionSettings_Lore Lore;
    ref PNH_MissionSettings_Dificuldade Dificuldade;
    ref PNH_MissionSettings_Locais Locais;
    ref PNH_MissionSettings_Cenario Cenario;
    
    void PNH_MissionData_Apartment() {
        Lore = new PNH_MissionSettings_Lore();
        Lore.NomesSobreviventes = new array<string>;
        Lore.MensagensRadio = new array<string>;
        
        Dificuldade = new PNH_MissionSettings_Dificuldade();
        Dificuldade.ClassesZumbis = new array<string>;
        
        Locais = new PNH_MissionSettings_Locais();
        Locais.SpawnsPredio = new array<string>;
        Locais.SpawnsBarril = new array<string>;
        Locais.SpawnsZumbis = new array<string>;
        
        Cenario = new PNH_MissionSettings_Cenario();
    }
}

// 2. DADOS DA MISSÃO BEAR HUNT
class PNH_MissionData_BearHunt {
    bool Ativa;
    float TempoLimiteSegundos;
    float RaioAvisoExterno;
    float RaioAvisoInterno;
    
    ref PNH_MissionSettings_Lore Lore;
    ref PNH_MissionSettings_Alvo Alvo; 
    ref PNH_MissionSettings_RecompensasSimples Recompensas;
    
    void PNH_MissionData_BearHunt() {
        Lore = new PNH_MissionSettings_Lore();
        Lore.MensagensRadio = new array<string>;
        
        Alvo = new PNH_MissionSettings_Alvo(); 
        
        Recompensas = new PNH_MissionSettings_RecompensasSimples();
        Recompensas.Itens = new array<string>;
    }
}

// 3. DADOS DA MISSÃO HORDE (A QUE FALTAVA)
class PNH_MissionData_Horde {
    bool Ativa;
    float TempoLimiteSegundos;
    float RaioAvisoExterno;
    float RaioAvisoInterno;
    float RaioVitoria; 
    
    ref PNH_MissionSettings_Lore Lore;
    ref PNH_MissionSettings_Dificuldade Dificuldade;
    ref PNH_MissionSettings_Piloto Piloto;
    ref PNH_MissionSettings_RecompensasHorda RecompensasHorda;
    
    void PNH_MissionData_Horde() {
        Lore = new PNH_MissionSettings_Lore();
        Lore.MensagensRadio = new array<string>;
        
        Dificuldade = new PNH_MissionSettings_Dificuldade();
        Dificuldade.ClassesZumbis = new array<string>;

        Piloto = new PNH_MissionSettings_Piloto();
        Piloto.Itens = new array<string>;
        
        RecompensasHorda = new PNH_MissionSettings_RecompensasHorda();
    }
}