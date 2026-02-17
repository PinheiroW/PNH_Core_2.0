class PNH_MissionManager
{
    static PNH_MissionManager m_Instance;
    ref PNH_MissionBase m_ActiveMission;
    float m_UpdateTimer;
    int m_MissionState;     
    int m_CooldownTimer;

    void PNH_MissionManager()
    {
        m_Instance = this; 
        m_UpdateTimer = 0;
        m_MissionState = 0;
        PNH_MissionSettings.Load();
        
        if (PNH_MissionSettings.GetData() && PNH_MissionSettings.GetData().ConfiguracoesGerais) {
            m_CooldownTimer = PNH_MissionSettings.GetData().ConfiguracoesGerais.TempoEntreMissoesMinutos * 60;
        } else {
            m_CooldownTimer = 60; 
        }
    }

    static PNH_MissionManager GetInstance() { return m_Instance; }

    // --- FUNÇÕES PARA COMANDOS DE ADMIN (RELOAD) ---
    void ReloadMissions()
    {
        PNH_MissionSettings.Load();
        PNH_Logger.Log("Missões", "[PNH_CORE] Configurações Mestre recarregadas com sucesso.");
    }

    void ForceMissionCycle()
    {
        if (m_ActiveMission) 
        {
            m_ActiveMission.CleanUp();
            m_ActiveMission = null;
        }
        m_MissionState = 0;
        m_CooldownTimer = 10; // Reinicia em 10 segundos
        PNH_Logger.Log("Missões", "[PNH_CORE] Ciclo de missões resetado por Admin.");
    }

    // --- SPAWN DE NPCs FIXOS ---
    void SpawnNPCs()
    {
        PNH_MissionSettingsData config = PNH_MissionSettings.GetData();
        if (!config || config.NPCsQuestGivers.Count() == 0) return;

        foreach (PNH_MissionSettings_NPC npcData : config.NPCsQuestGivers)
        {
            PlayerBase npc = PlayerBase.Cast(GetGame().CreateObject("SurvivorM_Mirek", npcData.Posicao.ToVector(), false, false, true));
            if (npc)
            {
                npc.SetPosition(npcData.Posicao.ToVector());
                npc.SetOrientation(npcData.Orientacao.ToVector());
                foreach (string item : npcData.Roupas) { npc.GetInventory().CreateInInventory(item); }
                npc.SetAllowDamage(false);
                if (npc.GetBrain()) npc.GetBrain().Stop();
            }
        }
    }

    void OnUpdate(float timeslice)
    {
        m_UpdateTimer += timeslice;
        if (m_UpdateTimer >= 2.0)
        {
            m_UpdateTimer = 0;
            if (m_MissionState == 0)
            {
                m_CooldownTimer -= 2;
                if (m_CooldownTimer <= 0) StartRandomMission();
            }
            else if (m_MissionState == 1 && m_ActiveMission)
            {
                if (m_ActiveMission.m_MissionAccepted)
                {
                    m_ActiveMission.m_MissionTime += 2;
                    if (m_ActiveMission.m_MissionTime >= m_ActiveMission.m_MissionTimeout) EndMission();
                    else
                    {
                        array<Man> players = new array<Man>;
                        GetGame().GetPlayers(players);
                        for (int i = 0; i < players.Count(); i++)
                        {
                            PlayerBase p = PlayerBase.Cast(players.Get(i));
                            if (p) m_ActiveMission.PlayerChecks(p);
                        }
                    }
                }
            }
        }
    }

    void StartRandomMission()
    {
        PNH_MissionSettingsData config = PNH_MissionSettings.GetData();
        array<string> todasAsMissoes = new array<string>;
        if (config.CatalogoMissoes) {
            foreach(string m1 : config.CatalogoMissoes.Tier1_Recruta) todasAsMissoes.Insert(m1);
            foreach(string m2 : config.CatalogoMissoes.Tier2_Mercenario) todasAsMissoes.Insert(m2);
            foreach(string m3 : config.CatalogoMissoes.Tier3_Especialista) todasAsMissoes.Insert(m3);
            foreach(string m4 : config.CatalogoMissoes.Tier4_Lenda) todasAsMissoes.Insert(m4);
        }

        if (todasAsMissoes.Count() == 0) { m_CooldownTimer = 60; return; }

        string selectedMission = todasAsMissoes.GetRandomElement();
        if (selectedMission == "Apartment") m_ActiveMission = new ApartmentMission();
        else if (selectedMission == "BearHunt") m_ActiveMission = new BearHuntMission();
        else if (selectedMission == "Horde") m_ActiveMission = new HordeMission();
        else if (selectedMission == "PlaneCrash") m_ActiveMission = new PlaneCrashMission();
        else if (selectedMission == "Transport") m_ActiveMission = new TransportMission();
        else if (selectedMission == "Graveyard") m_ActiveMission = new GraveyardMission();
        else if (selectedMission == "CityStore") m_ActiveMission = new CityStoreMission();

        if (!m_ActiveMission) return;

        PNH_EventsWorldData.Init(); 
        int selectedIdx = -1;
        for (int i = 0; i < PNH_EventsWorldData.MissionEvents.Count(); i++)
        {
            if (PNH_EventsWorldData.MissionEvents.Get(i).IndexOf(selectedMission) == 0) {
                selectedIdx = i; break;
            }
        }

        if (selectedIdx != -1) m_ActiveMission.m_MissionPosition = PNH_EventsWorldData.MissionPositions.Get(selectedIdx);
        else { m_ActiveMission = null; m_CooldownTimer = 30; return; }

        m_MissionState = 1;
        string npcLocal = "Green Mountain";
        if (m_ActiveMission.m_MissionTier >= 3) npcLocal = "Radio Zenit";

        PNH_Utils.SendMessageToAll("[ALERTA PNH] Nova operação disponível em " + npcLocal + ". Vá até o local para assinar o contrato!");
    }

    void EndMission()
    {
        if (m_ActiveMission) m_ActiveMission.CleanUp(); 
        m_ActiveMission = null; m_MissionState = 0;
        m_CooldownTimer = PNH_MissionSettings.GetData().ConfiguracoesGerais.TempoEntreMissoesMinutos * 60; 
    }
}