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