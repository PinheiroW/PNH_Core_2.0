class CityMallMission extends SurvivorMissions
{
	Object MissionBuilding;
	ItemBase MissionObject;
	int ReqDocAmount = 10;
	int ReqRagAmount = 5;
	int MsgDlyFinish = 60;
	ref array<vector> ExtendedPosList = new array<vector>;
	ref array<string> InfectedTypes = new array<string>;
	vector TargetPosition = "-5.3 -1.2 1.9";  
	vector RewardsPosition = "-3.46 -5.72 6.63"; 

    string m_WinnerName = "Desconhecido";
    bool m_IsVictory = false; 
	
	override bool IsExtended() return true; 
	
	void CityMallMission()
	{
		m_MissionExtended = true;
		m_MissionTimeout = 3600;
		m_MissionZoneOuterRadius = 200.0;	
		m_MissionZoneInnerRadius = 10.0; 
		m_MissionInformant = "PNH Corp. Intelligence"; 
		m_MissionMessage1 = "[RADIO PNH] ALERTA: Documentos abandonados no Shopping Center.";
		m_MissionMessage2 = "Recupere 10 Documentos e 5 Uniformes na mochila tática.";
		m_MissionMessage3 = "[RADIO PNH] Entregue na Delegacia de ** " + m_MissionLocation + " **.";
		InfectedTypes.Insert("ZmbM_usSoldier_normal_Woodland");
		InfectedTypes.Insert("ZmbM_SoldierNormal");
	}

    void SpawnRewards()
    {
        if (!MissionBuilding) return;
        ItemBase Chest = ItemBase.Cast( GetGame().CreateObject( "SeaChest", MissionBuilding.ModelToWorld( RewardsPosition ), false, false, false ) );
        if (Chest)
        {
            Chest.GetInventory().CreateInInventory("M4A1");
            Chest.GetInventory().CreateInInventory("Mag_STANAG_60Rnd");
            Chest.GetInventory().CreateInInventory("ACOGOptic");
            Chest.GetInventory().CreateInInventory("PressVest_Blue");
            m_MissionObjects.Insert( Chest );
        }
    }

    void SpawnCollectionItems()
    {
        if (!MissionBuilding) return;
        for (int d = 0; d < ReqDocAmount; d++) GetGame().CreateObject("Paper", MissionBuilding.ModelToWorld("-6.0 -1.2 3.0"), false, false, false);
        for (int r = 0; r < ReqRagAmount; r++) GetGame().CreateObject("Rag", MissionBuilding.ModelToWorld("-4.0 -1.2 1.0"), false, false, false);
    }

	override void MissionFinal()
	{	
        if (m_IsVictory) PNH_Logger.Log("Missões", "[PNH_CORE] MISSÃO_CONCLUÍDA: " + m_WinnerName + " recuperou os dados da PNH!");
		m_RewardsSpawned = true; m_MsgNum = -1;
		m_MsgChkTime = m_MissionTime + MsgDlyFinish;			
	}
	
	override void PlayerChecks( PlayerBase player )
	{
		if ( !m_MissionExtended && MissionObject )
		{
            if ( vector.Distance( MissionObject.GetPosition(), m_MissionPosition ) < 5.0 )
            {
                int docCount = 0; int ragCount = 0;
                CargoBase cargo = MissionObject.GetInventory().GetCargo();
                if (cargo) {
                    for ( int i = 0; i < cargo.GetItemCount(); i++ )
                    {
                        if ( cargo.GetItem(i).GetType() == "Paper" ) docCount++;
                        if ( cargo.GetItem(i).GetType() == "Rag" ) ragCount++;
                    }
                }
                if ( docCount >= ReqDocAmount && ragCount >= ReqRagAmount )
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
                MissionObject = ItemBase.Cast( GetGame().CreateObject( "CoyoteBackpack_Black", MissionBuilding.ModelToWorld( TargetPosition ), false, false, false ) );
                m_MissionObjects.Insert( MissionObject );
                SpawnCollectionItems();
                PNH_Logger.Log("Missões", "[PNH_CORE] MISSÃO_INICIADA: Recuperação de Dados em " + m_MissionLocation);
                return true;
            }
		} 
        return false;
	}
}