#include "ITeamAgent.h"

ULONGLONG ITeamAgent::ID_COUNT;
map<BYTE, map<BYTE, ERELATION_STATE>> ITeamAgent::m_mapTeamRelation;