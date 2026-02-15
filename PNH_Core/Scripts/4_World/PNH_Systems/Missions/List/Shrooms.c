class ShroomsMission extends SurvivorMissions
{
	Object MissionBuilding;
	ItemBase MissionObject; 
	int ReqShroomsCount = 10;				
	int ExtendedTimout = 1800;				
	int MsgDlyFinish = 60;					
	ref array<vector> ExtendedPosList = new array<vector>;
	ref array<string> InfectedTypes = new array<string>;
	vector RewardsPosition = "4.2 -5.2 6.0"; 

    string m_WinnerName = "Desconhecido";
    bool m_IsVictory = false; 
	
	override bool IsExtended() return true;
	
	void ShroomsMission()
	{	
		m_MissionExtended = true;
		m_MissionTimeout = 2700;			
		m_MissionZoneOuterRadius = 80.0;	
		m_MissionZoneInnerRadius = 5.0; 
		m_MissionInformant = "PNH Pharma-Division";
	
		m_MissionMessage1 = "[RADIO PNH] PRIORIDADE MÉDICA: Lote de 'Fungo Sambucus' regenerativo em Estufa Civil.";
		m_MissionMessage2 = "Localize a Mochila Laranja e colha 10 espécimes intactos.";
		m_MissionMessage3 = "[RADIO PNH] Transporte as amostras para o Hospital de ** " + m_MissionLocation + " **.";
		
		InfectedTypes.Insert("ZmbF_DoctorSkinny");
		InfectedTypes.Insert("ZmbM_DoctorFat");
	}

    void SpawnRewards()
    {
        if (!MissionBuilding) return;
        ItemBase Chest = ItemBase.Cast( GetGame().CreateObject( "SeaChest", MissionBuilding.ModelToWorld( RewardsPosition ), false, false, false ) );
        if (Chest)
        {
            Chest.GetInventory().CreateInInventory("AK101");
            Chest.GetInventory().CreateInInventory("Mag_AK101_30Rnd");
            Chest.GetInventory().CreateInInventory("KobraOptic");
            Chest.GetInventory().CreateInInventory("AK_Suppressor");
            ItemBase MedBag = ItemBase.Cast(Chest.GetInventory().CreateInInventory("FirstAidKit"));
            if (MedBag) {
                MedBag.GetInventory().CreateInInventory("BloodBagIV");
                MedBag.GetInventory().CreateInInventory("SalineBagIV");
            }
            m_MissionObjects.Insert( Chest );
        }
    }
	
	void SpawnObjects()
	{			
		if ( m_MissionExtended && MissionBuilding )
		{	
			MissionObject = ItemBase.Cast( GetGame().CreateObject( "DryBag_Orange", MissionBuilding.ModelToWorld("0 0 0"), false, false, false ));
			m_MissionObjects.Insert( MissionObject );
			for ( int i=0; i < ReqShroomsCount; i++ )
			{
				m_MissionObjects.Insert( GetGame().CreateObject( "SambucusMushroom", MissionBuilding.ModelToWorld( Vector(Math.RandomFloatInclusive(-1,1), -0.1, Math.RandomFloatInclusive(-3,3)) ), false, true ));
			}
		}
	}
	
	override void MissionFinal()
	{	
        if (m_IsVictory) PNH_Logger.Log("Missões", "[PNH_CORE] MISSÃO_CONCLUÍDA: " + m_WinnerName + " entregou os componentes médicos no Hospital!");
		m_RewardsSpawned = true; m_MsgNum = -1;
	}
	
	override void PlayerChecks( PlayerBase player )
	{
		if ( !m_MissionExtended && MissionObject )
		{
            if ( vector.Distance( MissionObject.GetPosition(), m_MissionPosition ) < 5.0 )
            {
                int count = 0;
                CargoBase cargo = MissionObject.GetInventory().GetCargo();
                if (cargo) {
                    for ( int i = 0; i < cargo.GetItemCount(); i++ )
                        if ( cargo.GetItem(i).GetType() == "SambucusMushroom" ) count++;
                }
                if ( count >= ReqShroomsCount )
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
				if ( m_ObjectList.Get(i).GetType() == "Land_City_GreenHouse" ) { MissionBuilding = m_ObjectList.Get(i); break; }
		} 
        else 
        {
            if ( EventsWorldData.GetBuildingsAtLoc("Land_City_Hospital", m_MissionDescription[3], ExtendedPosList ) )
            {
                m_MissionPosition = ExtendedPosList.GetRandomElement();
                GetGame().GetObjectsAtPosition( m_MissionPosition , 10.0 , m_ObjectList , m_ObjectCargoList );
                for ( int j=0; j < m_ObjectList.Count(); j++ )
                    if ( m_ObjectList.Get(j).GetType() == "Land_City_Hospital" ) { MissionBuilding = m_ObjectList.Get(j); m_MissionPosition = MissionBuilding.ModelToWorld( RewardsPosition ); break; }
            }
        }
					
		if ( MissionBuilding )
		{
            if (m_MissionExtended) {
                PNH_Logger.Log("Missões", "[PNH_CORE] MISSÃO_INICIADA: Operação Panaceia em " + m_MissionLocation);
                SpawnObjects();
            }
			return true;		
		}
		return false;
	}
}