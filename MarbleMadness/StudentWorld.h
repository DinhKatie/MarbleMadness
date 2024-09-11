#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "Level.h"
#include <string>
#include <vector>


// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp
class Actor;
class Player;
class Pea;
class ThiefBot;
class StudentWorld : public GameWorld
{
public:
	StudentWorld(std::string assetPath);
	virtual ~StudentWorld();
	virtual int init();
	virtual int move();
	virtual void cleanUp();

	//Check if actor is at a certain location, types specify certain conditions
	Actor* isAt(double x, double y, int type = 1) const;

	//Check for actors that can be swallowed
	Actor* isAtForPit(int x, int y) const;

	bool checkForCrystals() const;

	void addPea(int x, int y, int dir, bool shouldWait = false);

	void addThiefBot(int x, int y, int type = 0);

	//Check if the way is clear before a robot shoots
	bool ObstacleToRight(int x, int y);
	bool ObstacleToLeft(int x, int y);
	bool ObstacleToBottom(int x, int y);
	bool ObstacleToTop(int x, int y);

	//What goodie did a [Thiefbot] pick up?
	void goodiePickedUp(Actor* a);

	//Restore the health of the player
	void restoreHealth();

	//Points & levels
	inline int getBonus() const { return m_timebonus; }

	inline void awardPoints(int points) { increaseScore(points); }

	inline void finishedLevel() { m_levelFinished = true; }

	//Player getter functions
	int PlayerX() const;
	int PlayerY() const;
	inline Player* getPlayer() const { return m_player; }

private:
	void setDisplayText();
	std::string getNextLevel();

	std::vector<Actor*> m_actors;
	Player* m_player;
	int m_timebonus;
	bool m_levelFinished;
};


#endif // STUDENTWORLD_H_