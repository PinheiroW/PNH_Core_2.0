/// --- Documentação PNH_Core: PNH_Consts.c ---
/// Versão do Sistema: 3.0.0 (Integração do Tablet PDA)
/// Função do arquivo: Define constantes globais e agora inclui as chaves de comunicação de rede (RPC) entre o Servidor e o Cliente.
/// Comunicação com outros arquivos: É referenciado por quase todos os arquivos do mod que utilizam logs, salvamento de arquivos ou comunicação de rede.
/// Motivo da existência do arquivo no sistema: Centralizar configurações estáticas para evitar o uso de "strings mágicas" e garantir que os canais de rede não colidam com outros mods.
/// Dependências internas: Nenhuma.
/// Última atualização: 2026-02-18
/// IMPORTANTE: Ao alterar este arquivo, documente no CHANGELOG_PNH.md

// Enum para garantir que os nossos RPCs (Canais de Comunicação) são únicos
enum PNH_RPCs
{
    // Usamos um número alto (ex: 85550) para garantir que não colide com o DayZ Vanilla ou com outros mods que possas ter no servidor
    RPC_TABLET_SYNC_DATA = 85550,
    RPC_TABLET_OPEN_REQUEST = 85551
}

class PNH_Consts
{
    // Caminho base para salvar configs (Profiles/PNH_Settings/...)
    static const string ROOT_PROFILE_DIR = "$profile:PNH_Settings";
    
    // Prefixo visual para mensagens no chat ou logs
    static const string LOG_PREFIX = "[PNH SYSTEM]";
    
    // Versão atual do Core
    static const string VERSION = "3.0.0"; // Atualizado para a Fase 3 (Tablet)
}