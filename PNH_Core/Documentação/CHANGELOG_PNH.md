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
----------------------------------------------------------------------------------------------------------------------------------

📝 Resumo da FASE 1: Dinâmica de Narrativa e Documentação (2026-02-18)
Esta fase inicial teve como foco a organização técnica do repositório e a implementação de uma camada de imersão narrativa (Sistema de Rádio), permitindo que o mod "falasse" com o jogador durante o progresso das missões.

🛠️ Arquivos Modificados e Procedimentos
Documentação Global: Implementação de cabeçalhos de documentação técnica em todos os 23 ficheiros do sistema para mapear dependências e funções.

PNH_MissionData.c: Criação das classes PNH_LoreEtapas e PNH_DicionarioMissao, estabelecendo a estrutura para diálogos específicos por etapa de missão (Aceitou, 90m, 20m, Concluiu).

PNH_MissionBase.c: Adição de flags de controlo de estado (m_Msg90mSent, m_Msg20mSent, m_MsgConcluiuSent) para evitar a repetição de mensagens de rádio.

PNH_MissionSettings.c: Atualização do gerador automático para injetar narrativas padrão no JSON, permitindo que a missão "Horde" já nascesse com diálogos de lore.

PNH_MissionManager.c: Implementação do motor de injeção que vincula automaticamente a lore do dicionário JSON ao objeto da missão sorteada.

Horde.c: Implementação dos gatilhos físicos de distância, disparando notificações de rádio automáticas quando o jogador atinge os raios de 90 metros e 20 metros do objetivo.

PNH_Utils.c: Refatoração da lógica de patentes para incluir notificações visuais de promoção via PNH_BroadcastManager sempre que o XP atinge um novo patamar.

🎯 Impacto no Sistema
Imersão RPG: As missões deixaram de ser puramente mecânicas, passando a oferecer feedback narrativo constante ao jogador via rádio.

Rastreabilidade: A nova documentação estrutural permite uma manutenção facilitada e identificação rápida de conflitos entre camadas.

Progressão Visual: O sistema de patentes tornou-se gratificante através de alertas em tempo real durante a subida de nível.
----------------------------------------------------------------------------------------------------------------------------------

📝 Resumo da Fase 2: Localização Total & Estabilização (2026-02-18)
Esta etapa focou na remoção absoluta de strings fixas (hardcoded) no código-fonte, transferindo toda a comunicação visual e textual para o ficheiro de configuração JSON. Além disso, a fase incluiu a resolução de conflitos de compilação gerados pela transição estrutural da Fase 1 e a validação prática do mod em ambiente de servidor.

🛠️ Arquivos Modificados e Procedimentos
PNH_MissionData.c: Expansão do modelo de dados com 9 novos campos na classe PNH_TextosInterface, cobrindo mensagens de chat, status de perfil e avisos de recompensa.


PNH_MissionSettings.c: Atualização do gerador automático de configurações para incluir e salvar as novas strings padrão no ficheiro físico PNH_MissionSettings.json.


PNH_TreasuryManager.c: Implementação de notificações dinâmicas de recompensa, onde o sistema recupera a mensagem do JSON e injeta o valor de XP ganho em tempo real.

PNH_ChatManager.c: Localização total da interface de chat, garantindo que os retornos dos comandos !status, !perfil e !missao sejam lidos diretamente das configurações.

🐛 Hotfix: Resolução de Crash de Compilação
O Problema: Durante o arranque, o servidor apresentou o erro Can't compile "World" script module! Can't find variable 'm_MissionMessages'. Isto ocorreu porque o arquivo PNH_ContractBroker.c ainda tentava ler a variável antiga de briefing, que havia sido substituída na Fase 1.

A Solução: O PNH_ContractBroker.c foi atualizado para utilizar o novo sistema de narrativa. A chamada obsoleta foi substituída por m_LoreEtapas.Aceitou, integrada com o PNH_BroadcastManager para enviar o briefing inicial ao jogador de forma correta.

✅ Validação em Campo (Testes)
Os testes in-game confirmaram a estabilidade total das novas lógicas:

Comandos de Chat: Sucesso na execução dos comandos localizados !status, !perfil, !missao, !reload_mission e !aceitar.


Gatilhos de Distância (Rádio): O sistema monitorizou corretamente a distância do jogador, disparando os alertas de áudio e ecrã aos 90 e 20 metros do objetivo.


Sistema de Loot Inteligente: A missão "Horde" foi concluída com sucesso, resultando no spawn correto do contentor de recompensa contendo a quantidade limite configurada (10 itens).

🎯 Impacto no Sistema
Customização Externa: Administradores de servidor podem agora traduzir ou alterar o tom das mensagens do mod sem necessidade de editar ou recompilar scripts.

Estabilidade: O sistema de rádio e contratos interage de forma segura e limpa, garantindo progressão fluida sem falhas de execução.
----------------------------------------------------------------------------------------------------------------------------------

