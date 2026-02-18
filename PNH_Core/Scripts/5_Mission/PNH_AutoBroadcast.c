/// --- Documentação PNH_Core: PNH_AutoBroadcast.c ---
/// Versão do Sistema: 1.0.0 (Ref: PNH_Consts)
//[cite_start]/// Função do arquivo: Gerir o sistema de mensagens automáticas periódicas do servidor, lendo uma lista de frases e enviando-as globalmente num intervalo definido[cite: 31].
//[cite_start]/// Comunicação com outros arquivos: Lê a lista de mensagens e o intervalo de tempo do PNH_CoreConfig.c e utiliza o PNH_Utils.c para disparar a mensagem para todos os jogadores[cite: 31].
/// Motivo da existência no sistema: Automatizar avisos de regras, dicas de jogo ou boas-vindas, mantendo os jogadores informados de forma cíclica.
//[cite_start]/// Dependências internas: PNH_CoreConfig.c (definições de mensagens e tempo) e PNH_Utils.c (função de envio global)[cite: 31].
/// Última atualização: 2026-02-18
/// IMPORTANTE: Ao alterar este arquivo, documente no CHANGELOG_PNH.md

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