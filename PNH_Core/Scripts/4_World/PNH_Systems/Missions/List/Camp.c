class CampMission extends PNH_MissionBase
{
	TentBase MissionObject;
	FireplaceBase MissionCampfire;
    string m_WinnerName = "Desconhecido";
    bool m_IsVictory = false; 
	
	override bool IsExtended() return false; 
	
	void CampMission()
	{
		m_MissionTimeout = 2700;			
		m_MissionZoneOuterRadius = 170.0;	
		m_MissionZoneInnerRadius = 15.0; 
		m_MissionInformant = "PNH Tactical Command"; 
		m_MissionMessage1 = "[RADIO PNH] ALERTA: Unidade Echo-4 desaparecida.";
		m_MissionMessage2 = "O acampamento está minado. Recupere os ativos com cautela.";
		m_MissionMessage3 = "Localização aproximada: Florestas de ** " + m_MissionLocation + " **.";
	}

    void SpawnRewards()
    {
        ItemBase Chest = ItemBase.Cast( GetGame().CreateObject( "SeaChest", m_MissionPosition + "1 0 1", false, false, false ) );
        if (Chest)
        {
            Chest.GetInventory().CreateInInventory("FAL");
            Chest.GetInventory().CreateInInventory("Mag_FAL_20Rnd");
            m_MissionObjects.Insert( Chest );
        }
    }

	override void MissionFinal()
	{	
        if (m_IsVictory) PNH_Logger.Log("Missões", "[PNH_CORE] MISSÃO_CONCLUÍDA: " + m_WinnerName + " recuperou o posto Echo-4!");
		m_RewardsSpawned = true; m_MsgNum = -1;
	}
	
	override bool DeployMission()
	{
		if ( m_MissionPosition && m_MissionPosition != "0 0 0" )
		{
            PNH_Logger.Log("Missões", "[PNH_CORE] MISSÃO_INICIADA: Acampamento Echo-4 em " + m_MissionLocation);
            SpawnRewards();
            return true;
        }
		return false;
	}
}