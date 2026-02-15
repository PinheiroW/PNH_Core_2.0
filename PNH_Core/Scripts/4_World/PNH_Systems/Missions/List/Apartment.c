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
        
        // --- SPAWNPOINTS DOS BARRIS (11 PONTOS) ---
        Spawnpoints.Insert(Vector(8.1257, 2.7203, 3.1963)); Spawnpoints.Insert(Vector(-8.3906, -0.6797, -1.1826));
        Spawnpoints.Insert(Vector(-7.3033, 6.1203, -5.8271)); Spawnpoints.Insert(Vector(-3.6415, 6.1202, 5.5020));
        Spawnpoints.Insert(Vector(-4.4313, -4.0797, -1.4932)); Spawnpoints.Insert(Vector(3.4453, -6.3297, -2.0181));
        Spawnpoints.Insert(Vector(3.2646, 2.7203, -1.6377)); Spawnpoints.Insert(Vector(7.1572, 2.7203, -6.0815));
        Spawnpoints.Insert(Vector(2.8496, 2.7203, -6.1870)); Spawnpoints.Insert(Vector(2.9014, 2.7202, 4.9683));
        Spawnpoints.Insert(Vector(-10.0996, 6.1202, 5.7339));
        
        // --- SPAWNS DE INFECTADOS INTERNOS ---
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
        
        // --- BARRICADAS COMPLETAS (23 ITENS) ---
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
        ItemBase MissionObject = ItemBase.Cast(GetGame().CreateObject("Barrel_Green", m_MissionPosition, false, false, false));
        if (MissionObject) {
            MissionObject.Close(); 
            m_MissionObjects.Insert(MissionObject); 

            int selectedLoadout = Math.RandomIntInclusive(0, 2);
            EntityAI weapon;
            if (selectedLoadout == 0) {
                weapon = MissionObject.GetInventory().CreateInInventory("M4A1_Green");
                weapon.GetInventory().CreateAttachment("M4_RISHndgrd_Green");
                weapon.GetInventory().CreateAttachment("M4_MPBttstck");
                weapon.GetInventory().CreateAttachment("M4_Suppressor");
                weapon.GetInventory().CreateAttachment("ACOGOptic");
                MissionObject.GetInventory().CreateInInventory("Mag_STANAG_30Rnd");
                MissionObject.GetInventory().CreateInInventory("Mag_STANAG_30Rnd");
                MissionObject.GetInventory().CreateInInventory("AmmoBox_556x45_20Rnd");
            } else if (selectedLoadout == 1) {
                weapon = MissionObject.GetInventory().CreateInInventory("SVD");
                weapon.GetInventory().CreateAttachment("PSO1Optic");
                MissionObject.GetInventory().CreateInInventory("Mag_SVD_10Rnd");
                MissionObject.GetInventory().CreateInInventory("Mag_SVD_10Rnd");
                MissionObject.GetInventory().CreateInInventory("AmmoBox_762x54_20Rnd");
            } else {
                weapon = MissionObject.GetInventory().CreateInInventory("FAL");
                weapon.GetInventory().CreateAttachment("PSO11Optic");
                MissionObject.GetInventory().CreateInInventory("Mag_FAL_20Rnd");
                MissionObject.GetInventory().CreateInInventory("Mag_FAL_20Rnd");
                MissionObject.GetInventory().CreateInInventory("AmmoBox_308Win_20Rnd");
            }

            MissionObject.GetInventory().CreateInInventory("HuntingKnife");
            MissionObject.GetInventory().CreateInInventory("BakedBeansCan");
            MissionObject.GetInventory().CreateInInventory("Canteen");
            MissionObject.GetInventory().CreateInInventory("Battery9V");
        }

        for (int k = 0; k < Barricades.Count(); k++)
        {
            Param3<string,vector,vector> B = Barricades[k];
            vector pos = MissionBuilding.ModelToWorld(B.param2);
            Object plank = GetGame().CreateObject(B.param1, pos, false, false, true);
            plank.SetPosition(pos);
            plank.SetDirection(MissionBuilding.GetDirection());
            plank.SetOrientation(plank.GetOrientation() + B.param3);
            ItemBase pItem = ItemBase.Cast(plank);
            if (pItem) { pItem.SetQuantity(1); pItem.SetTakeable(false); }
            m_MissionObjects.Insert(plank);
        }
        
        Object phObj = GetGame().CreateObject("Land_Boat_Small1", MissionBuilding.ModelToWorld(Vector(0.360, -7.410, 6.730)), true);
        phObj.SetPosition(MissionBuilding.ModelToWorld(Vector(1.560, -7.340, 6.730)));
        phObj.SetDirection(MissionBuilding.GetDirection());
        phObj.SetOrientation(phObj.GetOrientation() + Vector(0, 90, 0));
        m_MissionObjects.Insert(phObj);
    }   
    
    void KillSurvivor(PlayerBase survivor) { if (survivor) survivor.SetHealth("", "", 0); }

    void SpawnAIs()
    {
        vector SurvPos = MissionBuilding.ModelToWorld(Vector(-6.3545, 6.1203, -4.6489));
        PlayerBase DeadSurvivor = PlayerBase.Cast(GetGame().CreateObject("SurvivorM_Oliver", SurvPos, false, true));
        if (DeadSurvivor) {
            DeadSurvivor.GetInventory().CreateInInventory("HikingJacket_Black");
            DeadSurvivor.GetInventory().CreateInInventory("Jeans_Blue");
            DeadSurvivor.GetInventory().CreateInInventory("HikingBoots_Brown");
            DeadSurvivor.GetInventory().CreateInInventory("Glock19");
            DeadSurvivor.GetInventory().CreateInInventory("Mag_Glock_15Rnd");
            DeadSurvivor.SetName(SurvivorName);
            DeadSurvivor.SetBloodyHands(true);
            DeadSurvivor.SetSynchDirty();
            m_MissionAIs.Insert(DeadSurvivor);
            GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(this.KillSurvivor, 2000, false, DeadSurvivor);
        }
                
        vector GirlPos = MissionBuilding.ModelToWorld(Vector(-9.4111, 6.1203, -4.8696));
        m_MissionAIs.Insert(GetGame().CreateObject("ZmbF_JournalistNormal_White", GirlPos, false, true));

        for (int j = 0; j < InfectedSpawns.Count(); j++)
        {
            vector pos = MissionBuilding.ModelToWorld(InfectedSpawns[j]);
            m_MissionAIs.Insert(GetGame().CreateObject(InfectedTypes.GetRandomElement(), pos, false, true));
        }
    }
    
    override void MissionFinal()
    {
        Building Tenement = Building.Cast(MissionBuilding);
        for (int i = 0; i < 32; i++) if (!Tenement.IsDoorOpen(i)) Tenement.OpenDoor(i);

        // SOM DO SCREAMER (Zumbi General) - Chama a atenção de todos
        GetGame().CreateSoundOnObject(MissionBuilding, "DayZInfected_Shout_SoundSet", 100, false);

        // HORDA EXTERNA DE 15 ZUMBIS
        for (int h = 0; h < 15; h++)
        {
            float angle = Math.RandomFloat(0, 360) * Math.DEG2RAD;
            float dist = Math.RandomFloat(15, 25); 
            vector hPos = MissionBuilding.GetPosition();
            hPos[0] = hPos[0] + (dist * Math.Cos(angle));
            hPos[2] = hPos[2] + (dist * Math.Sin(angle));
            hPos[1] = GetGame().SurfaceY(hPos[0], hPos[2]);
            m_MissionAIs.Insert(GetGame().CreateObject(InfectedTypes.GetRandomElement(), hPos, false, true));
        }
        m_RewardsSpawned = true;
    }
    
    override void PlayerChecks(PlayerBase player)
    {
        float distance = vector.Distance(player.GetPosition(), m_MissionPosition);

        // 1. AVISO DE PROXIMIDADE (90m)
        if (!m_ProximityWarned && distance <= m_MissionZoneOuterRadius)
        {
            m_ProximityWarned = true;
            string warnMsg = "Estou me aproximando do local nos apartamentos. Câmbio.";
            PNH_MissionSettingsData config = PNH_MissionSettings.GetData();
            if (config.UsarPDA) GetRPCManager().SendRPC("[GearPDA] ", "SendGlobalMessage", new Param2<string, string>("Operador", warnMsg), true);
            else PNH_Utils.SendMessageToAll("[RADIO PNH] " + warnMsg);
        }

        // 2. AVISO DE LOCAL ENCONTRADO (10m)
        if (!m_DoorProximityWarned && distance <= 10.0)
        {
            m_DoorProximityWarned = true;
            string foundMsg = "Você encontrou o local da missão!";
            PNH_MissionSettingsData configDoor = PNH_MissionSettings.GetData();
            if (configDoor.UsarPDA) GetRPCManager().SendRPC("[GearPDA] ", "SendGlobalMessage", new Param2<string, string>("Operador", foundMsg), true);
            else PNH_Utils.SendMessageToAll("[RADIO PNH] " + foundMsg);
        }

        // 3. ABERTURA E DISPARO DA HORDA E SOM (10m)
        if (!m_RewardsSpawned && distance <= 10.0 && m_MissionObjects.Count() > 0) 
        {
            ItemBase stash = ItemBase.Cast(m_MissionObjects[0]);
            if (stash && stash.IsOpen())
            {
                string endMsg = "Abri o barril do " + SurvivorName + "! Merda, o barulho atraiu uma horda do lado de fora! Defendendo a posição!";
                PNH_MissionSettingsData config2 = PNH_MissionSettings.GetData();
                if (config2.UsarPDA) GetRPCManager().SendRPC("[GearPDA] ", "SendGlobalMessage", new Param2<string, string>("Operador", endMsg), true);
                else PNH_Utils.SendMessageToAll("[RADIO PNH] " + endMsg);
                MissionFinal();
            }
        }

        // 4. SUCESSO PERSONALIZADO DISCORD (50m de distância do barril após abrir)
        if (m_RewardsSpawned && !m_CompletionMsgSent && distance > 50.0)
        {
            m_CompletionMsgSent = true;
            string playerName = player.GetIdentity().GetName();
            string successMsg = "[" + playerName + "] Sobrevivi à horda nos apartamentos. Contrato encerrado.";
            
            PNH_MissionSettingsData config3 = PNH_MissionSettings.GetData();
            if (config3.UsarPDA) GetRPCManager().SendRPC("[GearPDA] ", "SendGlobalMessage", new Param2<string, string>("Comando PNH", successMsg), true);
            else PNH_Utils.SendMessageToAll("[RADIO PNH] " + successMsg);
            
            // --- LOG E DISCORDA ---
            Print("[PNH SYSTEM] " + successMsg); // Confirmação no log do servidor
            PNH_Logger.Log("Missões", successMsg); // Envio para o Webhook
            // ----------------------

            m_MissionTimeout = m_MissionTime + 60; 
        }
    }
            
    override bool DeployMission()
    {
        m_MissionMessage3 = "Ele morava em um dos apartamentos em ** " + m_MissionLocation + " **.";
        m_MissionMessage4 = "Por favor, verifique se " + SurvivorName + " ainda está vivo.";
        m_MissionMessage5 = "Se o pior aconteceu, recupere os equipamentos dele. Eles serão cruciais para a sua própria jornada.";

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
            GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Call(this.SpawnObjects);
            GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Call(this.SpawnAIs);
            return true;
        }
        return false;
    }
}