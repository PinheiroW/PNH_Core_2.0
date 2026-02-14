class ApartmentMission extends PNH_MissionBase
{
	Object MissionBuilding;
	int MsgDlyFinish = 300; 
	
	ref array<vector> Spawnpoints = new array<vector>;
	ref array<ref Param3<string,vector,vector>> Barricades = new array<ref Param3<string,vector,vector>>;
	ref array<vector> InfectedSpawns = new array<vector>;
	ref array<string> InfectedTypes = new array<string>;
	
	string SurvivorName;	
	
	override bool IsExtended() { return false; }
	
	void ApartmentMission()
	{
		m_MissionTimeout = 2700; 
		m_MissionZoneOuterRadius = 90.0; 
		m_MissionZoneInnerRadius = 2.0;  
					
		m_MissionInformant = "Dr. Legasov";
		
		TStringArray SurvivorNames = {
			"Yuri","Michail","Boris","Valeri","Anatoli","Ivan",
			"Alexej","Dimitrij","Sergej","Nikolai"
		};
		SurvivorName = SurvivorNames.GetRandomElement();
						
		m_MissionMessage1 = SurvivorName +" foi um dos meus melhores alunos. Eu falava com ele pelo rádio, mas agora perdi o contato com ele.";
		m_MissionMessage2 = "Ele me disse que escondeu um baú com um bom equipamento em algum lugar dos apartamentos. Poucos dias atrás, ele barricou todo o prédio contra os infectados.";
		m_MissionMessage3 = "Ele morava em um dos apartamentos\n** "+ m_MissionLocationDir +" do "+ m_MissionLocation+" **\nPor favor, verifique se "+SurvivorName+" ainda está vivo. Se não, pegue as coisas dele para ajudá-lo a sobreviver.";
				
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
		
		InfectedSpawns.Insert("-1.5186 -7.4796 1.0269");
		InfectedSpawns.Insert("4.9775 -7.4796 -1.4146");
		InfectedSpawns.Insert("-7.1726 -4.0797 -6.2729");
		InfectedSpawns.Insert("-2.9209 -0.6797 4.6636");
		InfectedSpawns.Insert("5.0283 2.7203 -1.3276");
		InfectedSpawns.Insert("-7.2461 6.1203 -1.5884");
		InfectedSpawns.Insert("-1.6855 6.1204 5.9956");
		
		InfectedTypes.Insert("ZmbM_CitizenASkinny_Brown"); InfectedTypes.Insert("ZmbF_JournalistNormal_White");
		InfectedTypes.Insert("ZmbM_priestPopSkinny");      InfectedTypes.Insert("ZmbF_Clerk_Normal_White");
		InfectedTypes.Insert("ZmbM_HermitSkinny_Beige");   InfectedTypes.Insert("ZmbF_CitizenANormal_Blue");
		InfectedTypes.Insert("ZmbM_CitizenBFat_Red");      InfectedTypes.Insert("ZmbF_CitizenBSkinny");
		
		Barricades.Insert(new Param3<string,vector,vector>("WoodenLog", "-0.300 -6.740 6.940", "0 90 0"));
		Barricades.Insert(new Param3<string,vector,vector>("WoodenLog", "-0.300 -5.200 6.768", "0 -90 0"));
		Barricades.Insert(new Param3<string,vector,vector>("WoodenLog", "-0.300 -7.050 6.376", "0 45 0"));
		Barricades.Insert(new Param3<string,vector,vector>("WoodenPlank", "-0.300 -7.495 6.325", "-90 0 0"));
		Barricades.Insert(new Param3<string,vector,vector>("WoodenPlank", "0.660 -7.495 6.000", "-50 0 0"));
		Barricades.Insert(new Param3<string,vector,vector>("WoodenPlank", "-0.660 -7.495 6.000", "-100 0 0"));
		Barricades.Insert(new Param3<string,vector,vector>("WoodenPlank", "-1.260 -7.495 5.700", "-130 0 0"));
		Barricades.Insert(new Param3<string,vector,vector>("WoodenPlank", "-1.560 -7.450 5.900", "-230 0 0"));
	}
			
	void SpawnObjects()
	{
		ItemBase MissionObject = ItemBase.Cast(GetGame().CreateObject("SeaChest", m_MissionPosition, false, false, false));
		
		int selectedLoadout = Math.RandomIntInclusive(0,2);
		EntityAI weapon;

		if (selectedLoadout == 0)
		{
			weapon = MissionObject.GetInventory().CreateInInventory("M4A1_Green");
			MissionObject.GetInventory().CreateInInventory("Mag_STANAG_30Rnd");
			MissionObject.GetInventory().CreateInInventory("Ammo_556x45");
			MissionObject.GetInventory().CreateInInventory("Battery9V");
		}
		
		m_MissionObjects.Insert(MissionObject);
		
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
	}	
	
	void SpawnAIs()
	{
		vector SurvPos = MissionBuilding.ModelToWorld("-6.3545 6.1203 -4.6489");
		PlayerBase DeadSurvivor = PlayerBase.Cast(GetGame().CreatePlayer(null, "SurvivorM_Oliver", SurvPos, 0, "Oliver"));
		DeadSurvivor.SetHealth("","",20);
		DeadSurvivor.SetName(SurvivorName);
		DeadSurvivor.SetBloodyHands(true);
		m_MissionAIs.Insert(DeadSurvivor);

		for (int j = 0; j < InfectedSpawns.Count(); j++)
		{
			string RandInf = InfectedTypes.GetRandomElement();
			vector pos = MissionBuilding.ModelToWorld(InfectedSpawns[j]);
			DayZInfected Zed = DayZInfected.Cast(GetGame().CreateObject(RandInf, pos, false, true));
			m_MissionAIs.Insert(Zed);
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
				DayZInfected Zed = DayZInfected.Cast(GetGame().CreateObject(RandInf, pos, false, true));
				m_MissionAIs.Insert(Zed);
			}
		}
		
		m_RewardsSpawned = true;
		m_MsgNum = -1;
		m_MsgChkTime = m_MissionTime + MsgDlyFinish;
	}
	
	override void PlayerChecks(PlayerBase player)
	{
	    // CORREÇÃO: Removido o MissionSettings!
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
			for (int k = 0; k < 32; k++) if (Tenement.IsDoorOpen(k)) Tenement.CloseDoor(k);

			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Call(this.SpawnObjects);
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Call(this.SpawnAIs);
			return true;
		}

		return false;
	}
}