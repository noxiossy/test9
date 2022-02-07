#include "stdafx.h"
#include "account_manager_console.h"
#include "gamespy/GameSpy_Full.h"
#include "gamespy/GameSpy_GP.h"
#include "gamespy/GameSpy_SAKE.h"
#include "account_manager.h"
#include "login_manager.h"
#include "profile_store.h"
#include "stats_submitter.h"

void CCC_CreateGameSpyAccount::Execute(LPCSTR args)
{
		TInfo tmp_info;
		Info(tmp_info);
		Msg(tmp_info);
		return;
}

void CCC_GapySpyListProfiles::Execute(LPCSTR args)
{
		TInfo tmp_info;
		Info(tmp_info);
		Msg(tmp_info);
		return;
}

void CCC_GameSpyLogin::Execute(LPCSTR args)
{
		TInfo tmp_info;
		Info(tmp_info);
		Msg(tmp_info);
		return;
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
