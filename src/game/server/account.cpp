#define _HAS_EXCEPTIONS 0
#include <string.h>
#include <fstream>
#include <engine/config.h>
#include "account.h"
#include "gamecontext.h"
#include <cstdlib>
#include <engine/shared/config.h>

#if defined(CONF_FAMILY_UNIX)
	#include <fcntl.h>
	#include <unistd.h>
#endif

CAccount::CAccount(CPlayer *pPlayer, CGameContext *pGameServer)
{
   m_pPlayer = pPlayer;
   m_pGameServer = pGameServer;
}

void CAccount::RemoveLastChar(char* pString)
{
    str_format(pString, strlen(pString), pString);
}

void CAccount::Login(const char *pTypedUsername, const char *pTypedPassword)
{
	// check for too long username and password
	if(strlen(pTypedUsername) > 20)
    {
		m_pGameServer->SendChatTarget(m_pPlayer->GetCID(), "The username is too long !");
		return;
    }
	else if(strlen(pTypedPassword) > 20)
    {
		m_pGameServer->SendChatTarget(m_pPlayer->GetCID(), "The password is too long !");
		return;
    }
#if defined(CONF_FAMILY_WINDOWS)
    if(!str_comp_nocase(pTypedUsername, "con") || !str_comp_nocase(pTypedUsername, "com1") || !str_comp_nocase(pTypedUsername, "com2") ||
        !str_comp_nocase(pTypedUsername, "com3") || !str_comp_nocase(pTypedUsername, "com4") || !str_comp_nocase(pTypedUsername, "com5") ||
        !str_comp_nocase(pTypedUsername, "com6") || !str_comp_nocase(pTypedUsername, "com7") || !str_comp_nocase(pTypedUsername, "com8") ||
        !str_comp_nocase(pTypedUsername, "com9") || !str_comp_nocase(pTypedUsername, "nul") || !str_comp_nocase(pTypedUsername, "prn") ||
        !str_comp_nocase(pTypedUsername, "aux") || !str_comp_nocase(pTypedUsername, "clock$") || !str_comp_nocase(pTypedUsername, "lpt1") ||
        !str_comp_nocase(pTypedUsername, "lpt2") || !str_comp_nocase(pTypedUsername, "lpt3") || !str_comp_nocase(pTypedUsername, "lpt4") ||
        !str_comp_nocase(pTypedUsername, "lpt5") || !str_comp_nocase(pTypedUsername, "lpt6") || !str_comp_nocase(pTypedUsername, "lpt7") ||
        !str_comp_nocase(pTypedUsername, "lpt8") || !str_comp_nocase(pTypedUsername, "lpt9"))
	{
	    m_pGameServer->SendChatTarget(m_pPlayer->GetCID(), "The password is wrong");
	    return;
	}
#endif
	// check for existing accounts
	if(!Exists(pTypedUsername))
	{
		dbg_msg("account", "Account '%s' doesn't exist", pTypedUsername);
		m_pGameServer->SendChatTarget(m_pPlayer->GetCID(), "This account doesn't exist");
		m_pGameServer->SendChatTarget(m_pPlayer->GetCID(), "Please register first. (/register <username> <password>)");
		m_pGameServer->SendChatTarget(m_pPlayer->GetCID(), "(without adding the '<' and '>')");
		return;
	}

	char aBuffer[1024*2];
	str_format(aBuffer, sizeof(aBuffer), "accounts/%s.acc", pTypedUsername);

	char aDatas[DATA_NUM][64];

	// load data
	FILE* AccountFile;
	AccountFile = fopen(aBuffer, "r");
	for(int i = 0; i < DATA_NUM; i++)
        fgets(aDatas[i], sizeof(aDatas[i]), AccountFile);
	fclose (AccountFile);
	RemoveLastChar(aDatas[USERNAME]);
	RemoveLastChar(aDatas[PASSWORD]);

	// check for correct password and name
	if(strcmp(aDatas[USERNAME], pTypedUsername))
	{
		dbg_msg("account", "Account '%s' is not logged in due to wrong username", pTypedUsername);
		m_pGameServer->SendChatTarget(m_pPlayer->GetCID(), "The username is wrong");
		return;
	}
	if(strcmp(aDatas[PASSWORD], pTypedPassword))
	{
		dbg_msg("account", "Account '%s' is not logged in due to wrong password", pTypedUsername);
		m_pGameServer->SendChatTarget(m_pPlayer->GetCID(), "The password is wrong");
		return;
	}

	// check if someone is already logged in
	if(IsLoggedIn(pTypedUsername))
	{
		dbg_msg("account", "Account '%s' already logged in", pTypedUsername);
		m_pGameServer->SendChatTarget(m_pPlayer->GetCID(), "This account is already logged in !");
		return;
	}
	else if(LoggedInOtherServer(pTypedUsername))
    {
        if(!m_pGameServer->CheckLoggedAccount(pTypedUsername))
        {
            int Num = LoggedInOtherServer(pTypedUsername);
            if(Num == 0)
            {
                // do nothing
            }
            else
            {
                char aBuf[256];
                dbg_msg("account", "Account '%s' already logged in an other server", pTypedUsername);
                str_format(aBuf, sizeof(aBuf), "This account is already logged in, in the server #%d", Num);
                m_pGameServer->SendChatTarget(m_pPlayer->GetCID(), aBuf);
                m_pGameServer->SendChatTarget(m_pPlayer->GetCID(), "If you think that's a bug, go to that server and try to login");
                m_pGameServer->SendChatTarget(m_pPlayer->GetCID(), "If it still doesn't work, tell it to an admin");
                return;
            }
        }
    }

	// login
	for(int i = 0; i < DATA_NUM; i++)
    {
        if(i == USERNAME || i == PASSWORD || i == PLAYERNAME || i == IP)
        {
            if(i == USERNAME)
                str_copy(m_pPlayer->m_AccountData.m_Username, aDatas[i], sizeof(aDatas[i]));
            else if(i == PASSWORD)
                str_copy(m_pPlayer->m_AccountData.m_Password, aDatas[i], sizeof(aDatas[i]));
            else if(i == PLAYERNAME)
                str_copy(m_pPlayer->m_AccountData.m_PlayerName, m_pGameServer->Server()->ClientName(m_pPlayer->GetCID()), sizeof(aDatas[i]));
            else if(i == IP)
                str_copy(m_pPlayer->m_AccountData.m_IP, aDatas[i], sizeof(aDatas[i]));
        }
        else
            m_pPlayer->m_AccountData.m_aDatas[i] = m_pGameServer->CharToInt(aDatas[i]);
    }


	m_pPlayer->m_AccountData.m_IsLoggedIn = true;
	AddLoggedAccount(pTypedUsername);
	str_format(m_pGameServer->m_ConnectedAccounts[m_pPlayer->GetCID()], sizeof(m_pGameServer->m_ConnectedAccounts[m_pPlayer->GetCID()]), "%s", pTypedUsername);

	dbg_msg("account", "Account '%s' logged in successfully", pTypedUsername);
	m_pGameServer->SendChatTarget(m_pPlayer->GetCID(), "You are now logged in.");
	m_pPlayer->SetTeam(m_pGameServer->m_pController->GetAutoTeam(m_pPlayer->GetCID()));
	m_pGameServer->Server()->SetClientAccountName(m_pPlayer->GetCID(), pTypedUsername);
}

