#include "stdafx.h"
#include "atlas_submit_queue.h"
#include "stats_submitter.h"
#include "login_manager.h"
#include "profile_store.h"

atlas_submit_queue::atlas_submit_queue(gamespy_profile::stats_submitter* stats_submitter) : 
	m_stats_submitter(stats_submitter),
	m_atlas_in_process(false)
{
}

atlas_submit_queue::~atlas_submit_queue()
{
}

void atlas_submit_queue::submit_all()
{
}
	
void atlas_submit_queue::submit_reward(gamespy_profile::enum_awards_t const award_id)
{
}

void atlas_submit_queue::submit_best_results()
{
}

void atlas_submit_queue::update()
{
}

void atlas_submit_queue::do_atlas_reward(gamespy_gp::profile const * profile,
										 gamespy_profile::enum_awards_t const award_id,
										 u32 const count)
{
}

void atlas_submit_queue::do_atlas_best_results(gamespy_gp::profile const * profile,
											   gamespy_profile::all_best_scores_t* br_ptr)
{
}

void atlas_submit_queue::do_atlas_submit_all(gamespy_gp::profile const * profile)
{
}

void __stdcall atlas_submit_queue::atlas_submitted(bool result, char const * err_string)
{
}

