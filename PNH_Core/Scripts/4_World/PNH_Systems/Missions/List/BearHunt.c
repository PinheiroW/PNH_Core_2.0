class BearHuntMission extends PNH_MissionBase
{
    ItemBase MissionObject;
    
    // --- LISTA DE ANIMAIS (MATILHA) ---
    ref array<Object> m_TargetAnimals = new array<Object>; 
    
    int MsgDlyFinish = 60;
    string m_WinnerName = "Desconhecido";
    bool m_IsVictory = false; 

    // --- CONTROLE DO NOVO SISTEMA DE AVISOS ---
    bool m_ProximityWarned = false; 
    bool m_InnerProximityWarned = false; 
    bool m_CompletionMsgSent = false; 

    ref PNH_MissionData_BearHunt m_Config;

    override bool IsExtended() { return true; }
    
    void BearHuntMission()
    {   
        string caminhoJson = "$profile:PNH/Missions/BearHunt.json";
        m_Config = new PNH_MissionData_BearHunt();
        
        if (!FileExist("$profile:PNH/Missions")) MakeDirectory("$profile:PNH/Missions");
        
        if (FileExist(caminhoJson)) {
            JsonFileLoader<PNH_MissionData_BearHunt>.JsonLoadFile(caminhoJson, m_Config);
        } else {
            PNH_Logger.Error("Missões", "Arquivo BearHunt.json nao encontrado!");
            return;
        }

        m_MissionTimeout = m_Config.TempoLimiteSegundos;
        m_MissionZoneOuterRadius = m_Config.RaioAvisoExterno;
        m_MissionZoneInnerRadius = m_Config.RaioAvisoInterno;
        m_MissionInformant = m_Config.Lore.Informante;
        
        m_MissionMessage1 = m_Config.Lore.MensagensRadio[0];
        m_MissionMessage2 = m_Config.Lore.MensagensRadio[1];
        
        // A MENSAGEM 3 FOI REMOVIDA DAQUI (Pois a cidade ainda não é conhecida)
    }

    void SpawnAIs()
    {
        if (!m_Config || !m_Config.Alvo) return;
        
        // Loop para criar a quantidade de animais definida no JSON (Matilha)
        for (int i = 0; i < m_Config.Alvo.Quantidade; i++) 
        {
            vector spawnPos = m_MissionPosition;
            spawnPos[0] = spawnPos[0] + Math.RandomFloat(-5.0, 5.0);
            spawnPos[2] = spawnPos[2] + Math.RandomFloat(-5.0, 5.0);
            spawnPos[1] = GetGame().SurfaceY(spawnPos[0], spawnPos[2]);

            Object animal = GetGame().CreateObject(m_Config.Alvo.ClasseAnimal, spawnPos, false, true, true);
            if (animal) {
                m_TargetAnimals.Insert(animal);
                m_MissionAIs.Insert(animal); 
            }
        }
    }

    void SpawnRewards()
    {
        if (!m_Config || !m_Config.Recompensas) return;

        ItemBase Chest = ItemBase.Cast(GetGame().CreateObject(m_Config.Recompensas.Container, m_MissionPosition, false, false, false));
        if (Chest)
        {
            for (int i = 0; i < m_Config.Recompensas.Itens.Count(); i++) {
                Chest.GetInventory().CreateInInventory(m_Config.Recompensas.Itens[i]);
            }
            m_MissionObjects.Insert(Chest);
        }
    }

    override void MissionFinal()
    {   
        if (m_IsVictory) {
            PNH_Logger.Log("Missões", "[PNH_CORE] MISSÃO_CONCLUÍDA: " + m_WinnerName + " abateu todos os alvos!");
        }
        m_RewardsSpawned = true; 
    }

    override bool DeployMission()
    {
        if (!m_Config) return false;

        if (!m_Config.Ativa) 
        {
            PNH_Logger.Log("Missões", "[PNH_CORE] Missão BearHunt ignorada (Ativa = false no JSON).");
            return false; 
        }

        // --- CORREÇÃO 1: AQUI A CIDADE JÁ É CONHECIDA, ENTÃO MONTAMOS A MENSAGEM 3 ---
        if (m_Config.Lore.MensagensRadio.Count() >= 3) {
            m_MissionMessage3 = m_Config.Lore.MensagensRadio[2] + "** " + m_MissionLocation + " **.";
        }

        if (IsExtended()) {
            PNH_Logger.Log("Missões", "[PNH_CORE] MISSÃO_INICIADA: Caçada Alpha em " + m_MissionLocation);
            
            // --- CORREÇÃO 2: CHAMAMOS A FUNÇÃO PARA CRIAR OS URSOS AQUI ---
            SpawnAIs(); 
            
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
            string msg = "Atenção Operador, você está entrando na zona de caça do Alpha. Rastros recentes detectados.";
            if (config.ConfiguracoesGerais.UsarPDA) GetRPCManager().SendRPC("[GearPDA] ", "SendGlobalMessage", new Param2<string, string>(m_MissionInformant, msg), true);
            else PNH_Utils.SendMessageToAll("[RADIO PNH] " + msg);
        }

        if (!m_InnerProximityWarned && distance <= m_MissionZoneInnerRadius)
        {
            m_InnerProximityWarned = true;
            string msg2 = "O Espécime Alpha está visual! Fogo livre!";
            if (config.ConfiguracoesGerais.UsarPDA) GetRPCManager().SendRPC("[GearPDA] ", "SendGlobalMessage", new Param2<string, string>(m_MissionInformant, msg2), true);
            else PNH_Utils.SendMessageToAll("[RADIO PNH] " + msg2);
        }

        // --- CORREÇÃO 3: VERIFICA SE TODOS OS ANIMAIS DA MATILHA MORRERAM ---
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

            if (todosMortos) m_IsVictory = true;
        }

        if (m_IsVictory && !m_CompletionMsgSent)
        {
            m_CompletionMsgSent = true;
            string playerName = player.GetIdentity().GetName();
            m_WinnerName = playerName;

            string successMsg = "[PNH_CORE] SUCESSO: [" + playerName + "] caçou a ameaça Alpha!";
            
            if (config.ConfiguracoesGerais.UsarPDA) GetRPCManager().SendRPC("[GearPDA] ", "SendGlobalMessage", new Param2<string, string>("Comando PNH", successMsg), true);
            else PNH_Utils.SendMessageToAll("[RADIO PNH] " + successMsg);
            
            this.SpawnRewards(); 
            this.MissionFinal();
        }
    }
}