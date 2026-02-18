/// --- Documentação PNH_Core: PNH_MissionBase.c ---
/// Versão do Sistema: 1.0.0 (Ref: PNH_Consts)
/// Função do arquivo: Definir a classe base (blueprint) para todas as missões do sistema, contendo propriedades fundamentais como o dono do contrato, posição, tempo limite e métodos de limpeza (CleanUp) e aceitação.
/// Comunicação com outros arquivos: É a classe pai de todas as missões específicas (ex: Horde.c) e é gerida diretamente pelo PNH_MissionManager.c, que armazena a instância da missão ativa.
/// Motivo da existência do arquivo no sistema: Fornecer uma estrutura padronizada e reutilizável, garantindo que qualquer nova missão criada siga as mesmas regras de funcionamento e gestão de dados do Core.
/// Dependências internas: Nenhuma dependência direta de outros Managers na sua definição base, mas é estendido por classes que utilizam o PNH_LogisticsManager.c e PNH_BroadcastManager.c.
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

    // Sistema de Lore
    string m_MissionInformant;
    ref array<string> m_MissionMessages;

    void PNH_MissionBase()
    {
        m_MissionTime = 0;
        m_MissionTimeout = 3600; 
        m_MissionZoneOuterRadius = 200.0;
        m_MissionAccepted = false;
        m_MissionOwnerID = "";
        
        m_MissionInformant = "COMANDO PNH";
        m_MissionMessages = new array<string>;
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
    }
}