📝 CHANGELOG & ESTRUTURA: PNH_Core 2.0
📂 Resumo do Mod
O PNH_Core 2.0 é um framework modular para o DayZ projetado para gerir missões dinâmicas, progressão de jogadores (XP/Patentes) e automação de recompensas. O sistema é dividido em três camadas principais (3_Game, 4_World e 5_Mission) para garantir alta performance e facilidade de manutenção.

🚀 Alterações Recentes [2026-02-18]
Documentação Inline Total: Implementação de cabeçalhos de documentação em todos os 23 arquivos do sistema para mapeamento de dependências.


Sistema de Loot 2.0: Implementação de "roleta inteligente" no PNH_LogisticsManager.c, permitindo preencher contentores com até 10 itens baseados em probabilidades (RNG) do JSON.


Formatação Discord: Ajuste no PNH_ContractBroker.c para envio de coordenadas formatadas com parênteses retos [X: ..., Z: ...] para o canal de missões.


Estabilização de Comandos: Correção e validação dos comandos !resumo, !status, !missao e !reload_mission.
+1

🏗️ Arquitetura e Funções por Pasta
📁 3_Game (Infraestrutura Base)

PNH_Consts.c: Centraliza constantes globais como versões e prefixos de log.

PNH_CoreConfig.c: Gere o carregamento do Core_Config.json (Webhooks e permissões de Admin).

PNH_Discord.c: Motor de envio de Webhooks formatados para comunicação externa.

PNH_Logger.c: Centralizador de mensagens de log no servidor e alertas críticos.

📁 4_World (Cérebro e Sistemas)
PNH_Utils.c: Contém funções globais e o ProfileManager para gestão de XP e Patentes.

PNH_ChatManager.c: Processador lógico de todos os comandos iniciados por "!" ou "/".

PNH_MissionManager.c: Gestor do ciclo de vida das missões (Sorteio -> Ativação -> Finalização).

PNH_ContractBroker.c: Intermediário que valida assinaturas de contrato perto de NPCs.

PNH_MissionBase.c: Blueprint (modelo) que define as propriedades obrigatórias de cada missão.

PNH_BroadcastManager.c: Gestor de notificações visuais na tela e mensagens de rádio.

PNH_MissionData.c: Define a estrutura de classes que o JSON de missões deve seguir.

PNH_LogisticsManager.c: Responsável pelo spawn físico de inimigos, veículos e loot inteligente.

PNH_AuditManager.c: Regista cronologicamente todos os eventos de missão para auditoria.

PNH_IntelManager.c: Realiza cálculos de distância e validação de áreas de interesse.

PNH_TimeManager.c: Converte e formata temporizadores para exibição aos jogadores.

PNH_TreasuryManager.c: Processa o pagamento de XP e progressão após missões bem-sucedidas.

PNH_NPCManager.c: Gere o spawn e a persistência dos Oficiais Quest Givers no mapa.

PNH_EventsWorldData.c: Base de dados geográfica com todas as coordenadas de missões.

PNH_MissionSettings.c: Carregador e gerador automático do PNH_MissionSettings.json.

📁 5_Mission (Inicialização e Ganchos)
PNH_Init.c: Orquestrador que liga o sistema no arranque do servidor (OnInit).

PNH_ChatHook.c: Interceta mensagens no chat global para processar comandos ou logar no Discord.

PNH_ConnectionLogger.c: Monitoriza e envia alertas de entradas e saídas de jogadores.

PNH_AutoBroadcast.c: Sistema cíclico de mensagens automáticas no chat global.