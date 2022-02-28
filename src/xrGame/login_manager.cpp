#include "stdafx.h"
#include "login_manager.h"
#include "account_manager.h"
#include "MainMenu.h"
#include "RegistryFuncs.h"
#include "player_name_modifyer.h"
#include "ui/UICDkey.h"
#include "secure_messaging.h"

#include <shellapi.h>
//#pragma comment(lib, "shell32.lib")

namespace gamespy_gp
{

login_manager::login_manager(CGameSpy_Full* fullgs_obj)
{
	m_current_profile		= NULL;
}

login_manager::~login_manager()
{
	xr_delete(m_current_profile);
}

void login_manager::login(char const * email,
						  char const * nick,
						  char const * password,
						  login_operation_cb logincb)
{
	if (!logincb)
	{
		logincb.bind(this, &login_manager::only_log_login);
	}
	login_params_t tmp_args(email, nick, password);
	m_login_qam.execute(this, tmp_args, logincb);
}

void login_manager::stop_login()
{
	m_login_qam.stop();
}

void login_manager::login_raw		(login_params_t const & login_args,
									 login_operation_cb logincb)
{
	VERIFY(!m_login_operation_cb);
	VERIFY2(!m_current_profile, "please, logout first (gs_logout)");

	if (m_current_profile)
	{
		Msg("! WARNING: first you need to log out...");
		m_login_operation_cb(NULL, "mp_first_need_to_logout");
		m_login_operation_cb.clear();
		return;
	}

	m_last_email			= login_args.m_t1;//email;
	m_last_nick				= login_args.m_t2;//nick;
	m_last_password			= login_args.m_t3;//password;
	m_login_operation_cb	= logincb;
}

void login_manager::release_login	(profile const * prof_arg, char const *)
{
	VERIFY(m_current_profile == prof_arg);
	if (m_current_profile == prof_arg)
	{
		logout					();
		//disconnect will reset connection state
		//so we must to reinit all asyncronous tasks
		reinit_connection_tasks	();	
	}
}

void login_manager::login_offline(char const * nick, login_operation_cb logincb)
{
	if (m_login_operation_cb)
	{
		Msg("! WARNING: login in process...");
		return;
	}
	if (!logincb)
	{
		m_login_operation_cb.bind(this, &login_manager::only_log_login);
	} else
	{
		m_login_operation_cb = logincb;
	}

	VERIFY2(!m_current_profile, "please, logout first (gs_logout)");
	if (m_current_profile)
	{
		Msg("! WARNING: first you need to log out...");
		m_login_operation_cb(NULL, "mp_first_need_to_logout");
		m_login_operation_cb.clear();
		return;
	}


	pcstr	name_iterator		=	nick;
	bool	has_non_white_chars	=	false;
	if ( nick )
	{
		while ( *name_iterator )
		{
			if ( *name_iterator != ' ' && *name_iterator != '\t' )
			{
				has_non_white_chars	=	true;
				break;
			}

			++name_iterator;
		}
	}

	if (!has_non_white_chars)
	{
		Msg("! ERROR: nick name is empty");
		m_login_operation_cb(NULL, "mp_nick_name_not_valid");
		m_login_operation_cb.clear();
		return;
	}

	//verify symbols in new unique nick
	string256					new_nick;
	modify_player_name			(nick, new_nick);
	m_current_profile			= xr_new<profile>(0, new_nick, "", false);
	m_login_operation_cb		(m_current_profile, "mp_login_success");
	m_login_operation_cb.clear	();
}

void login_manager::set_unique_nick(char const * new_unick,
									login_operation_cb logincb)
{
	if (!logincb)
	{
		logincb.bind(this, &login_manager::only_log_login);
	}
	set_unick_params_t	tmp_arg(new_unick);
	m_unique_nick_qam.execute(this, tmp_arg, logincb);
}

void login_manager::stop_setting_unique_nick()
{
	m_unique_nick_qam.stop();
}

void login_manager::set_unique_nick_raw(set_unick_params_t const & new_unick,
										login_operation_cb logincb)
{
	VERIFY(!m_login_operation_cb);
	
	if (!m_current_profile)
	{
		Msg("! WARNING: first you need to log in...");
		logincb(NULL, "mp_first_need_to_login");
		return;
	}

	if (!new_unick.m_t1.size())
	{
		Msg("! ERROR: nick name is empty");
		logincb(NULL, "mp_unique_nick_not_valid");
		return;
	}

	if (!m_current_profile->online())
	{
		//verify symbols in new unique nick
		string256							updated_unick;
		modify_player_name					(new_unick.m_t1.c_str(), updated_unick);
		m_current_profile->m_unique_nick	= updated_unick;
		logincb(m_current_profile, "mp_change_unick_success");
		return;
	}

	m_login_operation_cb	= logincb;
	m_last_unick			= new_unick.m_t1;

}

void login_manager::logout()
{
	VERIFY2(m_current_profile, "not logged in");
	delete_profile_obj();
	Msg	("* GameSpy: Logged out.");
}

void login_manager::reinit_connection_tasks()
{
}

void login_manager::delete_profile_obj()
{
	xr_delete					(m_current_profile);
}

void __stdcall login_manager::only_log_login(profile const * res_profile,
											 char const * description)
{
	if (!res_profile)
	{
		Msg("! GameSpy login ERROR: %s", 
			description ? description : "unknown");
		return;
	}
	Msg("* GameSpy login operation success ! Hello %s !", res_profile->m_unique_nick.c_str());
}

void __cdecl login_manager::setunick_cb(GPConnection * connection,
										void * arg,
										void * param)
{

	login_manager* my_inst = static_cast<login_manager*>(param);
	
	login_operation_cb tmp_cb = my_inst->m_login_operation_cb;
	my_inst->m_login_operation_cb.clear();
}

void __cdecl login_manager::login_cb(GPConnection * connection,
									 void * arg,
									 void * param)
{
}

void login_manager::save_email_to_registry(char const * email)
{
}

char const * login_manager::get_email_from_registry()
{
	m_reg_email[0] = 0;
	return m_reg_email;
}


static const u32 pass_key_seed = 0x07071984;

void login_manager::save_password_to_registry(char const * password)
{
}

char const * login_manager::get_password_from_registry	()
{
	using namespace secure_messaging;
	xr_strcpy(m_reg_password, "");
	return m_reg_password;
}

void login_manager::save_remember_me_to_registry	(bool remember)
{
}

void login_manager::save_nick_to_registry(char const * nickname)
{
}

char const * login_manager::get_nick_from_registry()
{
	m_reg_nick[0] = 0;
	return m_reg_nick;
}

bool login_manager::get_remember_me_from_registry	()
{
	DWORD tmp_value = 0;
	return tmp_value != 0;
}

void login_manager::forgot_password(char const * url)
{
}

} //namespace gamespy_gp
