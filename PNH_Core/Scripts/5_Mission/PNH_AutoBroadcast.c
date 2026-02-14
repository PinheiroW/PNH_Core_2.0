modded class MissionServer
{
    private float m_PNH_BroadcastTimer;
    private int m_PNH_CurrentMsgIndex;

    override void OnInit()
    {
        super.OnInit();
        m_PNH_BroadcastTimer = 0;
        m_PNH_CurrentMsgIndex = 0;
    }

    override void OnUpdate(float timeslice)
    {
        super.OnUpdate(timeslice);

        // Se a config não carregou ou lista vazia, ignora
        if (!PNH_CoreConfig.m_Settings || PNH_CoreConfig.m_Settings.BroadcastMessages.Count() == 0) return;

        m_PNH_BroadcastTimer += timeslice;

        // Converte minutos do Config para segundos ( * 60 )
        float intervalSeconds = PNH_CoreConfig.m_Settings.BroadcastIntervalMin * 60;
        
        if (intervalSeconds < 60) intervalSeconds = 60; // Segurança mínima de 1 minuto

        if (m_PNH_BroadcastTimer >= intervalSeconds)
        {
            m_PNH_BroadcastTimer = 0;
            SendNextBroadcast();
        }
    }

    void SendNextBroadcast()
    {
        array<string> msgs = PNH_CoreConfig.m_Settings.BroadcastMessages;
        if (msgs.Count() == 0) return;

        if (m_PNH_CurrentMsgIndex >= msgs.Count()) m_PNH_CurrentMsgIndex = 0;

        string msg = msgs.Get(m_PNH_CurrentMsgIndex);
        
        // Usa a nova função do Utils para mandar a todos
        PNH_Utils.SendMessageToAll(msg);

        m_PNH_CurrentMsgIndex++;
    }
}