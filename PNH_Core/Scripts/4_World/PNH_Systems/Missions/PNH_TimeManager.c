/// --- Documentação PNH_Core: PNH_TimeManager.c ---
/// Versão do Sistema: 1.0.0 (Ref: PNH_Consts)
/// Função do arquivo: Fornecer ferramentas de manipulação de tempo, incluindo a conversão de minutos para segundos, validação de intervalos decorridos e a formatação de segundos em strings legíveis (MM:SS) para o rádio ou interface.
/// Comunicação com outros arquivos: É utilizado pelo PNH_MissionManager.c para calcular o tempo de conclusão de missões e pelo PNH_ChatManager.c para exibir o tempo restante no comando !resumo.
/// Motivo da existência no sistema: Centralizar a lógica matemática de tempo, evitando cálculos manuais e inconsistentes em diferentes partes do código.
/// Dependências internas: Nenhuma.
/// Última atualização: 2026-02-18
/// IMPORTANTE: Ao alterar este arquivo, documente no CHANGELOG_PNH.md

class PNH_TimeManager
{
    // Função para converter minutos em segundos (útil para as configs)
    static int MinutesToSeconds(int minutes)
    {
        return minutes * 60;
    }

    // Calcula se um tempo limite foi atingido
    static bool HasTimeElapsed(float currentTime, float timeout)
    {
        return (currentTime >= timeout);
    }

    // Formata segundos em string legível (ex: 05:30) para o rádio ou UI
    static string FormatTime(float seconds)
    {
        int totalSeconds = Math.Round(seconds);
        int mins = totalSeconds / 60;
        int secs = totalSeconds % 60;
        
        string sMins = mins.ToString();
        string sSecs = secs.ToString();
        
        if (mins < 10) sMins = "0" + sMins;
        if (secs < 10) sSecs = "0" + sSecs;
        
        return sMins + ":" + sSecs;
    }
}