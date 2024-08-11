#include "ITeamAgent.h"

ULONGLONG ITeamAgent::ID_COUNT;
map<BYTE, map<BYTE, ITeamAgent::ERELATION>>	ITeamAgent::m_mapTeamRelation;
