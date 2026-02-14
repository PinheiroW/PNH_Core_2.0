class TransportMission extends PNH_MissionBase
{
	Object MissionBuilding;
	ItemBase MissionObject; 
    Car MissionCar;         
	
	int ReqBiomassAmount = 15;				
	int ExtendedTimout = 1800;				
	int MsgDlyFinish = 60;					
	
	ref array<vector> ExtendedPosList = new array<vector>;
	ref array<string> InfectedTypes = new array<string>;
	
    string m_WinnerName = "Desconhecido";
    bool m_IsVictory = false; 
	
	override bool IsExtended() return true;
	
	void TransportMission()
	{	
		m_MissionExtended = true;
		m_MissionTimeout = 2700;			
		m_MissionZoneOuterRadius = 150.0;	
		m_MissionZoneInnerRadius = 5.0; 
				
		m_MissionInformant = "PNH Logistics";
	
		m_MissionMessage1 = "[RADIO PNH] FALHA LOGÍSTICA: Veículo abandonado na Garagem Comercial.";
		m_MissionMessage2 = "Localize o Sedan. Colete 15 Biomassas (Cannabis) na mochila laranja.";
		m_MissionMessage3 = "[RADIO PNH] Entregue no Posto de Gasolina de ** " + m_MissionLocation + " **.";
		
		InfectedTypes.Insert("ZmbM_ConstructWorkerNormal_Blue");
		InfectedTypes.Insert("ZmbM_Motobiker_Black");
	}

    void SpawnRewards()
    {
        if (!MissionBuilding) return;

        ItemBase Chest = ItemBase.Cast( GetGame().CreateObject( "SeaChest", MissionBuilding.ModelToWorld("0 0 0"), false, false, false ) );
        if (Chest)
        {
            Chest.GetInventory().CreateInInventory("Vikhr");
            Chest.GetInventory().CreateInInventory("Mag_Vikhr_30Rnd");
            Chest.GetInventory().CreateInInventory("CarBattery");
            Chest.GetInventory().CreateInInventory("SparkPlug");
            Chest.GetInventory().CreateInInventory("CarRadiator");
            Chest.GetInventory().CreateInInventory("CanisterGasoline");
            m_MissionObjects.Insert( Chest );
        }
    }
	
	void SpawnObjects()
	{			
		if ( m_MissionExtended && MissionBuilding )
		{	
            MissionCar = Car.Cast( GetGame().CreateObject( "CivilianSedan", MissionBuilding.ModelToWorld("0 0 0"), false, false, false ) );
            if (MissionCar) {
                MissionCar.SetOrientation( MissionBuilding.GetOrientation() );
                m_MissionObjects.Insert( MissionCar );
            }

			MissionObject = ItemBase.Cast( GetGame().CreateObject( "DryBag_Orange", MissionBuilding.ModelToWorld("2 0 0"), false, false, false ));
			m_MissionObjects.Insert( MissionObject );

			for ( int i=0; i < ReqBiomassAmount; i++ )
				m_MissionObjects.Insert( GetGame().CreateObject( "Cannabis", MissionBuilding.ModelToWorld( Vector(Math.RandomFloat(-3,3), -0.5, Math.RandomFloat(-3,3)) ), false, true ));
		}
	}
	
	override void MissionFinal()
	{	
        if (m_IsVictory) PNH_Logger.Log("Missões", "[PNH_CORE] MISSÃO_CONCLUÍDA: " + m_WinnerName + " reabasteceu a frota PNH!");
		m_RewardsSpawned = true; m_MsgNum = -1;
	}
	
	override void PlayerChecks( PlayerBase player )
	{
		if ( !m_MissionExtended && MissionObject )
		{
            if ( vector.Distance( MissionObject.GetPosition(), m_MissionPosition ) < 5.0 )
            {
                int weedCount = 0;
                CargoBase cargo = MissionObject.GetInventory().GetCargo();
                if (cargo) {
                    for ( int i = 0; i < cargo.GetItemCount(); i++ )
                        if ( cargo.GetItem(i).GetType() == "Cannabis" ) weedCount++;
                }
                if ( weedCount >= ReqBiomassAmount )
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
			GetGame().GetObjectsAtPosition( m_MissionPosition , 150.0 , m_ObjectList , m_ObjectCargoList ); 
			for ( int i = 0 ; i < m_ObjectList.Count(); i++ )
				if ( m_ObjectList.Get(i).GetType() == "Land_Garage_Office" ) { MissionBuilding = m_ObjectList.Get(i); break; }
		} 
        else 
        {
             if ( PNH_EventsWorldData.GetBuildingsAtLoc("Land_FuelStation_Build", m_MissionDescription[3], ExtendedPosList ) )
            {
                m_MissionPosition = ExtendedPosList.GetRandomElement();
                GetGame().GetObjectsAtPosition( m_MissionPosition , 10.0 , m_ObjectList , m_ObjectCargoList );
                for ( int j=0; j < m_ObjectList.Count(); j++ )
                    if ( m_ObjectList.Get(j).GetType() == "Land_FuelStation_Build" ) { 
                        MissionBuilding = m_ObjectList.Get(j); 
                        m_MissionPosition = MissionBuilding.ModelToWorld("0 0 0"); 
                        break; 
                    }
            }
        }
					
		if ( MissionBuilding )
		{
            if (m_MissionExtended) {
                PNH_Logger.Log("Missões", "[PNH_CORE] MISSÃO_INICIADA: Rota de Abastecimento em " + m_MissionLocation);
                SpawnObjects();
            }
			return true;		
		}
		return false;
	}
}