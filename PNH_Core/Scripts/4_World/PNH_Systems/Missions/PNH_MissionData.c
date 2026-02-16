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

// --- ESTRUTURAS ESPECÍFICAS DE RECOMPENSAS ---
class PNH_MissionSettings_Loadout {
    ref array<string> Itens;
    void PNH_MissionSettings_Loadout() { Itens = new array<string>; }
}

class PNH_MissionSettings_RecompensasHorda {
    string Container;
    ref array<ref PNH_MissionSettings_Loadout> Loadouts;
    void PNH_MissionSettings_RecompensasHorda() { Loadouts = new array<ref PNH_MissionSettings_Loadout>; }
}

class PNH_MissionSettings_RecompensasSimples {
    string Container;
    ref array<string> Itens;
    void PNH_MissionSettings_RecompensasSimples() { Itens = new array<string>; }
}

class PNH_MissionSettings_Piloto {
    string Classe;
    ref array<string> Itens;
    void PNH_MissionSettings_Piloto() { Itens = new array<string>; }
}

class PNH_MissionSettings_Alvo {
    string ClasseAnimal;
    int Quantidade;
}

// --- ESTRUTURAS GERAIS DE LORE E DIFICULDADE ---
class PNH_MissionSettings_Lore {
    string Informante;
    ref array<string> NomesSobreviventes;
    ref array<string> MensagensRadio;
    
    // Gatilhos de Texto
    string MensagemAproximacao;       // Gatilho: Raio Externo
    string MensagemNoObjetivo;        // Gatilho: Raio Interno
    string MensagemVitoria;           // Gatilho: Sucesso
    string MensagemFaseB;             // Gatilho: Evento Especial (se houver)

    void PNH_MissionSettings_Lore() {
        NomesSobreviventes = new array<string>;
        MensagensRadio = new array<string>;
    }
}

class PNH_MissionSettings_Dificuldade {
    int QuantidadeHordaFinal;
    ref array<string> ClassesZumbis;
    void PNH_MissionSettings_Dificuldade() { ClassesZumbis = new array<string>; }
}

class PNH_MissionSettings_Locais {
    ref array<string> SpawnsBarril;
    ref array<string> SpawnsZumbis;
    ref array<string> SpawnsFogo;
    void PNH_MissionSettings_Locais() {
        SpawnsBarril = new array<string>;
        SpawnsZumbis = new array<string>;
        SpawnsFogo = new array<string>;
    }
}

// --- DADOS ESPECÍFICOS DAS MISSÕES ---

class PNH_MissionData_Transport {
    bool Ativa;
    float TempoLimiteSegundos;
    float RaioAvisoExterno;
    float RaioAvisoInterno;
    
    string ClasseVeiculo; 
    string ItemCarga;     
    int QuantidadeCargaNecessaria;
    
    string PosicaoEntrega; 
    string CidadeEntrega;  
    
    string ClasseNPCColeta;
    ref array<string> RoupasNPCColeta;
    string ClasseNPCEntrega;
    ref array<string> RoupasNPCEntrega;
    
    ref PNH_MissionSettings_Lore Lore;
    ref PNH_MissionSettings_RecompensasSimples Recompensas;
    
    void PNH_MissionData_Transport() {
        Lore = new PNH_MissionSettings_Lore();
        Recompensas = new PNH_MissionSettings_RecompensasSimples();
        RoupasNPCColeta = new array<string>;
        RoupasNPCEntrega = new array<string>;
    }
}

class PNH_MissionData_Graveyard {
    bool Ativa;
    float TempoLimiteSegundos;
    float RaioAvisoExterno; 
    float RaioAvisoInterno; 
    
    ref PNH_MissionSettings_Lore Lore;
    ref PNH_MissionSettings_Dificuldade Dificuldade;
    ref PNH_MissionSettings_RecompensasHorda Recompensas; 
    
    void PNH_MissionData_Graveyard() {
        Lore = new PNH_MissionSettings_Lore();
        Dificuldade = new PNH_MissionSettings_Dificuldade();
        Recompensas = new PNH_MissionSettings_RecompensasHorda();
    }
}

