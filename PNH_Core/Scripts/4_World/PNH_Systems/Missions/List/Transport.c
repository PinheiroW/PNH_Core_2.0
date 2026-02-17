class TransportMission extends PNH_MissionBase
{
    Car m_MissionVehicle;
    Object m_DeadPriest;
    Object m_NPC;
    ItemBase m_RewardContainer;
    
    int m_CurrentPhase = 0; 
    bool m_AmbushChurchTriggered = false;
    bool m_AmbushShedTriggered = false;
    
    bool m_AWarnO = false; bool m_AWarnI = false;
    bool m_BWarnO = false; bool m_BWarnI = false;
    bool m_CWarnO = false; bool m_CWarnI = false;
    bool m_DWarnO = false; bool m_DWarnI = false;

    ref PNH_MissionData_Transport m_Config;

    override bool IsExtended() { return true; }

    void TransportMission()
    {
        string caminhoJson = "$profile:PNH/Missions/Transport.json";
        m_Config = new PNH_MissionData_Transport();
        if (FileExist(caminhoJson)) JsonFileLoader<PNH_MissionData_Transport>.JsonLoadFile(caminhoJson, m_Config);

        m_MissionTimeout = m_Config.TempoLimiteSegundos;
        m_MissionInformant = m_Config.Lore.Informante;
        
        if (m_Config.Lore.MensagensRadio && m_Config.Lore.MensagensRadio.Count() > 0)
            m_MissionMessage1 = m_Config.Lore.MensagensRadio[0];
    }

    int ContarItens(PlayerBase player, string classeAlvo)
    {
        int total = 0;
        array<EntityAI> itemsArray = new array<EntityAI>;
        player.GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER, itemsArray);
        for (int i = 0; i < itemsArray.Count(); i++) {
            if (itemsArray[i] && itemsArray[i].GetType() == classeAlvo) total++;
        }
        
        if (m_MissionVehicle) {
            array<EntityAI> carItems = new array<EntityAI>;
            m_MissionVehicle.GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER, carItems);
            for (int j = 0; j < carItems.Count(); j++) {
                if (carItems[j] && carItems[j].GetType() == classeAlvo) total++;
            }
        }
        return total;
    }

    override void PlayerChecks(PlayerBase player)
    {
        if (m_CurrentPhase == 4) return;
        vector playerPos = player.GetPosition();

        // --- FASE A: VYBOR ---
        if (m_CurrentPhase == 0)
        {
            float distA = vector.Distance(playerPos, m_Config.PosicaoVeiculo.ToVector());
            if (!m_AWarnO && distA <= 150.0) { m_AWarnO = true; EnviarAviso(m_MissionInformant, m_Config.Lore.MensagemAproximacao + m_Config.CidadeFaseA + "."); }
            if (!m_AWarnI && distA <= 20.0)  { m_AWarnI = true; EnviarAviso(m_MissionInformant, m_Config.Lore.MensagemNoObjetivo + " Monte o veículo."); }

            if (m_MissionVehicle && m_MissionVehicle.EngineIsOn())
            {
                m_CurrentPhase = 1;
                EnviarAviso(m_MissionInformant, m_Config.Lore.MensagemFaseB);
                PNH_Logger.Log("Missões", "[PNH] Fase A Concluída.");
            }
        }
        // --- FASE B: SKALISTY ---
        else if (m_CurrentPhase == 1)
        {
            float distB = vector.Distance(playerPos, m_Config.PosicaoGalpaoFaseB.ToVector());
            if (!m_BWarnO && distB <= 500.0) { m_BWarnO = true; EnviarAviso(m_MissionInformant, m_Config.Lore.MensagemAproximacao + m_Config.CidadeFaseB + "."); }
            if (!m_BWarnI && distB <= 30.0)  { m_BWarnI = true; EnviarAviso(m_MissionInformant, m_Config.Lore.MensagemNoObjetivo); }

            int qtdLivroArte = ContarItens(player, m_Config.ClasseLivroArteDaGuerra);
            
            if (qtdLivroArte >= 1 && !m_AmbushShedTriggered) {
                m_AmbushShedTriggered = true;
                SpawnZumbis(12, m_Config.PosicaoGalpaoFaseB.ToVector(), 20.0);
                EnviarAviso("ALERTA PNH", "Múltiplos infectados detectados cercando o galpão de Skalisty!");
            }

            if (ContarItens(player, m_Config.ClasseMaleta) >= 3 && qtdLivroArte >= 1) {
                m_CurrentPhase = 2;
                EnviarAviso(m_MissionInformant, m_Config.Lore.MensagemFaseC);
            }
        }
        // --- FASE C: IGREJA ---
        else if (m_CurrentPhase == 2)
        {
            float distC = vector.Distance(playerPos, m_Config.PosicaoCorpo.ToVector());
            if (!m_CWarnO && distC <= 150.0) { m_CWarnO = true; EnviarAviso(m_MissionInformant, m_Config.Lore.MensagemAproximacao + m_Config.CidadeFaseC + "."); }
            if (!m_CWarnI && distC <= 20.0)  { m_CWarnI = true; EnviarAviso(m_MissionInformant, "Igreja visualizada. Use a gazua para entrar!"); }

            int qtdBiblia = ContarItens(player, m_Config.ClasseLivroBiblia);
            
            if (qtdBiblia >= 1 && !m_AmbushChurchTriggered) {
                m_AmbushChurchTriggered = true;
                SpawnZumbis(m_Config.ZumbisFaseC, m_Config.PosicaoCorpo.ToVector(), 25.0);
                EnviarAviso("ALERTA PNH", "CÓDIGO RECUPERADO. A IGREJA ESTÁ CERCADA! FUJA!");
            }

            if (m_AmbushChurchTriggered && qtdBiblia >= 1 && distC > 100.0) {
                m_CurrentPhase = 3;
                EnviarAviso(m_MissionInformant, m_Config.Lore.MensagemFaseD + m_Config.CidadeFaseD + ".");
            }
        }
        // --- FASE D: ALTAR ---
        else if (m_CurrentPhase == 3)
        {
            float distD = vector.Distance(playerPos, m_Config.PosicaoNPC.ToVector());
            if (!m_DWarnO && distD <= 150.0) { m_DWarnO = true; EnviarAviso(m_MissionInformant, m_Config.Lore.MensagemAproximacao + m_Config.CidadeFaseD + "."); }
            if (!m_DWarnI && distD <= 20.0)  { m_DWarnI = true; EnviarAviso(m_MissionInformant, "Limpador visualizado. Estacione o veículo."); }

            if (m_NPC && m_MissionVehicle) {
                float distCarToNPC = vector.Distance(m_MissionVehicle.GetPosition(), m_Config.PosicaoNPC.ToVector());
                if (distD <= 10.0 && distCarToNPC <= 15.0) {
                    if (ContarItens(player, m_Config.ClasseMaleta) >= 3 && ContarItens(player, m_Config.ClasseLivroArteDaGuerra) >= 1 && ContarItens(player, m_Config.ClasseLivroBiblia) >= 1) {
                        m_CurrentPhase = 4;
                        GetGame().ObjectDelete(m_MissionVehicle);
                        vector rPos = m_Config.PosicaoBarril.ToVector();
                        m_RewardContainer = ItemBase.Cast(GetGame().CreateObject(m_Config.Recompensas.Container, rPos, false, false, true));
                        if (m_RewardContainer) {
                            auto loadout = m_Config.Recompensas.Loadouts.GetRandomElement();
                            for (int l = 0; l < loadout.Itens.Count(); l++) m_RewardContainer.GetInventory().CreateInInventory(loadout.Itens[l]);
                            m_RewardContainer.Close();
                            m_MissionObjects.Insert(m_RewardContainer);
                        }
                        EnviarAviso("Comando PNH", m_Config.Lore.MensagemVitoria);
                        PNH_MissionSettingsData config = PNH_MissionSettings.GetData();
                        m_MissionTimeout = m_MissionTime + config.ConfiguracoesGerais.TempoLimpezaSegundos;
                    }
                }
            }
        }
    }

    override bool DeployMission()
    {
        if (!m_Config || !m_Config.Ativa) return false;

        // FASE A
        m_MissionVehicle = Car.Cast(GetGame().CreateObject(m_Config.ClasseVeiculo, m_Config.PosicaoVeiculo.ToVector()));
        if (m_MissionVehicle) {
            m_MissionVehicle.SetOrientation(m_Config.OrientacaoVeiculo.ToVector());
            m_MissionVehicle.GetInventory().CreateAttachment("HatchbackDoors_Driver");
            m_MissionVehicle.GetInventory().CreateAttachment("HatchbackDoors_CoDriver");
            m_MissionVehicle.GetInventory().CreateAttachment("HatchbackHood");
            for(int w=0; w<4; w++) m_MissionVehicle.GetInventory().CreateAttachment("HatchbackWheel");
            m_MissionObjects.Insert(m_MissionVehicle);
        }
        for (int i = 0; i < m_Config.PecasVeiculo.Count(); i++) {
            PNH_MissionSettings_ItemPos p = m_Config.PecasVeiculo[i];
            Object peca = GetGame().CreateObject(p.Classe, p.Posicao.ToVector());
            if (peca) { peca.SetOrientation(p.Orientacao.ToVector()); m_MissionObjects.Insert(peca); }
        }
        SpawnZumbis(m_Config.ZumbisFaseA, m_Config.PosicaoVeiculo.ToVector(), 20.0);

        // FASE B
        for (int m = 0; m < m_Config.PosicoesMaletas.Count(); m++) {
            PNH_MissionSettings_ItemPos mal = m_Config.PosicoesMaletas[m];
            Object maleta = GetGame().CreateObject(mal.Classe, mal.Posicao.ToVector());
            if (maleta) { maleta.SetOrientation(mal.Orientacao.ToVector()); m_MissionObjects.Insert(maleta); }
        }
        Object livroA = GetGame().CreateObject(m_Config.ClasseLivroArteDaGuerra, m_Config.PosicaoLivroArteDaGuerra.ToVector());
        if (livroA) { livroA.SetOrientation(m_Config.OrientacaoLivroArteDaGuerra.ToVector()); m_MissionObjects.Insert(livroA); }
        Object gazua = GetGame().CreateObject(m_Config.ClasseFerramenta, m_Config.PosicaoFerramenta.ToVector());
        if (gazua) { gazua.SetOrientation(m_Config.OrientacaoFerramenta.ToVector()); m_MissionObjects.Insert(gazua); }
        SpawnZumbis(m_Config.ZumbisFaseB, m_Config.PosicaoGalpaoFaseB.ToVector(), 25.0);

        // FASE C
        Object churchObj = null;
        GetGame().GetObjectsAtPosition(m_Config.PosicaoCorpo.ToVector(), 30.0, m_ObjectList, m_ObjectCargoList);
        for (int b = 0; b < m_ObjectList.Count(); b++) {
            if (m_ObjectList[b].GetType().Contains("Church")) { churchObj = m_ObjectList[b]; break; }
        }
        if (churchObj) {
            Building church = Building.Cast(churchObj);
            for (int k = 0; k < 10; k++) {
                if (church.IsDoorOpen(k)) church.CloseDoor(k);
                if (!church.IsDoorLocked(k)) church.LockDoor(k);
            }
        }
        m_DeadPriest = GetGame().CreateObject(m_Config.ClasseCorpo, m_Config.PosicaoCorpo.ToVector(), false, false, true);
        if (m_DeadPriest) {
            m_DeadPriest.SetOrientation(m_Config.OrientacaoCorpo.ToVector());
            EntityAI priestEnt = EntityAI.Cast(m_DeadPriest);
            priestEnt.SetHealth("", "", 10);
            m_MissionObjects.Insert(m_DeadPriest);
            vector zKillPos = m_Config.PosicaoZumbiAssassino.ToVector();
            m_MissionAIs.Insert(GetGame().CreateObject(m_Config.ClasseZumbiAssassino, zKillPos, false, true, true));
        }
        Object livroB = GetGame().CreateObject(m_Config.ClasseLivroBiblia, m_Config.PosicaoLivroBiblia.ToVector());
        if (livroB) { livroB.SetOrientation(m_Config.OrientacaoLivroBiblia.ToVector()); m_MissionObjects.Insert(livroB); }

        // FASE D
        m_NPC = GetGame().CreateObject(m_Config.ClasseNPC, m_Config.PosicaoNPC.ToVector(), false, false, true);
        if (m_NPC) {
            m_NPC.SetOrientation(m_Config.OrientacaoNPC.ToVector());
            EntityAI npcEnt = EntityAI.Cast(m_NPC);
            for (int r = 0; r < m_Config.RoupasNPC.Count(); r++) npcEnt.GetInventory().CreateInInventory(m_Config.RoupasNPC[r]);
            m_MissionObjects.Insert(m_NPC);
        }

        PNH_Logger.Log("Missões", "[PNH SYSTEM] Operação Prometeu Iniciada.");
        return true;
    }
    
    override void CleanUp() {
        if (m_CurrentPhase < 4 && m_MissionVehicle) {
            m_MissionVehicle.SetHealth("", "", 0); 
            m_MissionObjects.RemoveItem(m_MissionVehicle); 
        }
        super.CleanUp();
    }

    void SpawnZumbis(int qtd, vector pos, float raio) {
        if (m_Config.Dificuldade && m_Config.Dificuldade.ClassesZumbis.Count() > 0) {
            for (int z = 0; z < qtd; z++) {
                vector zPos = pos + Vector(Math.RandomFloat(-raio, raio), 0, Math.RandomFloat(-raio, raio));
                zPos[1] = GetGame().SurfaceY(zPos[0], zPos[2]);
                m_MissionAIs.Insert(GetGame().CreateObject(m_Config.Dificuldade.ClassesZumbis.GetRandomElement(), zPos, false, true, true));
            }
        }
    }

    void EnviarAviso(string emissor, string msg) {
        PNH_MissionSettingsData config = PNH_MissionSettings.GetData();
        if (config.ConfiguracoesGerais.UsarPDA) GetRPCManager().SendRPC("[GearPDA] ", "SendGlobalMessage", new Param2<string, string>(emissor, msg), true);
        else PNH_Utils.SendMessageToAll("[" + emissor + "] " + msg);
    }
}