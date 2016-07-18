#include "gamecontext.h"
#include <stdio.h>

void CGameContext::ChatCmds(const char *pMsg, int ClientID)
{
    pMsg ++;//on ignore le slash

    //bool IsOwner = Server()->IsOwner(ClientID);
    bool IsAdmin = Server()->IsAdmin(ClientID);
    bool IsMod = Server()->IsMod(ClientID);
    //bool IsPremium = Server()->IsPremium(ClientID);
    //bool IsPolice = Server()->IsPolice(ClientID);
    //bool IsTerrorist = Server()->IsTerrorist(ClientID);

    CPlayer *pPlayer = m_apPlayers[ClientID];

    if(!str_comp_nocase_num(pMsg, "login", 5))
	{
        if(pPlayer->m_AccountData.m_IsLoggedIn)
        {
            SendChatTarget(ClientID, "You are already logged in !");
            return;
        }

        char aUsername[512];
        char aPassword[512];
        if(sscanf(pMsg, "login %s %s", aUsername, aPassword) != 2)
        {
            // notify the user that he is stupid
            SendChatTarget(ClientID, "Please stick to the given structure :");
            SendChatTarget(ClientID, "/login <username> <password>");
            return;
        }

        pPlayer->m_pAccount->Login(aUsername, aPassword);
        if(pPlayer->m_AccountData.m_IsLoggedIn)
            pPlayer->m_pAccount->Update();

		return;
	}

    if(!str_comp_nocase(pMsg, "logout"))
	{
		if(!pPlayer->m_AccountData.m_IsLoggedIn)
        {
			SendChatTarget(ClientID, "You are already logged out !");
			return;
        }

		pPlayer->SetTeam(TEAM_SPECTATORS);

		pPlayer->m_pAccount->Update();

		pPlayer->m_pAccount->Reset();
		SendChatTarget(ClientID, "You are now logged out");
		pPlayer->m_AccountData.m_IsLoggedIn = false;
		CheckLoggedAccount();
		Server()->SetClientAccountName(ClientID, "");
		return;
	}

	if(!str_comp_nocase_num(pMsg, "register", 8))
	{
		if(pPlayer->m_AccountData.m_IsLoggedIn)
        {
			SendChatTarget(ClientID, "You are already registered !");
			return;
        }

		char aUsername[512];
		char aPassword[512];
		if(sscanf(pMsg, "register %s %s", aUsername, aPassword) != 2)
		{
			SendChatTarget(ClientID, "Please stick to the given structure :");
			SendChatTarget(ClientID, "/register <username> <password>");
			return;
		}

		pPlayer->m_pAccount->Create(aUsername, aPassword);
		return;
	}

	if(!str_comp_nocase_num(pMsg, "password", 8))
	{
		if(!pPlayer->m_AccountData.m_IsLoggedIn)
        {
			SendChatTarget(ClientID, "You are not logged in !");
			return;
        }

		char aNewPassword[512];

		if(sscanf(pMsg, "password %s", aNewPassword) != 1)
		{
			SendChatTarget(ClientID, "Please stick to the given structure :");
			SendChatTarget(ClientID, "/password <new password>");
			return;
		}

		pPlayer->m_pAccount->ChangePassword(aNewPassword);
		return;
	}

    if(!str_comp_nocase(pMsg, "cmdlist"))
    {
        SendChatTarget(ClientID, "(***(---CMDLIST---)***)");
        SendChatTarget(ClientID, "Type /infos - All informations");
        SendChatTarget(ClientID, "Type /rules - Rules for all players");
        SendChatTarget(ClientID, "Type /rules moderator - Rules for moderators");
        SendChatTarget(ClientID, "Type /rules admin - Rules for admins");
        SendChatTarget(ClientID, "Type /help - If you have problems with the registration");
        SendChatTarget(ClientID, "Type /commands - All commands");
        SendChatTarget(ClientID, "Type /lastupdate - See the last update of the mod");
        return;
    }

    if(!str_comp_nocase(pMsg, "infos")) //cmdlist
    {
        SendChatTarget(ClientID, "(***(---INFOS---)***)");
        SendChatTarget(ClientID, "Type /infos mod - For more informations about the mod");
        SendChatTarget(ClientID, "Type /infos levels - For more informations about the levels");
        SendChatTarget(ClientID, "Type /infos weapons - For more informations about the weapons");
        SendChatTarget(ClientID, "Type /infos civil - For more informations about the civils");
        SendChatTarget(ClientID, "Type /infos mafia - For more informations about the mafia");
        SendChatTarget(ClientID, "Type /infos terrorist - For more informations about the terrorists");
        SendChatTarget(ClientID, "Type /infos police - For more informations about the polices");
        SendChatTarget(ClientID, "Type /infos benefits - For more informations about the benefits");
        return;
    }

    if(!str_comp_nocase(pMsg, "infos mod")) //infos
    {
        SendChatTarget(ClientID, "(***(---INFOS MOD---)***)");
        SendChatTarget(ClientID, "It's a mod civil, mafia, terrorist, police by Fefereta");
        SendChatTarget(ClientID, "Server version : ?.?.? - ??.??.????");
        SendChatTarget(ClientID, "Thx to Neox for helping me with the mod");
        SendChatTarget(ClientID, "Type \"/infos civil\", \"/infos mafia\", \"/infos terrorist\" and \"/infos police\" for more informations");
        return;
    }

	if(!str_comp_nocase(pMsg, "infos levels")) //infos
    {
        SendChatTarget(ClientID, "(***(---INFOS LEVELS---)***)");
        SendChatTarget(ClientID, "Your score shows your current level");
        SendChatTarget(ClientID, "You win 1 level per hour");
        SendChatTarget(ClientID, "When you left the game, your playtime is saved automatically");
        return;
    }

    if(!str_comp_nocase(pMsg, "infos weapons")) //infos
    {
        SendChatTarget(ClientID, "(***(---INFOS WEAPONS---)***)");
        SendChatTarget(ClientID, "All weapons : Infinite ammo");
        SendChatTarget(ClientID, "Hammer : No damage");
        SendChatTarget(ClientID, "Gun : Auto gun, 3 damages per hit, more speed, infinite life");
        SendChatTarget(ClientID, "Shotgun : 5 damages per ammo, long range");
        SendChatTarget(ClientID, "Grenade : Long range, more speed, infinite life");
        SendChatTarget(ClientID, "Laser : 15 damages per ammo");
        SendChatTarget(ClientID, "Ninja : Switch");
        return;
    }

    if(!str_comp_nocase(pMsg, "infos civil")) //infos
    {
        SendChatTarget(ClientID, "(***(---INFOS CIVIL---)***)");
        SendChatTarget(ClientID, "When you spawn, you are a civil");
        SendChatTarget(ClientID, "A civil can kill a terrorist and mafia members without penalty");
        SendChatTarget(ClientID, "If the civil makes a deadly series (only killing civil), the civil is wanted");
        SendChatTarget(ClientID, "If the civil kills a police, the civil is wanted");
        SendChatTarget(ClientID, "If the civil wanted by the police, is killed by the police, the civil will respawn in a jail for a while");
        return;
    }

    if(!str_comp_nocase(pMsg, "infos mafia")) //infos
    {
        SendChatTarget(ClientID, "(***(---INFOS MAFIA---)***)");
        SendChatTarget(ClientID, "If you want to enter in the mafia, type this command :");
        SendChatTarget(ClientID, "\"/mafia\" for enter in the mafia");
        SendChatTarget(ClientID, "They are many mafia base on this map");
        SendChatTarget(ClientID, "These bases could be controlled by anybody");
        SendChatTarget(ClientID, "If the mafia user make a deadly series, the mafia user is wanted");
        SendChatTarget(ClientID, "If the mafia user wanted by the police, is killed by the police, the mafia user respawn in a jail for a while");
        return;
    }
	
	if(!str_comp_nocase(pMsg, "infos terrorist")) //infos
    {
        SendChatTarget(ClientID, "(***(---INFOS TERRORIST---)***)");
        SendChatTarget(ClientID, "If you want to become a terrorist, type this command :");
        SendChatTarget(ClientID, "\"/terrorist\" for become a terrorist");
        SendChatTarget(ClientID, "When you are a terrorist,you can type the command :");
        SendChatTarget(ClientID, "\"/terroristbase\" If you write this, you'll be teleported to the terrorist base");
        SendChatTarget(ClientID, "In the terrorist base, you have the shotgun and the grenade");
        SendChatTarget(ClientID, "If the terrorist make a deadly series, the terrorist is wanted");
        SendChatTarget(ClientID, "If the terrorist wanted by the police, is killed by the police, the terrorist respawn in a jail for a while");
        return;
    }

    if(!str_comp_nocase(pMsg, "infos police")) //infos
    {
        SendChatTarget(ClientID, "(***(---INFOS POLICE---)***)");
        SendChatTarget(ClientID, "You can become a police if :");
        SendChatTarget(ClientID, "You are premium");
        SendChatTarget(ClientID, "You ask to an admin");
        SendChatTarget(ClientID, "If the command is unlock, type :");
        SendChatTarget(ClientID, "\"/police\" to become a police");
        SendChatTarget(ClientID, "When you are a police, you can type the command :");
        SendChatTarget(ClientID, "\"/policebase\" If you write this, you'll be teleported to the police base");
        SendChatTarget(ClientID, "In the police base, you have all weapons");
        SendChatTarget(ClientID, "The police can jail a player if he is wanted");
        return;
    }

    if(!str_comp_nocase(pMsg, "infos benefits")) //infos
    {
        SendChatTarget(ClientID, "(***(---INFOS BENEFITS---)***)");
        SendChatTarget(ClientID, "/premiumbenefits");
        SendChatTarget(ClientID, "/howtobuypremium");
        SendChatTarget(ClientID, "/basebenefits");
        SendChatTarget(ClientID, "/howtobuybase");
        SendChatTarget(ClientID, "/villabenefits");
        SendChatTarget(ClientID, "/howtobuyvilla");
        SendChatTarget(ClientID, "/powermodebenefits");
        SendChatTarget(ClientID, "/howtobuypowermode");
        return;
    }

    if(!str_comp_nocase(pMsg, "premiumbenefits")) //infos benefits
    {
        SendChatTarget(ClientID, "(***(---PREMIUM BENEFITS---)***)");
        SendChatTarget(ClientID, "Premium benefits (Level 168 or 2 euros) :");
        SendChatTarget(ClientID, "1 - A customizable house");
        SendChatTarget(ClientID, "2 - A command to control the door of the house (open/close)");
        SendChatTarget(ClientID, "3 - A command to teleport yourself in your house");
        SendChatTarget(ClientID, "4 - All weapons in the house");
        SendChatTarget(ClientID, "5 - Ninja");
        SendChatTarget(ClientID, "6 - Splash");
        SendChatTarget(ClientID, "7 - HP Regeneration");
        SendChatTarget(ClientID, "8 - Jet pack");
        SendChatTarget(ClientID, "9 - The ability to go into the premium places");
        SendChatTarget(ClientID, "10 - Emotes");
        SendChatTarget(ClientID, "11 - Infinite jumps/hook");
        SendChatTarget(ClientID, "12 - Fly");
        SendChatTarget(ClientID, "13 - Stars");
        SendChatTarget(ClientID, "14 - More hp and ap");
        SendChatTarget(ClientID, "15 - The command \"/police\" is unlocked");
        SendChatTarget(ClientID, "Type \"/howtobuypremium\" to know how to buy the premium");
        SendChatTarget(ClientID, "Press F1 (per default) to open the console and see all premium benefits");
        return;
    }

    if(!str_comp_nocase(pMsg, "howtobuypremium")) //infos benefits
    {
        SendChatTarget(ClientID, "(***(---HOW TO BUY PREMIUM---)***)");
        SendChatTarget(ClientID, "If you are level 168, you become automatically a premium user");
        SendChatTarget(ClientID, "If you want to become a premium user now, donate 2 euros ON PAYPAL");
        SendChatTarget(ClientID, "Donate here : Nicolas1998officiel@outlook.be");
        SendChatTarget(ClientID, "After your donation, ask Fefereta to become a premium user");
        return;
    }
	
	if(!str_comp_nocase(pMsg, "basebenefits")) //infos benefits
    {
        SendChatTarget(ClientID, "(***(---BASE BENEFITS---)***)");
        SendChatTarget(ClientID, "Base benefits (Level 200 or 5 euros) :");
        SendChatTarget(ClientID, "1 - A base for your clan");
        SendChatTarget(ClientID, "2 - Shotgun, grenade, laser in your base");
        SendChatTarget(ClientID, "3 - All players can control the door with the switch (members will have to clean it)");
        SendChatTarget(ClientID, "Type \"howtobuybase\" to know how buy a base");
        return;
    }
	
	if(!str_comp_nocase(pMsg, "howtobuybase")) //infos benefits
    {
        SendChatTarget(ClientID, "(***(---HOW TO BUY BASE---)***)");
        SendChatTarget(ClientID, "If you are level 200, you can have a base");
        SendChatTarget(ClientID, "If you want to have a base now, donate 5 euros ON PAYPAL");
        SendChatTarget(ClientID, "Donate here : Nicolas1998officiel@outlook.be");
        SendChatTarget(ClientID, "After your donation, ask Fefereta to have your base");
        return;
    }
	
	if(!str_comp_nocase(pMsg, "villabenefits")) //infos benefits
    {
        SendChatTarget(ClientID, "(***(---VILLA BENEFITS---)***)");
        SendChatTarget(ClientID, "Villa benefits (Level 300 or 5 euros) :");
        SendChatTarget(ClientID, "1 - More space than the house");
        SendChatTarget(ClientID, "2 - In the city");
        SendChatTarget(ClientID, "3 - You can choose what you want in your villa");
        SendChatTarget(ClientID, "4 - You have the control of the door");
        SendChatTarget(ClientID, "5 - You can invit your friends in your villa");
        SendChatTarget(ClientID, "Type \"howtobuyvilla\" to know how buy a villa");
        return;
    }
	
	if(!str_comp_nocase(pMsg, "howtobuyvilla")) //infos benefits
    {
        SendChatTarget(ClientID, "(***(---HOW TO BUY VILLA---)***)");
        SendChatTarget(ClientID, "If you are level 300, you can have a villa");
        SendChatTarget(ClientID, "If you want to have a villa now, donate 5 euros ON PAYPAL");
        SendChatTarget(ClientID, "Donate here : Nicolas1998officiel@outlook.be");
        SendChatTarget(ClientID, "After your donation, ask Fefereta to have your villa");
        return;
    }
	
	if(!str_comp_nocase(pMsg, "powermodebenefits")) //infos benefits
    {
        SendChatTarget(ClientID, "(***(---POWER MODE BENEFITS---)***)");
        SendChatTarget(ClientID, "Power mode benefits (Level 500 or 10 euros) :");
        SendChatTarget(ClientID, "Type \"howtobuypowermode\" to know how buy a villa");
        return;
    }
	
	if(!str_comp_nocase(pMsg, "howtobuypowermode")) //infos benefits
    {
        SendChatTarget(ClientID, "(***(---HOW TO BUY POWER MODE---)***)");
        SendChatTarget(ClientID, "If you are level 500, you have automatically the power mode");
        SendChatTarget(ClientID, "If you want to have the power mode now, donate 10 euros ON PAYPAL");
        SendChatTarget(ClientID, "Donate here : Nicolas1998officiel@outlook.be");
        SendChatTarget(ClientID, "After your donation, ask Fefereta to have your power mode");
        return;
    }

    if(!str_comp_nocase(pMsg, "rules")) //cmdlist
       {
        SendChatTarget(ClientID, "(***(---RULES---)***)");
        SendChatTarget(ClientID, "Rule 1 : Don't insult !");
        SendChatTarget(ClientID, "Rule 2 : Don't spam !");
        SendChatTarget(ClientID, "Rule 3 : Don't spray !");
        SendChatTarget(ClientID, "Rule 4 : Don't fake people !");
        SendChatTarget(ClientID, "Rule 5 : Don't start funvotes !");
        SendChatTarget(ClientID, "Rule 6 : Don't use bots !");
        SendChatTarget(ClientID, "Rule 7 : Don't abuse bugs !");
        SendChatTarget(ClientID, "Rule 8 : Don't troll !");
        SendChatTarget(ClientID, "Rule 9 : Don't use an anti-afk !");
        SendChatTarget(ClientID, "Rule 10 : Don't camp the spawn for kill a player !");
        SendChatTarget(ClientID, "Don't break any rules or you will get punished, have fun !");
        return;
    }
	
	if(!str_comp_nocase(pMsg, "rules moderator")) //cmdlist
       {
		   if(!IsMod)
        {
            SendChatTarget(ClientID, "You are not a moderator");
            return;
        }
        SendChatTarget(ClientID, "(***(---RULES MODERATOR---)***)");
        SendChatTarget(ClientID, "Rule 1 : You have to help beginners");
        SendChatTarget(ClientID, "Rule 2 : Don't use the moderation commands for fun");
        SendChatTarget(ClientID, "Rule 3 : Take a screenshot when you're using a moderation commands");
        return;
    }
	
	if(!str_comp_nocase(pMsg, "rules admin")) //cmdlist
       {
		   if(!IsAdmin)
        {
            SendChatTarget(ClientID, "You are not an admin");
            return;
        }
        SendChatTarget(ClientID, "(***(---RULES ADMIN---)***)");
        SendChatTarget(ClientID, "Rule 1 : Don't kill a player with the admin commands");
        SendChatTarget(ClientID, "Rule 2 : Don't use the admin rcon commands for fun");
        SendChatTarget(ClientID, "Rule 3 : Don't spam with commands text");
        SendChatTarget(ClientID, "Rule 4 : Don't use all the time the admin commands");
        SendChatTarget(ClientID, "Rule 5 : Don't edit the settings of the server");
        SendChatTarget(ClientID, "Rule 6 : Don't use admin rcon commands and admin commands on a other admin");
        return;
    }

    if(!str_comp_nocase(pMsg, "help")) //cmdlist
    {
        SendChatTarget(ClientID, "(***(---HELP---)***)");
        SendChatTarget(ClientID, "To register, type :");
        SendChatTarget(ClientID, "/register <name> <password>");
        SendChatTarget(ClientID, "Don't lost your account !");
        SendChatTarget(ClientID, "When you register for the first time, you join automatically the game");
        SendChatTarget(ClientID, "If you are already registered, type :");
        SendChatTarget(ClientID, "/login <yourname> <yourpassword>");
        SendChatTarget(ClientID, "If you can't register or login, ask a modo/admin for help");
        SendChatTarget(ClientID, "Please report any bugs to an admin !");
        SendChatTarget(ClientID, "Good game !");
        return;
    }

    if(!str_comp_nocase(pMsg, "commands")) //cmdlist
    {
        SendChatTarget(ClientID, "(***(---COMMANDS---)***)");
        SendChatTarget(ClientID, "/mafiacmds - Commands for mafia user");
        SendChatTarget(ClientID, "/terroristcmds - Commands for terrorist user");
        SendChatTarget(ClientID, "/policecmds - Commands for police user");
        SendChatTarget(ClientID, "/premiumcmds - Commands for premium user");
        SendChatTarget(ClientID, "/housecmds - Commands for control the house");
        SendChatTarget(ClientID, "/villacmds - Commands for control the house");
        SendChatTarget(ClientID, "/modercmds - Commands for moderator user");
        SendChatTarget(ClientID, "/admincmds - Commands for admin user");
        SendChatTarget(ClientID, "/adminrconcmds - Commands rcon for admin user");
        return;
    }

    if(!str_comp_nocase(pMsg, "mafiacmds")) //commands
    {
        /*if(!IsTerrorist)
        {
            SendChatTarget(ClientID, "You are not a mafia user");
            return;
        }*/

        SendChatTarget(ClientID, "(***(---MAFIA CMDS---)***)");
        SendChatTarget(ClientID, "/mafia - To enter in the mafia");
        return;
    }
	
	if(!str_comp_nocase(pMsg, "terroristcmds")) //commands
    {
        /*if(!IsTerrorist)
        {
            SendChatTarget(ClientID, "You are not a terrorist");
            return;
        }*/

        SendChatTarget(ClientID, "(***(---TERRORIST CMDS---)***)");
        SendChatTarget(ClientID, "/terrorist - To become a terrorist");
        SendChatTarget(ClientID, "/terroristbase - Teleports to the terrorist base");
        return;
    }

    if(!str_comp_nocase(pMsg, "policecmds")) //commands
    {
        /*if(!IsPolice)
        {
            SendChatTarget(ClientID, "You are not a police");
            return;
        }*/

        SendChatTarget(ClientID, "(***(---POLICE CMDS---)***)");
        SendChatTarget(ClientID, "/police - To become a police");
        SendChatTarget(ClientID, "/policebase - Teleport to the police base");
        return;
    }

    if(!str_comp_nocase(pMsg, "premiumcmds")) //commands
    {
        /*if(!IsPremium)
        {
            SendChatTarget(ClientID, "You are not a premium user");
            return;
        }*/

        SendChatTarget(ClientID, "(***(---PREMIUM CMDS---)***)");
        SendChatTarget(ClientID, "/ninja - Give the ninja");
        SendChatTarget(ClientID, "/rainbow");
        SendChatTarget(ClientID, "/splash");
        SendChatTarget(ClientID, "/jetpack");
        SendChatTarget(ClientID, "/emote, /stars");
        SendChatTarget(ClientID, "/tpto (id) - Teleports to others players");
        SendChatTarget(ClientID, "/tpfrom (id) - Teleports players to you");
        SendChatTarget(ClientID, "/up, /down, /left, /right - You can move you through walls");
        SendChatTarget(ClientID, "/jump, /hook, /fly - Infinite jumps and hook and you can fly");
        return;
    }

if(!str_comp_nocase(pMsg, "housecmds")) //commands
    {
        /*if(!IsPremium)
        {
            SendChatTarget(ClientID, "You are not a premium user");
            return;
        }*/

        SendChatTarget(ClientID, "(***(---HOUSE CMDS---)***)");
        SendChatTarget(ClientID, "/househome - Teleports you in your house");
        SendChatTarget(ClientID, "/doorhouse - Open/close your door of your house");
        return;
    }
	
	if(!str_comp_nocase(pMsg, "villacmds")) //commands
    {
        /*if(!IsPremium)
        {
            SendChatTarget(ClientID, "You are not a premium user");
            return;
        }*/

        SendChatTarget(ClientID, "(***(---VILLA CMDS---)***)");
        SendChatTarget(ClientID, "/villahome - Teleports you in your villa");
        SendChatTarget(ClientID, "/doorvilla - Open/close your door of your villa");
        return;
    }

    if(!str_comp_nocase(pMsg, "modercmds")) //commands
    {
        if(!IsMod)
        {
            SendChatTarget(ClientID, "You are not a moderator");
            return;
        }

        SendChatTarget(ClientID, "(***(---MODERATION CMDS---)***)");
        SendChatTarget(ClientID, "/status - See all players IDs");
        SendChatTarget(ClientID, "/ban (id) (time) (reason) - The command for ban a player");
        SendChatTarget(ClientID, "/kick (id) (time) (reason) - The command for kick a player");
        SendChatTarget(ClientID, "/mute (id) (time) (reason) - The command for mute a player");
        SendChatTarget(ClientID, "/jail (id) (time) (reason) - The command for jail a player");
        SendChatTarget(ClientID, "/kill (id) (reason) - The command for kill a player");
        SendChatTarget(ClientID, "/freeze (id) (time) (reason) - The command for freeze a player");
        SendChatTarget(ClientID, "/bans - See a list with all players banned and with all IPs");
        SendChatTarget(ClientID, "/unban (ip) - Unban a player");
        SendChatTarget(ClientID, "/unmute (id) - Unmute a player");
        SendChatTarget(ClientID, "/unjail (id) - Unjail a player");
        SendChatTarget(ClientID, "/unfreeze (id) - Unfreeze a player");
        SendChatTarget(ClientID, "/mutechat - Mute the chat, only admins/polices can write in the chat");
        SendChatTarget(ClientID, "/unmutechat - Unmute the chat");
        SendChatTarget(ClientID, "/slowchat (time) - Slow the chat, (time) is the time that the player must wait to write a message");
        SendChatTarget(ClientID, "Press F1 (per default) to open the console and see all commands");
        return;
    }

    if(!str_comp_nocase(pMsg, "admincmds")) //commands
    {
        if(!IsAdmin)
        {
            SendChatTarget(ClientID, "You are not an admin");
            return;
        }

        SendChatTarget(ClientID, "(***(---ADMIN CMDS---)***)");
        SendChatTarget(ClientID, "/immu - Immunity");
        SendChatTarget(ClientID, "/fshot - (very) fast reload");
        SendChatTarget(ClientID, "/invisible - Invisible on the server");
        //SendChatTarget(ClientID, "/fullinvisible - Full invisible on the server");
        SendChatTarget(ClientID, "/tele - Teleports you where you aim to");
        SendChatTarget(ClientID, "/aimbot (id) - Active an aimbot for a player");
        SendChatTarget(ClientID, "/giveweapons - Give all weapons");
        SendChatTarget(ClientID, "/rainbow - Rainbow color");
        return;
    }
	
	if(!str_comp_nocase(pMsg, "adminrconcmds")) //commands
    {
        if(!IsAdmin)
        {
            SendChatTarget(ClientID, "You are not an admin");
            return;
        }

        SendChatTarget(ClientID, "(***(---ADMIN RCON CMDS---)***)");
        SendChatTarget(ClientID, "Open the console of the server (F2) and you can type :");
        SendChatTarget(ClientID, "say (text)");
        SendChatTarget(ClientID, "sayby (id) (text)");
        SendChatTarget(ClientID, "broadcast (text)");
        SendChatTarget(ClientID, "change_name (id) (name)");
        SendChatTarget(ClientID, "change_clan (id) (clan)");
        SendChatTarget(ClientID, "set_hammer (id) (number)");
        return;
    }
	
	if(!str_comp_nocase(pMsg, "lastupdate")) //cmdlist
    {
		SendChatTarget(ClientID, "(***(---LAST UPDATE---)***)");
		SendChatTarget(ClientID, "Last update : 7/06/2016");
		SendChatTarget(ClientID, "/rules moderator - Modified");
		SendChatTarget(ClientID, "/rules admin - Modified");
		SendChatTarget(ClientID, "/infos weapons - Modified");
		SendChatTarget(ClientID, "/premiumbenefits - Modified");
		SendChatTarget(ClientID, "/powermodebenefits - Added");
		SendChatTarget(ClientID, "/howtobuypowermode - Added");
        return;
    }

    if(!str_comp_nocase_num(pMsg, "aimbot", 6))//6 = nombre de character de "aimbot"
    {
        if(!IsAdmin)
        {
            SendChatTarget(ClientID, "You are not an admin");
            return;
        }
        int Cid;//Cid = Client id
        if(sscanf(pMsg, "aimbot %d", &Cid) != 1)
        {
            SendChatTarget(ClientID, "Please stick to the given structure :");
            SendChatTarget(ClientID, "/aimbot <id>");
            return;
        }

        if(Cid < 0 || Cid >= MAX_CLIENTS)
        {
            SendChatTarget(ClientID, "You can only choose IDs between 0 and 15");
            return;
        }

        if(!m_apPlayers[Cid])
        {
            SendChatTarget(ClientID, "Invalid client ID");
            return;
        }

        //^= ça change de false en true, de true en false. Ne fonctionne uniquement quand de base c'est à false,
        //je sais pas trop comment ça marche, c'est Piko qui me l'a apprit ^^
        m_apPlayers[Cid]->m_AimBot ^= true;

        char aBuf[256];
        str_format(aBuf, sizeof(aBuf), "'%s's aimbot %s", Server()->ClientName(Cid), m_apPlayers[Cid]->m_AimBot ? "enabled" : "disabled");
        SendChatTarget(ClientID, aBuf);
        return;
    }

    if(!str_comp_nocase(pMsg, "invisible"))
    {
         if(!IsAdmin)
        {
            SendChatTarget(ClientID, "You are not an admin");
            return;
        }

        if(pPlayer->m_Invis)
        {
            pPlayer->m_Invis = false;
            SendChatTarget(ClientID, "Invisible desactivated!");
        }
        else
        {

            pPlayer->m_Invis = true;
            SendChatTarget(ClientID, "Invisible activated!");
        }
        return;
    }

    if(!str_comp_nocase(pMsg, "tele"))
    {
        if(!IsAdmin)
        {
            SendChatTarget(ClientID, "You are not an admin");
            return;
        }

        if(GetPlayerChar(ClientID))
            GetPlayerChar(ClientID)->MovePos(vec2(pPlayer->m_LatestActivity.m_TargetX, pPlayer->m_LatestActivity.m_TargetY));

        return;
    }

    if(!str_comp_nocase(pMsg, "giveweapons"))
    {
        if(!IsAdmin)
        {
            SendChatTarget(ClientID, "You are not an admin");
            return;
        }

        if(!GetPlayerChar(ClientID))
        {
            SendChatTarget(ClientID, "Wait to respawn to do this");
            return;
        }

        for(int i = 0; i < NUM_WEAPONS; i++)
        {
			if(i == WEAPON_NINJA)
                continue;
            GetPlayerChar(ClientID)->GiveWeapon(i, 10);//On peut mettre -1 aussi pour munitions illimités, mais si on met -1 ça va ramasser les armes.
        }

        SendChatTarget(ClientID, "You now have all weapons");
        return;
    }

    if(!str_comp_nocase(pMsg, "immu"))
    {
        if(!IsAdmin)
        {
            SendChatTarget(ClientID, "You are not an admin");
            return;
        }

        pPlayer->m_Immunity ^= true;
        SendChatTarget(ClientID,  pPlayer->m_Immunity ? "Immunity enabled !" : "Immunity disabled.");

        CCharacter *pChar = GetPlayerChar(ClientID);
        if(!pChar)
            return;

        CreateDeath(pChar->GetPos(), ClientID);
        return;
    }

    /*if(!str_comp_nocase(pMsg, "fullinvisible"))
    {
        if(!IsAdmin)
        {
            SendChatTarget(ClientID, "You are not an admin");
            return;
        }

        pPlayer->m_FullInvisible ^= true;
        SendChatTarget(ClientID,  pPlayer->m_Immunity ? "Fullinvisible enabled !" : "Fullinvisible disabled.");

        char aBuf[256];
        if(pPlayer->m_FullInvisible)
        {
            str_format(aBuf, sizeof(aBuf), "'%s' has left the game", Server()->ClientName(ClientID));
            SendChatTarget(-1, aBuf);
        }
        else
        {
            //faudra changer ça si tu veux faire plus réaliste, quand tu auras des comptes
            //pck tu vas entrer en spec avant, si y a pas le message de spec ça va paraitre (je l'ai mal écrit mais osef) chelou
            str_format(aBuf, sizeof(aBuf), "'%s' entered and joined the game", Server()->ClientName(ClientID));
            SendChatTarget(-1, aBuf);
            m_SendMsgTick = Server()->TickSpeed() * 2;
            m_SendMsgID = ClientID;
        }
        return;
    }*/

    if(!str_comp_nocase(pMsg, "fshot"))
    {
        if(!IsAdmin)
        {
            SendChatTarget(ClientID, "You are not an admin");
            return;
        }

        pPlayer->m_FastShooting ^= true;
        SendChatTarget(ClientID,  pPlayer->m_FastShooting ? "Fast shooting enabled !" : "Fast shooting disabled.");
        return;
    }

    if(!str_comp_nocase(pMsg, "up"))
    {
        if(!IsAdmin)
        {
            SendChatTarget(ClientID, "You are not an admin");
            return;
        }

        if(GetPlayerChar(ClientID))
            GetPlayerChar(ClientID)->MovePos(vec2(0, -32));
        return;
    }

    if(!str_comp_nocase(pMsg, "down"))
    {
        if(!IsAdmin)
        {
            SendChatTarget(ClientID, "You are not an admin");
            return;
        }

        if(GetPlayerChar(ClientID))
            GetPlayerChar(ClientID)->MovePos(vec2(0, 32));
        return;
    }

    if(!str_comp_nocase(pMsg, "left"))
    {
        if(!IsAdmin)
        {
            SendChatTarget(ClientID, "You are not an admin");
            return;
        }

        if(GetPlayerChar(ClientID))
            GetPlayerChar(ClientID)->MovePos(vec2(-32, 0));
        return;
    }

    if(!str_comp_nocase(pMsg, "right"))
    {
        if(!IsAdmin)
        {
            SendChatTarget(ClientID, "You are not an admin");
            return;
        }

        if(GetPlayerChar(ClientID))
            GetPlayerChar(ClientID)->MovePos(vec2(32, 0));
        return;
    }

    if(!str_comp_nocase(pMsg, "rainbow"))
    {
        if(!IsAdmin)
        {
            SendChatTarget(ClientID, "You are not an admin");
            return;
        }

        pPlayer->m_Rainbow ^= true;
        SendChatTarget(ClientID,  pPlayer->m_Rainbow ? "Rainbow enabled !" : "Rainbow disabled.");
        return;
    }

    SendChatTarget(ClientID, "Unknow command. Try \"/cmdlist\".");
}
