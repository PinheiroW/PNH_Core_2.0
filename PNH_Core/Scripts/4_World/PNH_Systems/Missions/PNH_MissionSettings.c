/// --- Documentação PNH_Core: PNH_MissionSettings.c ---
/// Versão do Sistema: 1.2.0 (Fase de Localização Total)
/// Função do arquivo: Gerir o ciclo de vida do ficheiro de configuração JSON, agora expandido para gerar e carregar todas as mensagens de interface, recompensas e chat de forma dinâmica.
/// Comunicação com outros arquivos: Providencia o método GetData() utilizado por todos os sistemas. Utiliza as classes definidas no PNH_MissionData.c para estruturar o ficheiro físico.
/// Motivo da existência no sistema: Centralizar a configuração de loot, XP e agora a localização total do mod, permitindo traduções sem necessidade de recompilar scripts.
/// Dependências internas: PNH_MissionData.c (estruturas de dados) e PNH_Logger.c (registos de estado).
/// Última atualização: 2026-02-18
/// IMPORTANTE: Ao alterar este arquivo, documente no CHANGELOG_PNH.md

class PNH_MissionSettings
{
    protected static string m_ConfigRoot = "$profile:\\PNH\\";
    protected static string m_ConfigPath = m_ConfigRoot + "PNH_MissionSettings.json";
    protected static ref PNH_MissionSettingsData m_ConfigData;

    static void Load()
    {
        if (!FileExist(m_ConfigRoot))
        {
            MakeDirectory(m_ConfigRoot);
            PNH_Logger.Log("Settings", "[PNH_CORE] Pasta de configuracao criada em: " + m_ConfigRoot);
        }

        if (!FileExist(m_ConfigPath))
        {
            PNH_Logger.Log("Settings", "[PNH_CORE] Ficheiro JSON nao encontrado. Gerando configuracao inicial completa...");
            CreateDefaultConfig();
        }

        if (!m_ConfigData) m_ConfigData = new PNH_MissionSettingsData();
        JsonFileLoader<PNH_MissionSettingsData>.JsonLoadFile(m_ConfigPath, m_ConfigData);
        
        if (m_ConfigData)
        {
            PNH_Logger.Log("Settings", "[PNH_CORE] Configuracoes carregadas com sucesso.");
        }
        else
        {
            PNH_Logger.Log("Settings", "[ERRO] Falha critica ao processar o JSON!");
        }
    }

    static PNH_MissionSettingsData GetData()
    {
        if (!m_ConfigData) Load();
        return m_ConfigData;
    }

