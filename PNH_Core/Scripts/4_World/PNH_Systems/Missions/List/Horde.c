class HordeMission extends PNH_MissionBase
{
    int MsgDlyFinish = 60;
    string m_WinnerName = "Desconhecido";
    bool m_IsVictory = false; 

    // --- CONTROLE DO NOVO SISTEMA DE AVISOS ---
    bool m_ProximityWarned = false; 
    bool m_InnerProximityWarned = false; 
    bool m_CompletionMsgSent = false; 

    ref PNH_MissionData_Horde m_Config;

    override bool IsExtended() { return false; }
    
    void HordeMission()
    {
        string caminhoJson = "$profile:PNH/Missions/Horde.json";
        m_Config = new PNH_MissionData_Horde();
        
        if (!FileExist("$profile:PNH/Missions")) MakeDirectory("$profile:PNH/Missions");
        
        if (FileExist(caminhoJson)) {
            JsonFileLoader<PNH_MissionData_Horde>.JsonLoadFile(caminhoJson, m_Config);
        } else {
            PNH_Logger.Error("Missões", "Arquivo Horde.json nao encontrado!");
            return;
        }

        m_MissionTimeout = m_Config.TempoLimiteSegundos;
        m_MissionZoneOuterRadius = m_Config.RaioAvisoExterno;
        m_MissionZoneInnerRadius = m_Config.RaioAvisoInterno;
        m_MissionInformant = m_Config.Lore.Informante;
        
        m_MissionMessage1 = m_Config.Lore.MensagensRadio[0];
        m_MissionMessage2 = m_Config.Lore.MensagensRadio[1];
    }

    void SpawnObjectsAndAIs()
    {
        if (!m_Config) return;

        // 1. SPAWN DA MOCHILA E LOADOUT ALEATÓRIO
        if (m_Config.RecompensasHorda && m_Config.RecompensasHorda.Loadouts.Count() > 0)
        {
            ItemBase Chest = ItemBase.Cast(GetGame().CreateObject(m_Config.RecompensasHorda.Container, m_MissionPosition, false, false, false));
            if (Chest)
            {
                Chest.SetPosition(m_MissionPosition + "0 0.2 0");
                
                // Sorteia 1 Loadout da lista do JSON
                PNH_MissionSettings_Loadout loadoutSorteado = m_Config.RecompensasHorda.Loadouts.GetRandomElement();
                for (int i = 0; i < loadoutSorteado.Itens.Count(); i++) {
                    Chest.GetInventory().CreateInInventory(loadoutSorteado.Itens[i]);
                }
                m_MissionObjects.Insert(Chest);
            }
        }

        // 2. SPAWN DO PILOTO MORTO
        if (m_Config.Piloto)
        {
            DayZInfected InfectedPilot = DayZInfected.Cast(GetGame().CreateObject(m_Config.Piloto.Classe, m_MissionPosition, false, true, true));
            if (InfectedPilot) {
                InfectedPilot.SetPosition(m_MissionPosition);
                for (int p = 0; p < m_Config.Piloto.Itens.Count(); p++) {
                    InfectedPilot.GetInventory().CreateInInventory(m_Config.Piloto.Itens[p]);
                }
                InfectedPilot.SetHealth("","",0); // Mata o piloto
                m_MissionAIs.Insert(InfectedPilot);
            }
        }

        // 3. SPAWN DA HORDA
        if (m_Config.Dificuldade)
        {
            int qtd = m_Config.Dificuldade.QuantidadeHordaFinal;
            for (int j = 0; j < qtd; j++)
            {
                float x = Math.RandomFloatInclusive(3.0, 30.0);
                float y = Math.RandomFloatInclusive(3.0, 30.0);    
                if (Math.RandomIntInclusive(0, 9) > 4) x *= -1.0;
                if (Math.RandomIntInclusive(0, 9) < 5) y *= -1.0;
                vector NewPosVector = {x, 0, y};

                string tipoZumbi = m_Config.Dificuldade.ClassesZumbis.GetRandomElement();
                m_MissionAIs.Insert(GetGame().CreateObject(tipoZumbi, m_MissionPosition + NewPosVector, false, true, true));
            }
        }
    }

    override void MissionFinal()
    {   
        // ALERTA A HORDA (Faz os zumbis correrem/investigarem)
        for (int j = 0; j < m_MissionAIs.Count(); j++)
        {
            DayZInfected Zed = DayZInfected.Cast(m_MissionAIs.Get(j));
            if (Zed && Zed.IsAlive())
            {
                DayZInfectedCommandMove moveCommandZ = Zed.GetCommand_Move();
                if (moveCommandZ) moveCommandZ.SetIdleState(2);
            }
        }

        PNH_Logger.Log("Missões", "[PNH_CORE] MISSÃO_CONCLUÍDA: " + m_WinnerName + " encontrou os equipamentos do piloto!");
        m_RewardsSpawned = true; 
    }

    override bool DeployMission()
    {
        if (!m_Config) return false;

        if (!m_Config.Ativa) 
        {
            PNH_Logger.Log("Missões", "[PNH_CORE] Missão Horde ignorada (Ativa = false no JSON).");
            return false; 
        }

        if (m_Config.Lore.MensagensRadio.Count() >= 3) {
            m_MissionMessage3 = m_Config.Lore.MensagensRadio[2] + "** " + m_MissionLocation + " **. Rompam o cerco.";
        }

        if (m_MissionPosition && m_MissionPosition != "0 0 0")
        {
            PNH_Logger.Log("Missões", "[PNH_CORE] MISSÃO_INICIADA: Horda em " + m_MissionLocation);
            
            // Cria a mochila, o piloto e os zumbis assim que a missão nasce
            SpawnObjectsAndAIs();
            return true;
        }
        return false;
    }

    override void PlayerChecks(PlayerBase player)
    {
        float distance = vector.Distance(player.GetPosition(), m_MissionPosition);
        PNH_MissionSettingsData config = PNH_MissionSettings.GetData();

        if (!m_ProximityWarned && distance <= m_MissionZoneOuterRadius)
        {
            m_ProximityWarned = true;
            string msg = "Atenção: Você está se aproximando das coordenadas do piloto. Câmbio.";
            if (config.ConfiguracoesGerais.UsarPDA) GetRPCManager().SendRPC("[GearPDA] ", "SendGlobalMessage", new Param2<string, string>(m_MissionInformant, msg), true);
            else PNH_Utils.SendMessageToAll("[RADIO PNH] " + msg);
        }

        if (!m_InnerProximityWarned && distance <= m_MissionZoneInnerRadius)
        {
            m_InnerProximityWarned = true;
            string msg2 = "Contato visual com o sinal. Há infectados por perto!";
            if (config.ConfiguracoesGerais.UsarPDA) GetRPCManager().SendRPC("[GearPDA] ", "SendGlobalMessage", new Param2<string, string>(m_MissionInformant, msg2), true);
            else PNH_Utils.SendMessageToAll("[RADIO PNH] " + msg2);
        }

        // A vitória ocorre quando o player entra no RaioVitoria (Ex: 3 metros da mochila)
        if (!m_IsVictory && !m_RewardsSpawned && distance <= m_Config.RaioVitoria)
        {
            m_IsVictory = true;
            string playerName = player.GetIdentity().GetName();
            m_WinnerName = playerName;

            string successMsg = "[PNH_CORE] SUCESSO: [" + playerName + "] encontrou a mochila! Os infectados o escutaram!";
            
            if (config.ConfiguracoesGerais.UsarPDA) GetRPCManager().SendRPC("[GearPDA] ", "SendGlobalMessage", new Param2<string, string>("Comando PNH", successMsg), true);
            else PNH_Utils.SendMessageToAll("[RADIO PNH] " + successMsg);

            MissionFinal(); // Chama o alerta da horda
        }
    }
}