class HordeMission extends PNH_MissionBase
{
	Object MissionBuilding; 
	int HordeDensity = 60;	
	int MsgDlyFinish = 60;					
	ref array<string> InfectedTypes = new array<string>;
    string m_WinnerName = "Desconhecido";
    bool m_IsVictory = false; 
	
	override bool IsExtended() return false; 
	
	void HordeMission()
	{
		m_MissionTimeout = 2700;			
		m_MissionZoneOuterRadius = 300.0;	
		m_MissionZoneInnerRadius = 5.0; 
		m_MissionInformant = "PNH Containment"; 
		m_MissionMessage1 = "[RADIO PNH] ALERTA MÁXIMO: Horda crítica detectada. Perímetro perdido.";
		m_MissionMessage2 = "O armamento de contenção ainda está no veículo da escolta no centro do enxame.";
		m_MissionMessage3 = "[RADIO PNH] Localização confirmada em ** " + m_MissionLocation + " **. Rompam o cerco.";
	}

    void SpawnRewards()
    {
        ItemBase Chest = ItemBase.Cast( GetGame().CreateObject( "SeaChest", m_MissionPosition, false, false, false ) );
        if (Chest)
        {
            Chest.GetInventory().CreateInInventory("M79");
            Chest.GetInventory().CreateInInventory("Ammo_40mm_Explosive");
            Chest.GetInventory().CreateInInventory("Ammo_40mm_Explosive");
            Chest.GetInventory().CreateInInventory("Saiga");
            Chest.GetInventory().CreateInInventory("Mag_Saiga_Drum20Rnd");
            m_MissionObjects.Insert( Chest );
        }
    }

	void SpawnAIs()
	{	
		for ( int i = 0 ; i < HordeDensity ; i++ )
		{
            float angle = Math.RandomFloat(0, 6.28);
            float radius = Math.RandomFloat(10, 35);
            vector pos = m_MissionPosition + Vector(Math.Cos(angle) * radius, 0, Math.Sin(angle) * radius);
            pos[1] = GetGame().SurfaceY(pos[0], pos[2]);
			m_MissionAIs.Insert( GetGame().CreateObject( "ZmbM_usSoldier_normal_Woodland", pos, false, true ) );
		}	
	}
	
	override void MissionFinal()
	{	
        if (m_IsVictory) PNH_Logger.Log("Missões", "[PNH_CORE] MISSÃO_CONCLUÍDA: " + m_WinnerName + " rompeu o cerco da Horda!");
		m_RewardsSpawned = true; m_MsgNum = -1;
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
		if ( m_MissionPosition && m_MissionPosition != "0 0 0" )
		{
            MissionBuilding = GetGame().CreateObject( "Wreck_PoliceCar", m_MissionPosition, false, false, false );
            if ( MissionBuilding )
            {
                vector pos = MissionBuilding.GetPosition();
                pos[1] = GetGame().SurfaceY(pos[0], pos[2]);
                MissionBuilding.SetPosition(pos);
                m_MissionPosition = pos;
                m_MissionObjects.Insert( MissionBuilding );
                
                PNH_Logger.Log("Missões", "[PNH_CORE] MISSÃO_INICIADA: Horda de Elite em " + m_MissionLocation);
                SpawnAIs();
                return true;
            }
        }
		return false;
	}
}