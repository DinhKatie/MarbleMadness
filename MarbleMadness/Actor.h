#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp ASK ABOUT VIRTUAL DESTRUCTORS
class StudentWorld;
class Actor : public GraphObject {
public:
	Actor(int imageID, int startX, int startY, int startDirection, bool walkOn, StudentWorld* sw) :
		GraphObject(imageID, startX, startY, startDirection),
		m_isAlive(true), m_world(sw), m_canBeWalkedOn(walkOn), m_isCollide(false), m_isGoodie(false)
	{
		setVisible(true);
	}

	virtual void doSomething() = 0;
	//Get the goodie a ThiefBot is holding
	inline virtual Actor* getGood() { return nullptr; }

	//Lower an actor's health, appropriate to their type
	inline virtual void damage() { return; }

	//Push a marble
	inline virtual bool push(int dir) { return false; }

	//Check if actor should wait a tick (typically for peas to avoid activating doSomething()
	//On the same tick they are spawned
	inline virtual bool checkWait() { return false; }

	//Checks alive status
	inline bool isAlive()
	{
		return m_isAlive;
	}

	//Set alive status
	inline void setAlive(bool alive)
	{
		m_isAlive = alive;
	}

	//Can this actor be walked on (goodies)
	inline bool canBeWalkedOn()
	{
		return m_canBeWalkedOn;
	}

	//Get the actor's world
	inline StudentWorld* getWorld()
	{
		return m_world;
	}

	//Does this actor count toward the exit being revealed?
	inline virtual bool countsForLevelFinish() { return false; }

	//Can this actor be pushed?
	inline virtual bool canBePushed() { return false; }

	//Does this actor count in the factory census (Thiefbots)
	inline virtual bool countsInFactoryCensus() { return false; }

	//Does this actor allow marbles on top of it (pits)
	inline virtual bool allowsMarbleOnTop() { return false; }

	//Is this actor a goodie?
	inline virtual bool isGoodie() { return false; }

	//Set goodie status (when Thiefbots steal goodies, to avoid other thiefbots from picking up the same object)
	inline virtual void setGoodie(bool g) { m_isGoodie = false; }

	//Do peas collide with this object
	inline bool isCollide()
	{
		if (m_isCollide)
			return true;
		return false;
	}

	//Change pea collide status
	inline void setCollide(bool c)
	{
		m_isCollide = c;
	}

private:
	bool m_isAlive;
	bool m_canBeWalkedOn;
	bool m_isCollide;
	bool m_isGoodie;
	StudentWorld* m_world;
};

class Wall : public Actor {
public:
	Wall(int startX, int startY, StudentWorld* sw) :
		Actor(IID_WALL, startX, startY, none, false, sw) {
		setCollide(true);
	}

	virtual void doSomething() { return; }
};

class Player : public Actor {
public:
	Player(int startX, int startY, StudentWorld* sw) :
		Actor(IID_PLAYER, startX, startY, right, false, sw),
		m_health(20), m_peas(20), m_maxHealth(20) {
		setCollide(true);
	}

	virtual void doSomething();
	virtual void damage();

	//Get current health
	inline int currentHealth() { return m_health; }

	//Get current ammo count
	inline int currentAmmo() { return m_peas; }

	//Add to ammo count
	inline void addAmmo(int ammo) { m_peas += ammo; }

	//Restore health to full
	inline void restoreHealth() { m_health = m_maxHealth; }

public:
	double m_maxHealth;
	int m_health;
	int m_peas;
};

class Exit : public Actor {
public:
	Exit(int startX, int startY, StudentWorld* sw) :
		Actor(IID_EXIT, startX, startY, none, true, sw)
	{
		setVisible(false);
		m_revealed = false;
	}
	virtual void doSomething();
private:
	bool m_revealed;
};

class Goodie : public Actor
{
public:
	Goodie(int imageID, int startX, int startY, StudentWorld* sw) :
		Actor(imageID, startX, startY, none, true, sw), m_points(0) {}

	virtual void doSomething();
	virtual void doEffect() = 0;
	inline void setPoints(int points) { m_points = points; }
	inline virtual bool isGoodie() { return true; }
private:
	int m_points;
};

class Crystal : public Goodie {
public:
	Crystal(int startX, int startY, StudentWorld* sw) :
		Goodie(IID_CRYSTAL, startX, startY, sw) {
		setPoints(50);
	}
	inline virtual void doEffect() { return; }
	inline virtual bool countsForLevelFinish() { return true; }
	inline virtual bool isGoodie() { return false; }
};

class Pea : public Actor {
public:
	Pea(int startX, int startY, int direction, StudentWorld* sw) :
		Actor(IID_PEA, startX, startY, direction, true, sw), m_waitUntilNextTick(false) {}

	virtual void doSomething();
	inline void setWait() { m_waitUntilNextTick = true; }
private:
	bool m_waitUntilNextTick;
};

