class PNH_MissionSettingsData
{
    bool UsarPDA;
    int TempoEntreMissoes; // Em minutos
    string DebugMission;   // Deixa vazio "" para aleatório, ou escreve o nome da missão (ex: "Camp") para forçar testes
    ref array<string> MissoesAtivas;

    void PNH_MissionSettingsData()
    {
        MissoesAtivas = new array<string>;
    }
}

class PNH_MissionSettings
{
    private static const string SETTINGS_PATH = "$profile:PNH_MissionSettings.json";
    private static ref PNH_MissionSettingsData m_Data;

    static void Load()
    {
        if (!m_Data) m_Data = new PNH_MissionSettingsData();

        if (FileExist(SETTINGS_PATH))
        {
            JsonFileLoader<PNH_MissionSettingsData>.JsonLoadFile(SETTINGS_PATH, m_Data);
        }
        else
        {
            // Cria as definições padrão caso o ficheiro não exista
            m_Data.UsarPDA = true;
            m_Data.TempoEntreMissoes = 35;
            m_Data.DebugMission = ""; 
            m_Data.MissoesAtivas.Insert("Camp");
            m_Data.MissoesAtivas.Insert("Horde");
            m_Data.MissoesAtivas.Insert("PlaneCrash");
            m_Data.MissoesAtivas.Insert("BearHunt");
            m_Data.MissoesAtivas.Insert("CityStore");
            m_Data.MissoesAtivas.Insert("Shrooms");
            m_Data.MissoesAtivas.Insert("Psilos");
            m_Data.MissoesAtivas.Insert("UrbanMall");
            m_Data.MissoesAtivas.Insert("Graveyard");
            
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