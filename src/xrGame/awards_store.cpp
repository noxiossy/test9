#include "stdafx.h"
#include "awards_store.h"

namespace gamespy_profile
{


awards_store::awards_store(CGameSpy_Full* fullgs_obj)
{
	init_field_names	();
}

awards_store::~awards_store()
{
}

void awards_store::init_field_names()
{
	for (int i = 0; i < at_awards_count; ++i)
	{
		int findex = i * ap_award_params_count;
		m_field_names_store[findex + ap_award_id]		= ATLAS_GET_STAT_NAME(
			get_award_id_stat(static_cast<enum_awards_t>(i))
		);
		m_field_names_store[findex + ap_award_rdate]	= ATLAS_GET_STAT_NAME(
			get_award_reward_date_stat(static_cast<enum_awards_t>(i))
		);
	}
}

void awards_store::reset_awards()
{
	m_awards_result.clear		();
	m_ltx_awards_result.clear	();

	for (int aidx = 0; aidx < at_awards_count; ++aidx)
	{
		m_awards_result.insert(
			std::make_pair(
				enum_awards_t(aidx),
				award_data(u16(0), u32(0))
			)
		);
	};
}

void awards_store::load_awards(store_operation_cb & opcb)
{
	m_award_operation_cb		= opcb;
}

void awards_store::load_awards_from_ltx	(CInifile& ini)
{
	for (int i = 0; i < at_awards_count; ++i)
	{
		enum_awards_t	tmp_awid = static_cast<enum_awards_t>(i);
		LPCSTR tmp_award_name = get_award_name(tmp_awid);
		u16 tmp_count = ini.r_u16(tmp_award_name, award_count_line);
		u32 tmp_rdate = ini.r_u32(tmp_award_name, award_rdate_line);
		m_ltx_awards_result.insert(
			std::make_pair(tmp_awid, award_data(tmp_count, tmp_rdate))
		);
	}
}
void awards_store::merge_sake_to_ltx_awards()
{
	for (all_awards_t::iterator i = m_ltx_awards_result.begin(),
		ie = m_ltx_awards_result.end(); i != ie; ++i)
	{
		all_awards_t::const_iterator tmp_awi = m_awards_result.find(
			i->first);
		if (tmp_awi != m_awards_result.end())
		{
			u16 tmp_count = std::max(i->second.m_count, tmp_awi->second.m_count);
			u32 tmp_rdate = std::max(i->second.m_last_reward_date, tmp_awi->second.m_last_reward_date);
			i->second = award_data(tmp_count, tmp_rdate);
		}
	}
}

all_awards_t & awards_store::get_player_awards()
{
	if (m_ltx_awards_result.empty())
		return m_awards_result;
	return m_ltx_awards_result;
}

bool awards_store::is_sake_equal_to_file() const
{
	VERIFY(!m_ltx_awards_result.empty());
	if (m_ltx_awards_result.empty())
		return true;	//unknown
	
	for (all_awards_t::const_iterator i = m_ltx_awards_result.begin(),
		ie = m_ltx_awards_result.end(); i != ie; ++i)
	{
		all_awards_t::const_iterator tmp_iter = m_awards_result.find(i->first);
		R_ASSERT(tmp_iter != m_awards_result.end());
		
		if (i->second.m_count != tmp_iter->second.m_count)
			return false;
	}
	return true;
}

void awards_store::process_award(SAKEField* award_params)
{
}

void awards_store::process_aw_out_response(SAKEGetMyRecordsOutput* tmp_out, int const out_fields_count)
{
}
}//namespace gamespy_profile
