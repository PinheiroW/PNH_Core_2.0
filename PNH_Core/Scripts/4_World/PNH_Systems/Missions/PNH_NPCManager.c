class PNH_NPCManager
{
    static ref PNH_NPCManager m_Instance;
    ref array<PlayerBase> m_SpawnedNPCs;

    void PNH_NPCManager()
    {
        m_SpawnedNPCs = new array<PlayerBase>;
    }

    static PNH_NPCManager GetInstance()
    {
        if (!m_Instance) {
            m_Instance = new PNH_NPCManager();
        }
        return m_Instance;
    }

    void SpawnAllNPCs()
    {
        // 1. Limpa NPCs antigos (Evita a criação de "Clones" se o Admin usar !reload_mission)
        CleanupNPCs();

        PNH_MissionSettingsData config = PNH_MissionSettings.GetData();
        if (!config || !config.NPCsQuestGivers) return;

        // 2. Faz o Spawn Limpo lendo o JSON
        // CORREÇÃO: Atualizado para o novo nome da classe (PNH_NPCQuestGiver)
        foreach (PNH_NPCQuestGiver npcData : config.NPCsQuestGivers)
        {
            PlayerBase npc = PlayerBase.Cast(GetGame().CreateObject("SurvivorM_Mirek", npcData.Posicao.ToVector(), false, false, true));
            if (npc) 
            {
                npc.SetPosition(npcData.Posicao.ToVector()); 
                npc.SetOrientation(npcData.Orientacao.ToVector());
                
                // Proteção: Só tenta equipar roupas se a lista existir no JSON
                if (npcData.Roupas && npcData.Roupas.Count() > 0)
                {
                    foreach (string item : npcData.Roupas) { 
                        npc.GetInventory().CreateInInventory(item); 
                    }
                }
                
                npc.SetAllowDamage(false); // Torna o NPC Imortal
                m_SpawnedNPCs.Insert(npc);
            }
        }
        
        PNH_Logger.Log("NPCs", "[PNH_CORE] Agente PNH_NPCManager assumiu o controlo. Oficiais a postos (" + m_SpawnedNPCs.Count() + ").");
    }

    void CleanupNPCs()
    {
        foreach (PlayerBase npc : m_SpawnedNPCs)
        {
            if (npc) GetGame().ObjectDelete(npc);
        }
        m_SpawnedNPCs.Clear();
    }
}