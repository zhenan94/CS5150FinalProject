// MIT License
// 
// Copyright(c) 2020 Arthur Bacon and Kevin Dill
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this softwareand associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and /or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
// 
// The above copyright noticeand this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "Controller_AI_ZhenanLiu.h"

#include "Constants.h"
#include "EntityStats.h"
#include "iPlayer.h"
#include "Vec2.h"
#include <vector>
#include <algorithm>

using namespace std;
static const Vec2 ksGiantPosL(LEFT_BRIDGE_CENTER_X, RIVER_TOP_Y - 0.5f);
static const Vec2 ksGiantPosR(RIGHT_BRIDGE_CENTER_X, RIVER_TOP_Y - 0.5f);
static const Vec2 ksArcherPosL(LEFT_BRIDGE_CENTER_X, 0.f);
static const Vec2 ksArcherPosR(RIGHT_BRIDGE_CENTER_X, 0.f);
float giantRange = iEntityStats::getStats(iEntityStats::Giant).getAttackRange();
float swordRange = iEntityStats::getStats(iEntityStats::Swordsman).getAttackRange();
float archerRange = iEntityStats::getStats(iEntityStats::Archer).getAttackRange();
float princeSize = iEntityStats::getBuildingStats(iEntityStats::Princess).getSize();
struct mobsData
{
	int type;
	int x;
	int y;
	bool crossRiver;
	bool inRange;
	int rank;
};

bool f = false;

bool sort(mobsData a, mobsData b) {

	return a.rank < b.rank;
}
//function for checking whether a mob is cross the river or not. 
bool Controller_AI_ZhenanLiu::crossRiver(bool isNorth, float x, float y) {
	if (isNorth) {
		if (y < RIVER_TOP_Y) {
			return true;
		}
		else {
			return false;
		}
	}
	else {
		if (y > RIVER_BOT_Y) {
			return true;
		}
		else {
			return false;
		}
	}
}
//function for checking whether a enemy mob is in the attack range of towers or not. 
bool Controller_AI_ZhenanLiu::inRnage(bool isNorth, float x, float y) {
	for (int i = 0; i < m_pPlayer->getNumBuildings(); i++) {
		if (m_pPlayer->getBuilding(i).m_Health > 0) {
			int dist = (x - m_pPlayer->getBuilding(i).m_Position.x) * (x - m_pPlayer->getBuilding(i).m_Position.x) +
				(y - m_pPlayer->getBuilding(i).m_Position.y) * (y - m_pPlayer->getBuilding(i).m_Position.y);
			if (dist <= m_pPlayer->getBuilding(i).m_Stats.getAttackRange() * m_pPlayer->getBuilding(i).m_Stats.getAttackRange()) {
				return true;

			}
		}

	}
	return false;
}


