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
		
		TStringArray SurvivorNames = {"Yuri","Michail","Boris","Valeri","Anatoli","Ivan","Alexej","Dimitrij","Sergej","Nikolai"};
		SurvivorName = SurvivorNames.GetRandomElement();
						
		m_MissionMessage1 = SurvivorName +" foi um dos meus melhores alunos. Eu falava com ele pelo rádio, mas agora perdi o contato.";
		m_MissionMessage2 = "Ele escondeu um baú com equipamentos nos apartamentos e barricou o prédio. Verifique se ele ainda está vivo.";
		m_MissionMessage3 = "Ele morava em um dos apartamentos ao "+ m_MissionLocationDir +" de "+ m_MissionLocation + ". Câmbio!";
				
		Spawnpoints.Insert("8.1257 2.7203 3.1963");
		Spawnpoints.Insert("-8.3906 -0.6797 -1.1826");
		Spawnpoints.Insert("-7.3033 6.1203 -5.8271");
		
		InfectedSpawns.Insert("-1.5186 -7.4796 1.0269");
		InfectedSpawns.Insert("4.9775 -7.4796 -1.4146");
		
		InfectedTypes.Insert("ZmbM_CitizenASkinny_Brown"); 
		InfectedTypes.Insert("ZmbF_JournalistNormal_White");
		
		Barricades.Insert(new Param3<string,vector,vector>("WoodenLog", "-0.300 -6.740 6.940", "0 90 0"));
		Barricades.Insert(new Param3<string,vector,vector>("WoodenPlank", "-0.300 -7.495 6.325", "-90 0 0"));
	}
			
	void SpawnObjects()
	{
		ItemBase MissionObject = ItemBase.Cast(GetGame().CreateObject("SeaChest", m_MissionPosition, false, false, false));
		
		// Loadout Fixo para garantir que nunca venha vazio até criarmos o JSON
		EntityAI weapon = MissionObject.GetInventory().CreateInInventory("M4A1_Green");
		weapon.GetInventory().CreateAttachment("M4_RISHndgrd_Green");
		weapon.GetInventory().CreateAttachment("M4_MPBttstck");
		weapon.GetInventory().CreateAttachment("ACOGOptic");
		weapon.GetInventory().CreateAttachment("M4_Suppressor");

		MissionObject.GetInventory().CreateInInventory("Mag_STANAG_30Rnd");
		MissionObject.GetInventory().CreateInInventory("Mag_STANAG_30Rnd");
		MissionObject.GetInventory().CreateInInventory("Ammo_556x45");
		MissionObject.GetInventory().CreateInInventory("Ammo_556x45");
		MissionObject.GetInventory().CreateInInventory("KitchenKnife");
		MissionObject.GetInventory().CreateInInventory("CanOpener");
		
		m_MissionObjects.Insert(MissionObject);
		
		// Spawn barricadas
		for (int k = 0; k < Barricades.Count(); k++)
		{
			Param3<string,vector,vector> B = Barricades[k];
			Object plank = GetGame().CreateObject(B.param1, MissionBuilding.ModelToWorld(B.param2), false, false, true);
			plank.SetDirection(MissionBuilding.GetDirection());
			plank.SetOrientation(plank.GetOrientation() + B.param3);
			m_MissionObjects.Insert(plank);
		}
	}	
	
	void SpawnAIs()
	{
		vector SurvPos = MissionBuilding.ModelToWorld("-6.3545 6.1203 -4.6489");
		PlayerBase DeadSurvivor = PlayerBase.Cast(GetGame().CreatePlayer(null, "SurvivorM_Oliver", SurvPos, 0, "Oliver"));
		
		// --- VESTIR O CORPO ---
		DeadSurvivor.GetInventory().CreateAttachment("M65Jacket_Black");
		DeadSurvivor.GetInventory().CreateAttachment("CargoPants_Black");
		DeadSurvivor.GetInventory().CreateAttachment("CombatBoots_Black");
		
		// --- MATAR O SOBREVIVENTE (Vida = 0) ---
		DeadSurvivor.SetHealth("", "", 0);
		
		DeadSurvivor.SetName(SurvivorName);
		DeadSurvivor.SetBloodyHands(true);
		m_MissionAIs.Insert(DeadSurvivor);

		for (int j = 0; j < InfectedSpawns.Count(); j++)
		{
			string RandInf = InfectedTypes.GetRandomElement();
			vector pos = MissionBuilding.ModelToWorld(InfectedSpawns[j]);
			m_MissionAIs.Insert(GetGame().CreateObject(RandInf, pos, false, true));
		}
	}
	
	override void MissionFinal()
	{
		Building Tenement = Building.Cast(MissionBuilding);
		for (int i = 0; i < 32; i++) if (!Tenement.IsDoorOpen(i)) Tenement.OpenDoor(i);
		m_RewardsSpawned = true;
		m_MsgNum = -1;
		m_MsgChkTime = m_MissionTime + MsgDlyFinish;
	}
	
	override void PlayerChecks(PlayerBase player)
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