bool CAccount::CharsValid(const char *pStr, const char *pValidChars)
{
	int Len = str_length(pValidChars);

	while(*pStr)
	{
		bool Found = false;
		for(int i = 0; i < Len; i++)
		{
			if(pValidChars[i] == *pStr)
			{
				Found = true;
				break;
			}
		}

		if(!Found)
			return false; // We found an invalid character

		pStr++;
	}

	return true; // Characters are valid
}

void CAccount::Create(const char *pUsername, const char *pPassword)
{
	// check if allready created an account
	if(m_pPlayer->m_AccountData.m_IsLoggedIn)
    {
		m_pGameServer->SendChatTarget(m_pPlayer->GetCID(), "You have already created an account !");
		return;
    }

	// check for too long username and password
	if(strlen(pUsername) > 20)
    {
		m_pGameServer->SendChatTarget(m_pPlayer->GetCID(), "The username is too long !");
		return;
    }
	else if(strlen(pPassword) > 20)
    {
		m_pGameServer->SendChatTarget(m_pPlayer->GetCID(), "The password is too long !");
		return;
    }
	// check for existing accounts
	if(Exists(pUsername))
	{
		dbg_msg("account", "Account '%s' already exists", pUsername);
		m_pGameServer->SendChatTarget(m_pPlayer->GetCID(), "This account already exists !");
		return;
	}

    const char *pValidChars = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz.-_";

    if(!CharsValid(pUsername, pValidChars))
    {
        m_pGameServer->SendChatTarget(m_pPlayer->GetCID(), "Don't use invalid characters for the username !");
        return;
    }

	if(fs_makedir("accounts"))
        dbg_msg("account.cpp", "Failed to create accounts folder (line %d)", __LINE__);

#if defined(CONF_FAMILY_WINDOWS)
	if(!str_comp_nocase(pUsername, "con") || !str_comp_nocase(pUsername, "com1") || !str_comp_nocase(pUsername, "com2") ||
        !str_comp_nocase(pUsername, "com3") || !str_comp_nocase(pUsername, "com4") || !str_comp_nocase(pUsername, "com5") ||
        !str_comp_nocase(pUsername, "com6") || !str_comp_nocase(pUsername, "com7") || !str_comp_nocase(pUsername, "com8") ||
        !str_comp_nocase(pUsername, "com9") || !str_comp_nocase(pUsername, "nul") || !str_comp_nocase(pUsername, "prn") ||
        !str_comp_nocase(pUsername, "aux") || !str_comp_nocase(pUsername, "clock$") || !str_comp_nocase(pUsername, "lpt1") ||
        !str_comp_nocase(pUsername, "lpt2") || !str_comp_nocase(pUsername, "lpt3") || !str_comp_nocase(pUsername, "lpt4") ||
        !str_comp_nocase(pUsername, "lpt5") || !str_comp_nocase(pUsername, "lpt6") || !str_comp_nocase(pUsername, "lpt7") ||
        !str_comp_nocase(pUsername, "lpt8") || !str_comp_nocase(pUsername, "lpt9"))
	{
		m_pGameServer->SendChatTarget(m_pPlayer->GetCID(), "This account already exists !");
		return;
	}
#endif

	// create account
	char aIP[NETADDR_MAXSTRSIZE] = {0};
	m_pGameServer->Server()->GetClientAddr(m_pPlayer->GetCID(), aIP, sizeof(aIP));
	char aBuffer[1024*2], aData[64];
	str_format(aBuffer, sizeof(aBuffer), "accounts/%s.acc", pUsername);
	FILE * AccountFile = fopen(aBuffer, "a+");
	//
	//FILE CONTENT
	str_format(aBuffer, sizeof(aBuffer), "");
	str_format(aData, sizeof(aData), "");
	for(int i = 0; i < DATA_NUM; i ++)
    {
        if(i == DATA_NUM - 1)
        {
            if(i == USERNAME || i == PASSWORD || i == PLAYERNAME || i == IP)
            {
                if(i == USERNAME)
                    str_format(aData, sizeof(aData), "%s", pUsername);
                else if(i == PASSWORD)
                    str_format(aData, sizeof(aData), "%s", pPassword);
                else if(i == PLAYERNAME)
                    str_format(aData, sizeof(aData), "%s", m_pGameServer->Server()->ClientName(m_pPlayer->GetCID()));
                else if(i == IP)
                    str_format(aData, sizeof(aData), "%s", aIP);
            }
            else
                str_format(aData, sizeof(aData), "0");
        }
        else
        {
            if(i == USERNAME || i == PASSWORD || i == PLAYERNAME || i == IP)
            {
                if(i == USERNAME)
                    str_format(aData, sizeof(aData), "%s\n", pUsername);
                else if(i == PASSWORD)
                    str_format(aData, sizeof(aData), "%s\n", pPassword);
                else if(i == PLAYERNAME)
                    str_format(aData, sizeof(aData), "%s\n", m_pGameServer->Server()->ClientName(m_pPlayer->GetCID()));
                else if(i == IP)
                    str_format(aData, sizeof(aData), "%s\n", aIP);
            }
            else
                str_format(aData, sizeof(aData), "0\n");
        }
        str_append(aBuffer, aData, sizeof(aBuffer));
    }
	//FILE CONTENT
	//
	fputs(aBuffer, AccountFile);
	fclose(AccountFile);
	dbg_msg("account", "Account '%s' was successfully created", pUsername);
	m_pGameServer->SendChatTarget(m_pPlayer->GetCID(), "Account was created successfully.");
	m_pGameServer->SendChatTarget(m_pPlayer->GetCID(), "You can login now. (/login <username> <password>)");
	Login(pUsername, pPassword);
}

