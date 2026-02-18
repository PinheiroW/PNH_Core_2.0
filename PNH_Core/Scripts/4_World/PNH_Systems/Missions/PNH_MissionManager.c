class PNH_MissionManager
{
    static ref PNH_MissionManager m_Instance; 
    
    ref PNH_MissionBase m_ActiveMission;
    float m_UpdateTimer;
    int m_MissionState;     
    int m_CooldownTimer;

    // --- DEFINIÇÃO DE ESTADOS PNH 2.0 ---
    const int STATE_IDLE          = 0; // Cooldown (Aguardando próximo ciclo)
    const int STATE_AVAILABLE     = 1; // Disponível (Sorteada, aguardando !aceitar)
    const int STATE_MATERIALIZING = 2; // Aceita (Aguardando 15s para o DayZ fazer o spawn)
    const int STATE_ACTIVE        = 3; // Em Curso (Vigilância de objetivos ligada)

    void PNH_MissionManager()
    {
        m_UpdateTimer = 0; 
        m_MissionState = STATE_IDLE;
        PNH_MissionSettings.Load();
        ResetCooldown();
    }

    static PNH_MissionManager GetInstance() 
    { 
        if (!m_Instance) m_Instance = new PNH_MissionManager();
        return m_Instance; 
    }

    void ReloadMissions() 
    { 
        PNH_MissionSettings.Load(); 
        PNH_NPCManager.GetInstance().SpawnAllNPCs(); 
    }

    void ForceMissionCycle() 
    { 
        if (m_ActiveMission) m_ActiveMission.CleanUp(); 
        m_ActiveMission = null; 
        m_MissionState = STATE_IDLE; 
        m_CooldownTimer = 10; 
    }

    void ResetCooldown()
    {
        if (PNH_MissionSettings.GetData() && PNH_MissionSettings.GetData().ConfiguracoesGerais) 
            m_CooldownTimer = PNH_MissionSettings.GetData().ConfiguracoesGerais.TempoEntreMissoesMinutos * 60;
        else m_CooldownTimer = 300; 
    }

    void OnUpdate(float timeslice)
    {
        m_UpdateTimer += timeslice;
        if (m_UpdateTimer >= 2.0)
        {
            m_UpdateTimer = 0;

            switch (m_MissionState)
            {
                case STATE_IDLE:
                    m_CooldownTimer -= 2;
                    if (m_CooldownTimer <= 0) StartRandomMission();
                    break;

                case STATE_AVAILABLE:
                    // Aguarda o comando !aceitar do jogador.
                    // Quando o ContractBroker valida o contrato, ele deve mudar o estado para STATE_MATERIALIZING.
                    if (m_ActiveMission && m_ActiveMission.m_MissionAccepted)
                    {
                        m_MissionState = STATE_MATERIALIZING;
                        m_ActiveMission.m_MissionTime = 0; // Reinicia o relógio da missão
                        PNH_Logger.Log("Missões", "[PNH_CORE] Contrato assinado. Iniciando materialização...");
                    }
                    break;

                case STATE_MATERIALIZING:
                    // TRAVÃO DE SEGURANÇA: Dá 15 segundos ao motor do DayZ para criar os NPCs.
                    // Durante este tempo, NENHUM PlayerCheck é executado.
                    m_ActiveMission.m_MissionTime += 2;
                    if (m_ActiveMission.m_MissionTime >= 15)
                    {
                        m_MissionState = STATE_ACTIVE;
                        PNH_Logger.Log("Missões", "[PNH_CORE] Materialização concluída. Vigilância ativa.");
                    }
                    break;

                case STATE_ACTIVE:
                    if (!m_ActiveMission) { m_MissionState = STATE_IDLE; return; }
                    
                    m_ActiveMission.m_MissionTime += 2;

                    // Verifica Timeout (Falha por tempo)
                    if (m_ActiveMission.m_MissionTime >= m_ActiveMission.m_MissionTimeout) 
                    {
                        EndMission();
                    }
                    else 
                    {
                        // Executa a vigilância (PlayerChecks) apenas neste estado
                        array<Man> players = new array<Man>; 
                        GetGame().GetPlayers(players);
                        for (int i = 0; i < players.Count(); i++) 
                        { 
                            PlayerBase p = PlayerBase.Cast(players.Get(i)); 
                            if (p) m_ActiveMission.PlayerChecks(p); 
                        }
                    }
                    break;
            }
        }
    }

    void StartRandomMission()
    {
        PNH_MissionSettingsData config = PNH_MissionSettings.GetData();
        string selectedMission = "";

        if (config && config.DebugSettings.DebugMode && config.DebugSettings.DebugMission != "") 
            selectedMission = config.DebugSettings.DebugMission;
        else {
            array<string> todasAsMissoes = new array<string>;
            if (config.CatalogoMissoes) {
                foreach(string m1 : config.CatalogoMissoes.Tier1_Recruta) todasAsMissoes.Insert(m1);
                foreach(string m2 : config.CatalogoMissoes.Tier2_Mercenario) todasAsMissoes.Insert(m2);
                foreach(string m3 : config.CatalogoMissoes.Tier3_Especialista) todasAsMissoes.Insert(m3);
                foreach(string m4 : config.CatalogoMissoes.Tier4_Lenda) todasAsMissoes.Insert(m4);
            }
            if (todasAsMissoes.Count() == 0) { m_CooldownTimer = 60; return; }
            selectedMission = todasAsMissoes.GetRandomElement();
        }

        // Instanciação da missão (Mesma lógica anterior)
        if (selectedMission == "Apartment") { m_ActiveMission = new ApartmentMission(); m_ActiveMission.m_MissionTier = 3; }
        else if (selectedMission == "Horde") { m_ActiveMission = new HordeMission(); m_ActiveMission.m_MissionTier = 1; }
        else if (selectedMission == "BearHunt") { m_ActiveMission = new BearHuntMission(); m_ActiveMission.m_MissionTier = 1; }
        else if (selectedMission == "PlaneCrash") { m_ActiveMission = new PlaneCrashMission(); m_ActiveMission.m_MissionTier = 3; }
        else if (selectedMission == "Transport") { m_ActiveMission = new TransportMission(); m_ActiveMission.m_MissionTier = 4; }
        else if (selectedMission == "Graveyard") { m_ActiveMission = new GraveyardMission(); m_ActiveMission.m_MissionTier = 1; }
        else if (selectedMission == "CityStore") { m_ActiveMission = new CityStoreMission(); m_ActiveMission.m_MissionTier = 2; }

        if (!m_ActiveMission) return;
        m_ActiveMission.m_MissionType = selectedMission;
        
        PNH_EventsWorldData.Init(); 
        int selectedIdx = -1;
        for (int i = 0; i < PNH_EventsWorldData.MissionEvents.Count(); i++) {
            if (PNH_EventsWorldData.MissionEvents.Get(i).IndexOf(selectedMission) == 0) { selectedIdx = i; break; }
        }

        if (selectedIdx != -1) {
            m_ActiveMission.m_MissionPosition = PNH_EventsWorldData.MissionPositions.Get(selectedIdx);
            string eventName = PNH_EventsWorldData.MissionEvents.Get(selectedIdx);
            int startPos = selectedMission.Length() + 1;
            if (eventName.Length() > startPos) 
                m_ActiveMission.m_MissionLocation = eventName.Substring(startPos, eventName.Length() - startPos);
            else 
                m_ActiveMission.m_MissionLocation = "Setor Desconhecido";
        }

        m_MissionState = STATE_AVAILABLE; // Muda para estado "Disponível"
        
        string npcLocal = "Green Mountain";
        if (m_ActiveMission.m_MissionTier >= 3) npcLocal = "Radio Zenit";
        
        PNH_BroadcastManager.GetInstance().AnnounceMissionAvailable(npcLocal);
    }

    void EndMission() 
    { 
        if (m_ActiveMission) m_ActiveMission.CleanUp(); 
        m_ActiveMission = null; 
        m_MissionState = STATE_IDLE; 
        ResetCooldown(); 
    }
}