void Controller_AI_ZhenanLiu::tick(float deltaTSec)
{
	if (m_pPlayer->getOpponentBuilding(0).m_Health <= 0&&!win_condition) {
		cout << "I win! Try harder next time!" << endl;
		win_condition = true;
	}
	if (m_pPlayer->getBuilding(0).m_Health <= 0&&!win_condition) {
		cout << "Oh I lost. you are really good." << endl;
		win_condition = true;
	}

	bool isNorth = m_pPlayer->isNorth();
	vector<mobsData> solider;
	Vec2 place;
	Vec2 place1;
	if (m_pPlayer->getOpponentBuilding(1).m_Health <= m_pPlayer->getOpponentBuilding(2).m_Health) {
		place = ksGiantPosL;
		place1 = ksArcherPosL;
	}
	else {
		place = ksGiantPosR;
		place1 = ksArcherPosR;
	}

	if ((m_pPlayer->getOpponentBuilding(1).m_Health <= 0 || m_pPlayer->getOpponentBuilding(2).m_Health <= 0) && !enemytower_condition) {
		cout << "Your Prince Tower has been destroyed" << endl;
		enemytower_condition = true;
	}
	if ((m_pPlayer->getBuilding(1).m_Health <= 0 || m_pPlayer->getBuilding(2).m_Health <= 0) && !mytower_condition) {
		cout << "Oh my Prince Tower has been destroyed" << endl;
		mytower_condition = true;
	}


	assert(m_pPlayer);
	//declare the number of each unit of both sides
	//ma:my archer ms... mg...
	//oa:opposite archer os... og...
	int ma = 0;
	int ms = 0;
	int mg = 0;
	int oa = 0;
	int os = 0;
	int og = 0;

	//count the number of each unit of our side.
	for (int i = 0; i < m_pPlayer->getNumMobs(); i++)
	{
		switch (m_pPlayer->getMob(i).m_Stats.getMobType())
		{
		case 0:
			ms += 1;

			break;
		case 1:
			ma += 1;

			break;
		case 2:
			mg += 1;
			break;
		default:
			break;
		}
	}


	//count the number of each unit of opposite side.
	// and store addintional data for enemy mobs in a vector.
	for (int i = 0; i < m_pPlayer->getNumOpponentMobs(); i++)
	{
		float x;
		float y;
		mobsData m;
		switch (m_pPlayer->getOpponentMob(i).m_Stats.getMobType())
		{
		case 0:
			os += 1;
			x = m_pPlayer->getOpponentMob(i).m_Position.x;
			y = m_pPlayer->getOpponentMob(i).m_Position.y;
			m.type = 0;
			m.x = x;
			m.y = y;
			m.rank = 2;
			m.crossRiver = crossRiver(isNorth, x, y);
			m.inRange = inRnage(isNorth, x, y);
			solider.push_back(m);
			break;
		case 1:
			oa += 1;
			x = m_pPlayer->getOpponentMob(i).m_Position.x;
			y = m_pPlayer->getOpponentMob(i).m_Position.y;
			m.x = x;
			m.y = y;
			m.type = 1;
			m.rank = 0;
			m.crossRiver = crossRiver(isNorth, x, y);
			m.inRange = inRnage(isNorth, x, y);
			solider.push_back(m);
			break;
		case 2:
			og += 1;
			x = m_pPlayer->getOpponentMob(i).m_Position.x;
			y = m_pPlayer->getOpponentMob(i).m_Position.y;
			m.x = x;
			m.y = y;
			m.type = 2;
			m.rank = 1;
			m.crossRiver = crossRiver(isNorth, x, y);
			solider.push_back(m);
			break;
		default:
			break;
		}
	}
	//sort the solider vector by the order of archer, giant and swordsman. 
	sort(solider.begin(), solider.end(), sort);
	//AI take following actions if opposite spawn  firstly or after first spawning.
	if (m_pPlayer->getNumOpponentMobs() >= 1 || start) {
		start = true;
		//the spawning for the swordsman.
		if (m_pPlayer->getElixir() >= 3 && ms <= 2) {
			bool flag = true;
			if (!solider.empty()) {
				for (int i = 0; i < solider.size(); i++) {
					if (solider[i].crossRiver) { flag = false; };
					//handling the attack of enemy swordsman.
					if (solider[i].type == 0) {
						if (solider[i].inRange) {
							Vec2 pos(solider[i].x, solider[i].y);
							m_pPlayer->placeMob(iEntityStats::Swordsman, pos);
							break;
						}
					}
					//handling the attack of enemy archer.
					if (solider[i].type == 1) {
						if (solider[i].crossRiver) {
							Vec2 pos(solider[i].x, solider[i].y);
							m_pPlayer->placeMob(iEntityStats::Swordsman, pos);

							break;
						}

					}
					//handling the attack of enemy giant.
					if (solider[i].type == 2) {
						if (solider[i].crossRiver) {
							Vec2 pos(solider[i].x, solider[i].y);
							m_pPlayer->placeMob(iEntityStats::Swordsman, pos.Player2Game(isNorth));

							break;
						}
					}

				}

			}
			//spawning as reinforcements 
			if (flag && ma > 0) {
				Vec2 pos = place.Player2Game(isNorth);
				m_pPlayer->placeMob(iEntityStats::Swordsman, pos);
			}


		}
		//the spawning for the archer.
		if (m_pPlayer->getElixir() >= 2 && ma < 2) {
			int margin;
			if (isNorth) { margin = -archerRange; }
			else { margin = archerRange; }

			if (!solider.empty()) {


				for (int i = 0; i < solider.size(); i++)
				{
					if (solider[i].crossRiver) {
						int dist = 10000;
						for (int j = 0; j < m_pPlayer->getNumBuildings(); j++)
						{
							int d = (solider[i].x - m_pPlayer->getBuilding(j).m_Position.x) *
								(solider[i].x - m_pPlayer->getBuilding(j).m_Position.x) +
								(solider[i].y - m_pPlayer->getBuilding(j).m_Position.y) *
								(solider[i].y - m_pPlayer->getBuilding(j).m_Position.y);
							if (d < dist) {
								dist = d;
							}
						}
						//handling the attack of enemy giant.
						if (solider[i].type == 2) {
							if (dist <= (giantRange + princeSize) * (giantRange + princeSize)) {
								Vec2 pos(solider[i].x, solider[i].y + margin);

								m_pPlayer->placeMob(iEntityStats::Archer, pos);
								break;
							}
						}
						//handling the attack of enemy swordsman.
						else if (solider[i].type == 0) {
							if (dist <= (swordRange + princeSize) * (swordRange + princeSize)) {
								Vec2 pos(solider[i].x, solider[i].y + margin);
								m_pPlayer->placeMob(iEntityStats::Archer, pos);
								break;
							}
						}
					}

				}

			}

			else {
				if (ms > 1) {

					m_pPlayer->placeMob(iEntityStats::Archer, place1.Player2Game(isNorth));
				}
			}
		}


		//Sitation for there is no mobs on the field.
		if (m_pPlayer->getElixir() >= 10 && m_pPlayer->getNumOpponentMobs() < 1) {
			cout << "See my ultimate attack!" << endl;
			Vec2 giantPos_Game = place.Player2Game(isNorth);
			m_pPlayer->placeMob(iEntityStats::Swordsman, giantPos_Game);
			m_pPlayer->placeMob(iEntityStats::Swordsman, giantPos_Game);
			Vec2 pos = place1.Player2Game(isNorth);
			m_pPlayer->placeMob(iEntityStats::Archer, pos);
			m_pPlayer->placeMob(iEntityStats::Archer, pos);
		}


	}
	//The AI takes actions if opposite does not take any actions after getting 15 Elixirs.
	else if (m_pPlayer->getElixir() >= 15) {
		start = true;
		//first time to spawn units.
		Vec2 pos = ksGiantPosL.Player2Game(isNorth);
		m_pPlayer->placeMob(iEntityStats::Giant, pos);
		pos = place1.Player2Game(isNorth);


	}
}



