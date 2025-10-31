#ifndef _MMATCHRULE_TRAINING_H
#define _MMATCHRULE_TRAINING_H


#include "MMatchRuleDeathMatch.h"

///////////////////////////////////////////////////////////////////////////////////////////////
class MMatchRuleTraining : public MMatchRuleSoloDeath {
public:
	MMatchRuleTraining(MMatchStage* pStage);
	virtual ~MMatchRuleTraining()	{ }
	virtual MMATCH_GAMETYPE GetGameType() { return MMATCH_GAMETYPE_TRAINING; }
};


///////////////////////////////////////////////////////////////////////////////////////////////
class MMatchRuleTeamTraining : public MMatchRuleTeamDeath
{
public:
	MMatchRuleTeamTraining( MMatchStage* pStage );
	virtual ~MMatchRuleTeamTraining()	{ }
	virtual MMATCH_GAMETYPE GetGameType() { return MMATCH_GAMETYPE_TEAM_TRAINING; }
};



#endif