/// --- Documentação PNH_Core: PNH_TabletClient.c ---
/// Versão do Sistema: 3.0.0
/// Função do arquivo: Gerir a interface gráfica e processar a Tecla P e a Tecla ESC.
/// Comunicação com outros arquivos: Lê dados do PNH_TabletData (4_World) e carrega o layout da pasta GUI.

// ==========================================
// 1. A CLASSE DA INTERFACE GRÁFICA (UI)
// ==========================================
class PNH_TabletUI extends UIScriptedMenu
{
    private Widget m_PanelTab1, m_PanelTab2, m_PanelTab3, m_PanelTab4;
    private ButtonWidget m_BtnTab1, m_BtnTab2, m_BtnTab3, m_BtnTab4;
    private RichTextWidget m_TxtInfoTab1, m_TxtInfoTab2, m_TxtInfoTab3;
    private TextWidget m_TxtTab1Title, m_TxtTab2Title, m_TxtTab3Title;

    override Widget Init()
    {
        layoutRoot = GetGame().GetWorkspace().CreateWidgets("PNH_Core/GUI/InfoPanel1.layout");

        m_PanelTab1 = layoutRoot.FindAnyWidget("Panel_Tab1");
        m_PanelTab2 = layoutRoot.FindAnyWidget("Panel_Tab2");
        m_PanelTab3 = layoutRoot.FindAnyWidget("Panel_Tab3");
        m_PanelTab4 = layoutRoot.FindAnyWidget("Panel_Tab4");

        m_BtnTab1 = ButtonWidget.Cast(layoutRoot.FindAnyWidget("btn_Tab1"));
        m_BtnTab2 = ButtonWidget.Cast(layoutRoot.FindAnyWidget("btn_Tab2"));
        m_BtnTab3 = ButtonWidget.Cast(layoutRoot.FindAnyWidget("btn_Tab3"));

        m_TxtInfoTab1 = RichTextWidget.Cast(layoutRoot.FindAnyWidget("txt_Info_Tab1"));
        m_TxtInfoTab2 = RichTextWidget.Cast(layoutRoot.FindAnyWidget("txt_Info_Tab2"));
        m_TxtInfoTab3 = RichTextWidget.Cast(layoutRoot.FindAnyWidget("txt_Info_Tab3"));
        
        m_TxtTab1Title = TextWidget.Cast(layoutRoot.FindAnyWidget("txt_Tab1"));
        m_TxtTab2Title = TextWidget.Cast(layoutRoot.FindAnyWidget("txt_Tab2"));
        m_TxtTab3Title = TextWidget.Cast(layoutRoot.FindAnyWidget("txt_Tab3"));

        if (m_PanelTab4) m_PanelTab4.Show(false);
        ShowTab(1);

        return layoutRoot;
    }

    override void OnShow()
    {
        super.OnShow();
        GetGame().GetUIManager().ShowCursor(true);
        GetGame().GetInput().ChangeGameFocus(1);

        PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
        if (player)
        {
            ScriptRPC rpc = new ScriptRPC();
            rpc.Send(player, PNH_RPCs.RPC_TABLET_OPEN_REQUEST, true, null);
            
            if (m_TxtInfoTab1) m_TxtInfoTab1.SetText("Sincronizando com a base de dados PNH...");
            if (m_TxtInfoTab2) m_TxtInfoTab2.SetText("Buscando contratos na rede...");
            if (m_TxtInfoTab3) m_TxtInfoTab3.SetText("Descriptografando Intel...");
        }
    }

    override void OnHide()
    {
        super.OnHide();
        GetGame().GetUIManager().ShowCursor(false);
        GetGame().GetInput().ResetGameFocus();
    }

    // CORREÇÃO: O nome correto para menus é OnKeyDown e deve retornar um booleano (bool)
    override bool OnKeyDown(Widget w, int x, int y, int key)
    {
        super.OnKeyDown(w, x, y, key);

        if (key == KeyCode.KC_ESCAPE)
        {
            GetGame().GetUIManager().HideScriptedMenu(this);
            return true;
        }
        return false;
    }