class PNH_MissionData_CityStore {
    bool Ativa;
    float TempoLimiteSegundos;
    float RaioAvisoExterno; 
    float RaioAvisoInterno; 
    
    string ItemCientifico;  
    int QtdItemCientifico;  
    string ItemEstabilizador; 
    int QtdItemEstabilizador; 
    
    string ClasseBolsa;
    string PosicaoBolsaLocal;
    string OrientacaoBolsaLocal;
    
    string PosicaoEntrega; 
    string CidadeEntrega;
    string ClasseNPC;       
    ref array<string> RoupaNPC; 
    
    ref PNH_MissionSettings_Lore Lore;
    ref PNH_MissionSettings_RecompensasHorda Recompensas;
    ref PNH_MissionSettings_Cenario Cenario; 
    ref PNH_MissionSettings_Dificuldade Dificuldade;
    
    void PNH_MissionData_CityStore() {
        Lore = new PNH_MissionSettings_Lore();
        Recompensas = new PNH_MissionSettings_RecompensasHorda();
        RoupaNPC = new array<string>;
        Cenario = new PNH_MissionSettings_Cenario(); 
        Dificuldade = new PNH_MissionSettings_Dificuldade();
    }
}

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
        Alvo = new PNH_MissionSettings_Alvo();
        Recompensas = new PNH_MissionSettings_RecompensasSimples();
    }
}

class PNH_MissionData_Apartment {
    bool Ativa;
    float TempoLimiteSegundos;
    float RaioAvisoExterno;
    float RaioAvisoInterno;
    string PosicaoRecompensaLocal; 
    
    string ClasseCorpo;
    string PosicaoCorpoLocal;
    string OrientacaoCorpoLocal;
    
    // NOVOS CAMPOS PARA O NPC VIVO E EQUIPADO
    ref array<string> RoupasNPC;
    ref array<string> InventarioNPC;

    ref PNH_MissionSettings_Lore Lore;
    ref PNH_MissionSettings_Dificuldade Dificuldade;
    ref PNH_MissionSettings_RecompensasHorda RecompensasHorda;
    ref PNH_MissionSettings_Cenario Cenario;

    void PNH_MissionData_Apartment() {
        Lore = new PNH_MissionSettings_Lore();
        Dificuldade = new PNH_MissionSettings_Dificuldade();
        RecompensasHorda = new PNH_MissionSettings_RecompensasHorda();
        Cenario = new PNH_MissionSettings_Cenario();
        RoupasNPC = new array<string>;
        InventarioNPC = new array<string>;
    }
}

class PNH_MissionData_PlaneCrash {
    bool Ativa;
    float TempoLimiteSegundos;
    float RaioAvisoExterno;
    float RaioAvisoInterno;
    string ObjetoCentral;

    ref PNH_MissionSettings_Lore Lore;
    ref PNH_MissionSettings_Dificuldade Dificuldade;
    ref PNH_MissionSettings_Locais Locais;
    ref PNH_MissionSettings_RecompensasHorda Recompensas; 

    void PNH_MissionData_PlaneCrash() {
        Lore = new PNH_MissionSettings_Lore();
        Dificuldade = new PNH_MissionSettings_Dificuldade();
        Locais = new PNH_MissionSettings_Locais();
        Recompensas = new PNH_MissionSettings_RecompensasHorda();
    }
}

class PNH_MissionData_Horde {
    bool Ativa;
    float TempoLimiteSegundos;
    float RaioAvisoExterno;
    float RaioAvisoInterno;

    ref PNH_MissionSettings_Lore Lore;
    ref PNH_MissionSettings_Dificuldade Dificuldade;
    ref PNH_MissionSettings_Piloto Piloto;
    ref PNH_MissionSettings_RecompensasHorda RecompensasHorda;

    void PNH_MissionData_Horde() {
        Lore = new PNH_MissionSettings_Lore();
        Dificuldade = new PNH_MissionSettings_Dificuldade();
        Piloto = new PNH_MissionSettings_Piloto();
        RecompensasHorda = new PNH_MissionSettings_RecompensasHorda();
    }
}