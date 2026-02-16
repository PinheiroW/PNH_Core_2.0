class PlaneCrashMission extends PNH_MissionBase
{
    ItemBase m_RewardContainer;
    Object m_Wreck;
    bool m_IsVictory = false;

    bool m_OuterWarned = false;
    bool m_InnerWarned = false;

    ref PNH_MissionData_PlaneCrash m_Config;

    override bool IsExtended() { return false; }

    void PlaneCrashMission()
    {
        string caminhoJson = "$profile:PNH/Missions/PlaneCrash.json";
        m_Config = new PNH_MissionData_PlaneCrash();
        if (FileExist(caminhoJson)) JsonFileLoader<PNH_MissionData_PlaneCrash>.JsonLoadFile(caminhoJson, m_Config);

        m_MissionTimeout = m_Config.TempoLimiteSegundos;
        m_MissionZoneOuterRadius = m_Config.RaioAvisoExterno;
        m_MissionZoneInnerRadius = m_Config.RaioAvisoInterno;

        if (m_Config.Lore) {
            m_MissionInformant = m_Config.Lore.Informante;
            if (m_Config.Lore.MensagensRadio && m_Config.Lore.MensagensRadio.Count() >= 2) {
                m_MissionMessage1 = m_Config.Lore.MensagensRadio[0];
                m_MissionMessage2 = m_Config.Lore.MensagensRadio[1];
            }
        }
    }

    override void PlayerChecks(PlayerBase player)
    {
        float distance = vector.Distance(player.GetPosition(), m_MissionPosition);

        if (!m_OuterWarned && distance <= m_MissionZoneOuterRadius) {
            m_OuterWarned = true;
            EnviarAviso(m_MissionInformant, "Aproximando-se do local da queda do C130. A área está repleta de militares infectados.");
        }

        if (!m_InnerWarned && distance <= m_MissionZoneInnerRadius) {
            m_InnerWarned = true;
            EnviarAviso(m_MissionInformant, "Você chegou aos destroços! Elimine a ameaça e abra o barril verde para recuperar a carga.");
        }

        // GATILHO: Abrir o barril
        if (!m_IsVictory && m_RewardContainer && m_RewardContainer.IsOpen())
        {
            FinalizarMissao(player);
        }
    }

    void FinalizarMissao(PlayerBase player)
    {
        m_IsVictory = true;
        
        string winMsg = "[PNH_CORE] MISSÃO_CONCLUÍDA: " + player.GetIdentity().GetName() + " recuperou a carga do avião caído!";
        PNH_Logger.Log("Missões", winMsg);

        string mensagemFinal = "Carga recuperada com sucesso!";
        if (m_Config.Lore && m_Config.Lore.MensagemVitoria != "") {
            mensagemFinal = m_Config.Lore.MensagemVitoria;
        }

        EnviarAviso("Comando PNH", mensagemFinal);

        PNH_MissionSettingsData config = PNH_MissionSettings.GetData();
        m_MissionTimeout = m_MissionTime + config.ConfiguracoesGerais.TempoLimpezaSegundos;
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

        if (m_Config.Lore && m_Config.Lore.MensagensRadio && m_Config.Lore.MensagensRadio.Count() >= 3) {
            m_MissionMessage3 = m_Config.Lore.MensagensRadio[2] + "** " + m_MissionLocation + " **.";
        }

        vector wreckPos = m_MissionPosition;
        wreckPos[1] = GetGame().SurfaceY(wreckPos[0], wreckPos[2]);
        
        // 1. O AVIÃO
        m_Wreck = GetGame().CreateObject(m_Config.ObjetoCentral, wreckPos, false, false, true);
        if (m_Wreck) {
            m_Wreck.SetOrientation(Vector(Math.RandomFloat(-180, 180), 0, 0));
            m_MissionObjects.Insert(m_Wreck);
        }

        // 2. FOGOS AO REDOR
        if (m_Wreck && m_Config.Locais && m_Config.Locais.SpawnsFogo)
        {
            for (int f = 0; f < m_Config.Locais.SpawnsFogo.Count(); f++)
            {
                vector fireLocal = m_Config.Locais.SpawnsFogo[f].ToVector();
                vector fireWorld = m_Wreck.ModelToWorld(fireLocal);
                fireWorld[1] = GetGame().SurfaceY(fireWorld[0], fireWorld[2]);
                Object fire = GetGame().CreateObject("Fireplace", fireWorld, false, false, true);
                if (fire) m_MissionObjects.Insert(fire);
            }
        }

        // 3. O BARRIL COM LOOT
        if (m_Wreck && m_Config.Locais && m_Config.Locais.SpawnsBarril && m_Config.Locais.SpawnsBarril.Count() > 0)
        {
            string rPos = m_Config.Locais.SpawnsBarril.GetRandomElement();
            vector barrelWorld = m_Wreck.ModelToWorld(rPos.ToVector());
            barrelWorld[1] = GetGame().SurfaceY(barrelWorld[0], barrelWorld[2]);
            
            m_RewardContainer = ItemBase.Cast(GetGame().CreateObject(m_Config.Recompensas.Container, barrelWorld, false, false, true));
            if (m_RewardContainer && m_Config.Recompensas.Loadouts && m_Config.Recompensas.Loadouts.Count() > 0)
            {
                auto loadout = m_Config.Recompensas.Loadouts.GetRandomElement();
                for (int i = 0; i < loadout.Itens.Count(); i++) m_RewardContainer.GetInventory().CreateInInventory(loadout.Itens[i]);
                m_RewardContainer.Close();
                m_MissionObjects.Insert(m_RewardContainer);
            }
        }

        // 4. HORDA DE MILITARES
        if (m_Config.Dificuldade && m_Config.Dificuldade.ClassesZumbis.Count() > 0)
        {
            for (int z = 0; z < m_Config.Dificuldade.QuantidadeHordaFinal; z++)
            {
                vector zPos = wreckPos + Vector(Math.RandomFloat(-25, 25), 0, Math.RandomFloat(-25, 25));
                zPos[1] = GetGame().SurfaceY(zPos[0], zPos[2]);
                m_MissionAIs.Insert(GetGame().CreateObject(m_Config.Dificuldade.ClassesZumbis.GetRandomElement(), zPos, false, true, true));
            }
        }

        PNH_Logger.Log("Missões", "[PNH_CORE] MISSÃO_INICIADA: Queda do C130 em " + m_MissionLocation);
        return true;
    }
}