    static void CreateDefaultConfig()
    {
        m_ConfigData = new PNH_MissionSettingsData();

        // 1. Debug e Configurações Gerais
        m_ConfigData.DebugSettings.DebugMode = true;
        m_ConfigData.DebugSettings.DebugMission = "Horde";
        m_ConfigData.DebugSettings.DebugShowInfo = false;
        m_ConfigData.ConfiguracoesGerais.TempoEntreMissoesMinutos = 1;
        m_ConfigData.ConfiguracoesGerais.TempoLimpezaSegundos = 5;
        m_ConfigData.ConfiguracoesGerais.MaxItensNoBarril = 10; 

        // 2. Catálogo de Missões e Tiers
        m_ConfigData.CatalogoMissoes.Guia_MissoesDisponiveis.Insert("Horde");
        m_ConfigData.CatalogoMissoes.Tier1_Recruta.Insert("Horde");

        // 3. Regras de Contratos
        m_ConfigData.RegrasContratos.CooldownMinutos = 10;
        m_ConfigData.RegrasContratos.RaioAbandonoMetros = 2000;
        m_ConfigData.RegrasContratos.TempoRecuperacaoPosMorteSegundos = 900;
        m_ConfigData.RegrasContratos.TempoAvisoAbandonoSegundos = 300;
        m_ConfigData.RegrasContratos.MaxSimultaneasBaixas = 3;
        m_ConfigData.RegrasContratos.MaxSimultaneasAltas = 2;

        // 4. Progressão e Tabela de XP
        m_ConfigData.TabelaXP.XP_Tier_1 = 100;
        m_ConfigData.TabelaXP.XP_Tier_2 = 200;
        m_ConfigData.TabelaXP.XP_Tier_3 = 400;
        m_ConfigData.TabelaXP.XP_Tier_4 = 800;
        m_ConfigData.TabelaXP.XP_Mercenario = 200;
        m_ConfigData.TabelaXP.XP_Especialista = 600;
        m_ConfigData.TabelaXP.XP_Lenda = 1500;

        // 5. Oficiais (NPCs)
        PNH_NPCQuestGiver npc1 = new PNH_NPCQuestGiver();
        npc1.Nome = "Ivan O Coletor";
        npc1.Posicao = "4308.2 321.5 5414.6";
        npc1.Orientacao = "-50.4 0 0";
        npc1.TiersDisponiveis.Insert(1);
        npc1.Roupas.Insert("TTsKO_Jacket_Cammo");
        m_ConfigData.NPCsQuestGivers.Insert(npc1);

        // =======================================================
        // 6. TEXTOS EDITÁVEIS DE INTERFACE (LOCALIZAÇÃO TOTAL)
        // =======================================================
        // Mensagens de Assinatura
        m_ConfigData.Textos.Erro_LongeNPC = "[ERRO PNH] Voce precisa estar perto do Oficial PNH para assinar o contrato!";
        m_ConfigData.Textos.Erro_JaAssinado = "[ERRO PNH] Este contrato ja foi assinado por outro mercenario.";
        m_ConfigData.Textos.Erro_PatenteBaixa = "[ERRO PNH] A tua patente e insuficiente para este contrato!";
        m_ConfigData.Textos.Sucesso_Assinatura = "=== CONTRATO ASSINADO COM SUCESSO ===";

        // Mensagens de Recompensa (Treasury)
        m_ConfigData.Textos.Msg_ContratoLiquidado = "[PNH] CONTRATO LIQUIDADO: +%1 XP";

        // Mensagens de Status de Perfil
        m_ConfigData.Textos.Msg_StatusOperador = "Operador: %1";
        m_ConfigData.Textos.Msg_StatusPatente = "Patente: %1";
        m_ConfigData.Textos.Msg_StatusXP = "XP Atual: %1";
        m_ConfigData.Textos.Msg_StatusProximoNivel = "Proximo Nivel: %1 XP para %2";
        m_ConfigData.Textos.Msg_StatusMissaoAtiva = "Missao Ativa: %1";

        // Mensagens de Consulta de Missão
        m_ConfigData.Textos.Msg_MissaoDisponivel = "[COMANDO PNH] Temos um contrato disponivel na regiao de: %1";
        m_ConfigData.Textos.Msg_MissaoEmOperacao = "[COMANDO PNH] Ja existe um esquadrao em operacao no momento.";
        m_ConfigData.Textos.Msg_SemOperacoes = "[COMANDO PNH] Sem operacoes no momento. Aguardando Intel...";

        // 7. Dicionário de Missões (Narrativa Dinâmica)
        PNH_DicionarioMissao dicHorde = new PNH_DicionarioMissao("Horde");
        dicHorde.Etapas.Aceitou = "Mercenario, uma horda foi avistada. Limpe a area imediatamente!";
        dicHorde.Etapas.Chegou_90m = "Aviso: Movimentacao hostil detectada a 90 metros. Prepare as armas.";
        dicHorde.Etapas.Chegou_20m = "Contato iminente! Voce entrou no perimetro da horda.";
        dicHorde.Etapas.Concluiu = "Excelente trabalho. Perimetro limpo. Suprimentos a caminho.";
        m_ConfigData.DicionarioMissoes.Insert(dicHorde);

        // 8. Loot Tier 1
        PNH_LootItem loot1_1 = new PNH_LootItem(); loot1_1.Item = "SKS"; loot1_1.Chance = 15; loot1_1.Quantidade = 1;
        m_ConfigData.Loot_Tier1.Insert(loot1_1);

        // Salva o arquivo fisicamente
        JsonFileLoader<PNH_MissionSettingsData>.JsonSaveFile(m_ConfigPath, m_ConfigData);
        PNH_Logger.Log("Settings", "[PNH_CORE] PNH_MissionSettings.json atualizado com Localização Total.");
    }

    static void Reload()
    {
        m_ConfigData = null;
        Load();
    }
}