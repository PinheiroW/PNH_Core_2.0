/// --- Documentação PNH_Core: PNH_TabletServer.c ---
/// Versão do Sistema: 3.0.0
/// Função do arquivo: Interceta pedidos e gere o armazenamento temporário de dados para a UI.

// ==========================================
// 1. O ESPELHO DE DADOS (Ponte 4_World -> 5_Mission)
// ==========================================
class PNH_TabletData
{
    static string m_pName, m_patenteNome, m_patenteProxima, m_missaoDisponivel, m_alvoAtual, m_intelText;
    static int m_xpAtual, m_xpProximo;
    static bool m_NovaSincronizacao = false; 
}

// ==========================================
// 2. O COMUNICADOR DE REDE
// ==========================================
modded class PlayerBase
{
    override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
    {
        super.OnRPC(sender, rpc_type, ctx);

        // [LADO DO SERVIDOR]
        if (GetGame().IsServer() && rpc_type == PNH_RPCs.RPC_TABLET_OPEN_REQUEST)
        {
            if (!sender) return;

            string plainId = sender.GetPlainId();
            string pName = sender.GetName();
            
            PNH_PlayerProfileData pData = PNH_ProfileManager.LoadProfile(plainId, pName);
            PNH_MissionSettingsData settings = PNH_MissionSettings.GetData();
            
            string patenteNome = "Recruta";
            int xpAtual = 0;
            int xpProximo = 0;
            string patenteProxima = "Mercenario";

            if (pData && settings)
            {
                xpAtual = pData.XP;
                xpProximo = settings.TabelaXP.XP_Mercenario;
                if (pData.Patente == 2) { patenteNome = "Mercenario"; xpProximo = settings.TabelaXP.XP_Especialista; patenteProxima = "Especialista"; }
                else if (pData.Patente == 3) { patenteNome = "Especialista"; xpProximo = settings.TabelaXP.XP_Lenda; patenteProxima = "Lenda"; }
                else if (pData.Patente == 4) { patenteNome = "Lenda"; xpProximo = 0; patenteProxima = "N/A"; }
            }

            string missaoDisponivel = "Sem operações ativas.";
            string alvoAtual = "Nenhum";
            string intelText = "Aguardando ordens do Comando PNH.";
            
            PNH_MissionManager mngr = PNH_MissionManager.GetInstance();
            if (mngr)
            {
                if (mngr.m_MissionState == 1 && mngr.m_ActiveMission) missaoDisponivel = "Contrato Disponível: " + mngr.m_ActiveMission.m_MissionLocation;
                else if (mngr.m_MissionState >= 2 && mngr.m_ActiveMission)
                {
                    missaoDisponivel = "Operação em andamento.";
                    if (mngr.m_ActiveMission.IsContractOwner(this))
                    {
                        alvoAtual = mngr.m_ActiveMission.m_MissionLocation;
                        
                        // LÓGICA DO HISTÓRICO DE INTEL COM AS VARIÁVEIS CORRETAS
                        if (mngr.m_ActiveMission.m_LoreEtapas)
                        {
                            // 1. Mensagem Base (Aceitou)
                            if (mngr.m_ActiveMission.m_LoreEtapas.Aceitou != "") 
                                intelText = mngr.m_ActiveMission.m_LoreEtapas.Aceitou;
                            
                            // 2. Atualização 90m
                            if (mngr.m_ActiveMission.m_Msg90mSent && mngr.m_ActiveMission.m_LoreEtapas.Chegou_90m != "")
                                intelText += "<br/><br/><b>[ ATUALIZAÇÃO TÁTICA 90m ]:</b><br/>" + mngr.m_ActiveMission.m_LoreEtapas.Chegou_90m;
                            
                            // 3. Atualização 20m
                            if (mngr.m_ActiveMission.m_Msg20mSent && mngr.m_ActiveMission.m_LoreEtapas.Chegou_20m != "")
                                intelText += "<br/><br/><b>[ ALERTA DE CONTACTO 20m ]:</b><br/>" + mngr.m_ActiveMission.m_LoreEtapas.Chegou_20m;
                            
                            // 4. Conclusão
                            if (mngr.m_ActiveMission.m_MsgConcluiuSent && mngr.m_ActiveMission.m_LoreEtapas.Concluiu != "")
                                intelText += "<br/><br/><b>[ RELATÓRIO FINAL ]:</b><br/>" + mngr.m_ActiveMission.m_LoreEtapas.Concluiu;
                        }
                    }
                }
            }

            ScriptRPC rpc = new ScriptRPC();
            rpc.Write(pName); rpc.Write(patenteNome); rpc.Write(xpAtual); rpc.Write(xpProximo); rpc.Write(patenteProxima);
            rpc.Write(missaoDisponivel); rpc.Write(alvoAtual); rpc.Write(intelText);
            rpc.Send(this, PNH_RPCs.RPC_TABLET_SYNC_DATA, true, sender);
        }

        // [LADO DO CLIENTE]
        if (GetGame().IsClient() && rpc_type == PNH_RPCs.RPC_TABLET_SYNC_DATA)
        {
            string c_pName, c_patenteNome, c_patenteProxima, c_missaoDisponivel, c_alvoAtual, c_intelText;
            int c_xpAtual, c_xpProximo;

            if (!ctx.Read(c_pName)) return;
            if (!ctx.Read(c_patenteNome)) return;
            if (!ctx.Read(c_xpAtual)) return;
            if (!ctx.Read(c_xpProximo)) return;
            if (!ctx.Read(c_patenteProxima)) return;
            if (!ctx.Read(c_missaoDisponivel)) return;
            if (!ctx.Read(c_alvoAtual)) return;
            if (!ctx.Read(c_intelText)) return;

            PNH_TabletData.m_pName = c_pName; PNH_TabletData.m_patenteNome = c_patenteNome;
            PNH_TabletData.m_xpAtual = c_xpAtual; PNH_TabletData.m_xpProximo = c_xpProximo;
            PNH_TabletData.m_patenteProxima = c_patenteProxima; PNH_TabletData.m_missaoDisponivel = c_missaoDisponivel;
            PNH_TabletData.m_alvoAtual = c_alvoAtual; PNH_TabletData.m_intelText = c_intelText;
            
            PNH_TabletData.m_NovaSincronizacao = true;
        }
    }
}