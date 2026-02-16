class PlaneCrashMission extends PNH_MissionBase
{
    Object MissionBuilding;
    string m_WinnerName = "Desconhecido";
    bool m_IsVictory = false; 

    // --- CONTROLE DO NOVO SISTEMA DE AVISOS ---
    bool m_ProximityWarned = false; 
    bool m_InnerProximityWarned = false; 

    // O CÉREBRO DA MISSÃO QUE LÊ O JSON
    ref PNH_MissionData_PlaneCrash m_Config;

    override bool IsExtended() { return false; } 
    
    void PlaneCrashMission()
    {
        string caminhoJson = "$profile:PNH/Missions/PlaneCrash.json";
        m_Config = new PNH_MissionData_PlaneCrash();
        
        if (!FileExist("$profile:PNH/Missions")) MakeDirectory("$profile:PNH/Missions");
        
        if (FileExist(caminhoJson)) {
            JsonFileLoader<PNH_MissionData_PlaneCrash>.JsonLoadFile(caminhoJson, m_Config);
        } else {
            PNH_Logger.Error("Missões", "Arquivo PlaneCrash.json nao encontrado!");
            return;
        }

        m_MissionTimeout = m_Config.TempoLimiteSegundos;
        m_MissionZoneOuterRadius = m_Config.RaioAvisoExterno;
        m_MissionZoneInnerRadius = m_Config.RaioAvisoInterno;
        m_MissionInformant = m_Config.Lore.Informante;
        
        m_MissionMessage1 = m_Config.Lore.MensagensRadio[0];
        m_MissionMessage2 = m_Config.Lore.MensagensRadio[1];
    }

    void SpawnObjectsAndAIs()
    {
        if (!m_Config || !m_Config.Locais) return;

        // 1. SPAWN DOS FOGOS (Acendendo as fogueiras)
        if (m_Config.Locais.SpawnsFogo)
        {
            for (int f = 0; f < m_Config.Locais.SpawnsFogo.Count(); f++)
            {
                vector fPos = MissionBuilding.ModelToWorld(m_Config.Locais.SpawnsFogo[f].ToVector());
                fPos[1] = GetGame().SurfaceY(fPos[0], fPos[2]);
                
                ItemBase Fire = ItemBase.Cast(GetGame().CreateObject("Fireplace", fPos, false, true, true));
                if (Fire) {
                    Fire.GetInventory().CreateInInventory("Firewood");
                    Fire.GetInventory().CreateInInventory("WoodenStick");
                    Fire.GetInventory().CreateInInventory("Rag");
                    
                    // CORREÇÃO FINAL: Para fogueiras, usamos o SwitchOn no Energy Manager
                    if (Fire.GetCompEM()) 
                    {
                        Fire.GetCompEM().SwitchOn();
                    }
                    
                    m_MissionObjects.Insert(Fire);
                }
            }
        }

        // 2. SPAWN DAS CAIXAS COM LOOT ALEATÓRIO
        if (m_Config.Locais.SpawnsBarril && m_Config.Recompensas)
        {
            for (int c = 0; c < m_Config.Locais.SpawnsBarril.Count(); c++)
            {
                vector cPos = MissionBuilding.ModelToWorld(m_Config.Locais.SpawnsBarril[c].ToVector());
                cPos[1] = GetGame().SurfaceY(cPos[0], cPos[2]);
                ItemBase Chest = ItemBase.Cast(GetGame().CreateObject(m_Config.Recompensas.Container, cPos, false, false, true));
                if (Chest) {
                    int numItems = Math.RandomIntInclusive(2, 4);
                    for(int k = 0; k < numItems; k++) {
                        Chest.GetInventory().CreateInInventory(m_Config.Recompensas.Itens.GetRandomElement());
                    }
                    m_MissionObjects.Insert(Chest);
                }
            }
        }

        // 3. SPAWN DOS INFECTADOS INICIAIS
        if (m_Config.Locais.SpawnsZumbis && m_Config.Dificuldade)
        {
            for (int z = 0; z < m_Config.Locais.SpawnsZumbis.Count(); z++)
            {
                vector zPos = MissionBuilding.ModelToWorld(m_Config.Locais.SpawnsZumbis[z].ToVector());
                zPos[1] = GetGame().SurfaceY(zPos[0], zPos[2]);
                string tipoZumbi = m_Config.Dificuldade.ClassesZumbis.GetRandomElement();
                m_MissionAIs.Insert(GetGame().CreateObject(tipoZumbi, zPos, false, true, true));
            }
        }
    }

    override void MissionFinal()
    {   
        if (m_Config && m_Config.Locais && m_Config.Locais.SpawnsZumbis && m_Config.Dificuldade)
        {
            for (int z = 0; z < m_Config.Locais.SpawnsZumbis.Count(); z++)
            {
                vector zPos = MissionBuilding.ModelToWorld(m_Config.Locais.SpawnsZumbis[z].ToVector());
                zPos[1] = GetGame().SurfaceY(zPos[0], zPos[2]);
                string tipoZumbi = m_Config.Dificuldade.ClassesZumbis.GetRandomElement();
                m_MissionAIs.Insert(GetGame().CreateObject(tipoZumbi, zPos, false, true, true));
            }
        }

        PNH_Logger.Log("Missões", "[PNH_CORE] MISSÃO_CONCLUÍDA: " + m_WinnerName + " saqueou os destroços do C130!");
        m_RewardsSpawned = true; 
    }

    override bool DeployMission()
    {
        if (!m_Config) return false;

        if (!m_Config.Ativa) 
        {
            PNH_Logger.Log("Missões", "[PNH_CORE] Missão PlaneCrash ignorada (Ativa = false no JSON).");
            return false; 
        }

        if (m_Config.Lore.MensagensRadio.Count() >= 3) {
            m_MissionMessage3 = m_Config.Lore.MensagensRadio[2] + "** " + m_MissionLocation + " **.";
        }

        if (m_MissionPosition && m_MissionPosition != "0 0 0")
        {
            MissionBuilding = GetGame().CreateObject(m_Config.ObjetoCentral, m_MissionPosition, false, false, false);
            if (MissionBuilding)
            {
                vector pos = MissionBuilding.GetPosition();
                pos[1] = GetGame().SurfaceY(pos[0], pos[2]);
                MissionBuilding.SetPosition(pos);
                m_MissionObjects.Insert(MissionBuilding);
            }

            PNH_Logger.Log("Missões", "[PNH_CORE] MISSÃO_INICIADA: Queda de Avião C130 em " + m_MissionLocation);
            SpawnObjectsAndAIs();
            return true;
        }
        return false;
    }

    override void PlayerChecks(PlayerBase player)
    {
        float distance = vector.Distance(player.GetPosition(), m_MissionPosition);
        PNH_MissionSettingsData config = PNH_MissionSettings.GetData();

        if (!m_ProximityWarned && distance <= m_MissionZoneOuterRadius)
        {
            m_ProximityWarned = true;
            string msg = "Atenção Operador: Estou vendo fumaça negra ao longe. Vou investigar.";
            if (config.ConfiguracoesGerais.UsarPDA) GetRPCManager().SendRPC("[GearPDA] ", "SendGlobalMessage", new Param2<string, string>("Operador", msg), true);
            else PNH_Utils.SendMessageToAll("[RADIO PNH] " + msg);
        }

        if (!m_InnerProximityWarned && distance <= m_MissionZoneInnerRadius)
        {
            m_InnerProximityWarned = true;
            string msg2 = "Contato visual com os destroços. Tem equipamento espalhado, mas os infectados já chegaram!";
            if (config.ConfiguracoesGerais.UsarPDA) GetRPCManager().SendRPC("[GearPDA] ", "SendGlobalMessage", new Param2<string, string>("Operador", msg2), true);
            else PNH_Utils.SendMessageToAll("[RADIO PNH] " + msg2);
        }

        if (!m_IsVictory && !m_RewardsSpawned && distance <= m_Config.RaioVitoria)
        {
            m_IsVictory = true;
            string playerName = player.GetIdentity().GetName();
            m_WinnerName = playerName;

            string successMsg = "[PNH_CORE] SUCESSO: [" + playerName + "] alcançou os destroços do avião e iniciou a extração do equipamento!";
            
            if (config.ConfiguracoesGerais.UsarPDA) GetRPCManager().SendRPC("[GearPDA] ", "SendGlobalMessage", new Param2<string, string>("Comando PNH", successMsg), true);
            else PNH_Utils.SendMessageToAll("[RADIO PNH] " + successMsg);

            m_MissionTimeout = m_MissionTime + config.ConfiguracoesGerais.TempoLimpezaSegundos;

            MissionFinal(); 
        }
    }
}