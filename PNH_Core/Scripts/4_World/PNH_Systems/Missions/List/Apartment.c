class ApartmentMission extends PNH_MissionBase
{
    Object MissionBuilding;
    int MsgDlyFinish = 300; 
    
    ref array<vector> Spawnpoints = new array<vector>;
    
    string SurvivorName;    
    bool m_ProximityWarned = false; 
    bool m_DoorProximityWarned = false; 
    bool m_SurvivorFound = false; 
    bool m_CompletionMsgSent = false; 

    // O CÉREBRO DA MISSÃO QUE LÊ O JSON
    ref PNH_MissionData_Apartment m_Config; 

    override bool IsExtended() { return false; }
    
    void ApartmentMission()
    {
        string caminhoJson = "$profile:PNH/Missions/Apartment.json";
        m_Config = new PNH_MissionData_Apartment();
        
        if (!FileExist("$profile:PNH/Missions")) MakeDirectory("$profile:PNH/Missions");
        
        if (FileExist(caminhoJson)) {
            JsonFileLoader<PNH_MissionData_Apartment>.JsonLoadFile(caminhoJson, m_Config);
        } else {
            PNH_Logger.Error("Missões", "Arquivo Apartment.json nao encontrado!");
            return;
        }

        // CARREGA DEFINIÇÕES GERAIS
        m_MissionTimeout = m_Config.TempoLimiteSegundos;
        m_MissionZoneOuterRadius = m_Config.RaioAvisoExterno;
        m_MissionZoneInnerRadius = m_Config.RaioAvisoInterno;
        m_MissionInformant = m_Config.Lore.Informante;
        
        // CARREGA A LORE DINÂMICA
        SurvivorName = m_Config.Lore.NomesSobreviventes.GetRandomElement();
        m_MissionMessage1 = SurvivorName + m_Config.Lore.MensagensRadio[0];
        m_MissionMessage2 = m_Config.Lore.MensagensRadio[1];
        
        // CARREGA SPAWNS DOS BARRIS
        for (int i = 0; i < m_Config.Locais.SpawnsBarril.Count(); i++) {
            Spawnpoints.Insert(m_Config.Locais.SpawnsBarril[i].ToVector());
        }
    }
        
    void SpawnObjects()
    {
        ItemBase MissionObject = ItemBase.Cast(GetGame().CreateObject("Barrel_Green", m_MissionPosition, false, false, true));
        if (MissionObject) {
            MissionObject.Close(); 
            m_MissionObjects.Insert(MissionObject); 

            // Na próxima fase podemos migrar este loot para ler do JSON também!
            int sel = Math.RandomIntInclusive(0, 2);
            EntityAI weapon;
            if (sel == 0) {
                weapon = MissionObject.GetInventory().CreateInInventory("M4A1_Green");
                weapon.GetInventory().CreateAttachment("ACOGOptic");
                MissionObject.GetInventory().CreateInInventory("Mag_STANAG_30Rnd");
                MissionObject.GetInventory().CreateInInventory("AmmoBox_556x45_20Rnd");
            } else if (sel == 1) {
                weapon = MissionObject.GetInventory().CreateInInventory("SVD");
                weapon.GetInventory().CreateAttachment("PSO1Optic");
                MissionObject.GetInventory().CreateInInventory("Mag_SVD_10Rnd");
                MissionObject.GetInventory().CreateInInventory("AmmoBox_762x54_20Rnd");
            } else {
                weapon = MissionObject.GetInventory().CreateInInventory("FAL");
                weapon.GetInventory().CreateAttachment("PSO11Optic");
                MissionObject.GetInventory().CreateInInventory("Mag_FAL_20Rnd");
                MissionObject.GetInventory().CreateInInventory("AmmoBox_308Win_20Rnd");
            }
            MissionObject.GetInventory().CreateInInventory("HuntingKnife");
            MissionObject.GetInventory().CreateInInventory("BakedBeansCan");
            MissionObject.GetInventory().CreateInInventory("Canteen");
            MissionObject.GetInventory().CreateInInventory("Battery9V");
        }
    }   
    
    void KillSurvivor(PlayerBase survivor) { if (survivor) survivor.SetHealth("", "", 0); }

    void SpawnAIs()
    {
        // 1. BARRICADAS E DECORAÇÃO LIDOS DO JSON
        if (m_Config.Cenario && m_Config.Cenario.Barricadas)
        {
            for (int k = 0; k < m_Config.Cenario.Barricadas.Count(); k++)
            {
                PNH_MissionSettings_Barricada bInfo = m_Config.Cenario.Barricadas[k];
                vector bPos = MissionBuilding.ModelToWorld(bInfo.PosicaoLocal.ToVector());
                
                Object plank = GetGame().CreateObject(bInfo.Classe, bPos, false, false, true);
                if (plank) {
                    plank.SetPosition(bPos);
                    plank.SetDirection(MissionBuilding.GetDirection());
                    plank.SetOrientation(plank.GetOrientation() + bInfo.OrientacaoLocal.ToVector());
                    
                    ItemBase pItem = ItemBase.Cast(plank);
                    if (pItem) { pItem.SetQuantity(1); pItem.SetTakeable(false); }
                    m_MissionObjects.Insert(plank);
                }
            }
        }

        // 2. SOBREVIVENTE IVAN (CORPO REAL)
        vector SurvPos = MissionBuilding.ModelToWorld(Vector(-6.3545, 6.1203, -4.6489));
        PlayerBase DeadSurvivor = PlayerBase.Cast(GetGame().CreatePlayer(null, "SurvivorM_Oliver", SurvPos, 0, "Oliver"));
        if (DeadSurvivor) {
            DeadSurvivor.GetInventory().CreateInInventory("HikingJacket_Black");
            DeadSurvivor.GetInventory().CreateInInventory("Jeans_Blue");
            DeadSurvivor.GetInventory().CreateInInventory("HikingBoots_Brown");
            DeadSurvivor.GetInventory().CreateInInventory("Glock19");
            DeadSurvivor.GetInventory().CreateInInventory("Mag_Glock_15Rnd");
            DeadSurvivor.SetName(SurvivorName);
            DeadSurvivor.SetBloodyHands(true);
            m_MissionAIs.Insert(DeadSurvivor);
            GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(this.KillSurvivor, 1500, false, DeadSurvivor);
        }
                
        // 3. ZUMBIS INTERNOS LIDOS DO JSON
        for (int j = 0; j < m_Config.Locais.SpawnsZumbis.Count(); j++)
        {
            vector iPos = MissionBuilding.ModelToWorld(m_Config.Locais.SpawnsZumbis[j].ToVector());
            string tipoZumbi = m_Config.Dificuldade.ClassesZumbis.GetRandomElement();
            m_MissionAIs.Insert(GetGame().CreateObject(tipoZumbi, iPos, false, true, true));
        }
    }
    
    override void MissionFinal()
    {
        Building Tenement = Building.Cast(MissionBuilding);
        for (int i = 0; i < 32; i++) if (!Tenement.IsDoorOpen(i)) Tenement.OpenDoor(i);

        GetGame().CreateSoundOnObject(MissionBuilding, "ZmbM_Screamer_Shout_SoundSet", 100, false);

        // HORDA EXTERNA
        int tamanhoHorda = m_Config.Dificuldade.QuantidadeHordaFinal;
        for (int h = 0; h < tamanhoHorda; h++)
        {
            float angle = Math.RandomFloat(0, 360) * Math.DEG2RAD;
            float dist = Math.RandomFloat(15, 25); 
            vector hPos = MissionBuilding.GetPosition();
            hPos[0] = hPos[0] + (dist * Math.Cos(angle));
            hPos[2] = hPos[2] + (dist * Math.Sin(angle));
            hPos[1] = GetGame().SurfaceY(hPos[0], hPos[2]);
            
            string hordaTipoZumbi = m_Config.Dificuldade.ClassesZumbis.GetRandomElement();
            m_MissionAIs.Insert(GetGame().CreateObject(hordaTipoZumbi, hPos, false, true, true));
        }
        m_RewardsSpawned = true;
    }
    
    override void PlayerChecks(PlayerBase player)
    {
        float distance = vector.Distance(player.GetPosition(), m_MissionPosition);
        vector bodyPos = MissionBuilding.ModelToWorld(Vector(-6.3545, 6.1203, -4.6489));
        float distToBody = vector.Distance(player.GetPosition(), bodyPos);
        PNH_MissionSettingsData config = PNH_MissionSettings.GetData();

        if (!m_ProximityWarned && distance <= m_MissionZoneOuterRadius)
        {
            m_ProximityWarned = true;
            string msg = "Estou me aproximando do local nos apartamentos. Câmbio.";
            if (config.ConfiguracoesGerais.UsarPDA) GetRPCManager().SendRPC("[GearPDA] ", "SendGlobalMessage", new Param2<string, string>("Operador", msg), true);
            else PNH_Utils.SendMessageToAll("[RADIO PNH] " + msg);
        }

        if (!m_DoorProximityWarned && distance <= 10.0)
        {
            m_DoorProximityWarned = true;
            string msg2 = "Você encontrou o local da missão!";
            if (config.ConfiguracoesGerais.UsarPDA) GetRPCManager().SendRPC("[GearPDA] ", "SendGlobalMessage", new Param2<string, string>("Operador", msg2), true);
            else PNH_Utils.SendMessageToAll("[RADIO PNH] " + msg2);
        }

        if (!m_SurvivorFound && distToBody <= 3.0)
        {
            m_SurvivorFound = true;
            string msg3 = "Encontrei o corpo de " + SurvivorName + "! O barril deve estar escondido por aqui!";
            if (config.ConfiguracoesGerais.UsarPDA) GetRPCManager().SendRPC("[GearPDA] ", "SendGlobalMessage", new Param2<string, string>("Operador", msg3), true);
            else PNH_Utils.SendMessageToAll("[RADIO PNH] " + msg3);
            
            PNH_Logger.Log("Missões", "[PNH_CORE] CORPO_ENCONTRADO: O corpo do sobrevivente foi localizado.");
            this.SpawnObjects(); 
        }

        if (m_SurvivorFound && !m_RewardsSpawned && distance <= 10.0 && m_MissionObjects.Count() > 0) 
        {
            ItemBase stash = ItemBase.Cast(m_MissionObjects[0]);
            if (stash && stash.IsOpen())
            {
                string msg4 = "Abri o barril! Merda, o barulho atraiu uma horda do lado de fora!";
                if (config.ConfiguracoesGerais.UsarPDA) GetRPCManager().SendRPC("[GearPDA] ", "SendGlobalMessage", new Param2<string, string>("Operador", msg4), true);
                else PNH_Utils.SendMessageToAll("[RADIO PNH] " + msg4);
                MissionFinal();
            }
        }

        if (m_RewardsSpawned && !m_CompletionMsgSent && distance > 50.0)
        {
            m_CompletionMsgSent = true;
            string playerName = player.GetIdentity().GetName();
            string successMsg = "[PNH_CORE] MISSÃO_CONCLUÍDA: [" + playerName + "] Sobrevivi à horda nos apartamentos.";
            
            if (config.ConfiguracoesGerais.UsarPDA) GetRPCManager().SendRPC("[GearPDA] ", "SendGlobalMessage", new Param2<string, string>("Comando PNH", successMsg), true);
            else PNH_Utils.SendMessageToAll("[RADIO PNH] " + successMsg);
            
            PNH_Logger.Log("Missões", successMsg);
            m_MissionTimeout = m_MissionTime + 60; 
        }
    }
            
    override bool DeployMission()
    {
        // Trava de segurança Anti-Crash
        if (!m_Config) return false;

        // --- AQUI ESTÁ A MÁGICA DO BOTÃO LIGA/DESLIGA ---
        if (!m_Config.Ativa) 
        {
            PNH_Logger.Log("Missões", "[PNH_CORE] Missão Apartment ignorada (Ativa = false no JSON).");
            return false; // Cancela o início da missão
        }
        // ------------------------------------------------

        // --- TRAVA DE SEGURANÇA SE O JSON ESTIVER VAZIO ---
        if (Spawnpoints.Count() == 0) 
        {
            PNH_Logger.Error("Missões", "ERRO CRÍTICO: A missão não pôde iniciar porque o JSON não foi carregado ou está vazio.");
            return false; 
        }
        // -------------------------------------

        GetGame().GetObjectsAtPosition(m_MissionPosition, 1.5, m_ObjectList, m_ObjectCargoList);
        for (int i = 0; i < m_ObjectList.Count(); i++)
        {
            if (m_ObjectList[i].GetType().Contains("Land_Tenement_Small")) {   
                MissionBuilding = m_ObjectList[i];
                m_MissionPosition = MissionBuilding.ModelToWorld(Spawnpoints.GetRandomElement());
                break;
            }
        }   
                
        if (MissionBuilding)
        {   
            Building Tenement = Building.Cast(MissionBuilding);
            for (int k = 0; k < 32; k++) if (Tenement.IsDoorOpen(k)) Tenement.CloseDoor(k);
            Print("[PNH SYSTEM] Missão Apartment Iniciada.");
            this.SpawnAIs(); 
            return true;
        }
        return false;
    }
}