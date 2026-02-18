/// --- Documentação PNH_Core: PNH_Utils.c ---
/// Versão do Sistema: 1.0.0 (Ref: PNH_Consts)
///[cite_start]/// Função do arquivo: Centralizar funções utilitárias estáticas para envio de mensagens (individuais e globais) e busca de jogadores, além de conter a implementação completa do sistema de perfis (PNH_PlayerProfileData) e o gestor de perfis (PNH_ProfileManager) que controla o XP e as Patentes[cite: 31].
///[cite_start]/// Comunicação com outros arquivos: É o ficheiro com maior rede de comunicação do mod, sendo invocado pelo PNH_ChatManager.c para comandos de perfil, pelo PNH_TreasuryManager.c para atribuição de recompensas e pelo PNH_Init.c para carregar dados ao conectar jogadores[cite: 21, 28, 30].
///[cite_start]/// Motivo da existência: Garantir que a persistência de progresso dos mercenários (XP e Patente) seja feita de forma padronizada via JSON e fornecer ferramentas de ajuda que simplificam o código nos outros Managers[cite: 31].
///[cite_start]/// Dependências internas: PNH_MissionSettings.c (necessário para o PNH_ProfileManager consultar a TabelaXP e validar subidas de patente)[cite: 31].
/// Última atualização: 2026-02-18
/// IMPORTANTE: Ao alterar este arquivo, documente no CHANGELOG_PNH.md

class PNH_Utils
{
    // --- NOVA FUNÇÃO: Enviar para TODOS ---
    static void SendMessageToAll(string message)
    {
        array<Man> players = new array<Man>;
        GetGame().GetPlayers(players);

        foreach(Man p : players)
        {
            PlayerBase pb = PlayerBase.Cast(p);
            if(pb)
            {
                SendMessage(pb, message);
            }
        }
    }

    // Enviar Individual
    static void SendMessage(PlayerBase player, string message)
    {
        if (player && player.GetIdentity())
        {
            Param1<string> m = new Param1<string>("[PNH] " + message);
            GetGame().RPCSingleParam(player, ERPCs.RPC_USER_ACTION_MESSAGE, m, true, player.GetIdentity());
        }
    }
    
    static string GetSteamID(PlayerBase player)
    {
        if (player && player.GetIdentity()) return player.GetIdentity().GetPlainId();
        return "";
    }
    
    static string GetName(PlayerBase player)
    {
        if (player && player.GetIdentity()) return player.GetIdentity().GetName();
        return "";
    }

    static PlayerBase GetPlayerByName(string name)
    {
        if (name == "") return null;
        
        name.ToLower();
        array<Man> players = new array<Man>;
        GetGame().GetPlayers(players);
        
        for (int i = 0; i < players.Count(); i++)
        {
            PlayerBase pb = PlayerBase.Cast(players.Get(i));
            if (pb && pb.GetIdentity())
            {
                string pName = pb.GetIdentity().GetName();
                pName.ToLower();
                if (pName.Contains(name)) return pb;
            }
        }
        return null;
    }
}

// =========================================================
// SISTEMA DE PERFIS PNH 2.0 (Adicionado no Utils para garantir leitura)
// =========================================================

class PNH_PlayerProfileData {
    string PlayerName;
    string SteamID;
    int XP;
    int Patente; // 1 = Recruta, 2 = Mercenário, 3 = Especialista, 4 = Lenda
    bool TemMissaoAtiva;
    string ClasseMissaoAtiva;

    void PNH_PlayerProfileData(string name, string id) {
        PlayerName = name;
        SteamID = id;
        XP = 0;
        Patente = 1; 
        TemMissaoAtiva = false;
        ClasseMissaoAtiva = "";
    }
}

class PNH_ProfileManager {
    static string m_ProfilesDir = "$profile:PNH/PlayerData/";

    static PNH_PlayerProfileData LoadProfile(string id, string name) {
        if (id == "") return null;

        string filePath = m_ProfilesDir + id + ".json";
        
        if (!FileExist(m_ProfilesDir)) MakeDirectory(m_ProfilesDir);

        PNH_PlayerProfileData data = new PNH_PlayerProfileData(name, id);

        if (FileExist(filePath)) {
            JsonFileLoader<PNH_PlayerProfileData>.JsonLoadFile(filePath, data);
            
            if (data.PlayerName != name) {
                data.PlayerName = name;
                SaveProfile(data);
            }
        } else {
            SaveProfile(data);
        }

        return data;
    }

    static void SaveProfile(PNH_PlayerProfileData data) {
        if (!data) return;
        string filePath = m_ProfilesDir + data.SteamID + ".json";
        JsonFileLoader<PNH_PlayerProfileData>.JsonSaveFile(filePath, data);
    }

    static void AddXP(PlayerBase player, int amount) {
        if (!player || !player.GetIdentity()) return;
        // CORREÇÃO: Usando GetPlainId()
        PNH_PlayerProfileData data = LoadProfile(player.GetIdentity().GetPlainId(), player.GetIdentity().GetName());
        if (!data) return;

        data.XP += amount;
        CheckRankUpdate(data, player);
        SaveProfile(data);
    }

    static void RemoveXP(PlayerBase player, int amount) {
        if (!player || !player.GetIdentity()) return;
        // CORREÇÃO: Usando GetPlainId()
        PNH_PlayerProfileData data = LoadProfile(player.GetIdentity().GetPlainId(), player.GetIdentity().GetName());
        if (!data) return;

        data.XP -= amount;
        if (data.XP < 0) data.XP = 0; 
        
        CheckRankUpdate(data, player); 
        SaveProfile(data);
    }

    static void CheckRankUpdate(PNH_PlayerProfileData data, PlayerBase player) {
        PNH_MissionSettingsData settings = PNH_MissionSettings.GetData();
        if (!settings) return;
        
        if (data.XP >= settings.TabelaXP.XP_Lenda) data.Patente = 4;
        else if (data.XP >= settings.TabelaXP.XP_Especialista) data.Patente = 3;
        else if (data.XP >= settings.TabelaXP.XP_Mercenario) data.Patente = 2;
        else data.Patente = 1;
    }
    
    static int GetPlayerRank(PlayerBase player) {
        if (!player || !player.GetIdentity()) return 1;
        // CORREÇÃO: Usando GetPlainId()
        PNH_PlayerProfileData data = LoadProfile(player.GetIdentity().GetPlainId(), player.GetIdentity().GetName());
        if (data) return data.Patente;
        return 1;
    }
}