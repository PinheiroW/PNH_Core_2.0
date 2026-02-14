/*
    MOD: PNH_Core
    SCRIPT: PNH_MissionBase.c (4_World)
    DESC: Classe base proprietária para missões PNH. 
          Gere os arrays, as variáveis e a limpeza automática de lixo do servidor.
*/

class PNH_MissionBase
{
    // --- VARIÁVEIS DE CONFIGURAÇÃO ---
    bool m_MissionExtended;
    int m_MissionTimeout;
    float m_MissionZoneOuterRadius;
    float m_MissionZoneInnerRadius;
    
    // --- VARIÁVEIS DE NARRATIVA ---
    string m_MissionInformant;
    string m_MissionMessage1;
    string m_MissionMessage2;
    string m_MissionMessage3;
    string m_MissionLocation;
    string m_MissionLocationDir;
    ref TStringArray m_MissionDescription;

    // --- VARIÁVEIS DE CONTROLE DE TEMPO ---
    int m_MissionTime;
    int m_MsgNum;
    int m_MsgChkTime;
    bool m_RewardsSpawned;
    bool m_ContainerWasTaken;

    // --- POSIÇÃO E OBJETOS (O Loot e os Zumbis) ---
    vector m_MissionPosition;
    ref array<Object> m_ObjectList;
    ref array<CargoBase> m_ObjectCargoList;
    ref array<Object> m_MissionObjects;
    ref array<Object> m_MissionAIs;
    ref array<PlayerBase> m_PlayersInZone; // <--- CORREÇÃO ADICIONADA AQUI

    // --- CONSTRUTOR ---
    void PNH_MissionBase()
    {
        // Inicializa os arrays de forma segura
        m_MissionDescription = new TStringArray;
        m_ObjectList = new array<Object>;
        m_ObjectCargoList = new array<CargoBase>;
        m_MissionObjects = new array<Object>;
        m_MissionAIs = new array<Object>;
        m_PlayersInZone = new array<PlayerBase>; // <--- CORREÇÃO ADICIONADA AQUI
        
        m_MissionTime = 0;
        m_RewardsSpawned = false;
        m_ContainerWasTaken = false;
    }

    // --- DESTRUTOR (Limpeza Automática Anti-Lag) ---
    void ~PNH_MissionBase()
    {
        CleanUp();
    }

    // --- FUNÇÕES VIRTUAIS (As missões vão sobrescrever isto) ---
    bool IsExtended() { return false; }
    bool DeployMission() { return false; }
    void PlayerChecks(PlayerBase player) {}
    void MissionFinal() {}
    
    // --- LIMPEZA DE MEMÓRIA (Remove lixo do mapa) ---
    void CleanUp()
    {
        if (m_MissionObjects)
        {
            for (int i = 0; i < m_MissionObjects.Count(); i++)
            {
                if (m_MissionObjects.Get(i))
                    GetGame().ObjectDelete(m_MissionObjects.Get(i));
            }
            m_MissionObjects.Clear();
        }

        if (m_MissionAIs)
        {
            for (int j = 0; j < m_MissionAIs.Count(); j++)
            {
                if (m_MissionAIs.Get(j))
                    GetGame().ObjectDelete(m_MissionAIs.Get(j));
            }
            m_MissionAIs.Clear();
        }
    }
}