class UrbanMallMission extends SurvivorMissions
{
	Object MissionBuilding;
	ItemBase MissionObject; // DryBag Preta (Kit Técnico)
	
	int ReqWireAmount = 5;	// 5 Arames
	int ReqBattAmount = 5;	// 5 Baterias 9V
	int ExtendedTimout = 1800;				
	int MsgDlyFinish = 60;					
	
	ref array<vector> ExtendedPosList = new array<vector>;
	ref array<string> InfectedTypes = new array<string>;

	vector TargetPosition = "-5.3 -1.2 1.9"; 
	vector RewardsPosition = "0 -2 3.0"; 

    string m_WinnerName = "Desconhecido";
    bool m_IsVictory = false; 
	
	override bool IsExtended() return true; 
	
	void UrbanMallMission()
	{
		m_MissionExtended = true;
		m_MissionTimeout = 3600; 
		m_MissionZoneOuterRadius = 200.0;	
		m_MissionZoneInnerRadius = 10.0; 
				
		m_MissionInformant = "PNH Tech-Ops"; 
		
		m_MissionMessage1 = "[RADIO PNH] FALHA CRÍTICA: Torre offline. Componentes no Shopping Center.";
		m_MissionMessage2 = "Recupere o Kit Técnico (Mochila Preta) com 5 Arames e 5 Baterias.";
		m_MissionMessage3 = "[RADIO PNH] Entregue na Delegacia Central de ** " + m_MissionLocation + " **.";

		InfectedTypes.Insert("ZmbM_PoliceFat");
		InfectedTypes.Insert("ZmbM_usSoldier_normal_Woodland");
	}

    void SpawnRewards()
    {
        if (!MissionBuilding) return;

        ItemBase Chest = ItemBase.Cast( GetGame().CreateObject( "SeaChest", MissionBuilding.ModelToWorld( RewardsPosition ), false, false, false ) );
        if (Chest)
        {
            Chest.GetInventory().CreateInInventory("M4A1_Black");
            Chest.GetInventory().CreateInInventory("Mag_STANAG_60Rnd");
            Chest.GetInventory().CreateInInventory("NVGoggles");
            Chest.GetInventory().CreateInInventory("Battery9V");
            Chest.GetInventory().CreateInInventory("PlateCarrierVest_Black");
            m_MissionObjects.Insert( Chest );
        }
    }

    void SpawnCollectionItems()
    {
        if (!MissionBuilding) return;

        for (int w = 0; w < ReqWireAmount; w++) 
            GetGame().CreateObject("MetalWire", MissionBuilding.ModelToWorld("-6.0 -1.2 3.0"), false, false, false);
        
        for (int b = 0; b < ReqBattAmount; b++) 
            GetGame().CreateObject("Battery9V", MissionBuilding.ModelToWorld("-4.0 -1.2 1.0"), false, false, false);
    }

	override void MissionFinal()
	{	
        if (m_IsVictory) PNH_Logger.Log("Missões", "[PNH_CORE] MISSÃO_CONCLUÍDA: " + m_WinnerName + " reativou a rede elétrica na Delegacia!");
		m_RewardsSpawned = true; m_MsgNum = -1;
		m_MsgChkTime = m_MissionTime + MsgDlyFinish;			
	}
	
	override void PlayerChecks( PlayerBase player )
	{
		if ( !m_MissionExtended && MissionObject )
		{
            if ( vector.Distance( MissionObject.GetPosition(), m_MissionPosition ) < 5.0 )
            {
                int wireCount = 0; int battCount = 0;
                CargoBase cargo = MissionObject.GetInventory().GetCargo();
                if (cargo) {
                    for ( int i = 0; i < cargo.GetItemCount(); i++ )
                    {
                        if ( cargo.GetItem(i).GetType() == "MetalWire" ) wireCount++;
                        if ( cargo.GetItem(i).GetType() == "Battery9V" ) battCount++;
                    }
                }
                if ( wireCount >= ReqWireAmount && battCount >= ReqBattAmount )
                {
                    m_IsVictory = true;
                    if (player.GetIdentity()) m_WinnerName = player.GetIdentity().GetName();
                    GetGame().ObjectDelete( MissionObject );
                    SpawnRewards();
                    MissionFinal();
                }
            }
		}		
	}
	
	override bool DeployMission()
	{
		if ( m_MissionExtended )
		{			
			GetGame().GetObjectsAtPosition( m_MissionPosition , 1.0 , m_ObjectList , m_ObjectCargoList );
			for ( int i=0; i < m_ObjectList.Count(); i++ )
				if ( m_ObjectList.Get(i).GetType() == "Land_City_Mall" ) { MissionBuilding = m_ObjectList.Get(i); break; }
			
            if (MissionBuilding) {
                MissionObject = ItemBase.Cast( GetGame().CreateObject( "DryBag_Black", MissionBuilding.ModelToWorld( TargetPosition ), false, false, false ) );
                m_MissionObjects.Insert( MissionObject );
                SpawnCollectionItems();
                PNH_Logger.Log("Missões", "[PNH_CORE] MISSÃO_INICIADA: Operação Blackout no Shopping de " + m_MissionLocation);
                return true;
            }
		} else {
            if ( EventsWorldData.GetBuildingsAtLoc("Land_City_PoliceStation", m_MissionDescription[3], ExtendedPosList ))
            {
                m_MissionPosition = ExtendedPosList.GetRandomElement();
                GetGame().GetObjectsAtPosition( m_MissionPosition , 1.0 , m_ObjectList , m_ObjectCargoList );
                for ( int j=0; j < m_ObjectList.Count(); j++ )
                    if ( m_ObjectList.Get(j).GetType() == "Land_City_PoliceStation" ) { 
                        MissionBuilding = m_ObjectList.Get(j); 
                        m_MissionPosition = MissionBuilding.ModelToWorld( RewardsPosition ); 
                        break; 
                    }
            }
        }
		return false; 
	}
}