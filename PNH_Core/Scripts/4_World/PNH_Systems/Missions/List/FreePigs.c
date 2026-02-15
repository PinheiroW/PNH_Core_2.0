class FreePigsMission extends SurvivorMissions
{
	Object MissionBuilding;
	ItemBase MissionObject; 
	int ReqMeatAmount = 5; 
	int MsgDlyFinish = 60;
	ref array<vector> ExtendedPosList = new array<vector>;
	ref array<string> InfectedTypes = new array<string>;
	vector RewardsPosition = "0.84 -0.75 0.16";	
	vector TargetPosition = "-1.74 -3.01 0.49"; 

    string m_WinnerName = "Desconhecido";
    bool m_IsVictory = false; 
	
	override bool IsExtended() return true; 
	
	void FreePigsMission()
	{		
		m_MissionExtended = true;
		m_MissionTimeout = 2700;			
		m_MissionZoneOuterRadius = 150.0;	
		m_MissionZoneInnerRadius = 5.0; 
		m_MissionInformant = "PNH Bio-Controle"; 
		m_MissionMessage1 = "[RADIO PNH] ALERTA: Infecção saltou para espécimes suínos. Abata os porcos contaminados.";
		m_MissionMessage2 = "Colete 5 amostras de carne e entregue no Barril Azul no Abrigo de Trilha.";
		m_MissionMessage3 = "[RADIO PNH] Localização da Fazenda: ** " + m_MissionLocation + " **.";
	}

    void SpawnRewards()
    {
        if (!MissionBuilding) return;
        ItemBase Chest = ItemBase.Cast( GetGame().CreateObject( "SeaChest", MissionBuilding.ModelToWorld("0 0 0"), false, false, false ) );
        if (Chest)
        {
            Chest.GetInventory().CreateInInventory("M70_Tundra");
            Chest.GetInventory().CreateInInventory("HuntingOptic");
            Chest.GetInventory().CreateInInventory("GhillieSuit_Mossy");
            m_MissionObjects.Insert( Chest );
        }
    }

	override void MissionFinal()
	{	
        if (m_IsVictory) PNH_Logger.Log("Missões", "[PNH_CORE] MISSÃO_CONCLUÍDA: " + m_WinnerName + " extraiu as amostras do Hospedeiro Zero!");
		m_RewardsSpawned = true; m_MsgNum = -1;
	}

	override void PlayerChecks( PlayerBase player )
	{
		if ( !m_MissionExtended && MissionObject )
		{
            if ( vector.Distance( player.GetPosition(), m_MissionPosition ) < 5.0 )
            {
                int count = 0;
                CargoBase cargo = MissionObject.GetInventory().GetCargo();
                if (cargo) {
                    for ( int i = 0; i < cargo.GetItemCount(); i++ )
                        if ( cargo.GetItem(i).GetType() == "PigMeat" ) count++;
                }
                if ( count >= ReqMeatAmount )
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
				if ( m_ObjectList.Get(i).GetType() == "Land_Barn_Metal_Big" ) { MissionBuilding = m_ObjectList.Get(i); break; }
			
            if (MissionBuilding) {
                PNH_Logger.Log("Missões", "[PNH_CORE] MISSÃO_INICIADA: Hospedeiro Zero em " + m_MissionLocation);
                return true;
            }
		}
		return false;
	}
}