bool CAccount::Exists(const char *pUsername)
{
	char aBuffer[512];
	str_format(aBuffer, sizeof(aBuffer), "accounts/%s.acc", pUsername);
    if (FILE * AccountFile = fopen(aBuffer, "r"))
    {
        fclose(AccountFile);
        return true;
    }
    return false;
}

bool CAccount::IsLoggedIn(const char *pUsername)
{
	for(int i = 0; i < MAX_CLIENTS; i++)
	{
		if(m_pGameServer->m_apPlayers[i] && !strcmp(pUsername, m_pGameServer->m_apPlayers[i]->m_AccountData.m_Username))
			return true;
	}
	return false;
}

int CAccount::LoggedInOtherServer(const char *pUsername)
{
	char aBuffer[256];
	if(fs_makedir("LoggedAccounts"))
        dbg_msg("account.cpp", "Failed to create LoggedAccounts folder");

	str_format(aBuffer, sizeof(aBuffer), "LoggedAccounts/%s.acc", pUsername);
    if(FILE * AccountFile = fopen(aBuffer, "r"))
    {
        char aNum[64];
        fscanf(AccountFile, "%s", aNum);
        int ServerNum = atoi(aNum);
        fclose(AccountFile);
        return ServerNum;
    }
    return 0;
}

