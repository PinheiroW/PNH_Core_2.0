// --- ESTRUTURAS DO CENÁRIO ---
class PNH_MissionSettings_Barricada {
    string Classe;
    string PosicaoLocal;
    string OrientacaoLocal;
}

class PNH_MissionSettings_Cenario {
    ref array<ref PNH_MissionSettings_Barricada> Barricadas;
    void PNH_MissionSettings_Cenario() { Barricadas = new array<ref PNH_MissionSettings_Barricada>; }
}

// --- ESTRUTURAS GERAIS ---
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

// --- CLASSE PRINCIPAL DO JSON ---
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