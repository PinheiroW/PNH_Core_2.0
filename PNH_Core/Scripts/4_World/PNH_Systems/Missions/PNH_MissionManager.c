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
        m_UpdateTimer = 0; m_MissionState = 0;
        PNH_MissionSettings.Load();
        ResetCooldown();
    }

    static PNH_MissionManager GetInstance() { return m_Instance; }

    void ReloadMissions() 
    { 
        PNH_MissionSettings.Load(); 
        // PNH 2.0: Avisa o Agente de NPCs para atualizar o mapa se o JSON mudou
        PNH_NPCManager.GetInstance().SpawnAllNPCs(); 
    }

    void ForceMissionCycle() 
    { 
        if (m_ActiveMission) m_ActiveMission.CleanUp(); 
        m_ActiveMission = null; 
        m_MissionState = 0; 
        m_CooldownTimer = 10; 
    }

    void ResetCooldown()
    {
        if (PNH_MissionSettings.GetData() && PNH_MissionSettings.GetData().ConfiguracoesGerais) 
            m_CooldownTimer = PNH_MissionSettings.GetData().ConfiguracoesGerais.TempoEntreMissoesMinutos * 60;
        else m_CooldownTimer = 300; 
    }

    void SendLoreMessages()
    {
        if (!m_ActiveMission) return;
        
        string tag = "[" + m_ActiveMission.m_MissionInformant + "] ";
        PNH_Utils.SendMessageToAll(tag + m_ActiveMission.m_MissionMessage1);
        
        if (m_ActiveMission.m_MissionMessage2 != "")
            GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(this.DelayedMessage, 3000, false, tag + m_ActiveMission.m_MissionMessage2);
        
        if (m_ActiveMission.m_MissionMessage3 != "")
            GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(this.DelayedMessage, 6000, false, tag + m_ActiveMission.m_MissionMessage3);
            
        if (m_ActiveMission.m_MissionMessage4 != "")
            GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(this.DelayedMessage, 9000, false, tag + m_ActiveMission.m_MissionMessage4);
    }

    void DelayedMessage(string msg) { PNH_Utils.SendMessageToAll(msg); }

    void OnUpdate(float timeslice)
    {
        m_UpdateTimer += timeslice;
        if (m_UpdateTimer >= 2.0)
        {
            m_UpdateTimer = 0;
            if (m_MissionState == 0) { m_CooldownTimer -= 2; if (m_CooldownTimer <= 0) StartRandomMission(); }
            else if (m_MissionState == 1 && m_ActiveMission && m_ActiveMission.m_MissionAccepted)
            {
                m_ActiveMission.m_MissionTime += 2;
                if (m_ActiveMission.m_MissionTime >= m_ActiveMission.m_MissionTimeout) EndMission();
                else {
                    array<Man> players = new array<Man>; GetGame().GetPlayers(players);
                    for (int i = 0; i < players.Count(); i++) { PlayerBase p = PlayerBase.Cast(players.Get(i)); if (p) m_ActiveMission.PlayerChecks(p); }
                }
            }
        }
    }

    void StartRandomMission()
    {
        PNH_MissionSettingsData config = PNH_MissionSettings.GetData();
        string selectedMission = "";

        if (config && config.DebugSettings.DebugMode && config.DebugSettings.DebugMission != "") selectedMission = config.DebugSettings.DebugMission;
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
            if (eventName.Length() > startPos) {
                m_ActiveMission.m_MissionLocation = eventName.Substring(startPos, eventName.Length() - startPos);
            } else {
                m_ActiveMission.m_MissionLocation = "Setor Desconhecido";
            }
        }

        m_MissionState = 1;
        string npcLocal = "Green Mountain";
        if (m_ActiveMission.m_MissionTier >= 3) npcLocal = "Radio Zenit";
        PNH_Utils.SendMessageToAll("[ALERTA PNH] Nova operacao disponivel em " + npcLocal + ". Va ate ao local para assinar o contrato!");
    }

    void EndMission() { if (m_ActiveMission) m_ActiveMission.CleanUp(); m_ActiveMission = null; m_MissionState = 0; ResetCooldown(); }
}