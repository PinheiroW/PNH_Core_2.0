class HordeMission extends PNH_MissionBase
{
	Object MissionBuilding; 
	int HordeDensity = 40;	
    bool m_IsVictory = false; 
	
	override bool IsExtended() { return false; }
	
	void HordeMission()
	{
		m_MissionTimeout = 2700;			
		m_MissionZoneOuterRadius = 300.0;	
		m_MissionZoneInnerRadius = 10.0; 
		m_MissionInformant = "PNH Containment"; 
	}

	override bool DeployMission()
	{
		if ( m_MissionPosition != "0 0 0" )
		{
            // Spawn do "centro" da horda
            MissionBuilding = GetGame().CreateObject( "Wreck_PoliceCar", m_MissionPosition, false, false, true );
            
            m_MissionMessage1 = "ALERTA: Horda crítica detectada. Perímetro perdido.";
            m_MissionMessage2 = "O armamento de contenção está num veículo no centro do enxame.";
            m_MissionMessage3 = "Localização: ** " + m_MissionLocation + " **. Rompam o cerco!";

            // Spawn dos Zumbis
            for ( int i = 0; i < HordeDensity; i++ )
            {
                vector pos = m_MissionPosition + Vector(Math.RandomFloatInclusive(-35,35), 0, Math.RandomFloatInclusive(-35,35));
                pos[1] = GetGame().SurfaceY(pos[0], pos[2]);
                m_MissionAIs.Insert( GetGame().CreateObject( "ZmbM_usSoldier_normal_Woodland", pos, false, true ) );
            }
            return true;
		}
		return false;
	}
    
    override void PlayerChecks( PlayerBase player )
	{
		// Quando o player chega a 10 metros do centro
		if ( vector.Distance( player.GetPosition(), m_MissionPosition ) < m_MissionZoneInnerRadius && !m_RewardsSpawned )
		{
            m_RewardsSpawned = true;
            
            // Criar o Baú e o Loot
            ItemBase Chest = ItemBase.Cast( GetGame().CreateObject( "SeaChest", m_MissionPosition, false, false, false ) );
            if (Chest)
            {
                Chest.GetInventory().CreateInInventory("M79");
                Chest.GetInventory().CreateInInventory("Ammo_40mm_Explosive");
                Chest.GetInventory().CreateInInventory("Ammo_40mm_Explosive");
                Chest.GetInventory().CreateInInventory("Ammo_40mm_Explosive");
                m_MissionObjects.Insert(Chest);
                
                // Avisar no log quem completou
                if (player.GetIdentity()) PNH_Logger.Log("Missões", "[PNH_CORE] Horda dominada por: " + player.GetIdentity().GetName());
            }
            
            m_MsgNum = -1; // Para de enviar mensagens de rádio
		}
	}
}