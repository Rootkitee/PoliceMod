#include <engine/shared/config.h>
#include <engine/console.h>
#include "gamecontext.h"

void CGameContext::RegisterExtraCmds()
{
    Console()->Register("sayby", "ir", CFGFLAG_SERVER, ConSayBy, this, "Say something by someone");
    Console()->Register("change_name", "ir", CFGFLAG_SERVER, ConChangeName, this, "Change the name of the player");
    Console()->Register("change_clan", "ir", CFGFLAG_SERVER, ConChangeClan, this, "Change the clan of the player");
    Console()->Register("set_hammer", "ii", CFGFLAG_SERVER, ConSetHammerPower, this, "Set the hammer power of someone");
}

void CGameContext::ConSayBy(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	int ClientID = pResult->GetInteger(0);
	const char* pMsg = pResult->GetString(1);

    if(ClientID >= 0 && ClientID < MAX_CLIENTS && !pSelf->m_apPlayers[ClientID])
    {
        pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "sayby", "Invalid client ID.");
        return;
    }

    if(ClientID < 0 || ClientID >= MAX_CLIENTS)
    {
        for(int i = 0; i < MAX_CLIENTS; i ++)
        {
            if(!pSelf->m_apPlayers[i])
                continue;
            if(pSelf->Server()->IsAdmin(i))
                continue;

            pSelf->SendChat(i, CGameContext::CHAT_ALL, pMsg);
        }
        return;
    }

	pSelf->SendChat(ClientID, CGameContext::CHAT_ALL, pMsg);
}

void CGameContext::ConChangeName(IConsole::IResult *pResult, void *pUserData)
{
    CGameContext *pSelf = (CGameContext *)pUserData;
	int ClientID = pResult->GetInteger(0);
	const char* pMsg = pResult->GetString(1);
	char aChatText[256];
	char aOldNamePl[MAX_NAME_LENGTH];

    str_copy(aOldNamePl, pSelf->Server()->ClientName(ClientID), sizeof(aOldNamePl));

    if(ClientID >= 0 && ClientID < MAX_CLIENTS && !pSelf->m_apPlayers[ClientID])
    {
        pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "change_name", "Invalid client ID.");
        return;
    }

    if(ClientID < 0 || ClientID >= MAX_CLIENTS)
    {
        for(int i = 0; i < MAX_CLIENTS; i ++)
        {
            if(!pSelf->m_apPlayers[i])
                continue;
            if(pSelf->Server()->IsAdmin(i))
                continue;

            pSelf->Server()->SetClientName(i, pMsg);
        }

        str_format(aChatText, sizeof(aChatText), "An admin changed all names to '%s'", pMsg);
        pSelf->SendChat(-1, CGameContext::CHAT_ALL, aChatText);
        return;
    }

	pSelf->Server()->SetClientName(ClientID, pMsg);
	str_format(aChatText, sizeof(aChatText), "An admin changed the name of '%s' to '%s'", aOldNamePl, pMsg);
	pSelf->SendChat(-1, CGameContext::CHAT_ALL, aChatText);
}

void CGameContext::ConChangeClan(IConsole::IResult *pResult, void *pUserData)
{
    CGameContext *pSelf = (CGameContext *)pUserData;
	int ClientID = pResult->GetInteger(0);
	const char* pMsg = pResult->GetString(1);
	char aChatText[256];
	char ClientName[MAX_NAME_LENGTH];
	char aOldClanPl[MAX_NAME_LENGTH];

    str_copy(ClientName, pSelf->Server()->ClientName(ClientID), sizeof(ClientName));
    str_copy(aOldClanPl, pSelf->Server()->ClientClan(ClientID), sizeof(aOldClanPl));

    if(ClientID >= 0 && ClientID < MAX_CLIENTS && !pSelf->m_apPlayers[ClientID])
    {
        pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "change_clan", "Invalid client ID.");
        return;
    }

    if(ClientID < 0 || ClientID >= MAX_CLIENTS)
    {
        for(int i = 0; i < MAX_CLIENTS; i ++)
        {
            if(!pSelf->m_apPlayers[i])
                continue;
            if(pSelf->Server()->IsAdmin(i))
                continue;

            pSelf->Server()->SetClientClan(i, pMsg);
        }

        str_format(aChatText, sizeof(aChatText), "An admin changed all clans to '%s'", pMsg);
        pSelf->SendChat(-1, CGameContext::CHAT_ALL, aChatText);
        return;
    }

	pSelf->Server()->SetClientClan(ClientID, pMsg);
	str_format(aChatText, sizeof(aChatText), "An admin changed the clan '%s' of '%s' to '%s'", aOldClanPl, ClientName, pMsg);
	pSelf->SendChat(-1, CGameContext::CHAT_ALL, aChatText);
}

void CGameContext::ConSetHammerPower(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	int ClientID = pResult->GetInteger(0);//-1 = tout le monde
	int HammerPower = clamp(pResult->GetInteger(1), 0, 1000);//0 = ne pousse pas du tout

    if(ClientID >= 0 && ClientID < MAX_CLIENTS && !pSelf->m_apPlayers[ClientID])
    {
        pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "hammer power", "Invalid client ID.");
        return;
    }

	char aBuf[256];
	str_format(aBuf, sizeof(aBuf), "Hammer power set to %d", HammerPower);
	pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "hammer power", aBuf);

    if(ClientID < 0 || ClientID >= MAX_CLIENTS)
    {
        for(int i = 0; i < MAX_CLIENTS; i ++)
        {
            if(!pSelf->m_apPlayers[i])
                continue;
            pSelf->m_apPlayers[i]->m_HammerPower = HammerPower;
        }
        return;
    }

    pSelf->m_apPlayers[ClientID]->m_HammerPower = HammerPower;
}
