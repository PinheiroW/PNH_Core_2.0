class HordeMission extends PNH_MissionBase
{
	Object MissionBuilding; 
	int HordeDensity = 40;	
	int MsgDlyFinish = 60;	
    string m_WinnerName = "Desconhecido";
    bool m_IsVictory = false; 
	
	override bool IsExtended() { return false; }
	
	void HordeMission()
	{
		m_MissionTimeout = 2700;			
		m_MissionZoneOuterRadius = 300.0;	
		m_MissionZoneInnerRadius = 10.0; 
		m_MissionInformant = "PNH Containment"; 
		
		// As mensagens serão atualizadas no DeployMission para pegarem a cidade correta
	}

    void SpawnRewards()
    {
        ItemBase Chest = ItemBase.Cast( GetGame().CreateObject( "SeaChest", m_MissionPosition, false, false, false ) );
        if (Chest)
        {
            Chest.GetInventory().CreateInInventory("M79");
            Chest.GetInventory().CreateInInventory("Ammo_40mm_Explosive");
            Chest.GetInventory().CreateInInventory("Ammo_40mm_Explosive");
            Chest.GetInventory().CreateInInventory("Ammo_40mm_Explosive");
            m_MissionObjects.Insert(Chest);
        }
    }

	void SpawnAIs()
	{
		for ( int i = 0; i < HordeDensity; i++ )
		{
			vector pos = m_MissionPosition + Vector(Math.RandomFloatInclusive(-40,40), 0, Math.RandomFloatInclusive(-40,40));
			pos[1] = GetGame().SurfaceY(pos[0], pos[2]);
			m_MissionAIs.Insert( GetGame().CreateObject( "ZmbM_usSoldier_normal_Woodland", pos, false, true ) );
		}	
	}
	
	override void MissionFinal()
	{	
        if (m_IsVictory) PNH_Logger.Log("Missões", "[PNH_CORE] MISSÃO_CONCLUÍDA: " + m_WinnerName + " rompeu o cerco da Horda!");
		m_RewardsSpawned = true; 
        m_MsgNum = -1;
	}
	
	override void PlayerChecks( PlayerBase player )
	{
		if ( vector.Distance( player.GetPosition(), m_MissionPosition ) < m_MissionZoneInnerRadius )
		{
            if (m_RewardsSpawned) return;
            m_IsVictory = true;
            if (player.GetIdentity()) m_WinnerName = player.GetIdentity().GetName();
            SpawnRewards();
            MissionFinal();
		}
	}
	
	override bool DeployMission()
	{
		if ( m_MissionPosition != Vector(0,0,0) )
		{
            // Criamos um ponto de referência (Carro de polícia destruído)
            MissionBuilding = GetGame().CreateObject( "Wreck_PoliceCar", m_MissionPosition, false, false, true );
            
            // Configura as mensagens com a localização correta
            m_MissionMessage1 = "[RADIO PNH] ALERTA MÁXIMO: Horda crítica detectada. Perímetro perdido.";
            m_MissionMessage2 = "O armamento de contenção está num veículo de escolta no centro do enxame.";
            m_MissionMessage3 = "[RADIO PNH] Localização: ** " + m_MissionLocation + " **. Rompam o cerco!";

            SpawnAIs();
            return true;
		}
		return false;
	}
}