// --- NOVA ESTRUTURA PNH 2.0: INFORMAÇÕES DO CONTRATO ---
class PNH_MissionSettings_ContratoInfo {
    int Tier;               // Nível da missão (1, 2, 3 ou 4)
    int BonusXP;            // XP extra manual
    int ChanceEpica;        // Chance % de virar lendária (0 a 100)
    string DescricaoLore;   // O "Briefing" enviado ao HUD do jogador
}

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

// --- ESTRUTURA PARA POSIÇÕES EXATAS DE ITENS ---
class PNH_MissionSettings_ItemPos {
    string Classe;
    string Posicao;
    string Orientacao;
}

// --- NOVA ESTRUTURA DE ROTAS (Para Missão de Transporte) ---
class PNH_MissionSettings_Rota {
    string NomeDestino;
    string StartPos;
    vector StartPosVec; // Auxiliar para conversão
    string EndPos;
    vector EndPosVec;   // Auxiliar para conversão
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
    
    string MensagemAproximacao;       
    string MensagemNoObjetivo;        
    string MensagemVitoria;           
    
    string MensagemFaseB;             
    string MensagemFaseC; 
    string MensagemFaseD; 
    string MensagemAproximacaoEntrega;
    string MensagemNoObjetivoEntrega; 

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

// --- DADOS: MISSÃO TRANSPORTE ---
class PNH_MissionData_Transport {
    bool Ativa;
    float TempoLimiteSegundos;
    ref PNH_MissionSettings_ContratoInfo Contrato;
    
    // Campos necessários para a lógica do Transport.c
    string ClasseObjeto; 
    ref array<ref PNH_MissionSettings_Rota> Rotas;

    // Fases originais da Operação Prometeu (Mantidas para compatibilidade futura)
    string CidadeFaseA;
    string ClasseVeiculo;
    string PosicaoVeiculo;
    string OrientacaoVeiculo;
    ref array<ref PNH_MissionSettings_ItemPos> PecasVeiculo;
    int ZumbisFaseA;
    
    string CidadeFaseB;
    string PosicaoGalpaoFaseB; 
    string ClasseMaleta;
    ref array<ref PNH_MissionSettings_ItemPos> PosicoesMaletas;
    string ClasseLivroArteDaGuerra;
    string PosicaoLivroArteDaGuerra;
    string OrientacaoLivroArteDaGuerra;
    string ClasseFerramenta;
    string PosicaoFerramenta;
    string OrientacaoFerramenta;
    int ZumbisFaseB;
    
    string CidadeFaseC;
    string ClasseCorpo;
    string PosicaoCorpo;
    string OrientacaoCorpo;
    string ClasseZumbiAssassino;
    string PosicaoZumbiAssassino;
    string ClasseLivroBiblia;
    string PosicaoLivroBiblia;
    string OrientacaoLivroBiblia;
    int ZumbisFaseC;
    
    string CidadeFaseD;
    string ClasseNPC;
    string PosicaoNPC;
    string OrientacaoNPC;
    ref array<string> RoupasNPC;
    string PosicaoBarril;
    
    ref PNH_MissionSettings_Lore Lore;
    ref PNH_MissionSettings_Dificuldade Dificuldade;
    ref PNH_MissionSettings_RecompensasHorda Recompensas;
    
    void PNH_MissionData_Transport() {
        Contrato = new PNH_MissionSettings_ContratoInfo();
        Rotas = new array<ref PNH_MissionSettings_Rota>; // Inicialização obrigatória
        PecasVeiculo = new array<ref PNH_MissionSettings_ItemPos>;
        PosicoesMaletas = new array<ref PNH_MissionSettings_ItemPos>;
        RoupasNPC = new array<string>;
        Lore = new PNH_MissionSettings_Lore();
        Dificuldade = new PNH_MissionSettings_Dificuldade();
        Recompensas = new PNH_MissionSettings_RecompensasHorda();
    }
}

class PNH_MissionData_Graveyard {
    bool Ativa;
    float TempoLimiteSegundos;
    float RaioAvisoExterno; 
    float RaioAvisoInterno; 
    ref PNH_MissionSettings_ContratoInfo Contrato;
    
