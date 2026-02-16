// --- SUB-CLASSES DE ORGANIZAÇÃO DO JSON ---
class PNH_MissionSettings_Debug {
    bool DebugMode;
    string DebugMission;
    bool DebugShowInfo;
}

class PNH_MissionSettings_Geral {
    int TempoEntreMissoesMinutos;
    int TempoLimpezaSegundos;
    bool UsarPDA;
    bool ModoRP;
}

// --- CLASSE PRINCIPAL DOS DADOS MESTRES ---
class PNH_MissionSettingsData {
    // CORREÇÃO: Usando 'DebugSettings' para evitar conflito com a classe nativa do DayZ
    ref PNH_MissionSettings_Debug DebugSettings; 
    ref PNH_MissionSettings_Geral ConfiguracoesGerais;
    ref array<string> MissoesAtivas;

    // Inicializa as categorias para evitar Null Pointers
    void PNH_MissionSettingsData() {
        DebugSettings = new PNH_MissionSettings_Debug();
        ConfiguracoesGerais = new PNH_MissionSettings_Geral();
        MissoesAtivas = new array<string>;
    }
}

// --- GERENCIADOR DO ARQUIVO ---
class PNH_MissionSettings {
    static ref PNH_MissionSettingsData m_Data;

    static void Load() {
        string dirPath = "$profile:PNH/";
        string filePath = dirPath + "PNH_MissionSettings.json";

        if (!FileExist(dirPath)) MakeDirectory(dirPath);

        m_Data = new PNH_MissionSettingsData();

        if (FileExist(filePath)) {
            JsonFileLoader<PNH_MissionSettingsData>.JsonLoadFile(filePath, m_Data);
        } else {
            // Se o arquivo não existir, gera um JSON padrão automaticamente!
            m_Data.DebugSettings.DebugMode = false;
            m_Data.DebugSettings.DebugMission = "";
            m_Data.DebugSettings.DebugShowInfo = false;
            
            m_Data.ConfiguracoesGerais.TempoEntreMissoesMinutos = 5;
            m_Data.ConfiguracoesGerais.TempoLimpezaSegundos = 60;
            m_Data.ConfiguracoesGerais.UsarPDA = true;
            m_Data.ConfiguracoesGerais.ModoRP = false;
            
            m_Data.MissoesAtivas.Insert("Apartment");
            m_Data.MissoesAtivas.Insert("BearHunt");
            
            JsonFileLoader<PNH_MissionSettingsData>.JsonSaveFile(filePath, m_Data);
        }
    }

    static PNH_MissionSettingsData GetData() {
        if (!m_Data) Load();
        return m_Data;
    }
}