class Marble : public Actor
{
public:
	Marble(int startX, int startY, StudentWorld* sw) :
		Actor(IID_MARBLE, startX, startY, none, false, sw) {
		m_health = 10;
		setCollide(true);
	}
	virtual void doSomething() { return; }
	inline virtual void damage()
	{
		m_health -= 2;
		if (m_health <= 0)
			setAlive(false);
	}
	inline virtual bool canBePushed() { return true; }
	virtual bool push(int dir);

private:
	int m_health;
};

class AmmoGoodie : public Goodie {
public:
	AmmoGoodie(int startX, int startY, StudentWorld* sw) :
		Goodie(IID_AMMO, startX, startY, sw) {
		setPoints(100);
	}
	virtual void doEffect();
};

class ExtraLifeGoodie : public Goodie {
public:
	ExtraLifeGoodie(int startX, int startY, StudentWorld* sw) :
		Goodie(IID_EXTRA_LIFE, startX, startY, sw) {
		setPoints(1000);
	}
	virtual void doEffect();
};

class RestoreHealth : public Goodie {
public:
	RestoreHealth(int startX, int startY, StudentWorld* sw) :
		Goodie(IID_RESTORE_HEALTH, startX, startY, sw) {
		setPoints(500);
	}
	virtual void doEffect();
};

class Robot : public Actor
{
public:
	Robot(int imageID, int startX, int startY, int dir, int health, StudentWorld* sw, int level) :
		Actor(imageID, startX, startY, dir, false, sw), m_health(health), m_currentTick(0)
	{
		m_ticksBeforeTurn = (28 - level) / 4;
		if (m_ticksBeforeTurn < 3)
			m_ticksBeforeTurn = 3;
		setCollide(true);
	}
	virtual void doSomething() = 0;
	virtual void damage();

	inline void incrementTick() { m_currentTick++; }
	inline bool checkTick() { //Returns true if robot should move this turn
		if (m_currentTick == m_ticksBeforeTurn)
		{
			m_currentTick = 0;
			return true;
		}
		return false;
	}
	//Returns true if pea was shot
	bool shootPea();
private:
	int m_health;
	int m_ticksBeforeTurn;
	int m_currentTick;
};

class RageBot : public Robot
{
public:
	RageBot(int startX, int startY, int dir, StudentWorld* sw, int level) :
		Robot(IID_RAGEBOT, startX, startY, dir, 10, sw, level) {}

	virtual void doSomething() = 0;
};

class HorizRageBot : public RageBot {
public:
	HorizRageBot(int startX, int startY, StudentWorld* sw, int level) :
		RageBot(startX, startY, right, sw, level) {}

	virtual void doSomething();
};

class VertRageBot : public RageBot {
public:
	VertRageBot(int startX, int startY, StudentWorld* sw, int level) :
		RageBot(startX, startY, down, sw, level) {}

	virtual void doSomething();
};

class Pit : public Actor {
public:
	Pit(int startX, int startY, StudentWorld* sw) :
		Actor(IID_PIT, startX, startY, none, false, sw) {}

	virtual void doSomething();
	inline virtual bool allowsMarbleOnTop() { return true; }
};

class ThiefBot : public Robot {
public:
	ThiefBot(int imageID, int startX, int startY, StudentWorld* sw, int dir, int health, int level) :
		Robot(imageID, startX, startY, dir, health, sw, level), m_hasPickedUpGoodie(nullptr)
	{
		m_distanceBeforeTurning = randInt(1, 6);
	}
	virtual bool countsInFactoryCensus() { return true; }
	virtual inline Actor* getGood() { return m_hasPickedUpGoodie; }
	inline void setPickedUpGoodie(Actor* g) { m_hasPickedUpGoodie = g; }

	inline void decDistance() { m_distanceBeforeTurning--; }
	inline void setDistance(int d) { m_distanceBeforeTurning = d; }
	inline int disBeforeTurn() { return m_distanceBeforeTurning; }

	virtual void doSomething();

private:
	int findNewDirection();
	inline virtual bool shouldIncrement() { return true; };
	Actor* m_hasPickedUpGoodie;
	int m_distanceBeforeTurning;
};

class RegularThiefBot : public ThiefBot {
public:
	RegularThiefBot(int startX, int startY, StudentWorld* sw, int level) :
		ThiefBot(IID_THIEFBOT, startX, startY, sw, right, 5, level) {}

};

class MeanThiefBot : public ThiefBot {
public: 
	MeanThiefBot(int startX, int startY, StudentWorld* sw, int level) :
		ThiefBot(IID_MEAN_THIEFBOT, startX, startY, sw, right, 8, level) {}

	virtual void doSomething();
	inline virtual bool shouldIncrement() { return false; }

};

class ThiefBotFactory : public Actor {
public:
	ThiefBotFactory(int startX, int startY, int type, StudentWorld* sw) :
		Actor(IID_ROBOT_FACTORY, startX, startY, none, false, sw) 
	{
		m_typeOfBot = type;
		setCollide(true);
	}
	virtual void doSomething();
private:
	int m_typeOfBot; //0 is Normal Thief, 1 is Mean
};
#endif // ACTOR_H_