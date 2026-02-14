class ApartmentMission extends PNH_MissionBase
{
	// Mission related entities 
	Object MissionBuilding;

	// Mission parameters	
	int MsgDlyFinish = 300; // seconds, message delay time after player has finished mission
	
	// Mission containers
	ref array<vector> Spawnpoints = new array<vector>;
	ref array<ref Param3<string,vector,vector>> Barricades = new array<ref Param3<string,vector,vector>>;
	ref array<vector> InfectedSpawns = new array<vector>;
	ref array<string> InfectedTypes = new array<string>;
	
	// Mission variables 
	string SurvivorName;	
	
	override bool IsExtended()
	{
		return false;
	}
	
	void ApartmentMission()
	{
		// Mission timeout
		m_MissionTimeout = 2700; // seconds, mission duration
		
		// Mission zone params
		m_MissionZoneOuterRadius = 90.0; // meters
		m_MissionZoneInnerRadius = 2.0;  // meters
					
		// Mission informant
		m_MissionInformant = "Dr. Legasov";
		
		// Random survivor name
		TStringArray SurvivorNames = {
			"Yuri","Michail","Boris","Valeri","Anatoli","Ivan",
			"Alexej","Dimitrij","Sergej","Nikolai"
		};
		SurvivorName = SurvivorNames.GetRandomElement();
						
		// Mission messages 
		m_MissionMessage1 = SurvivorName +" foi um dos meus melhores alunos. Eu falava com ele pelo rádio, mas agora perdi o contato com ele.";
		m_MissionMessage2 = "Ele me disse que escondeu um baú com um bom equipamento em algum lugar dos apartamentos. Poucos dias atrás, ele barricou todo o prédio contra os infectados.";
		m_MissionMessage3 = "Ele morava em um dos apartamentos\n** "+ m_MissionLocationDir +" do "+ m_MissionLocation+" **\nPor favor, verifique se "+SurvivorName+" ainda está vivo. Se não, pegue as coisas dele para ajudá-lo a sobreviver.";
				
		// Mission object spawnpoints	
		Spawnpoints.Insert("8.1257 2.7203 3.1963");
		Spawnpoints.Insert("-8.3906 -0.6797 -1.1826");
		Spawnpoints.Insert("-7.3033 6.1203 -5.8271");
		Spawnpoints.Insert("-3.6415 6.1202 5.5020");
		Spawnpoints.Insert("-4.4313 -4.0797 -1.4932");
		Spawnpoints.Insert("3.4453 -6.3297 -2.0181");
		Spawnpoints.Insert("3.2646 2.7203 -1.6377");
		Spawnpoints.Insert("7.1572 2.7203 -6.0815");
		Spawnpoints.Insert("2.8496 2.7203 -6.1870");
		Spawnpoints.Insert("2.9014 2.7202 4.9683");
		Spawnpoints.Insert("-10.0996 6.1202 5.7339");
		
		// Infected spawnpoints
		InfectedSpawns.Insert("-1.5186 -7.4796 1.0269");
		InfectedSpawns.Insert("4.9775 -7.4796 -1.4146");
		InfectedSpawns.Insert("-7.1726 -4.0797 -6.2729");
		InfectedSpawns.Insert("-2.9209 -0.6797 4.6636");
		InfectedSpawns.Insert("5.0283 2.7203 -1.3276");
		InfectedSpawns.Insert("-7.2461 6.1203 -1.5884");
		InfectedSpawns.Insert("-1.6855 6.1204 5.9956");
		
		// Infected types (male & female)
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
		
		// Entrance barricades
		Barricades.Insert(new Param3<string,vector,vector>("WoodenLog", "-0.300 -6.740 6.940", "0 90 0"));
		Barricades.Insert(new Param3<string,vector,vector>("WoodenLog", "-0.300 -5.200 6.768", "0 -90 0"));
		Barricades.Insert(new Param3<string,vector,vector>("WoodenLog", "-0.300 -7.050 6.376", "0 45 0"));
		Barricades.Insert(new Param3<string,vector,vector>("WoodenPlank", "-0.300 -7.495 6.325", "-90 0 0"));
		
		// Plank clutter
		Barricades.Insert(new Param3<string,vector,vector>("WoodenPlank", "0.660 -7.495 6.000", "-50 0 0"));
		Barricades.Insert(new Param3<string,vector,vector>("WoodenPlank", "-0.660 -7.495 6.000", "-100 0 0"));
		Barricades.Insert(new Param3<string,vector,vector>("WoodenPlank", "-1.260 -7.495 5.700", "-130 0 0"));
		Barricades.Insert(new Param3<string,vector,vector>("WoodenPlank", "-1.560 -7.450 5.900", "-230 0 0"));
		Barricades.Insert(new Param3<string,vector,vector>("WoodenPlank", "0.390 -7.460 6.100", "-35 -5 -2"));
		Barricades.Insert(new Param3<string,vector,vector>("WoodenPlank", "-0.990 -7.460 6.100", "-85 -5 -2"));
		
		// Outside planks
		Barricades.Insert(new Param3<string,vector,vector>("WoodenPlank", "-0.660 -8.115 7.800", "30 -45 -20"));
		Barricades.Insert(new Param3<string,vector,vector>("WoodenPlank", "0.360 -8.015 7.730", "120 17 -40"));
		
		// Right plank wall
		Barricades.Insert(new Param3<string,vector,vector>("WoodenPlank", "0.360 -7.410 6.860", "0 90 0"));
		Barricades.Insert(new Param3<string,vector,vector>("WoodenPlank", "0.360 -7.207 6.860", "0 -90 0"));
		Barricades.Insert(new Param3<string,vector,vector>("WoodenPlank", "0.360 -7.004 6.860", "180 -90 0"));
		Barricades.Insert(new Param3<string,vector,vector>("WoodenPlank", "0.360 -6.831 6.705", "0 100 23"));
		Barricades.Insert(new Param3<string,vector,vector>("WoodenPlank", "0.360 -5.583 6.850", "180 96 -2"));
		Barricades.Insert(new Param3<string,vector,vector>("WoodenPlank", "0.360 -5.377 6.860", "0 90 0"));
		Barricades.Insert(new Param3<string,vector,vector>("WoodenPlank", "0.360 -5.165 6.860", "180 90 0"));
		Barricades.Insert(new Param3<string,vector,vector>("WoodenPlank", "0.360 -4.955 6.860", "0 -90 0"));
		
		// Left plank wall
		Barricades.Insert(new Param3<string,vector,vector>("WoodenPlank", "-0.920 -5.377 6.860", "0 -90 0"));
		Barricades.Insert(new Param3<string,vector,vector>("WoodenPlank", "-0.920 -5.165 6.860", "180 -90 0"));
		Barricades.Insert(new Param3<string,vector,vector>("WoodenPlank", "-0.920 -4.955 6.860", "0 90 0"));
	}
			
	void SpawnObjects()
	{
		// Spawn reward SeaChest
		ItemBase MissionObject = ItemBase.Cast(GetGame().CreateObject("SeaChest", m_MissionPosition, false, false, false));
		
		// Pick random loadout
		int selectedLoadout = Math.RandomIntInclusive(0,11);
		
		EntityAI weapon;

		// LOADOUT 0
		if (selectedLoadout == 0)
		{
			weapon = MissionObject.GetInventory().CreateInInventory("M4A1_Green");
			weapon.GetInventory().CreateAttachment("M4_RISHndgrd_Green");
			weapon.GetInventory().CreateAttachment("M4_MPBttstck");
			weapon.GetInventory().CreateAttachment("M4_Suppressor");
			weapon.GetInventory().CreateAttachment("ACOGOptic");

			MissionObject.GetInventory().CreateInInventory("Mag_STANAG_30Rnd");
			MissionObject.GetInventory().CreateInInventory("Ammo_556x45");
			MissionObject.GetInventory().CreateInInventory("Ammo_556x45");
			MissionObject.GetInventory().CreateInInventory("HuntingKnife");
			MissionObject.GetInventory().CreateInInventory("BakedBeansCan");
			MissionObject.GetInventory().CreateInInventory("Canteen");
			MissionObject.GetInventory().CreateInInventory("Battery9V");
		}

		m_MissionObjects.Insert(MissionObject);
		
		// Spawn barricades
		for (int k = 0; k < Barricades.Count(); k++)
		{
			Param3<string,vector,vector> B = Barricades[k];
			string type = B.param1;
			vector pos = MissionBuilding.ModelToWorld(B.param2);
			vector ori = B.param3;

			Object plank = GetGame().CreateObject(type, pos, false, false, true);
			plank.SetPosition(pos);
			plank.SetDirection(MissionBuilding.GetDirection());
			plank.SetOrientation(plank.GetOrientation() + ori);

			ItemBase.Cast(plank).SetQuantity(1);
			ItemBase.Cast(plank).SetTakeable(false);

			m_MissionObjects.Insert(plank);
		}
		
		Object phObj = GetGame().CreateObject("Land_Boat_Small1", MissionBuilding.ModelToWorld("0.360 -7.410 6.730"), true);
		phObj.SetPosition(MissionBuilding.ModelToWorld("1.560 -7.340 6.730"));
		phObj.SetDirection(MissionBuilding.GetDirection());
		phObj.SetOrientation(phObj.GetOrientation() + "0 90 0");
		m_MissionObjects.Insert(phObj);
	}	
	
	void SpawnAIs()
	{
		vector SurvPos = MissionBuilding.ModelToWorld("-6.3545 6.1203 -4.6489");
		
		PlayerBase DeadSurvivor = PlayerBase.Cast(GetGame().CreatePlayer(
			null, "SurvivorM_Oliver", SurvPos, 0, "Oliver"));

		DeadSurvivor.GetInventory().CreateInInventory("HikingJacket_Black");
		DeadSurvivor.GetInventory().CreateInInventory("Jeans_Blue");
		DeadSurvivor.GetInventory().CreateInInventory("HikingBoots_Brown");
		DeadSurvivor.GetInventory().CreateInInventory("Glock19");
		DeadSurvivor.GetInventory().CreateInInventory("Mag_Glock_15Rnd");

		DeadSurvivor.SetHealth("","",20);
		DeadSurvivor.SetName(SurvivorName);
		DeadSurvivor.SetBloodyHands(true);
		DeadSurvivor.SetSynchDirty();

		m_MissionAIs.Insert(DeadSurvivor);
				
		vector GirlPos = MissionBuilding.ModelToWorld("-9.4111 6.1203 -4.8696");
		m_MissionAIs.Insert(GetGame().CreateObject("ZmbF_JournalistNormal_White", GirlPos, false, true));

		for (int j = 0; j < InfectedSpawns.Count(); j++)
		{
			string RandInf = InfectedTypes.GetRandomElement();
			vector pos = MissionBuilding.ModelToWorld(InfectedSpawns[j]);

			DayZInfected Zed = DayZInfected.Cast(GetGame().CreateObject(RandInf, pos, false, true));
			m_MissionAIs.Insert(Zed);
		}
	}
			
	void ObjDespawn()
	{
		for (int i = 0; i < m_MissionObjects.Count(); i++)
		{
			if (!m_MissionObjects[i]) continue;

			string type = m_MissionObjects[i].GetType();
			if (type == "WoodenLog" || type == "WoodenPlank" || type == "Land_Boat_Small1") continue;

			GetGame().ObjectDelete(m_MissionObjects[i]);
		}
	}
	
	override void MissionFinal()
	{
		Building Tenement = Building.Cast(MissionBuilding);

		for (int i = 0; i < 32; i++)
			if (!Tenement.IsDoorOpen(i)) Tenement.OpenDoor(i);

		for (int j = 0; j < InfectedSpawns.Count(); j++)
		{
			string RandInf = InfectedTypes.GetRandomElement();
			vector pos = MissionBuilding.ModelToWorld(InfectedSpawns[j]);

			if (vector.Distance(pos, m_MissionPosition) > 5.0)
			{
				DayZInfected Zed = DayZInfected.Cast(
					GetGame().CreateObject(RandInf, pos, false, true));
				
				m_MissionAIs.Insert(Zed);
			}
		}
		
		m_RewardsSpawned = true;
		m_MsgNum = -1;
		m_MsgChkTime = m_MissionTime + MsgDlyFinish;
	}
	
	override void PlayerChecks(PlayerBase player)
	{
		// AQUI ESTÁ A CORREÇÃO! Substituímos a variável inexistente por 'true'
		if (true) 
		{
			if (m_MissionObjects.Count() > 0 && m_MissionObjects[0] && m_MissionObjects[0].ClassName() == "SeaChest")
			{
				if (player.GetInventory().HasEntityInInventory(EntityAI.Cast(m_MissionObjects[0])) && !m_ContainerWasTaken)
				{
					m_ContainerWasTaken = true;
					GetGame().ObjectDelete(m_MissionObjects[0]);
				}
			}
		}
	}
	
	void UpdateBots(float dt) {}
			
	override bool DeployMission()
	{
		GetGame().GetObjectsAtPosition(m_MissionPosition, 1.0, m_ObjectList, m_ObjectCargoList);

		for (int i = 0; i < m_ObjectList.Count(); i++)
		{
			Object obj = m_ObjectList[i];

			if (obj.GetType() == "Land_Tenement_Small")
			{	
				MissionBuilding = obj;
				m_MissionPosition = MissionBuilding.ModelToWorld(Spawnpoints.GetRandomElement());
				break;
			}
		}	
				
		if (MissionBuilding)
		{	
			Building Tenement = Building.Cast(MissionBuilding);

			for (int k = 0; k < 32; k++)
				if (Tenement.IsDoorOpen(k)) Tenement.CloseDoor(k);

			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Call(this.SpawnObjects);
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Call(this.SpawnAIs);

			return true;
		}

		return false;
	}
}