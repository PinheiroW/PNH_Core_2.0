/*
    MOD: PNH_Core 2.0
    SCRIPT: PNH_MissionManager.c (4_World)
    DESC: O Cérebro do sistema. Controla o tempo, sorteia as missões,
          faz o tracking dos jogadores e finaliza os eventos.
*/

class PNH_MissionManager
{
    ref PNH_MissionBase m_ActiveMission;
    
    float m_UpdateTimer;
    int m_MissionState;     // 0 = Aguardando (Cooldown), 1 = Missão Ativa
    int m_CooldownTimer;    // Tempo até a próxima missão nascer

    void PNH_MissionManager()
    {
        m_UpdateTimer = 0;
        m_MissionState = 0;
        m_CooldownTimer = 60; // Para testes, a primeira missão nasce em 60 segundos após o server ligar
        
        PNH_Logger.Log("Missões", "[PNH_CORE] Mission Manager Inicializado. Aguardando para iniciar missões...");
    }

    // Função chamada constantemente pelo servidor
    void OnUpdate(float timeslice)
    {
        m_UpdateTimer += timeslice;

        // OTIMIZAÇÃO: Só roda a lógica a cada 2 segundos para não pesar o servidor
        if (m_UpdateTimer >= 2.0)
        {
            m_UpdateTimer = 0;

            if (m_MissionState == 0) // ESTADO: AGUARDANDO
            {
                m_CooldownTimer -= 2;
                if (m_CooldownTimer <= 0)
                {
                    StartRandomMission();
                }
            }
            else if (m_MissionState == 1) // ESTADO: MISSÃO RODANDO
            {
                if (m_ActiveMission)
                {
                    m_ActiveMission.m_MissionTime += 2;

                    // 1. Verifica se o tempo da missão acabou (Timeout)
                    if (m_ActiveMission.m_MissionTime >= m_ActiveMission.m_MissionTimeout)
                    {
                        PNH_Logger.Log("Missões", "[PNH_CORE] O tempo da missão esgotou. Iniciando protocolo de limpeza...");
                        VPPNotification.Push("COMANDO PNH", "Tempo esgotado. Missão abortada.", "set:dayz_gui_vpp image:vpp_icon_mission", 16711680, 7.0);
                        EndMission();
                        return;
                    }

                    // 2. Verifica se a missão já acabou com Vitória
                    // (Aguardando para limpar o mapa X segundos depois do sucesso)
                    if (m_ActiveMission.m_MsgNum == -1) 
                    {
                        if (m_ActiveMission.m_MissionTime >= m_ActiveMission.m_MsgChkTime)
                        {
                            EndMission();
                            return;
                        }
                    }

                    // 3. Verifica a distância dos jogadores (Trigger e Checks)
                    array<Man> players = new array<Man>;
                    GetGame().GetPlayers(players);
                    for (int i = 0; i < players.Count(); i++)
                    {
                        PlayerBase player = PlayerBase.Cast(players[i]);
                        if (player && player.IsAlive())
                        {
                            // Envia o jogador para a missão fazer a matemática de distância
                            m_ActiveMission.PlayerChecks(player);
                        }
                    }
                }
            }
        }
    }

    void StartRandomMission()
    {
        PNH_Logger.Log("Missões", "[PNH_CORE] Preparando nova operação...");

        // NOTA: Na Fase 3, colocaremos aqui o Sorteio do EventsWorldData.
        // Por enquanto, a estrutura já está pronta para receber a missão escolhida.
        
        // Exemplo do que vai acontecer aqui em breve:
        // m_ActiveMission = new TransportMission();
        // m_ActiveMission.DeployMission();

        m_MissionState = 1; // Muda o estado para "Rodando"
    }

    void EndMission()
    {
        // A MÁGICA ACONTECE AQUI:
        // Ao igualar a nulo, o DayZ chama o Destrutor (~PNH_MissionBase) 
        // e limpa todos os carros e zumbis automaticamente do mapa!
        m_ActiveMission = null; 
        
        m_MissionState = 0;
        m_CooldownTimer = 1800; // 30 Minutos de intervalo até a próxima missão nascer
        
        PNH_Logger.Log("Missões", "[PNH_CORE] Missão finalizada e mapa limpo. Aguardando próximo ciclo.");
    }
}