void CAccount::AddLoggedAccount(const char *pUsername)
{
    char aBuffer[256];
	if(fs_makedir("LoggedAccounts"))
        dbg_msg("account.cpp", "Failed to create LoggedAccounts folder");

    str_format(aBuffer, sizeof(aBuffer), "LoggedAccounts/%s.acc", pUsername);

    FILE * AccountFile = fopen (aBuffer,"a+");
	str_format(aBuffer, sizeof(aBuffer), "%d", g_Config.m_SvServerNum);
	fputs(aBuffer, AccountFile);
	fclose(AccountFile);
}

int CAccount::WhoIs(const char *pUsername)
{
	for(int i = 0; i < MAX_CLIENTS; i++)
	{
		if(m_pGameServer->m_apPlayers[i] && !strcmp(pUsername, m_pGameServer->m_apPlayers[i]->m_AccountData.m_Username))
			return i;
	}
	return -1;
}

void CAccount::Update()
{
	char aBuffer[1024*2], aData[64], aPath[128];
	char aIP[NETADDR_MAXSTRSIZE] = {0};
	m_pGameServer->Server()->GetClientAddr(m_pPlayer->GetCID(), aIP, sizeof(aIP));
	//
	//FILE CONTENT
	str_format(aBuffer, sizeof(aBuffer), "");
	str_format(aData, sizeof(aData), "");
	str_format(m_pPlayer->m_AccountData.m_PlayerName, sizeof(m_pPlayer->m_AccountData.m_PlayerName), m_pGameServer->Server()->ClientName(m_pPlayer->GetCID()));
	for(int i = 0; i < DATA_NUM; i ++)
    {
        if(i == DATA_NUM - 1)
        {
            if(i == USERNAME || i == PASSWORD || i == PLAYERNAME || i == IP)
            {
                if(i == USERNAME)
                    str_format(aData, sizeof(aData), "%s", m_pPlayer->m_AccountData.m_Username);
                else if(i == PASSWORD)
                    str_format(aData, sizeof(aData), "%s", m_pPlayer->m_AccountData.m_Password);
                else if(i == PLAYERNAME)
                    str_format(aData, sizeof(aData), "%s", m_pGameServer->Server()->ClientName(m_pPlayer->GetCID()));
                else if(i == IP)
                    str_format(aData, sizeof(aData), "%s", aIP);
            }
            else
                str_format(aData, sizeof(aData), "%d", m_pPlayer->m_AccountData.m_aDatas[i]);
        }
        else
        {
            if(i == USERNAME || i == PASSWORD || i == PLAYERNAME || i == IP)
            {
                if(i == USERNAME)
                    str_format(aData, sizeof(aData), "%s\n", m_pPlayer->m_AccountData.m_Username);
                else if(i == PASSWORD)
                    str_format(aData, sizeof(aData), "%s\n", m_pPlayer->m_AccountData.m_Password);
                else if(i == PLAYERNAME)
                    str_format(aData, sizeof(aData), "%s\n", m_pGameServer->Server()->ClientName(m_pPlayer->GetCID()));
                else if(i == IP)
                    str_format(aData, sizeof(aData), "%s\n", aIP);
            }
            else
                str_format(aData, sizeof(aData), "%d\n", m_pPlayer->m_AccountData.m_aDatas[i]);
        }
        str_append(aBuffer, aData, sizeof(aBuffer));
    }
	str_format(aPath, sizeof(aPath), "accounts/%s.acc", m_pPlayer->m_AccountData.m_Username);
	std::remove(aPath);
	FILE * AccountFile = fopen(aPath, "a+");
	fputs(aBuffer, AccountFile);
	fclose(AccountFile);
}