    override void Update(float timeslice)
    {
        super.Update(timeslice);
        
        if (PNH_TabletData.m_NovaSincronizacao)
        {
            string v_nome = PNH_TabletData.m_pName;
            string v_patente = PNH_TabletData.m_patenteNome;
            int v_xp = PNH_TabletData.m_xpAtual;
            int v_proxXp = PNH_TabletData.m_xpProximo;
            string v_proxPat = PNH_TabletData.m_patenteProxima;
            string v_missao = PNH_TabletData.m_missaoDisponivel;
            string v_alvo = PNH_TabletData.m_alvoAtual;
            string v_intel = PNH_TabletData.m_intelText;

            UpdateTabletData(v_nome, v_patente, v_xp, v_proxXp, v_proxPat, v_missao, v_alvo, v_intel);
            
            PNH_TabletData.m_NovaSincronizacao = false;
        }
    }

    override bool OnClick(Widget w, int x, int y, int button)
    {
        if (w == m_BtnTab1) { ShowTab(1); return true; }
        if (w == m_BtnTab2) { ShowTab(2); return true; }
        if (w == m_BtnTab3) { ShowTab(3); return true; }
        return super.OnClick(w, x, y, button);
    }

    void ShowTab(int tabIndex)
    {
        if (m_PanelTab1) m_PanelTab1.Show(tabIndex == 1);
        if (m_PanelTab2) m_PanelTab2.Show(tabIndex == 2);
        if (m_PanelTab3) m_PanelTab3.Show(tabIndex == 3);
    }

    void UpdateTabletData(string pName, string patenteNome, int xpAtual, int xpProximo, string patenteProxima, string missaoDisponivel, string alvoAtual, string intelText)
    {
        if (m_TxtTab1Title) m_TxtTab1Title.SetText("DOSSIÊ DO OPERADOR");
        if (m_TxtInfoTab1)
        {
            string perfil = "<br/><b>NOME DO AGENTE:</b> " + pName + "<br/>";
            perfil += "<b>PATENTE ATUAL:</b> " + patenteNome + "<br/>";
            perfil += "<b>PONTOS DE EXPERIÊNCIA:</b> " + xpAtual + " XP<br/><br/>";
            if (xpProximo > 0) perfil += "<b>PRÓXIMA PROMOÇÃO:</b> " + patenteProxima + " (Faltam " + (xpProximo - xpAtual) + " XP)<br/>";
            else perfil += "<b>PRÓXIMA PROMOÇÃO:</b> PATENTE MÁXIMA ATINGIDA<br/>";
            m_TxtInfoTab1.SetText(perfil);
            Widget scroll1 = layoutRoot.FindAnyWidget("Scroll_Tab1");
            if (scroll1) scroll1.Show(true);
        }

        if (m_TxtTab2Title) m_TxtTab2Title.SetText("REDE DE CONTRATOS");
        if (m_TxtInfoTab2)
        {
            m_TxtInfoTab2.SetText("<br/><b>STATUS GLOBAL DO SATÉLITE:</b><br/>" + missaoDisponivel);
            Widget scroll2 = layoutRoot.FindAnyWidget("Scroll_Tab2");
            if (scroll2) scroll2.Show(true);
        }

        if (m_TxtTab3Title) m_TxtTab3Title.SetText("INTELIGÊNCIA TÁTICA");
        if (m_TxtInfoTab3)
        {
            string intel = "<br/><b>ALVO ATUAL:</b> " + alvoAtual + "<br/><br/>";
            intel += "<b>MENSAGEM DO COMANDO:</b><br/>" + intelText;
            m_TxtInfoTab3.SetText(intel);
            Widget scroll3 = layoutRoot.FindAnyWidget("Scroll_Tab3");
            if (scroll3) scroll3.Show(true);
        }
    }
}

// ==========================================
// 2. O CONTROLO DE TECLAS (Gameplay)
// ==========================================
modded class MissionGameplay
{
    ref PNH_TabletUI m_PNH_TabletMenu;

    override void OnKeyPress(int key)
    {
        super.OnKeyPress(key);

        if (key == KeyCode.KC_P)
        {
            if (GetGame().GetUIManager().GetMenu() == null) 
            {
                if (!m_PNH_TabletMenu) 
                { 
                    m_PNH_TabletMenu = new PNH_TabletUI; 
                }
                GetGame().GetUIManager().ShowScriptedMenu(m_PNH_TabletMenu, NULL);
            }
            else if (GetGame().GetUIManager().GetMenu() == m_PNH_TabletMenu)
            {
                GetGame().GetUIManager().HideScriptedMenu(m_PNH_TabletMenu);
            }
        }
    }
}