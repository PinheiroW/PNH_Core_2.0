/// --- Documentação PNH_Core: PNH_Utils.c ---
/// Versão do Sistema: 1.1.0 (Refatoração de Progressão)
/// Função do arquivo: Centralizar funções utilitárias estáticas e gerir o sistema de perfis (PNH_PlayerProfileData), incluindo agora a lógica de promoção automática com notificações visuais.
/// Comunicação com outros arquivos: Providencia dados de perfil para o PNH_ChatManager.c, valida recompensas para o PNH_TreasuryManager.c e utiliza o PNH_BroadcastManager.c para anúncios de patente[cite: 21, 28, 15].
/// Motivo da existência: Garantir a persistência de progresso e fornecer ferramentas que simplificam a comunicação entre os Managers.
/// Dependências internas: PNH_MissionSettings.c (para consulta da TabelaXP) e PNH_BroadcastManager.c (para notificações de promoção)[cite: 31, 15].
/// Última atualização: 2026-02-18
/// IMPORTANTE: Ao alterar este arquivo, documente no CHANGELOG_PNH.md

class PNH_Utils
{
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
        PNH_PlayerProfileData data = LoadProfile(player.GetIdentity().GetPlainId(), player.GetIdentity().GetName());
        if (!data) return;
        data.XP += amount;
        CheckRankUpdate(data, player);
        SaveProfile(data);
    }

    static void RemoveXP(PlayerBase player, int amount) {
        if (!player || !player.GetIdentity()) return;
        PNH_PlayerProfileData data = LoadProfile(player.GetIdentity().GetPlainId(), player.GetIdentity().GetName());
        if (!data) return;
        data.XP -= amount;
        if (data.XP < 0) data.XP = 0; 
        CheckRankUpdate(data, player); 
        SaveProfile(data);
    }

    // --- LOGICA DE PROGRESSÃO REFORMULADA ---
    static void CheckRankUpdate(PNH_PlayerProfileData data, PlayerBase player) {
        PNH_MissionSettingsData settings = PNH_MissionSettings.GetData();
        if (!settings || !player) return;
        
        int oldRank = data.Patente;
        int newRank = 1;

        if (data.XP >= settings.TabelaXP.XP_Lenda) newRank = 4;
        else if (data.XP >= settings.TabelaXP.XP_Especialista) newRank = 3;
        else if (data.XP >= settings.TabelaXP.XP_Mercenario) newRank = 2;
        else newRank = 1;

        // Se houver mudança de patente, notifica o jogador
        if (newRank != oldRank)
        {
            data.Patente = newRank;
            string rankName = "Recruta";
            if (newRank == 2) rankName = "Mercenario";
            else if (newRank == 3) rankName = "Especialista";
            else if (newRank == 4) rankName = "Lenda";

            PNH_BroadcastManager.GetInstance().SendNotificationToPlayer(player, "PROMOÇÃO PNH", "Parabens! Sua patente subiu para: " + rankName, 10.0, "Notifications/gui/data/warning.edds");
            PNH_Logger.Log("Profile", "[PNH] Jogador " + data.PlayerName + " subiu para a patente: " + rankName);
        }
    }
    
    static int GetPlayerRank(PlayerBase player) {
        if (!player || !player.GetIdentity()) return 1;
        PNH_PlayerProfileData data = LoadProfile(player.GetIdentity().GetPlainId(), player.GetIdentity().GetName());
        if (data) return data.Patente;
        return 1;
    }
}