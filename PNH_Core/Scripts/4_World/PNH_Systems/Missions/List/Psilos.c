class PsilosMission extends SurvivorMissions
{
	Object MissionBuilding;
	ItemBase MissionObject; 
	int ReqShroomsCount = 10;				
	int MsgDlyFinish = 60;					
	ref array<vector> ExtendedPosList = new array<vector>;
	ref array<string> InfectedTypes = new array<string>;
	vector RewardsPosition = "0 0 0"; 
    string m_WinnerName = "Desconhecido";
    bool m_IsVictory = false; 
	
	override bool IsExtended() return true;
	
	void PsilosMission()
	{	
		m_MissionExtended = true;
		m_MissionTimeout = 2700;			
		m_MissionZoneOuterRadius = 80.0;	
		m_MissionZoneInnerRadius = 5.0; 
		m_MissionInformant = "PNH Bio-Labs";
		m_MissionMessage1 = "[RADIO PNH] PROJETO MYCELIUM: Cultivo experimental na Estufa Militar.";
		m_MissionMessage2 = "Colha 10 amostras na mochila amarela dentro da estufa.";
		m_MissionMessage3 = "[RADIO PNH] Entregue na Delegacia de ** " + m_MissionLocation + " **.";
		InfectedTypes.Insert("ZmbM_NBC_Grey");
	}

    void SpawnRewards()
    {
        if (!MissionBuilding) return;
        ItemBase Chest = ItemBase.Cast( GetGame().CreateObject( "SeaChest", MissionBuilding.ModelToWorld( RewardsPosition ), false, false, false ) );
        if (Chest)
        {
            Chest.GetInventory().CreateInInventory("ASVAL");
            Chest.GetInventory().CreateInInventory("Mag_VAL_20Rnd");
            Chest.GetInventory().CreateInInventory("NBCSuit_Gray");
            Chest.GetInventory().CreateInInventory("NBCHood_Gray");
            Chest.GetInventory().CreateInInventory("Antidote");
            m_MissionObjects.Insert( Chest );
        }
    }

	void SpawnObjects()
	{			
		if ( m_MissionExtended && MissionBuilding )
		{	
			MissionObject = ItemBase.Cast( GetGame().CreateObject( "DryBag_Yellow", MissionBuilding.ModelToWorld("0 0 0"), false, false, false ));
			m_MissionObjects.Insert( MissionObject );
			for ( int i=0; i < ReqShroomsCount; i++ ) 
                m_MissionObjects.Insert( GetGame().CreateObject( "PsilocybeMushroom", MissionBuilding.ModelToWorld( Vector(Math.RandomFloat(-1.5,1.5), -0.1, Math.RandomFloat(-4,4)) ), false, true ));
		}
	}
	
	override void MissionFinal()
	{	
        if (m_IsVictory) PNH_Logger.Log("Missões", "[PNH_CORE] MISSÃO_CONCLUÍDA: " + m_WinnerName + " entregou os fungos do laboratório!");
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
                        if ( cargo.GetItem(i).GetType() == "PsilocybeMushroom" ) count++;
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
				if ( m_ObjectList.Get(i).GetType() == "Land_Mil_Greenhouse" ) { MissionBuilding = m_ObjectList.Get(i); break; }
		} 
        else 
        {
            if ( EventsWorldData.GetBuildingsAtLoc("Land_Village_PoliceStation", m_MissionDescription[3], ExtendedPosList ) )
            {
                m_MissionPosition = ExtendedPosList.GetRandomElement();
                GetGame().GetObjectsAtPosition( m_MissionPosition , 10.0 , m_ObjectList , m_ObjectCargoList );
                for ( int j=0; j < m_ObjectList.Count(); j++ )
                    if ( m_ObjectList.Get(j).GetType() == "Land_Village_PoliceStation" ) { MissionBuilding = m_ObjectList.Get(j); m_MissionPosition = MissionBuilding.ModelToWorld("0 0 0"); break; }
            }
        }
        if (MissionBuilding) {
            SpawnObjects();
            PNH_Logger.Log("Missões", "[PNH_CORE] MISSÃO_INICIADA: Mycelium na Estufa de " + m_MissionLocation);
            return true;
        }
		return false;
	}
}