/*
 * This file is part of the BlizzLikeCore Project.
 * See CREDITS and LICENSE files for Copyright information.
 */

/* ScriptData
Name: Magisters_Terrace
Complete(%): 100
Comment: Quest support: 11490(post-event)
Category: Magisters Terrace
EndScriptData */

/* ContentData
npc_kalecgos
EndContentData */

#include "ScriptPCH.h"

/*######
## npc_kalecgos
######*/

enum eEnums
{
    SPELL_TRANSFORM_TO_KAEL     = 44670,
    SPELL_ORB_KILL_CREDIT       = 46307,
    NPC_KAEL                    = 24848,                    //human form entry
    POINT_ID_LAND               = 1
};

const float afKaelLandPoint[] = {225.045, -276.236, -5.434};

#define GOSSIP_ITEM_KAEL_1      "Who are you?"
#define GOSSIP_ITEM_KAEL_2      "What can we do to assist you?"
#define GOSSIP_ITEM_KAEL_3      "What brings you to the Sunwell?"
#define GOSSIP_ITEM_KAEL_4      "You're not alone here?"
#define GOSSIP_ITEM_KAEL_5      "What would Kil'jaeden want with a mortal woman?"

// This is friendly keal that appear after used Orb.
// If we assume DB handle summon, summon appear somewhere outside the platform where Orb is
struct npc_kalecgosAI : public ScriptedAI
{
    npc_kalecgosAI(Creature* pCreature) : ScriptedAI(pCreature) {}

    uint32 m_uiTransformTimer;

    void Reset()
    {
        m_uiTransformTimer = 0;

        // we must assume he appear as dragon somewhere outside the platform of orb, and then move directly to here
        if (me->GetEntry() != NPC_KAEL)
            me->GetMotionMaster()->MovePoint(POINT_ID_LAND, afKaelLandPoint[0], afKaelLandPoint[1], afKaelLandPoint[2]);
    }

    void MovementInform(uint32 uiType, uint32 uiPointId)
    {
        if (uiType != POINT_MOTION_TYPE)
            return;

        if (uiPointId == POINT_ID_LAND)
            m_uiTransformTimer = MINUTE*IN_MILLISECONDS;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_uiTransformTimer)
        {
            if (m_uiTransformTimer <= uiDiff)
            {
                DoCast(me, SPELL_ORB_KILL_CREDIT, false);

                // Transform and update entry, now ready for quest/read gossip
                DoCast(me, SPELL_TRANSFORM_TO_KAEL, false);
                me->UpdateEntry(NPC_KAEL);

                m_uiTransformTimer = 0;
            } else m_uiTransformTimer -= uiDiff;
        }
    }
};

CreatureAI* GetAI_npc_kalecgos(Creature* pCreature)
{
    return new npc_kalecgosAI(pCreature);
}

bool GossipHello_npc_kalecgos(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetGUID());

    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_KAEL_1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
    pPlayer->SEND_GOSSIP_MENU(12498, pCreature->GetGUID());

    return true;
}

bool GossipSelect_npc_kalecgos(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    switch (uiAction)
    {
        case GOSSIP_ACTION_INFO_DEF:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_KAEL_2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
            pPlayer->SEND_GOSSIP_MENU(12500, pCreature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+1:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_KAEL_3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
            pPlayer->SEND_GOSSIP_MENU(12502, pCreature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+2:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_KAEL_4, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
            pPlayer->SEND_GOSSIP_MENU(12606, pCreature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+3:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_KAEL_5, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);
            pPlayer->SEND_GOSSIP_MENU(12607, pCreature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+4:
            pPlayer->SEND_GOSSIP_MENU(12608, pCreature->GetGUID());
            break;
    }

    return true;
}

void AddSC_magisters_terrace()
{
    Script* newscript;

    newscript = new Script;
    newscript->Name = "npc_kalecgos";
    newscript->GetAI = &GetAI_npc_kalecgos;
    newscript->pGossipHello = &GossipHello_npc_kalecgos;
    newscript->pGossipSelect = &GossipSelect_npc_kalecgos;
    newscript->RegisterSelf();
}
