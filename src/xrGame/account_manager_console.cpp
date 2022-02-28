#include "stdafx.h"
#include "account_manager_console.h"
#include "account_manager.h"
#include "login_manager.h"
#include "profile_store.h"
#include "stats_submitter.h"
#include "MainMenu.h"

void CCC_CreateGameSpyAccount::Execute(LPCSTR args)
{
	if (!args || (xr_strlen(args) == 0))
	{
		TInfo tmp_info;
		Info(tmp_info);
		Msg(tmp_info);
		return;
	}
	string256	tmp_nick;
	string256	tmp_unick;
	string256	tmp_email;
	string256	tmp_password;
	
	sscanf_s(args, "%s %s %s %s",
		tmp_nick, sizeof(tmp_nick),
		tmp_unick, sizeof(tmp_unick),
		tmp_email, sizeof(tmp_email),
		tmp_password, sizeof(tmp_password));
}

void CCC_GapySpyListProfiles::Execute(LPCSTR args)
{
	if (!args || (xr_strlen(args) == 0))
	{
		TInfo tmp_info;
		Info(tmp_info);
		Msg(tmp_info);
		return;
	}
	string256	tmp_email;
	string256	tmp_password;
			
	sscanf_s(args, "%s %s",
		tmp_email,		sizeof(tmp_email),
		tmp_password,	sizeof(tmp_password));
}

void CCC_GameSpyLogin::Execute(LPCSTR args)
{
	if (!args || (xr_strlen(args) == 0))
	{
		TInfo tmp_info;
		Info(tmp_info);
		Msg(tmp_info);
		return;
	}
	string256	tmp_email;
	string256	tmp_nick;
	string256	tmp_password;
			
	sscanf_s(args, "%s %s %s", 
		tmp_email,		sizeof(tmp_email),
		tmp_nick,		sizeof(tmp_nick),
		tmp_password,	sizeof(tmp_password)
	);
}

void CCC_GameSpyLogout::Execute(LPCSTR args)
{
	
}

static char const * print_time(time_t const & src_time, string64 & dest_time)
{
	tm* tmp_tm = _localtime64(&src_time);
	xr_sprintf(dest_time, sizeof(dest_time),
		"%02d.%02d.%d_%02d:%02d:%02d",
		tmp_tm->tm_mday, 
		tmp_tm->tm_mon+1, 
		tmp_tm->tm_year+1900, 
		tmp_tm->tm_hour, 
		tmp_tm->tm_min, 
		tmp_tm->tm_sec
	);
	return dest_time;
}

void CCC_GameSpyPrintProfile::Execute(LPCSTR args)
{
	{
		Msg("- No profile. You are not loged in.");
	}
}

void CCC_GameSpySuggestUNicks::Execute(LPCSTR args)
{
}

void CCC_GameSpyRegisterUniqueNick::Execute(LPCSTR args)
{
}


void CCC_GameSpyDeleteProfile::Execute(LPCSTR args)
{
}

static gamespy_profile::all_best_scores_t debug_best_scores;

void CCC_GameSpyProfile::Execute(LPCSTR args)
{
}
