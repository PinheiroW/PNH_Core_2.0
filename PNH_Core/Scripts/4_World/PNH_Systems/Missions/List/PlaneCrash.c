class PlaneCrashMission extends SurvivorMissions
{
	Object CrashedC130;
	int MsgDlyFinish = 60;					
	ref array<string> InfectedTypes = new array<string>;
    string m_WinnerName = "Desconhecido";
    bool m_IsVictory = false; 
	
	override bool IsExtended() return false; 
	
	void PlaneCrashMission()
	{
		m_MissionTimeout = 2700;			
		m_MissionZoneOuterRadius = 700.0;	
		m_MissionZoneInnerRadius = 15.0; 
		m_MissionInformant = "PNH Air Traffic"; 
		m_MissionMessage1 = "[RADIO PNH] MAYDAY! Voo tático 'Valkyrie' abatido. Coordenadas recebidas.";
		m_MissionMessage2 = "A carga contém protótipos de NVG. A fumaça do impacto é visível no horizonte.";
		m_MissionMessage3 = "[RADIO PNH] Destroços na região de ** " + m_MissionLocation + " **. Assegurem a Caixa Preta.";
		InfectedTypes.Insert("ZmbM_usSoldier_normal_Woodland");
	}

    void SpawnRewards()
    {
        ItemBase Chest = ItemBase.Cast( GetGame().CreateObject( "SeaChest", m_MissionPosition, false, false, false ) );
        if (Chest)
        {
            Chest.GetInventory().CreateInInventory("FAL");
            Chest.GetInventory().CreateInInventory("Mag_FAL_20Rnd");
            Chest.GetInventory().CreateInInventory("NVGoggles");
            Chest.GetInventory().CreateInInventory("NVGHeadstrap");
            m_MissionObjects.Insert( Chest );
        }
    }

	void SpawnAIs()
	{	
		for ( int j = 0 ; j < 15 ; j++ ) m_MissionAIs.Insert( GetGame().CreateObject( InfectedTypes.GetRandomElement(), m_MissionPosition + Vector(Math.RandomFloatInclusive(-20,20), 0, Math.RandomFloatInclusive(-20,20)), false, true ) );
	}
	
	override void MissionFinal()
	{	
        if (m_IsVictory) PNH_Logger.Log("Missões", "[PNH_CORE] MISSÃO_CONCLUÍDA: " + m_WinnerName + " recuperou a Caixa Preta do avião!");
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
            CrashedC130 = GetGame().CreateObject( "Wreck_C130J", m_MissionPosition, false, false, false );
            if (CrashedC130) {
                vector pos = CrashedC130.GetPosition();
                pos[1] = GetGame().SurfaceY(pos[0], pos[2]);
                CrashedC130.SetPosition(pos);
                m_MissionObjects.Insert( CrashedC130 );
                PNH_Logger.Log("Missões", "[PNH_CORE] MISSÃO_INICIADA: Queda de Aeronave em " + m_MissionLocation);
                SpawnAIs();
                return true;
            }
        }
		return false;
	}
}