class GraveyardMission extends PNH_MissionBase
{
    UndergroundStash m_Stash;
    ItemBase m_Chest;
    string m_SurvivorName;
    bool m_IsVictory = false;
    
    // Controles de Perímetro
    bool m_OuterWarned = false; // 90m
    bool m_InnerWarned = false; // 20m

    ref PNH_MissionData_Graveyard m_Config;

    override bool IsExtended() { return false; }
    
    void GraveyardMission()
    {
        string caminhoJson = "$profile:PNH/Missions/Graveyard.json";
        m_Config = new PNH_MissionData_Graveyard();
        if (FileExist(caminhoJson)) JsonFileLoader<PNH_MissionData_Graveyard>.JsonLoadFile(caminhoJson, m_Config);

        if (m_Config.Lore && m_Config.Lore.NomesSobreviventes && m_Config.Lore.NomesSobreviventes.Count() > 0)
            m_SurvivorName = m_Config.Lore.NomesSobreviventes.GetRandomElement();
        else
            m_SurvivorName = "Desconhecido";

        m_MissionTimeout = m_Config.TempoLimiteSegundos;
        m_MissionZoneOuterRadius = m_Config.RaioAvisoExterno;
        m_MissionZoneInnerRadius = m_Config.RaioAvisoInterno;
        m_MissionInformant = m_Config.Lore.Informante;
        
        if (m_Config.Lore && m_Config.Lore.MensagensRadio && m_Config.Lore.MensagensRadio.Count() >= 2)
        {
            string msgTemp = m_Config.Lore.MensagensRadio[0];
            msgTemp.Replace("Major General", "Major General " + m_SurvivorName);
            
            m_MissionMessage1 = msgTemp; 
            m_MissionMessage2 = m_Config.Lore.MensagensRadio[1];
        }
    }

    override void PlayerChecks(PlayerBase player)
    {
        PNH_MissionSettingsData globalConfig = PNH_MissionSettings.GetData();
        float distance = vector.Distance(player.GetPosition(), m_MissionPosition);

        // 1. AVISO DE PERÍMETRO (90m)
        if (!m_OuterWarned && distance <= m_MissionZoneOuterRadius)
        {
            m_OuterWarned = true;
            EnviarAviso(m_MissionInformant, "Você está entrando na área do cemitério. Fique atento aos arredores.");
        }

        // 2. AVISO DE CHEGADA (20m)
        if (!m_InnerWarned && distance <= m_MissionZoneInnerRadius)
        {
            m_InnerWarned = true;
            EnviarAviso(m_MissionInformant, "Você chegou ao local. Procure por terra remexida e comece a cavar.");
        }

        // 3. GATILHO DE VITÓRIA: FINALIZAR DESENTERRAR (Checagem de Hierarquia)
        if (!m_IsVictory && m_Chest)
        {
            // Obtém o objeto onde o baú está guardado
            Object parent = m_Chest.GetHierarchyParent();
            
            // Se o baú não tem parente (caiu no chão) ou o parente NÃO é mais o Stash (foi desenterrado)
            if (!parent || parent.GetType() != "UndergroundStash")
            {
                m_IsVictory = true;
                string playerName = player.GetIdentity().GetName();
                string successMsg = "[PNH_CORE] MISSÃO_CONCLUÍDA: " + playerName + " recuperou o legado de " + m_SurvivorName + "!";
                
                PNH_Logger.Log("Missões", successMsg);
                
                if (globalConfig.ConfiguracoesGerais.UsarPDA) 
                    GetRPCManager().SendRPC("[GearPDA] ", "SendGlobalMessage", new Param2<string, string>("Comando PNH", successMsg), true);
                else 
                    PNH_Utils.SendMessageToAll(successMsg);

                m_MissionTimeout = m_MissionTime + globalConfig.ConfiguracoesGerais.TempoLimpezaSegundos;
            }
        }
    }

    void EnviarAviso(string emissor, string msg)
    {
        PNH_MissionSettingsData config = PNH_MissionSettings.GetData();
        if (config.ConfiguracoesGerais.UsarPDA) GetRPCManager().SendRPC("[GearPDA] ", "SendGlobalMessage", new Param2<string, string>(emissor, msg), true);
        else PNH_Utils.SendMessageToAll("[" + emissor + "] " + msg);
    }

    override bool DeployMission()
    {
        if (!m_Config || !m_Config.Ativa) return false;

        if (m_Config.Lore && m_Config.Lore.MensagensRadio && m_Config.Lore.MensagensRadio.Count() >= 3)
            m_MissionMessage3 = m_Config.Lore.MensagensRadio[2] + "** " + m_MissionLocation + " **.";

        // Spawn do Stash (O próprio objeto já funciona como algo enterrado no DayZ)
        m_Stash = UndergroundStash.Cast(GetGame().CreateObject("UndergroundStash", m_MissionPosition, false, false, true));
        if (m_Stash)
        {
            m_Stash.PlaceOnGround(); 
            
            m_Chest = ItemBase.Cast(m_Stash.GetInventory().CreateInInventory(m_Config.Recompensas.Container));
            
            if (m_Chest && m_Config.Recompensas.Loadouts && m_Config.Recompensas.Loadouts.Count() > 0)
            {
                PNH_MissionSettings_Loadout loadout = m_Config.Recompensas.Loadouts.GetRandomElement();
                for (int j = 0; j < loadout.Itens.Count(); j++)
                {
                    m_Chest.GetInventory().CreateInInventory(loadout.Itens[j]);
                }
            }
            m_MissionObjects.Insert(m_Stash);
        }

        // Spawn de Guardiões
        if (m_Config.Dificuldade && m_Config.Dificuldade.ClassesZumbis)
        {
            for (int i = 0; i < m_Config.Dificuldade.QuantidadeHordaFinal; i++)
            {
                vector zPos = m_MissionPosition + Vector(Math.RandomFloat(-20, 20), 0, Math.RandomFloat(-20, 20));
                zPos[1] = GetGame().SurfaceY(zPos[0], zPos[2]);
                m_MissionAIs.Insert(GetGame().CreateObject(m_Config.Dificuldade.ClassesZumbis.GetRandomElement(), zPos, false, true, true));
            }
        }

        PNH_Logger.Log("Missões", "[PNH_CORE] MISSÃO_INICIADA: Graveyard em " + m_MissionLocation);
        return true;
    }
}