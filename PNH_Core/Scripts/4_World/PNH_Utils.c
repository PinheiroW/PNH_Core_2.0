/// --- Documentação PNH_Core: PNH_Utils.c ---
/// Versão do Sistema: 1.1.1 (Sincronização Total com Chat e Treasury)
/// Função: Centralizar funções utilitárias e gerir os perfis dos jogadores (.json).
/// Correção: Garantida a variável 'PlayerName' para compatibilidade com ChatManager v1.2.2.

class PNH_Utils
{
    // Envia mensagem para todos os jogadores online
    static void SendMessageToAll(string message)
    {
        array<Man> players = new array<Man>;
        GetGame().GetPlayers(players);

        foreach(Man p : players)
        {
            PlayerBase pb = PlayerBase.Cast(p);
            if(pb) SendMessage(pb, message);
        }
    }

    // Envia mensagem via RPC (Chat) para um jogador específico
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

    // Procura um jogador online pelo nome (parcial)
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

// ESTRUTURA DE DADOS DO PERFIL (Garantir 'PlayerName' para o ChatManager)
class PNH_PlayerProfileData {
    string PlayerName;
    string SteamID;
    int XP;
    int Patente; 
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

// GESTOR DE FICHEIROS DE PERFIL
class PNH_ProfileManager {
    static string m_ProfilesDir = "$profile:PNH/PlayerData/";

    static PNH_PlayerProfileData LoadProfile(string id, string name) {
        if (id == "") return null;
        string filePath = m_ProfilesDir + id + ".json";
        if (!FileExist(m_ProfilesDir)) MakeDirectory(m_ProfilesDir);
        
        PNH_PlayerProfileData data = new PNH_PlayerProfileData(name, id);
        if (FileExist(filePath)) {
            JsonFileLoader<PNH_PlayerProfileData>.JsonLoadFile(filePath, data);
            // Atualiza o nome se o jogador mudou de nome na Steam
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

    // LÓGICA DE PROMOÇÃO AUTOMÁTICA
    static void CheckRankUpdate(PNH_PlayerProfileData data, PlayerBase player) {
        PNH_MissionSettingsData settings = PNH_MissionSettings.GetData();
        if (!settings || !player) return;
        
        int oldRank = data.Patente;
        int newRank = 1;

        // Valida contra os valores de XP definidos no PNH_MissionSettings.json
        if (data.XP >= settings.TabelaXP.XP_Lenda) newRank = 4;
        else if (data.XP >= settings.TabelaXP.XP_Especialista) newRank = 3;
        else if (data.XP >= settings.TabelaXP.XP_Mercenario) newRank = 2;
        else newRank = 1;

        if (newRank != oldRank)
        {
            data.Patente = newRank;
            string rankName = "Recruta";
            if (newRank == 2)      rankName = "Mercenario";
            else if (newRank == 3) rankName = "Especialista";
            else if (newRank == 4) rankName = "Lenda";

            PNH_BroadcastManager.GetInstance().SendNotificationToPlayer(player, "PROMOÇÃO PNH", "Parabens! Sua patente subiu para: " + rankName, 10.0, "Notifications/gui/data/warning.edds");
            PNH_Logger.Log("Profile", "[PNH] Operador " + data.PlayerName + " promovido a " + rankName);
        }
    }
}