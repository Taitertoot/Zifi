/*
 * This file is part of the BlizzLikeCore Project.
 * See CREDITS and LICENSE files for Copyright information.
 */

/* ScriptData
Name: Razorfen_Downs
Complete(%): 100
Comment: Support for Henry Stern(2 recipes)
Category: Razorfen Downs
EndScriptData */

/* ContentData
npc_henry_stern
EndContentData */

#include "ScriptPCH.h"
#include "razorfen_downs.h"

/*###
# npc_henry_stern
####*/

enum eEnums
{
    SPELL_GOLDTHORN_TEA                         = 13028,
    SPELL_TEACHING_GOLDTHORN_TEA                = 13029,
    SPELL_MIGHT_TROLLS_BLOOD_POTION             = 3451,
    SPELL_TEACHING_MIGHTY_TROLLS_BLOOD_POTION   = 13030,
    GOSSIP_TEXT_TEA_ANSWER                      = 2114,
    GOSSIP_TEXT_POTION_ANSWER                   = 2115,
};

#define GOSSIP_ITEM_TEA     "Teach me the cooking recipe"
#define GOSSIP_ITEM_POTION  "Teach me the alchemy recipe"

bool GossipHello_npc_henry_stern (Player* pPlayer, Creature* pCreature)
{
    if (pPlayer->GetBaseSkillValue(SKILL_COOKING) >= 175 && !pPlayer->HasSpell(SPELL_GOLDTHORN_TEA))
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_TEA, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

    if (pPlayer->GetBaseSkillValue(SKILL_ALCHEMY) >= 180 && !pPlayer->HasSpell(SPELL_MIGHT_TROLLS_BLOOD_POTION))
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_POTION, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);

    pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetGUID());
    return true;
}

bool GossipSelect_npc_henry_stern (Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_INFO_DEF + 1)
    {
        pPlayer->CastSpell(pPlayer, SPELL_TEACHING_GOLDTHORN_TEA, true);
        pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXT_TEA_ANSWER, pCreature->GetGUID());
    }

    if (uiAction == GOSSIP_ACTION_INFO_DEF + 2)
    {
        pPlayer->CastSpell(pPlayer, SPELL_TEACHING_MIGHTY_TROLLS_BLOOD_POTION, true);
        pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXT_POTION_ANSWER, pCreature->GetGUID());
    }

    return true;
}

/*######
## go_gong
######*/

bool GOHello_go_gong(Player* /*pPlayer*/, GameObject* pGO)
{
    //basic support, not blizzlike data is missing...
    ScriptedInstance* pInstance = pGO->GetInstanceData();

    if (pInstance)
    {
        pInstance->SetData(DATA_GONG_WAVES,pInstance->GetData(DATA_GONG_WAVES)+1);
        return true;
    }

    return false;
}

enum eTombCreature
{
    SPELL_WEB                   = 745
};

struct npc_tomb_creatureAI : public ScriptedAI
{
    npc_tomb_creatureAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        pInstance = pCreature->GetInstanceData();
    }

    ScriptedInstance* pInstance;

    uint32 uiWebTimer;

    void Reset()
    {
        uiWebTimer = urand(5000,8000);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!UpdateVictim())
            return;

        //from acid
        if (me->GetEntry() == CREATURE_TOMB_REAVER)
        {
            if (uiWebTimer <= uiDiff)
            {
                DoCast(me->getVictim(), SPELL_WEB);
                uiWebTimer = urand(7000,16000);
            } else uiWebTimer -= uiDiff;
        }

        DoMeleeAttackIfReady();
    }

    void JustDied(Unit* /*pKiller*/)
    {
        if (pInstance)
            pInstance->SetData(DATA_GONG_WAVES,pInstance->GetData(DATA_GONG_WAVES)+1);
    }
};

CreatureAI* GetAI_npc_tomb_creature(Creature* pCreature)
{
    return new npc_tomb_creatureAI (pCreature);
}

void AddSC_razorfen_downs()
{
    Script* newscript;

    newscript = new Script;
    newscript->Name = "npc_henry_stern";
    newscript->pGossipHello = &GossipHello_npc_henry_stern;
    newscript->pGossipSelect = &GossipSelect_npc_henry_stern;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "go_gong";
    newscript->pGOHello =           &GOHello_go_gong;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_tomb_creature";
    newscript->GetAI = &GetAI_npc_tomb_creature;
    newscript->RegisterSelf();
}