void CAccount::Reset()
{
	str_copy(m_pPlayer->m_AccountData.m_Username, "", 64);
	str_copy(m_pPlayer->m_AccountData.m_Password, "", 64);
	for(int i = 0; i < DATA_NUM; i ++)
    {
        if(i == USERNAME || i == PASSWORD || i == PLAYERNAME || i == IP)
            continue;
        else
            m_pPlayer->m_AccountData.m_aDatas[i] = 0;
    }
}

void CAccount::ChangePassword(const char *pNewPassword)
{
	// check if new password too long
	if(strlen(pNewPassword) > 20)
    {
		m_pGameServer->SendChatTarget(m_pPlayer->GetCID(), "The password is too long !");
		return;
    }

	// Update pass
	char aBuf[128];
	str_format(aBuf, sizeof(aBuf), "Password successfully changed from \"%s\" to \"%s\" !", m_pPlayer->m_AccountData.m_Password, pNewPassword);
	str_copy(m_pPlayer->m_AccountData.m_Password, pNewPassword, 32);
	Update();

	// dbg msg
	char aBuffer[512];
	str_format(aBuffer, sizeof(aBuffer), "'%s''s password changed to '%s'!", m_pPlayer->m_AccountData.m_Username, m_pPlayer->m_AccountData.m_Password);
	dbg_msg("account", aBuffer);
	m_pGameServer->SendChatTarget(m_pPlayer->GetCID(), aBuf);
}
