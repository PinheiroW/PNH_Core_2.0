class BearHuntMission extends PNH_MissionBase
{
    ItemBase MissionObject;
    Object MissionBuilding;
    int ReqMeatAmount = 4;
    int MsgDlyFinish = 60;
    ref array<vector> ExtendedPosList = new array<vector>;
    ref array<string> InfectedTypes = new array<string>;
    string m_WinnerName = "Desconhecido";
    bool m_IsVictory = false; 

    // --- CONTROLE DO NOVO SISTEMA DE AVISOS ---
    bool m_ProximityWarned = false; 
    bool m_InnerProximityWarned = false; 
    bool m_CompletionMsgSent = false; 

    // O CÉREBRO DA MISSÃO QUE LÊ O JSON
    ref PNH_MissionData_BearHunt m_Config;

    override bool IsExtended() return true;
    
    void BearHuntMission()
    {   
        m_MissionExtended = true;

        // 1. CARREGAMENTO DO JSON
        string caminhoJson = "$profile:PNH/Missions/BearHunt.json";
        m_Config = new PNH_MissionData_BearHunt();
        
        if (!FileExist("$profile:PNH/Missions")) MakeDirectory("$profile:PNH/Missions");
        
        if (FileExist(caminhoJson)) {
            JsonFileLoader<PNH_MissionData_BearHunt>.JsonLoadFile(caminhoJson, m_Config);
        } else {
            PNH_Logger.Error("Missões", "Arquivo BearHunt.json nao encontrado!");
            return;
        }

        // 2. APLICANDO AS CONFIGURAÇÕES
        m_MissionTimeout = m_Config.TempoLimiteSegundos;
        m_MissionZoneOuterRadius = m_Config.RaioAvisoExterno;
        m_MissionZoneInnerRadius = m_Config.RaioAvisoInterno;
        m_MissionInformant = m_Config.Lore.Informante;
        
        m_MissionMessage1 = m_Config.Lore.MensagensRadio[0];
        m_MissionMessage2 = m_Config.Lore.MensagensRadio[1];
        
        // A terceira mensagem junta o texto do JSON com o local dinâmico do mapa
        m_MissionMessage3 = m_Config.Lore.MensagensRadio[2] + "** " + m_MissionLocation + " **.";
    }

    void SpawnRewards()
    {
        if (!m_Config || !m_Config.Recompensas) return;

        // Cria o baú lendo do JSON (ex: "SeaChest", "WoodenCrate")
        ItemBase Chest = ItemBase.Cast(GetGame().CreateObject(m_Config.Recompensas.Container, m_MissionPosition, false, false, false));
        if (Chest)
        {
            // Cria TODOS os itens listados na array do JSON de forma automática
            for (int i = 0; i < m_Config.Recompensas.Itens.Count(); i++)
            {
                Chest.GetInventory().CreateInInventory(m_Config.Recompensas.Itens[i]);
            }
            m_MissionObjects.Insert(Chest);
        }
    }

    override void MissionFinal()
    {   
        if (m_IsVictory) {
            PNH_Logger.Log("Missões", "[PNH_CORE] MISSÃO_CONCLUÍDA: " + m_WinnerName + " extraiu o tecido mutante!");
        }
        m_RewardsSpawned = true; 
        m_MsgNum = -1;
    }

    override bool DeployMission()
    {
        // Trava de segurança Anti-Crash
        if (!m_Config) return false;

        if (m_MissionExtended) {
            PNH_Logger.Log("Missões", "[PNH_CORE] MISSÃO_INICIADA: Caçada Alpha em " + m_MissionLocation);
            return true;
        }
        return false;
    }

    // --- NOVO SISTEMA DE VERIFICAÇÃO E ALERTAS (Estilo Apartment) ---
    override void PlayerChecks(PlayerBase player)
    {
        float distance = vector.Distance(player.GetPosition(), m_MissionPosition);
        PNH_MissionSettingsData config = PNH_MissionSettings.GetData();

        // 1. PROXIMIDADE DO ALVO
        if (!m_ProximityWarned && distance <= m_MissionZoneOuterRadius)
        {
            m_ProximityWarned = true;
            string msg = "Atenção Operador, você está entrando na zona de caça do Alpha. Rastros recentes detectados.";
            if (config.UsarPDA) GetRPCManager().SendRPC("[GearPDA] ", "SendGlobalMessage", new Param2<string, string>(m_MissionInformant, msg), true);
            else PNH_Utils.SendMessageToAll("[RADIO PNH] " + msg);
        }

        // 2. PERIGO IMINENTE
        if (!m_InnerProximityWarned && distance <= m_MissionZoneInnerRadius)
        {
            m_InnerProximityWarned = true;
            string msg2 = "O Espécime Alpha está visual! Fogo livre!";
            if (config.UsarPDA) GetRPCManager().SendRPC("[GearPDA] ", "SendGlobalMessage", new Param2<string, string>(m_MissionInformant, msg2), true);
            else PNH_Utils.SendMessageToAll("[RADIO PNH] " + msg2);
        }

        // 3. MENSAGEM GLOBAL DE SUCESSO E SPAWN DO LOOT
        if (m_IsVictory && !m_CompletionMsgSent)
        {
            m_CompletionMsgSent = true;
            string playerName = player.GetIdentity().GetName();
            m_WinnerName = playerName;

            string successMsg = "[PNH_CORE] SUCESSO: [" + playerName + "] caçou o Alpha e entregou o tecido mutante!";
            
            if (config.UsarPDA) GetRPCManager().SendRPC("[GearPDA] ", "SendGlobalMessage", new Param2<string, string>("Comando PNH", successMsg), true);
            else PNH_Utils.SendMessageToAll("[RADIO PNH] " + successMsg);
            
            // Instancia o baú com as recompensas dinâmicas ao atingir o objetivo
            this.SpawnRewards();
            this.MissionFinal();
        }
    }
}