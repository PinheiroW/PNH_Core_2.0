/// --- Documentação PNH_Core: PNH_MissionBase.c ---
/// Versão do Sistema: 1.1.0 (Suporte à Narrativa Dinâmica)
/// Função do arquivo: Definir a classe base (blueprint) para todas as missões, agora incluindo flags de controlo para mensagens de rádio por proximidade e suporte ao novo dicionário de etapas.
/// Comunicação com outros arquivos: É estendido por missões como Horde.c e gerido pelo PNH_MissionManager.c. Utiliza o PNH_BroadcastManager.c para disparar as mensagens de rádio.
/// Motivo da existência: Garantir que todas as missões herdem a lógica de estados e as flags de comunicação, evitando que mensagens de rádio sejam disparadas repetidamente.
/// Dependências internas: PNH_MissionData.c (para a classe PNH_LoreEtapas).
/// Última atualização: 2026-02-18
/// IMPORTANTE: Ao alterar este arquivo, documente no CHANGELOG_PNH.md

class PNH_MissionBase
{
    string m_MissionOwnerID;
    string m_MissionOwnerName;
    bool m_MissionAccepted;
    int m_MissionTier;
    string m_MissionType; 
    string m_MissionLocation;

    vector m_MissionPosition;
    float m_MissionTimeout; 
    float m_MissionTime;
    float m_MissionZoneOuterRadius;
    
    ref array<Object> m_MissionAIs = new array<Object>;

    // --- SISTEMA DE NARRATIVA DINÂMICA ---
    string m_MissionInformant;
    ref PNH_LoreEtapas m_LoreEtapas; // Armazena os textos das 4 etapas
    
    // Flags de controlo para evitar repetição de rádio
    bool m_Msg90mSent;
    bool m_Msg20mSent;
    bool m_MsgConcluiuSent;

    void PNH_MissionBase()
    {
        m_MissionTime = 0;
        m_MissionTimeout = 3600; 
        m_MissionZoneOuterRadius = 200.0;
        m_MissionAccepted = false;
        m_MissionOwnerID = "";
        
        m_MissionInformant = "COMANDO PNH";
        m_LoreEtapas = new PNH_LoreEtapas();
        
        m_Msg90mSent = false;
        m_Msg20mSent = false;
        m_MsgConcluiuSent = false;
    }

    void AcceptContract(PlayerBase player, int tier, string missionName)
    {
        if (!player || !player.GetIdentity()) return;
        m_MissionOwnerID = player.GetIdentity().GetPlainId();
        m_MissionOwnerName = player.GetIdentity().GetName();
        m_MissionAccepted = true;
        m_MissionTier = tier;
        m_MissionType = missionName;
    }

    bool IsContractOwner(PlayerBase player)
    {
        if (!m_MissionAccepted || !player || !player.GetIdentity()) return false;
        return (m_MissionOwnerID == player.GetIdentity().GetPlainId());
    }

    bool DeployMission() { return true; }
    void MissionFinal() {}
    void MissionChecks() {}

    void CleanUp()
    {
        foreach (Object ai : m_MissionAIs) { if (ai) GetGame().ObjectDelete(ai); }
        m_MissionAIs.Clear();
        m_MissionAccepted = false;
        
        // Reset das flags para o próximo ciclo
        m_Msg90mSent = false;
        m_Msg20mSent = false;
        m_MsgConcluiuSent = false;
    }
}