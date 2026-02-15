class PNH_MissionConfig
{
    string Nome;
    string Tipo;       // Camp, Horde, Apartment, etc.
    string Local;
    vector Posicao;
    string Informante;
    ref array<string> Mensagens;
    ref array<ref PNH_ObjectConfig> Objetos;
    ref array<ref PNH_LootConfig> Loot;

    void PNH_MissionConfig()
    {
        Mensagens = new array<string>;
        Objetos = new array<ref PNH_ObjectConfig>;
        Loot = new array<ref PNH_LootConfig>;
    }
}

class PNH_ObjectConfig
{
    string Classe;
    vector PosicaoRelativa;
    bool Abrir; // Para tendas ou portas

    void PNH_ObjectConfig(string c, vector p, bool a = false)
    {
        Classe = c;
        PosicaoRelativa = p;
        Abrir = a;
    }
}

class PNH_LootConfig
{
    string ClasseItem;
    int Quantidade;
    int IndiceAlvo; // 0 para o centro, ou o índice do objeto na lista

    void PNH_LootConfig(string c, int q, int idx)
    {
        ClasseItem = c;
        Quantidade = q;
        IndiceAlvo = idx;
    }
}