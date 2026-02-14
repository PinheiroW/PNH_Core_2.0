class BearHuntMission extends PNH_MissionBase
{
	ItemBase MissionObject;
	Object MissionBuilding;
	int ReqMeatAmount = 4;
	int MsgDlyFinish = 60;
	ref array<vector> ExtendedPosList = new array<vector>;
	ref array<string> InfectedTypes = new array<string>;
    string m_WinnerName = "Desconhecido";
    bool m_IsVictory = false; 

	override bool IsExtended() return true;
	
	void BearHuntMission()
	{	
		m_MissionExtended = true;
		m_MissionTimeout = 2700;
		m_MissionZoneOuterRadius = 150.0;
		m_MissionZoneInnerRadius = 3.0;
		m_MissionInformant = "PNH Bio-Intelligence";
		m_MissionMessage1 = "[RADIO PNH] ALERTA: Espécime Alpha mutante detectado na floresta.";
		m_MissionMessage2 = "Abata o espécime e colete 4 amostras de tecido.";
		m_MissionMessage3 = "Entregue as amostras no laboratório da Escola de ** " + m_MissionLocation + " **.";
	}

    void SpawnRewards()
    {
        ItemBase Chest = ItemBase.Cast( GetGame().CreateObject( "SeaChest", m_MissionPosition, false, false, false ) );
        if (Chest)
        {
            Chest.GetInventory().CreateInInventory("Winchester70");
            Chest.GetInventory().CreateInInventory("HuntingOptic");
            Chest.GetInventory().CreateInInventory("Ammo_308Win");
            m_MissionObjects.Insert( Chest );
        }
    }

	override void MissionFinal()
	{	
        if (m_IsVictory) PNH_Logger.Log("Missões", "[PNH_CORE] MISSÃO_CONCLUÍDA: " + m_WinnerName + " extraiu o tecido mutante!");
		m_RewardsSpawned = true; m_MsgNum = -1;
	}

	override bool DeployMission()
	{
		if ( m_MissionExtended ) {
            PNH_Logger.Log("Missões", "[PNH_CORE] MISSÃO_INICIADA: Caçada Alpha em " + m_MissionLocation);
            return true;
        }
		return false;
	}
}