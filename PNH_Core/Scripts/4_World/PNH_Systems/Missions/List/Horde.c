class HordeMission extends PNH_MissionBase
{
    // --- PARÂMETROS DA MISSÃO ---
    int m_HordeDensity = 15; // Quantidade de alvos

    override bool DeployMission()
    {
        if (!m_MissionAccepted) return false; 
        
        m_MissionInformant = "COMANDO PNH";
        m_MissionMessage1 = "ATENCAO MERCENARIO: Horda militar detectada em " + m_MissionLocation;
        m_MissionMessage2 = "Os alvos estao armados e sao extremamente agressivos.";
        m_MissionMessage3 = "Elimine todos para limpar o setor e receber o seu XP.";
        m_MissionMessage4 = "Mantenha a radio ligada para novas atualizacoes.";
        
        // PNH 2.0: Limpeza preventiva
        m_MissionAIs.Clear();

        // PNH 2.0: Usamos o sistema de filas do DayZ para garantir que o spawn ocorra 
        // de forma suave e não trave a lógica principal.
        GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Call(this.SpawnHorde);
        
        return true;
    }

    // Função dedicada apenas ao Spawn Físico
    void SpawnHorde()
    {
        for (int i = 0; i < m_HordeDensity; i++)
        {
            vector spawnPos = m_MissionPosition + Vector(Math.RandomFloat(-15, 15), 0, Math.RandomFloat(-15, 15));
            spawnPos[1] = GetGame().SurfaceY(spawnPos[0], spawnPos[2]); 
            
            // Método de Spawn garantido (PNH Universal)
            Object npc = GetGame().CreateObject("ZmbM_SoldierNormal", spawnPos, false, true, true);
            if (!npc) npc = GetGame().CreateObject("ZmbM_CitizenASkinny", spawnPos, false, true, true);
            
            if (npc) 
            {
                npc.SetPosition(spawnPos);
                m_MissionAIs.Insert(npc);
            }
        }
        PNH_Logger.Log("Missões", "[PNH_CORE] Spawn físico da horda concluído: " + m_MissionAIs.Count() + " alvos.");
    }

    override void PlayerChecks(PlayerBase player)
    {
        // O Manager só chama esta função quando o estado é STATE_ACTIVE.
        // Isso significa que já passaram pelo menos 15 segundos desde o !aceitar.
        if (!m_MissionAccepted || !IsContractOwner(player)) return; 

        // SEGUNDA TRAVA DE SEGURANÇA:
        // Se a lista ainda estiver vazia após 15 segundos, o spawn falhou gravemente.
        // Em vez de encerrar como vitória, registamos o erro e limpamos.
        if (m_MissionAIs.Count() == 0) 
        {
            PNH_Logger.Log("Missões", "[ERRO CRÍTICO] Missão abortada: Spawn de NPCs falhou em " + m_MissionLocation);
            PNH_MissionManager.GetInstance().EndMission();
            return;
        }

        if (vector.Distance(player.GetPosition(), m_MissionPosition) <= m_MissionZoneOuterRadius)
        {
            bool zombiesAlive = false;
            foreach (Object ai : m_MissionAIs) 
            { 
                if (ai && EntityAI.Cast(ai).IsAlive()) 
                { 
                    zombiesAlive = true; 
                    break; 
                } 
            }
            
            // Só termina se TODOS os alvos forem abatidos
            if (!zombiesAlive) 
            { 
                MissionFinal(); 
                PNH_MissionManager.GetInstance().EndMission(); 
            }
        }
    }

    override void MissionFinal()
    {
        // Agente de Comunicação anuncia a vitória
        PNH_BroadcastManager.GetInstance().AnnounceMissionEnded(m_MissionOwnerName);

        if (m_MissionAccepted && m_MissionOwnerID != "")
        {
            PNH_PlayerProfileData pData = PNH_ProfileManager.LoadProfile(m_MissionOwnerID, m_MissionOwnerName);
            PNH_MissionSettingsData settings = PNH_MissionSettings.GetData();
            if (pData && settings) 
            {
                pData.TemMissaoAtiva = false; 
                pData.ClasseMissaoAtiva = ""; 
                PNH_ProfileManager.SaveProfile(pData);
                PNH_ProfileManager.AddXP(PNH_Utils.GetPlayerByName(m_MissionOwnerName), settings.TabelaXP.XP_Tier_1);
            }
        }
    }
}