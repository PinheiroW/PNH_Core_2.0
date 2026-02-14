/*
    MOD: PNH_Core 2.0
    SCRIPT: PNH_MissionManager.c (4_World)
    DESC: O Cérebro Final do sistema. Controla sorteios, timeouts e limpeza anti-lag.
*/

class PNH_MissionManager
{
    ref PNH_MissionBase m_ActiveMission;
    
    float m_UpdateTimer;
    int m_MissionState;     
    int m_CooldownTimer;

    void PNH_MissionManager()
    {
        m_UpdateTimer = 0;
        m_MissionState = 0;
        m_CooldownTimer = 15; 
        
        PNH_Logger.Log("Missões", "[PNH_CORE] Mission Manager Inicializado. Carregando satélites...");
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
            else if (m_MissionState == 1)
            {
                if (m_ActiveMission)
                {
                    m_ActiveMission.m_MissionTime += 2;

                    // 1. Timeout (Acabou o tempo)
                    if (m_ActiveMission.m_MissionTime >= m_ActiveMission.m_MissionTimeout)
                    {
                        PNH_Logger.Log("Missões", "[PNH_CORE] O tempo da missão esgotou. Abortando.");
                        PNH_Utils.SendMessageToAll("[COMANDO PNH] Janela tática fechada. Missão abortada.");
                        EndMission();
                        return;
                    }

                    // 2. Concluída 
                    if (m_ActiveMission.m_MsgNum == -1) 
                    {
                        if (m_ActiveMission.m_MissionTime >= m_ActiveMission.m_MsgChkTime)
                        {
                            EndMission();
                            return;
                        }
                    }

                    // 3. Verifica a distância dos jogadores
                    array<Man> players = new array<Man>;
                    GetGame().GetPlayers(players);
                    for (int i = 0; i < players.Count(); i++)
                    {
                        PlayerBase player = PlayerBase.Cast(players[i]);
                        if (player && player.IsAlive())
                        {
                            m_ActiveMission.PlayerChecks(player);
                        }
                    }
                }
            }
        }
    }

    void StartRandomMission()
    {
        PNH_EventsWorldData.Init();

        if (PNH_EventsWorldData.MissionEvents.Count() == 0)
        {
            PNH_Logger.Log("Missões", "[PNH_CORE] ERRO: Nenhuma coordenada no PNH_EventsWorldData!");
            m_CooldownTimer = 60;
            return;
        }

        int idx = Math.RandomIntInclusive(0, PNH_EventsWorldData.MissionEvents.Count() - 1);
        string eventData = PNH_EventsWorldData.MissionEvents[idx];
        vector eventPos = PNH_EventsWorldData.MissionPositions[idx];

        TStringArray dataArray = new TStringArray;
        eventData.Split(" ", dataArray);
        
        if (dataArray.Count() < 2) return; 

        string missionType = dataArray[0]; 
        string city = dataArray[1];        
        string dir = "";
        if (dataArray.Count() > 2) dir = dataArray[2];         

        // A CORREÇÃO PRINCIPAL: As missões de verdade
        if (missionType == "Transport") m_ActiveMission = new TransportMission();
        else if (missionType == "Apartment") m_ActiveMission = new ApartmentMission();
        else if (missionType == "CityMall") m_ActiveMission = new CityMallMission();
        else if (missionType == "UrbanMall") m_ActiveMission = new UrbanMallMission();
        else if (missionType == "Horde") m_ActiveMission = new HordeMission();
        else if (missionType == "PlaneCrash") m_ActiveMission = new PlaneCrashMission();
        else if (missionType == "Psilos") m_ActiveMission = new PsilosMission();
        else if (missionType == "Shrooms") m_ActiveMission = new ShroomsMission();
        else if (missionType == "CityStore") m_ActiveMission = new CityStoreMission();
        else if (missionType == "Camp") m_ActiveMission = new CampMission();
        else if (missionType == "FreePigs") m_ActiveMission = new FreePigsMission();
        else if (missionType == "Ganja") m_ActiveMission = new GanjaMission();
        else if (missionType == "Graveyard") m_ActiveMission = new GraveyardMission();
        else if (missionType == "BearHunt") m_ActiveMission = new BearHuntMission();
        else 
        {
            PNH_Logger.Log("Missões", "[PNH_CORE] IGNORADO: Missão não configurada: " + missionType);
            m_CooldownTimer = 10;
            return;
        }

        PNH_Logger.Log("Missões", "[PNH_CORE] Operação Escolhida: " + missionType + " em " + city);

        m_ActiveMission.m_MissionPosition = eventPos;
        m_ActiveMission.m_MissionLocation = city;
        m_ActiveMission.m_MissionLocationDir = dir;
        m_ActiveMission.m_MissionDescription = dataArray;

        if (m_ActiveMission.DeployMission())
        {
            m_MissionState = 1; 
        }
        else
        {
            PNH_Logger.Log("Missões", "[PNH_CORE] Falha (Local inválido). Sorteando outra em 10s...");
            m_ActiveMission = null;
            m_CooldownTimer = 10; 
        }
    }

    void EndMission()
    {
        m_ActiveMission = null; 
        m_MissionState = 0;
        m_CooldownTimer = 1800; 
        PNH_Logger.Log("Missões", "[PNH_CORE] Operação arquivada. Mapa limpo.");
    }
}