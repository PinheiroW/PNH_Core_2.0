class ApartmentMission extends PNH_MissionBase
{
    Object MissionBuilding;
    int MsgDlyFinish = 300; 
    
    ref array<vector> Spawnpoints = new array<vector>;
    ref array<ref Param3<string,vector,vector>> Barricades = new array<ref Param3<string,vector,vector>>;
    ref array<vector> InfectedSpawns = new array<vector>;
    ref array<string> InfectedTypes = new array<string>;
    
    string SurvivorName;    
    bool m_ProximityWarned = false; 
    bool m_DoorProximityWarned = false; 
    bool m_SurvivorFound = false; // Controle de exploração
    bool m_CompletionMsgSent = false; 

    override bool IsExtended() { return false; }
    
    void ApartmentMission()
    {
        // --- CONFIGURAÇÕES TÉCNICAS ---
        m_MissionTimeout = 2700;
        m_MissionZoneOuterRadius = 90.0;
        m_MissionZoneInnerRadius = 2.0; 
        m_MissionInformant = "Dr. Legasov";
        
        TStringArray SurvivorNames = {"Yuri","Michail","Boris","Valeri","Anatoli","Ivan","Alexej","Dimitrij","Sergej","Nikolai"};
        SurvivorName = SurvivorNames.GetRandomElement();
                        
        m_MissionMessage1 = SurvivorName + " foi um dos meus melhores alunos. Eu falava com ele pelo rádio, mas agora perdi o contato com ele.";
        m_MissionMessage2 = "Ele me disse que escondeu um barril com um bom equipamento em algum lugar dos apartamentos. Poucos dias atrás, ele barricou todo o prédio contra os infectados.";
        
        // --- SPAWNPOINTS DOS BARRIS ---
        Spawnpoints.Insert(Vector(8.1257, 2.7203, 3.1963)); Spawnpoints.Insert(Vector(-8.3906, -0.6797, -1.1826));
        Spawnpoints.Insert(Vector(-7.3033, 6.1203, -5.8271)); Spawnpoints.Insert(Vector(-3.6415, 6.1202, 5.5020));
        Spawnpoints.Insert(Vector(-4.4313, -4.0797, -1.4932)); Spawnpoints.Insert(Vector(3.4453, -6.3297, -2.0181));
        Spawnpoints.Insert(Vector(3.2646, 2.7203, -1.6377)); Spawnpoints.Insert(Vector(7.1572, 2.7203, -6.0815));
        Spawnpoints.Insert(Vector(2.8496, 2.7203, -6.1870)); Spawnpoints.Insert(Vector(2.9014, 2.7202, 4.9683));
        Spawnpoints.Insert(Vector(-10.0996, 6.1202, 5.7339));
        
        // --- PONTOS DE SPAWN DE INFECTADOS ---
        InfectedSpawns.Insert(Vector(-1.5186, -7.4796, 1.0269)); InfectedSpawns.Insert(Vector(4.9775, -7.4796, -1.4146));
        InfectedSpawns.Insert(Vector(-7.1726, -4.0797, -6.2729)); InfectedSpawns.Insert(Vector(-2.9209, -0.6797, 4.6636));
        InfectedSpawns.Insert(Vector(5.0283, 2.7203, -1.3276)); InfectedSpawns.Insert(Vector(-7.2461, 6.1203, -1.5884));
        InfectedSpawns.Insert(Vector(-1.6855, 6.1204, 5.9956));
        
        // --- LISTA COMPLETA DE INFECTADOS ---
        InfectedTypes.Insert("ZmbM_CitizenASkinny_Brown"); InfectedTypes.Insert("ZmbF_JournalistNormal_White");
        InfectedTypes.Insert("ZmbM_priestPopSkinny");      InfectedTypes.Insert("ZmbF_Clerk_Normal_White");
        InfectedTypes.Insert("ZmbM_HermitSkinny_Beige");   InfectedTypes.Insert("ZmbF_CitizenANormal_Blue");
        InfectedTypes.Insert("ZmbM_CitizenBFat_Red");      InfectedTypes.Insert("ZmbF_CitizenBSkinny");
        InfectedTypes.Insert("ZmbM_FishermanOld_Green");   InfectedTypes.Insert("ZmbF_HikerSkinny_Grey");
        InfectedTypes.Insert("ZmbM_HunterOld_Autumn");     InfectedTypes.Insert("ZmbF_SurvivorNormal_Orange");
        InfectedTypes.Insert("ZmbM_CitizenBFat_Blue");     InfectedTypes.Insert("ZmbF_HikerSkinny_Green");
        InfectedTypes.Insert("ZmbM_MotobikerFat_Black");   InfectedTypes.Insert("ZmbF_JoggerSkinny_Green");
        InfectedTypes.Insert("ZmbM_JoggerSkinny_Red");     InfectedTypes.Insert("ZmbF_SkaterYoung_Striped");
        InfectedTypes.Insert("ZmbM_MechanicSkinny_Grey");  InfectedTypes.Insert("ZmbF_BlueCollarFat_Red");
        InfectedTypes.Insert("ZmbM_HandymanNormal_Green"); InfectedTypes.Insert("ZmbF_MechanicNormal_Beige");
        InfectedTypes.Insert("ZmbM_HeavyIndustryWorker");  InfectedTypes.Insert("ZmbF_PatientOld");
        InfectedTypes.Insert("ZmbM_Jacket_black");         InfectedTypes.Insert("ZmbF_ShortSkirt_beige");
        InfectedTypes.Insert("ZmbM_Jacket_stripes");       InfectedTypes.Insert("ZmbF_VillagerOld_Red");
        InfectedTypes.Insert("ZmbM_HikerSkinny_Blue");     InfectedTypes.Insert("ZmbF_JoggerSkinny_Red");
        InfectedTypes.Insert("ZmbM_HikerSkinny_Yellow");   InfectedTypes.Insert("ZmbF_MilkMaidOld_Beige");
        InfectedTypes.Insert("ZmbM_PolicemanFat");         InfectedTypes.Insert("ZmbF_VillagerOld_Green");
        InfectedTypes.Insert("ZmbM_PatrolNormal_Summer");  InfectedTypes.Insert("ZmbF_ShortSkirt_yellow");
        InfectedTypes.Insert("ZmbM_JoggerSkinny_Blue");    InfectedTypes.Insert("ZmbF_NurseFat");
        InfectedTypes.Insert("ZmbM_VillagerOld_White");    InfectedTypes.Insert("ZmbF_PoliceWomanNormal");
        InfectedTypes.Insert("ZmbM_SkaterYoung_Brown");    InfectedTypes.Insert("ZmbF_HikerSkinny_Blue");
        InfectedTypes.Insert("ZmbM_MechanicSkinny_GREEN"); InfectedTypes.Insert("ZmbF_ParamedicNormal_Green");
        InfectedTypes.Insert("ZmbM_DoctorFat");            InfectedTypes.Insert("ZmbF_JournalistNormal_Red");
        InfectedTypes.Insert("ZmbM_PatientSkinny");        InfectedTypes.Insert("ZmbF_SurvivorNormal_White");
        InfectedTypes.Insert("ZmbM_ClerkFat_Brown");       InfectedTypes.Insert("ZmbF_JoggerSkinny_Brown");
        InfectedTypes.Insert("ZmbM_ClerkFat_White");       InfectedTypes.Insert("ZmbF_MechanicNormal_Grey");
        InfectedTypes.Insert("ZmbM_Jacket_magenta");       InfectedTypes.Insert("ZmbF_BlueCollarFat_Green");
        InfectedTypes.Insert("ZmbM_PolicemanSpecForce");   InfectedTypes.Insert("ZmbF_DoctorSkinny");
        
        // --- BARRICADAS COMPLETAS ---
        Barricades.Insert(new Param3<string,vector,vector>("WoodenLog", Vector(-0.300, -6.740, 6.940), Vector(0, 90, 0)));
        Barricades.Insert(new Param3<string,vector,vector>("WoodenLog", Vector(-0.300, -5.200, 6.768), Vector(0, -90, 0)));
        Barricades.Insert(new Param3<string,vector,vector>("WoodenLog", Vector(-0.300, -7.050, 6.376), Vector(0, 45, 0)));
        Barricades.Insert(new Param3<string,vector,vector>("WoodenPlank", Vector(-0.300, -7.495, 6.325), Vector(-90, 0, 0)));
        Barricades.Insert(new Param3<string,vector,vector>("WoodenPlank", Vector(0.660, -7.495, 6.000), Vector(-50, 0, 0)));
        Barricades.Insert(new Param3<string,vector,vector>("WoodenPlank", Vector(-0.660, -7.495, 6.000), Vector(-100, 0, 0)));
        Barricades.Insert(new Param3<string,vector,vector>("WoodenPlank", Vector(-1.260, -7.495, 5.700), Vector(-130, 0, 0)));
        Barricades.Insert(new Param3<string,vector,vector>("WoodenPlank", Vector(-1.560, -7.450, 5.900), Vector(-230, 0, 0)));
        Barricades.Insert(new Param3<string,vector,vector>("WoodenPlank", Vector(0.390, -7.460, 6.100), Vector(-35, -5, -2)));
        Barricades.Insert(new Param3<string,vector,vector>("WoodenPlank", Vector(-0.990, -7.460, 6.100), Vector(-85, -5, -2)));
        Barricades.Insert(new Param3<string,vector,vector>("WoodenPlank", Vector(-0.660, -8.115, 7.800), Vector(30, -45, -20)));
        Barricades.Insert(new Param3<string,vector,vector>("WoodenPlank", Vector(0.360, -8.015, 7.730), Vector(120, 17, -40)));
        Barricades.Insert(new Param3<string,vector,vector>("WoodenPlank", Vector(0.360, -7.410, 6.860), Vector(0, 90, 0)));
        Barricades.Insert(new Param3<string,vector,vector>("WoodenPlank", Vector(0.360, -7.207, 6.860), Vector(0, -90, 0)));
        Barricades.Insert(new Param3<string,vector,vector>("WoodenPlank", Vector(0.360, -7.004, 6.860), Vector(180, -90, 0)));
        Barricades.Insert(new Param3<string,vector,vector>("WoodenPlank", Vector(0.360, -6.831, 6.705), Vector(0, 100, 23)));
        Barricades.Insert(new Param3<string,vector,vector>("WoodenPlank", Vector(0.360, -5.583, 6.850), Vector(180, 96, -2)));
        Barricades.Insert(new Param3<string,vector,vector>("WoodenPlank", Vector(0.360, -5.377, 6.860), Vector(0, 90, 0)));
        Barricades.Insert(new Param3<string,vector,vector>("WoodenPlank", Vector(0.360, -5.165, 6.860), Vector(180, 90, 0)));
        Barricades.Insert(new Param3<string,vector,vector>("WoodenPlank", Vector(0.360, -4.955, 6.860), Vector(0, -90, 0)));
        Barricades.Insert(new Param3<string,vector,vector>("WoodenPlank", Vector(-0.920, -5.377, 6.860), Vector(0, -90, 0)));
        Barricades.Insert(new Param3<string,vector,vector>("WoodenPlank", Vector(-0.920, -5.165, 6.860), Vector(180, -90, 0)));
        Barricades.Insert(new Param3<string,vector,vector>("WoodenPlank", Vector(-0.920, -4.955, 6.860), Vector(0, 90, 0)));
    }
        
    void SpawnObjects()
    {
        // Barrel Spawn (Chamada via trigger próximo ao corpo)
        ItemBase MissionObject = ItemBase.Cast(GetGame().CreateObject("Barrel_Green", m_MissionPosition, false, false, true));
        if (MissionObject) {
            MissionObject.Close(); 
            m_MissionObjects.Insert(MissionObject); 

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
        // 1. BARRICADAS E DECORAÇÃO
        for (int k = 0; k < Barricades.Count(); k++)
        {
            Param3<string,vector,vector> B = Barricades[k];
            vector bPos = MissionBuilding.ModelToWorld(B.param2);
            Object plank = GetGame().CreateObject(B.param1, bPos, false, false, true);
            plank.SetPosition(bPos);
            plank.SetDirection(MissionBuilding.GetDirection());
            plank.SetOrientation(plank.GetOrientation() + B.param3);
            ItemBase pItem = ItemBase.Cast(plank);
            if (pItem) { pItem.SetQuantity(1); pItem.SetTakeable(false); }
            m_MissionObjects.Insert(plank);
        }
        
        Object phObj = GetGame().CreateObject("Land_Boat_Small1", MissionBuilding.ModelToWorld(Vector(0.360, -7.410, 6.730)), false, false, true);
        phObj.SetPosition(MissionBuilding.ModelToWorld(Vector(1.560, -7.340, 6.730)));
        phObj.SetDirection(MissionBuilding.GetDirection());
        phObj.SetOrientation(phObj.GetOrientation() + Vector(0, 90, 0));
        m_MissionObjects.Insert(phObj);

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
                
        // 3. ZUMBIS INTERNOS
        for (int j = 0; j < InfectedSpawns.Count(); j++)
        {
            vector iPos = MissionBuilding.ModelToWorld(InfectedSpawns[j]);
            m_MissionAIs.Insert(GetGame().CreateObject(InfectedTypes.GetRandomElement(), iPos, false, true, true));
        }
    }
    
    override void MissionFinal()
    {
        Building Tenement = Building.Cast(MissionBuilding);
        for (int i = 0; i < 32; i++) if (!Tenement.IsDoorOpen(i)) Tenement.OpenDoor(i);

        // SOM DO SCREAMER (Zumbi General)
        GetGame().CreateSoundOnObject(MissionBuilding, "ZmbM_Screamer_Shout_SoundSet", 100, false);

        // HORDA EXTERNA DE 15 ZUMBIS
        for (int h = 0; h < 15; h++)
        {
            float angle = Math.RandomFloat(0, 360) * Math.DEG2RAD;
            float dist = Math.RandomFloat(15, 25); 
            vector hPos = MissionBuilding.GetPosition();
            hPos[0] = hPos[0] + (dist * Math.Cos(angle));
            hPos[2] = hPos[2] + (dist * Math.Sin(angle));
            hPos[1] = GetGame().SurfaceY(hPos[0], hPos[2]);
            m_MissionAIs.Insert(GetGame().CreateObject(InfectedTypes.GetRandomElement(), hPos, false, true, true));
        }
        m_RewardsSpawned = true;
    }
    
    override void PlayerChecks(PlayerBase player)
    {
        float distance = vector.Distance(player.GetPosition(), m_MissionPosition);
        vector bodyPos = MissionBuilding.ModelToWorld(Vector(-6.3545, 6.1203, -4.6489));
        float distToBody = vector.Distance(player.GetPosition(), bodyPos);
        PNH_MissionSettingsData config = PNH_MissionSettings.GetData();

        // 1. PROXIMIDADE GERAL (90m) - FIX PDA
        if (!m_ProximityWarned && distance <= m_MissionZoneOuterRadius)
        {
            m_ProximityWarned = true;
            string msg = "Estou me aproximando do local nos apartamentos. Câmbio.";
            if (config.UsarPDA) GetRPCManager().SendRPC("[GearPDA] ", "SendGlobalMessage", new Param2<string, string>("Operador", msg), true);
            else PNH_Utils.SendMessageToAll("[RADIO PNH] " + msg);
        }

        // 2. PROXIMIDADE DA PORTA (10m) - FIX PDA
        if (!m_DoorProximityWarned && distance <= 10.0)
        {
            m_DoorProximityWarned = true;
            string msg2 = "Você encontrou o local da missão!";
            if (config.UsarPDA) GetRPCManager().SendRPC("[GearPDA] ", "SendGlobalMessage", new Param2<string, string>("Operador", msg2), true);
            else PNH_Utils.SendMessageToAll("[RADIO PNH] " + msg2);
        }

        // 3. GATILHO: ENCONTRAR CORPO -> LIBERA BARRIL - FIX PDA + DISCORD
        if (!m_SurvivorFound && distToBody <= 3.0)
        {
            m_SurvivorFound = true;
            string msg3 = "Encontrei o corpo de " + SurvivorName + "! O barril deve estar escondido por aqui!";
            if (config.UsarPDA) GetRPCManager().SendRPC("[GearPDA] ", "SendGlobalMessage", new Param2<string, string>("Operador", msg3), true);
            else PNH_Utils.SendMessageToAll("[RADIO PNH] " + msg3);
            
            PNH_Logger.Log("Missões", "[PNH_CORE] CORPO_ENCONTRADO: O corpo do sobrevivente foi localizado.");
            this.SpawnObjects(); 
        }

        // 4. ABERTURA DO BARRIL (Só se encontrou o corpo) - FIX PDA
        if (m_SurvivorFound && !m_RewardsSpawned && distance <= 10.0 && m_MissionObjects.Count() > 0) 
        {
            ItemBase stash = ItemBase.Cast(m_MissionObjects[0]);
            if (stash && stash.IsOpen())
            {
                string msg4 = "Abri o barril! Merda, o barulho atraiu uma horda do lado de fora!";
                if (config.UsarPDA) GetRPCManager().SendRPC("[GearPDA] ", "SendGlobalMessage", new Param2<string, string>("Operador", msg4), true);
                else PNH_Utils.SendMessageToAll("[RADIO PNH] " + msg4);
                MissionFinal();
            }
        }

        // 5. SUCESSO FINAL - FIX PDA + DISCORD
        if (m_RewardsSpawned && !m_CompletionMsgSent && distance > 50.0)
        {
            m_CompletionMsgSent = true;
            string playerName = player.GetIdentity().GetName();
            string successMsg = "[PNH_CORE] MISSÃO_CONCLUÍDA: [" + playerName + "] Sobrevivi à horda nos apartamentos.";
            
            if (config.UsarPDA) GetRPCManager().SendRPC("[GearPDA] ", "SendGlobalMessage", new Param2<string, string>("Comando PNH", successMsg), true);
            else PNH_Utils.SendMessageToAll("[RADIO PNH] " + successMsg);
            
            PNH_Logger.Log("Missões", successMsg);
            m_MissionTimeout = m_MissionTime + 60; 
        }
    }
            
    override bool DeployMission()
    {
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