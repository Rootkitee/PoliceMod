#ifndef ACCOUNT_H
#define ACCOUNT_H
#include <engine/server.h>

class CAccount
{
public:
	class CPlayer *m_pPlayer;
	class CGameContext *m_pGameServer;
	CAccount(CPlayer *pPlayer, CGameContext *pGameServer);

	void Login(const char *pTypedUsername, const char *pTypedPassword);
	void Create(const char *pUsername, const char *pPassword);
	void Update();
	void Reset();
	void ChangePassword(const char *pNewPassword);
	int LoggedInOtherServer(const char *pUsername);
	void AddLoggedAccount(const char *pUsername);

	bool Exists(const char *pUsername);
	bool IsLoggedIn(const char *pUsername);
	int WhoIs(const char *pUsername);
	bool CharsValid(const char *pStr, const char *pValidChars);

	void RemoveLastChar(char* pString);

};

#endif
