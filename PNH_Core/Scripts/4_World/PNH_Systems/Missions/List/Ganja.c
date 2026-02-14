class GanjaMission extends PNH_MissionBase
{
	Object MissionBuilding;
	ItemBase MissionObject; 
	int ReqWeedAmount = 10;	
	int MsgDlyFinish = 60;
	ref array<vector> ExtendedPosList = new array<vector>;
	ref array<string> InfectedTypes = new array<string>;

    string m_WinnerName = "Desconhecido";
    bool m_IsVictory = false; 
	
	override bool IsExtended() return true; 
	
	void GanjaMission()
	{		
		m_MissionExtended = true;
		m_MissionTimeout = 2700;			
		m_MissionZoneOuterRadius = 150.0;	
		m_MissionZoneInnerRadius = 5.0; 
		m_MissionInformant = "PNH Pharma-Div";
		m_MissionMessage1 = "[RADIO PNH] ALERTA: Cultivo de bio-sintéticos localizado na Estufa Civil.";
		m_MissionMessage2 = "Localize a Mochila DryBag e recupere 10 amostras de Cannabis.";
		m_MissionMessage3 = "[RADIO PNH] Entregue no Posto de Gasolina de ** " + m_MissionLocation + " **.";
		InfectedTypes.Insert("ZmbM_NBC_Yellow");
	}

    void SpawnRewards()
    {
        ItemBase Chest = ItemBase.Cast( GetGame().CreateObject( "SeaChest", m_MissionPosition, false, false, false ) );
        if (Chest)
        {
            Chest.GetInventory().CreateInInventory("Aug");
            Chest.GetInventory().CreateInInventory("NBCSuit_Yellow");
            Chest.GetInventory().CreateInInventory("Antidote");
            m_MissionObjects.Insert( Chest );
        }
    }

	override void MissionFinal()
	{	
        if (m_IsVictory) PNH_Logger.Log("Missões", "[PNH_CORE] MISSÃO_CONCLUÍDA: " + m_WinnerName + " entregou os bio-sintéticos!");
		m_RewardsSpawned = true; m_MsgNum = -1;
	}

	override bool DeployMission()
	{
		if ( m_MissionExtended )
		{
			GetGame().GetObjectsAtPosition( m_MissionPosition , 1.0 , m_ObjectList , m_ObjectCargoList );
			for ( int i=0; i < m_ObjectList.Count(); i++ )
				if ( m_ObjectList.Get(i).GetType() == "Land_City_GreenHouse" ) { MissionBuilding = m_ObjectList.Get(i); break; }
			
            if (MissionBuilding) {
                MissionObject = ItemBase.Cast( GetGame().CreateObject( "DryBag_Green", MissionBuilding.ModelToWorld("0 0 0"), false, false, false ) );
                m_MissionObjects.Insert( MissionObject );
                PNH_Logger.Log("Missões", "[PNH_CORE] MISSÃO_INICIADA: Operação Bio-Sintético em " + m_MissionLocation);
                return true;
            }
		}
		return false;
	}
}