class BearHuntMission extends PNH_MissionBase
{
    ItemBase m_RewardChest;
    ref array<Object> m_TargetAnimals = new array<Object>; 
    
    bool m_IsVictory = false; 

    // Controles de Perímetro e Avisos
    bool m_ProximityWarned = false; 
    bool m_InnerProximityWarned = false; 

    ref PNH_MissionData_BearHunt m_Config;

    override bool IsExtended() { return false; }
    
    void BearHuntMission()
    {   
        string caminhoJson = "$profile:PNH/Missions/BearHunt.json";
        m_Config = new PNH_MissionData_BearHunt();
        
        if (FileExist(caminhoJson)) {
            JsonFileLoader<PNH_MissionData_BearHunt>.JsonLoadFile(caminhoJson, m_Config);
        }

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
        PNH_MissionSettingsData config = PNH_MissionSettings.GetData();

        // Avisos de Perímetro
        if (!m_ProximityWarned && distance <= m_MissionZoneOuterRadius)
        {
            m_ProximityWarned = true;
            EnviarAviso(m_MissionInformant, "Atenção Operador, você está entrando na zona de caça do Alpha. Rastros recentes detectados.");
        }

        if (!m_InnerProximityWarned && distance <= m_MissionZoneInnerRadius)
        {
            m_InnerProximityWarned = true;
            EnviarAviso(m_MissionInformant, "Atenção! O Espécime Alpha e a sua matilha estão muito próximos! Fogo livre!");
        }

        // VERIFICA SE TODOS OS ANIMAIS DA MATILHA MORRERAM
        if (!m_IsVictory && m_TargetAnimals.Count() > 0)
        {
            bool todosMortos = true;
            for (int i = 0; i < m_TargetAnimals.Count(); i++)
            {
                if (m_TargetAnimals[i] && m_TargetAnimals[i].IsAlive())
                {
                    todosMortos = false;
                    break;
                }
            }

            if (todosMortos) FinalizarMissao(player);
        }
    }

    void FinalizarMissao(PlayerBase player)
    {
        m_IsVictory = true;
        string playerName = player.GetIdentity().GetName();

        string winMsg = "[PNH_CORE] SUCESSO: [" + playerName + "] caçou a ameaça Alpha na região de " + m_MissionLocation + "!";
        PNH_Logger.Log("Missões", winMsg);
        
        string mensagemFinal = "Ameaça eliminada! Recompensa liberada.";
        if (m_Config.Lore && m_Config.Lore.MensagemVitoria != "") {
            mensagemFinal = m_Config.Lore.MensagemVitoria;
        }
        
        EnviarAviso("Comando PNH", mensagemFinal);
        
        SpawnRewards(); 
        
        PNH_MissionSettingsData config = PNH_MissionSettings.GetData();
        m_MissionTimeout = m_MissionTime + config.ConfiguracoesGerais.TempoLimpezaSegundos;
    }

    void SpawnRewards()
    {
        if (!m_Config || !m_Config.Recompensas) return;

        m_RewardChest = ItemBase.Cast(GetGame().CreateObject(m_Config.Recompensas.Container, m_MissionPosition, false, false, false));
        if (m_RewardChest)
        {
            for (int i = 0; i < m_Config.Recompensas.Itens.Count(); i++) {
                m_RewardChest.GetInventory().CreateInInventory(m_Config.Recompensas.Itens[i]);
            }
            m_MissionObjects.Insert(m_RewardChest);
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

        if (m_Config.Lore && m_Config.Lore.MensagensRadio && m_Config.Lore.MensagensRadio.Count() >= 3) {
            m_MissionMessage3 = m_Config.Lore.MensagensRadio[2] + "** " + m_MissionLocation + " **.";
        }

        // SPAWN DOS ANIMAIS (MATILHA)
        if (m_Config.Alvo)
        {
            for (int i = 0; i < m_Config.Alvo.Quantidade; i++) 
            {
                vector spawnPos = m_MissionPosition;
                spawnPos[0] = spawnPos[0] + Math.RandomFloat(-10.0, 10.0);
                spawnPos[2] = spawnPos[2] + Math.RandomFloat(-10.0, 10.0);
                spawnPos[1] = GetGame().SurfaceY(spawnPos[0], spawnPos[2]);

                Object animal = GetGame().CreateObject(m_Config.Alvo.ClasseAnimal, spawnPos, false, true, true);
                if (animal) {
                    m_TargetAnimals.Insert(animal);
                    m_MissionAIs.Insert(animal); 
                }
            }
        }

        PNH_Logger.Log("Missões", "[PNH_CORE] MISSÃO_INICIADA: Caçada Alpha em " + m_MissionLocation);
        return true;
    }
}