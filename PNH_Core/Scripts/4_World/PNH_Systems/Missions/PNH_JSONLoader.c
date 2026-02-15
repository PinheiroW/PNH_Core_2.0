class PNH_JSONLoader
{
    static const string pastaMissions = "$profile:PNH_Missions/";

    static void Carregar(out array<ref PNH_MissionConfig> lista)
    {
        // Cria a pasta se ela não existir
        if (!FileExist(pastaMissions)) 
        {
            MakeDirectory(pastaMissions);
            PNH_Logger.Log("Missões", "[PNH_CORE] Pasta PNH_Missions criada em Profiles.");
        }

        lista.Clear(); // Limpa a lista antes de recarregar (importante para o /reload_mission)
        
        string fileName;
        int fileAttr;
        int flags;
        
        // Inicia a busca por arquivos .json
        FindFileHandle handle = FindFile(pastaMissions + "*.json", fileName, fileAttr, flags);
        
        // Se encontrou algum arquivo (handle válido)
        if (handle != 0) 
        {
            // Processa o primeiro arquivo encontrado
            if (fileName != "") ProcessarArquivo(fileName, lista);
            
            // Faz um loop procurando os próximos arquivos na pasta
            while (FindNextFile(handle, fileName, fileAttr))
            {
                if (fileName != "") ProcessarArquivo(fileName, lista);
            }
            
            // Fecha o ponteiro de busca para liberar memória
            CloseFindFile(handle);
        }
    }

    // Função separada para organizar a leitura
    static void ProcessarArquivo(string nomeArquivo, out array<ref PNH_MissionConfig> lista)
    {
        ref PNH_MissionConfig cfg = new PNH_MissionConfig();
        string caminhoCompleto = pastaMissions + nomeArquivo;
        
        // O DayZ tenta carregar o JSON e colocar os dados dentro da variável 'cfg'
        JsonFileLoader<ref PNH_MissionConfig>.JsonLoadFile(caminhoCompleto, cfg);
        
        // Se deu certo e a missão tem um nome
        if (cfg && cfg.Nome != "")
        {
            lista.Insert(cfg);
        }
    }
}