/// --- Documentação PNH_Core: PNH_Consts.c ---
/// Função do arquivo: Define constantes globais acessíveis por todo o sistema, como caminhos de diretórios e versão do mod.
/// Comunicação com outros arquivos: É referenciado por quase todos os arquivos do mod que utilizam logs, salvamento de arquivos ou verificação de versão.
/// Motivo da existência do arquivo no sistema: Centralizar configurações estáticas para evitar o uso de "strings mágicas" espalhadas pelo código, facilitando manutenções globais.
/// Dependências internas: Nenhuma.
/// Última atualização: 2026-02-18
/// Sempre que alterar ou documentar um arquivo, atualize também o arquivo CHANGELOG_PNH

class PNH_Consts
{
    // Caminho base para salvar configs (Profiles/PNH_Settings/...)
    static const string ROOT_PROFILE_DIR = "$profile:PNH_Settings";
    
    // Prefixo visual para mensagens no chat ou logs
    static const string LOG_PREFIX = "[PNH SYSTEM]";
    
    // Versão atual do Core
    static const string VERSION = "1.0.0";
}