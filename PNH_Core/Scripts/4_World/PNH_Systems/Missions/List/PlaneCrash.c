class PlaneCrashMission extends PNH_MissionBase
{
	Object CrashedC130;
	int MsgDlyFinish = 60;					
    string m_WinnerName = "Desconhecido";
    bool m_IsVictory = false; 
	
	override bool IsExtended() { return false; }
	
	void PlaneCrashMission()
	{
		m_MissionTimeout = 2700;			
		m_MissionZoneOuterRadius = 700.0;	
		m_MissionZoneInnerRadius = 15.0; 
		m_MissionInformant = "PNH Air Traffic"; 
	}

    void SpawnRewards()
    {
        ItemBase Chest = ItemBase.Cast( GetGame().CreateObject( "SeaChest", m_MissionPosition, false, false, false ) );
        if (Chest)
        {
            Chest.GetInventory().CreateInInventory("NVGoggles");
            Chest.GetInventory().CreateInInventory("Battery9V");
            Chest.GetInventory().CreateInInventory("Falcon_Stabilizer");
            m_MissionObjects.Insert(Chest);
        }
    }

	void SpawnAIs()
	{
		for ( int i = 0; i < 20; i++ )
		{
			vector pos = m_MissionPosition + Vector(Math.RandomFloatInclusive(-25,25), 0, Math.RandomFloatInclusive(-25,25));
			pos[1] = GetGame().SurfaceY(pos[0], pos[2]);
			m_MissionAIs.Insert( GetGame().CreateObject( "ZmbM_usSoldier_normal_Woodland", pos, false, true ) );
		}
	}
	
	override void MissionFinal()
	{	
        if (m_IsVictory) PNH_Logger.Log("Missões", "[PNH_CORE] MISSÃO_CONCLUÍDA: " + m_WinnerName + " recuperou a Caixa Preta!");
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
            CrashedC130 = GetGame().CreateObject( "Wreck_C130J", m_MissionPosition, false, false, true );
            
            m_MissionMessage1 = "[RADIO PNH] MAYDAY! Voo tático 'Valkyrie' abatido. Coordenadas recebidas.";
            m_MissionMessage2 = "A carga contém protótipos de NVG. A fumaça é visível no horizonte.";
            m_MissionMessage3 = "[RADIO PNH] Destroços na região de ** " + m_MissionLocation + " **. Assegurem a carga.";

            SpawnAIs();
            return true;
		}
		return false;
	}
}