    ref PNH_MissionSettings_Lore Lore;
    ref PNH_MissionSettings_Dificuldade Dificuldade;
    ref PNH_MissionSettings_RecompensasHorda Recompensas; 
    
    void PNH_MissionData_Graveyard() {
        Contrato = new PNH_MissionSettings_ContratoInfo();
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
    ref PNH_MissionSettings_ContratoInfo Contrato;
    
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
        Contrato = new PNH_MissionSettings_ContratoInfo();
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
    ref PNH_MissionSettings_ContratoInfo Contrato;

    ref PNH_MissionSettings_Lore Lore;
    ref PNH_MissionSettings_Alvo Alvo;
    ref PNH_MissionSettings_RecompensasSimples Recompensas;

    void PNH_MissionData_BearHunt() {
        Contrato = new PNH_MissionSettings_ContratoInfo();
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
    ref PNH_MissionSettings_ContratoInfo Contrato;
    
    string PosicaoRecompensaLocal; 
    string ClasseCorpo;
    string PosicaoCorpoLocal;
    string OrientacaoCorpoLocal;
    
    ref array<string> RoupasNPC;
    ref array<string> InventarioNPC;
    string ClasseZumbiAssassino;
    string PosicaoZumbiAssassinoLocal;
    ref array<string> SpawnsZumbisInternos;

    string ItemMissao;
    string PosicaoEntrega;
    string CidadeEntrega;
    string ClasseNPCEntrega;
    ref array<string> RoupasNPCEntrega;
    
    string PosicaoBarrilEntrega;
    string OrientacaoBarrilEntrega;

    ref PNH_MissionSettings_Lore Lore;
    ref PNH_MissionSettings_Dificuldade Dificuldade;
    ref PNH_MissionSettings_RecompensasHorda RecompensasHorda;
    ref PNH_MissionSettings_Cenario Cenario;

    void PNH_MissionData_Apartment() {
        Contrato = new PNH_MissionSettings_ContratoInfo();
        Lore = new PNH_MissionSettings_Lore();
        Dificuldade = new PNH_MissionSettings_Dificuldade();
        RecompensasHorda = new PNH_MissionSettings_RecompensasHorda();
        Cenario = new PNH_MissionSettings_Cenario();
        
        RoupasNPC = new array<string>;
        InventarioNPC = new array<string>;
        SpawnsZumbisInternos = new array<string>;
        RoupasNPCEntrega = new array<string>;
    }
}

class PNH_MissionData_PlaneCrash {
    bool Ativa;
    float TempoLimiteSegundos;
    float RaioAvisoExterno;
    float RaioAvisoInterno;
    string ObjetoCentral;
    ref PNH_MissionSettings_ContratoInfo Contrato;

    ref PNH_MissionSettings_Lore Lore;
    ref PNH_MissionSettings_Dificuldade Dificuldade;
    ref PNH_MissionSettings_Locais Locais;
    ref PNH_MissionSettings_RecompensasHorda Recompensas; 

    void PNH_MissionData_PlaneCrash() {
        Contrato = new PNH_MissionSettings_ContratoInfo();
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
    ref PNH_MissionSettings_ContratoInfo Contrato;

    ref PNH_MissionSettings_Lore Lore;
    ref PNH_MissionSettings_Dificuldade Dificuldade;
    ref PNH_MissionSettings_Piloto Piloto;
    ref PNH_MissionSettings_RecompensasHorda RecompensasHorda;

    void PNH_MissionData_Horde() {
        Contrato = new PNH_MissionSettings_ContratoInfo();
        Lore = new PNH_MissionSettings_Lore();
        Dificuldade = new PNH_MissionSettings_Dificuldade();
        Piloto = new PNH_MissionSettings_Piloto();
        RecompensasHorda = new PNH_MissionSettings_RecompensasHorda();
    }
}