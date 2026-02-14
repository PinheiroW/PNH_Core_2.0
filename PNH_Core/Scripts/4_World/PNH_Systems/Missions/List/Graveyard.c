class GraveyardMission extends PNH_MissionBase
{
	Object MissionObject; // A Cruz de Madeira
	
	int MsgDlyFinish = 60;					
	ref array<vector> InfectedSpawns = new array<vector>;
	ref array<string> InfectedTypes = new array<string>;

    string m_WinnerName = "Desconhecido";
    bool m_IsVictory = false; 
	
	override bool IsExtended() return false; 
	
	void GraveyardMission()
	{
		m_MissionTimeout = 2700;			
		m_MissionZoneOuterRadius = 100.0;	
		m_MissionZoneInnerRadius = 3.0;
				
		m_MissionInformant = "PNH Corp. Bio-Hazard";
	
		m_MissionMessage1 = "[RADIO PNH] ALERTA: Sinal do Dr. Ivanov (Paciente Zero) detectado no cemitério local.";
		m_MissionMessage2 = "Ele enterrou os dados da Cepa-X antes de morrer. Procure por uma Cruz de Madeira recente.";
		m_MissionMessage3 = "[RADIO PNH] Túmulo localizado em ** " + m_MissionLocation + " **. Exumação autorizada.";
				
		InfectedTypes.Insert("ZmbM_priestPopSkinny");
		InfectedTypes.Insert("ZmbM_HermitSkinny_Beige");
	}

    void SpawnRewards()
    {
        ItemBase Chest = ItemBase.Cast( GetGame().CreateObject( "SeaChest", m_MissionPosition, false, false, false ) );
        if (Chest)
        {
            Chest.GetInventory().CreateInInventory("SVD");
            Chest.GetInventory().CreateInInventory("Mag_SVD_10Rnd");
            Chest.GetInventory().CreateInInventory("PSO1Optic");
            Chest.GetInventory().CreateInInventory("NBCSuit_Yellow");
            m_MissionObjects.Insert( Chest );
        }
    }

	void SpawnAIs()
	{	
		for ( int j = 0 ; j < 15 ; j++ )
		{
			m_MissionAIs.Insert( GetGame().CreateObject( InfectedTypes.GetRandomElement(), m_MissionPosition + Vector(Math.RandomFloatInclusive(-10,10), 0, Math.RandomFloatInclusive(-10,10)), false, true ) );
		}	
	}
	
	override void MissionFinal()
	{	
        if (m_IsVictory) PNH_Logger.Log("Missões", "[PNH_CORE] MISSÃO_CONCLUÍDA: " + m_WinnerName + " recuperou os dados da Cepa-X no Cemitério!");
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
            MissionObject = GetGame().CreateObject( "Land_Misc_Grave_Cross_Wood", m_MissionPosition, false, false, false );
            if ( MissionObject )
            {
                vector pos = MissionObject.GetPosition();
                pos[1] = GetGame().SurfaceY(pos[0], pos[2]);
                MissionObject.SetPosition(pos);
                m_MissionPosition = pos; 
                m_MissionObjects.Insert( MissionObject );
                
                PNH_Logger.Log("Missões", "[PNH_CORE] MISSÃO_INICIADA: Operação Paciente Zero em " + m_MissionLocation);
                GetGame().GetCallQueue( CALL_CATEGORY_GAMEPLAY ).Call( this.SpawnAIs );
                return true;
            }
        }
		return false;
	}
}