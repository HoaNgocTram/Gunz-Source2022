#include "stdafx.h"
#include "ZRule.h"
#include "ZMatch.h"
#include "ZRuleDeathMatch.h"
#include "ZRuleGladiator.h"
#include "ZRuleAssassinate.h"
#include "ZRuleTraining.h"
#include "ZRuleQuest.h"
#include "ZRuleSurvival.h"
#include "ZRuleQuestChallenge.h"
#include "ZRuleBerserker.h"
#include "ZRuleDuel.h"
#include "ZRuleDuelTournament.h"
#include "ZRuleBlitzKrieg.h"
#include "ZRuleSpy.h"
#include "ZRuleSkillMap.h"

ZRule::ZRule(ZMatch* pMatch)
{
	m_pMatch = pMatch;				// 초기화 =_=
}

ZRule::~ZRule()
{

}

void ZRule::Update(float fDelta)
{
	OnUpdate(fDelta);
}

ZRule* ZRule::CreateRule(ZMatch* pMatch, MMATCH_GAMETYPE nGameType)
{
	switch(nGameType)
	{
	case MMATCH_GAMETYPE_DEATHMATCH_SOLO:
		{
			return (new ZRuleSoloDeathMatch(pMatch));
		}
		break;
	case MMATCH_GAMETYPE_DEATHMATCH_TEAM:
		{
			return (new ZRuleTeamDeathMatch(pMatch));
		}
		break;
	case MMATCH_GAMETYPE_GLADIATOR_SOLO:
		{
			return (new ZRuleSoloGladiator(pMatch));
		}
		break;
	case MMATCH_GAMETYPE_GLADIATOR_TEAM:
		{
			return (new ZRuleTeamGladiator(pMatch));
		}
		break;
	case MMATCH_GAMETYPE_ASSASSINATE:
		{
			return (new ZRuleAssassinate(pMatch));
		}
		break;
	case MMATCH_GAMETYPE_TRAINING:
		{
			return (new ZRuleTraining(pMatch));
		}
		break;
#ifdef _QUEST
	case MMATCH_GAMETYPE_SURVIVAL:
		{
			return (new ZRuleSurvival(pMatch));
		}
		break;
	case MMATCH_GAMETYPE_QUEST:
		{
			return (new ZRuleQuest(pMatch));
		}
		break;
#endif
	case MMATCH_GAMETYPE_QUEST_CHALLENGE:
		{
			return (new ZRuleQuestChallenge(pMatch));
		}
		break;
	case MMATCH_GAMETYPE_BERSERKER:
		{
			return (new ZRuleBerserker(pMatch));
		}
		break;
	case MMATCH_GAMETYPE_DEATHMATCH_TEAM2:
		{
			return (new ZRuleTeamDeathMatch2(pMatch));
		}
		break;
	case MMATCH_GAMETYPE_DUEL:
		{
			return (new ZRuleDuel(pMatch));
		}
		break;

	case MMATCH_GAMETYPE_DUELTOURNAMENT:
		{
			return (new ZRuleDuelTournament(pMatch));
		}
		break;

	case MMATCH_GAMETYPE_TEAM_TRAINING:
		{
			return (new ZRuleTeamTraining(pMatch));
		}
		break;

	case MMATCH_GAMETYPE_CTF:
		{
			return (new ZRuleTeamCTF(pMatch));
		}
		break;
	case MMATCH_GAMETYPE_INFECTED:
		{
			return (new ZRuleTeamInfected(pMatch));
		}
		break;

	case MMATCH_GAMETYPE_GUNGAME:
		{
			return (new ZRuleGunGame(pMatch));
		}
		break;
	case MMATCH_GAMETYPE_SPY:
	{
		return (new ZRuleSpy(pMatch));
	}
	break;
	case MMATCH_GAMETYPE_TURBO:
	{
		return (new ZRuleTurbo(pMatch));
	}
	break;
	case MMATCH_GAMETYPE_BLITZKRIEG:
	{
		return (new ZRuleBlitzKrieg(pMatch));
	}
	break;
	case MMATCH_GAMETYPE_SKILLMAP:
	{
		return (new ZRuleSkillMap(pMatch));
	}break;
	default:
		{
			// 게임 타입이 없습니다.
			_ASSERT(0);
		}
	}
	return NULL;

}

bool ZRule::OnCommand(MCommand* pCommand)
{
	return false;
}

void ZRule::AfterCommandProcessed( MCommand* pCommand )
{

}

void ZRule::OnResponseRuleInfo(MTD_RuleInfo* pInfo)
{

}