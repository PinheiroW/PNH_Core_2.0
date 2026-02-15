class PNH_MissionSettingsData
{
    bool UsarPDA;
    int TempoEntreMissoes; // Em minutos
    string DebugMission;   // Deixa vazio "" para aleatório, ou escreve o nome da missão (ex: "Apartment") para forçar testes
    ref array<string> MissoesAtivas;

    void PNH_MissionSettingsData()
    {
        MissoesAtivas = new array<string>;
    }
}

class PNH_MissionSettings
{
    // Caminho da pasta oficial PNH no profiles
    private static const string SETTINGS_PATH = "$profile:PNH/PNH_MissionSettings.json";
    private static ref PNH_MissionSettingsData m_Data;

    static void Load()
    {
        if (!m_Data) m_Data = new PNH_MissionSettingsData();

        // Garante que a pasta PNH existe antes de carregar ou salvar
        if (!FileExist("$profile:PNH")) 
        {
            MakeDirectory("$profile:PNH");
        }

        if (FileExist(SETTINGS_PATH))
        {
            JsonFileLoader<PNH_MissionSettingsData>.JsonLoadFile(SETTINGS_PATH, m_Data);
        }
        else
        {
            // Cria as definições padrão caso o ficheiro não exista (Apenas a base limpa)
            m_Data.UsarPDA = true;
            m_Data.TempoEntreMissoes = 35;
            m_Data.DebugMission = ""; 
            
            // Insere a única missão ativa e configurada do mod
            m_Data.MissoesAtivas.Insert("Apartment");
            
            Save();
        }
    }

    static void Save()
    {
        JsonFileLoader<PNH_MissionSettingsData>.JsonSaveFile(SETTINGS_PATH, m_Data);
    }

    static PNH_MissionSettingsData GetData()
    {
        if (!m_Data) Load();
        return m_Data;
    }
}