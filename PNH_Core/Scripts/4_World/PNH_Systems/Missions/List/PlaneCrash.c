class PlaneCrashMission extends PNH_MissionBase
{
	override bool IsExtended() { return false; }
	
	void PlaneCrashMission()
	{
		m_MissionTimeout = 2700;			
		m_MissionZoneOuterRadius = 700.0;	
		m_MissionZoneInnerRadius = 15.0; 
		m_MissionInformant = "PNH Air Traffic"; 
	}

	override bool DeployMission()
	{
		if ( m_MissionPosition != "0 0 0" )
		{
            // Spawn do Avião
            GetGame().CreateObject( "Wreck_C130J", m_MissionPosition, false, false, true );
            
            m_MissionMessage1 = "MAYDAY! Voo tático abatido. Coordenadas recebidas.";
            m_MissionMessage2 = "A carga contém protótipos de NVG. A fumaça é visível.";
            m_MissionMessage3 = "Destroços na região de ** " + m_MissionLocation + " **. Assegurem a carga.";

            // Spawn dos Zumbis Militares
            for ( int i = 0; i < 25; i++ )
            {
                vector pos = m_MissionPosition + Vector(Math.RandomFloatInclusive(-25,25), 0, Math.RandomFloatInclusive(-25,25));
                pos[1] = GetGame().SurfaceY(pos[0], pos[2]);
                m_MissionAIs.Insert( GetGame().CreateObject( "ZmbM_usSoldier_normal_Woodland", pos, false, true ) );
            }
            return true;
		}
		return false;
	}

    override void PlayerChecks( PlayerBase player )
	{
		if ( vector.Distance( player.GetPosition(), m_MissionPosition ) < m_MissionZoneInnerRadius && !m_RewardsSpawned )
		{
            m_RewardsSpawned = true;
            
            ItemBase Chest = ItemBase.Cast( GetGame().CreateObject( "SeaChest", m_MissionPosition, false, false, false ) );
            if (Chest)
            {
                Chest.GetInventory().CreateInInventory("NVGoggles");
                Chest.GetInventory().CreateInInventory("Battery9V");
                Chest.GetInventory().CreateInInventory("Battery9V");
                Chest.GetInventory().CreateInInventory("KitchenKnife");
                m_MissionObjects.Insert(Chest);
            }
            
            m_MsgNum = -1;
		}
	}
}