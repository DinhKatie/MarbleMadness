#include "Actor.h"
#include "StudentWorld.h"
#include <iostream>
using namespace std;

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp
void Player::doSomething()
{
	//User inputs
	int ch;
	if (getWorld()->getKey(ch))
	{
		// user hit a key this tick!
		switch (ch)
		{
		case KEY_PRESS_LEFT:
		{
			setDirection(left);
			Actor* a = getWorld()->isAt(getX() - 1, getY(), 2);
			if (a == nullptr || a->canBeWalkedOn()) //Move if spot is clear
				moveTo(getX() - 1, getY());
			else if (a->canBePushed()) //If marble, then check for push
			{
				if (a->push(left)) //Push and move to location
					moveTo(getX() - 1, getY());
			}
			break;

		}
		case KEY_PRESS_RIGHT:
		{
			setDirection(right);
			Actor* a = getWorld()->isAt(getX() + 1, getY(), 2);
			if (a == nullptr || a->canBeWalkedOn()) //Move if spot is clear
				moveTo(getX() + 1, getY());
			else if (a->canBePushed()) //If marble, then check for push
			{
				if (a->push(right)) //Push and move to location
					moveTo(getX() + 1, getY());
			}
			break;

		}
		case KEY_PRESS_DOWN:
		{
			setDirection(down);
			Actor* a = getWorld()->isAt(getX(), getY() - 1, 2);
			if (a == nullptr || a->canBeWalkedOn()) //Move if spot is clear
				moveTo(getX(), getY() - 1);
			else if (a->canBePushed()) //If marble, then check for push
			{
				if (a->push(down)) //Push and move to location
					moveTo(getX(), getY() - 1);
			}
			break;
		}
		case KEY_PRESS_UP:
		{
			setDirection(up);
			Actor* a = getWorld()->isAt(getX(), getY() + 1, 2);
			if (a == nullptr || a->canBeWalkedOn()) //Move if spot is clear
				moveTo(getX(), getY() + 1);
			else if (a->canBePushed()) //If marble, then check for push
			{
				if (a->push(up)) //Push and move to location
					moveTo(getX(), getY() + 1);
			}
			break;
		}
		case KEY_PRESS_ESCAPE:
			setAlive(false);
			break;
			
		case KEY_PRESS_SPACE:
		{
			if (m_peas <= 0)
				break;
			m_peas--;
			int dir = getDirection();
			Pea* p;
			switch (dir)
			{
			case right:
				getWorld()->addPea(getX() + 1, getY(), dir);
				break;
			case left:
				getWorld()->addPea(getX() - 1, getY(), dir);
				break;
			case down:
				getWorld()->addPea(getX(), getY() - 1, dir);
				break;
			case up:
				getWorld()->addPea(getX(), getY() + 1, dir);
				break;
			}
			getWorld()->playSound(SOUND_PLAYER_FIRE);
		}
		break;
		}
	}

}

void Player::damage()
{
	m_health -= 2;
	if (m_health > 0)
		getWorld()->playSound(SOUND_PLAYER_IMPACT);
	else
		setAlive(false);
}

void Exit::doSomething()
{
	//If there are no crystals on the map and the exit is hidden
	if (getWorld()->checkForCrystals() == false && m_revealed == false)
	{
		getWorld()->playSound(SOUND_REVEAL_EXIT);
		setVisible(true);
		m_revealed = true;
	}

	if (getWorld()->PlayerX() == getX() && getWorld()->PlayerY() == getY()
		&& m_revealed == true)
		getWorld()->finishedLevel();
}

void Pea::doSomething()
{
	if (!isAlive())
		return;
	if (m_waitUntilNextTick)
	{
		m_waitUntilNextTick = false;
		return;
	}
	//Check if something is on same square as pea, and if it is damageable
	//Damage the object and do nothing else
	Actor* actor = getWorld()->isAt(getX(), getY());
	if (actor != nullptr)
	{
		actor->damage();
		setAlive(false);
		return;
	}
	int dir = getDirection();
	switch (dir)
	{
	case right:
		moveTo(getX() + 1, getY());
		break;
	case left:
		moveTo(getX() - 1, getY());
		break;
	case down:
		moveTo(getX(), getY() - 1);
		break;
	case up:
		moveTo(getX(), getY() + 1);
		break;
	}
	actor = getWorld()->isAt(getX(), getY());
	if (actor != nullptr)
	{
		actor->damage();
		setAlive(false);
	}
}

void Goodie::doSomething()
{
	if (!isAlive())
		return;
	if (getWorld()->PlayerX() == getX() && getWorld()->PlayerY() == getY())
	{
		getWorld()->awardPoints(m_points);
		setAlive(false);
		getWorld()->playSound(SOUND_GOT_GOODIE);
		doEffect();
	}
}

void AmmoGoodie::doEffect()
{
	getWorld()->getPlayer()->addAmmo(20);
}

void ExtraLifeGoodie::doEffect()
{
	getWorld()->incLives();
}

void RestoreHealth::doEffect()
{
	getWorld()->restoreHealth();
}

bool Marble::push(int dir)
{
	switch (dir)
	{
	case right:
	{
		Actor* a = getWorld()->isAt(getX() + 1, getY(), 2);
		//If there is no actor on x,y or if the actor there allows a marble on top of it
		//Then move the marble to that location
		if (a == nullptr || a->allowsMarbleOnTop())
		{
			moveTo(getX() + 1, getY());
			return true;
		}
		break;
	}
	case left:
	{
		Actor* a = getWorld()->isAt(getX() - 1, getY(), 2);
		if (a == nullptr || a->allowsMarbleOnTop())
		{
			moveTo(getX() - 1, getY());
			return true;
		}
		break;
	}
	case down:
	{
		Actor* a = getWorld()->isAt(getX(), getY() - 1, 2);
		if (a == nullptr || a->allowsMarbleOnTop())
		{
			moveTo(getX(), getY() - 1);
			return true;
		}
		break;
	}
	case up:
	{
		Actor* a = getWorld()->isAt(getX(), getY() + 1, 2);
		if (a == nullptr || a->allowsMarbleOnTop())
		{
			moveTo(getX(), getY() + 1);
			return true;
		}
		break;
	}
	}
	return false;
}

void Pit::doSomething()
{
	if (!isAlive())
		return;
	Actor* a = getWorld()->isAtForPit(getX(), getY());
	if (a != nullptr)
	{
		setAlive(false);
		a->setAlive(false);
	}
}

void Robot::damage()
{
	m_health -= 2;
	if (m_health <= 0)
	{
		setAlive(false);
		getWorld()->playSound(SOUND_ROBOT_DIE);
		getWorld()->awardPoints(100);
	}
	else
		getWorld()->playSound(SOUND_ROBOT_IMPACT);
}

bool Robot::shootPea()
{
	int dir = getDirection();
	StudentWorld* sw = getWorld();
	if (dir == right && !sw->ObstacleToRight(getX(), getY()) &&
		sw->PlayerY() == getY() && sw->PlayerX() > getX())
	{
		sw->playSound(SOUND_ENEMY_FIRE);
		sw->addPea(getX() + 1, getY(), dir, true);
		return true;
	}
	else if (dir == left && !sw->ObstacleToLeft(getX(), getY()) &&
		sw->PlayerY() == getY() && sw->PlayerX() < getX())
	{
		sw->playSound(SOUND_ENEMY_FIRE);
		sw->addPea(getX() - 1, getY(), dir, true);
		return true;
	}
	//If robot is facing down, and there are no obstacles and robot is facing player
	else if (dir == down && !sw->ObstacleToBottom(getX(), getY()) &&
		sw->PlayerX() == getX() && sw->PlayerY() < getY())
	{
		sw->playSound(SOUND_ENEMY_FIRE);
		sw->addPea(getX(), getY() - 1, dir, true);
		return true;
	}
	//If robot is facing up, and there are no obstacles and robot is facing player
	else if (dir == up && !sw->ObstacleToTop(getX(), getY()) &&
		sw->PlayerX() == getX() && sw->PlayerY() > getY())
	{
		sw->playSound(SOUND_ENEMY_FIRE);
		sw->addPea(getX(), getY() + 1, dir, true);
		return true;
	}
	return false;
}

void HorizRageBot::doSomething()
{
	StudentWorld* sw = getWorld();
	int dir = getDirection();
	if (!isAlive())
		return;
	if (checkTick() == false)
	{
		incrementTick();
		return;
	}
	else if (shootPea())
		return;
	else //If no player, then just move
	{
		Actor* a;
		if (dir == right)
		{
			//Check for actors at next location
			a = getWorld()->isAt(getX() + 1, getY(), 5);
			if (a == nullptr || a->canBeWalkedOn())
				moveTo(getX() + 1, getY());
			else
				setDirection(left);
		}
		if (dir == left)
		{
			a = getWorld()->isAt(getX() - 1, getY(), 5);
			if (a == nullptr || a->canBeWalkedOn())
				moveTo(getX() - 1, getY());
			else
				setDirection(right);
		}
	}
}

void VertRageBot::doSomething()
{
	StudentWorld* sw = getWorld();
	int dir = getDirection();
	if (!isAlive())
		return;
	if (checkTick() == false)
	{
		incrementTick();
		return;
	}
	//If robot is facing down, and there are no obstacles and robot is facing player
	else if (shootPea())
		return;
	else //If no player, then just move
	{
		Actor* a;
		if (dir == down)
		{
			//Check for actors at next location
			a = getWorld()->isAt(getX(), getY() - 1, 5);
			if (a == nullptr || a->canBeWalkedOn())
				moveTo(getX(), getY() - 1);
			else
				setDirection(up);
		}
		if (dir == up)
		{
			a = getWorld()->isAt(getX(), getY() + 1, 5);
			if (a == nullptr || a->canBeWalkedOn())
				moveTo(getX(), getY() + 1);
			else
				setDirection(down);
		}

	}
}

void ThiefBot::doSomething()
{
	int dir = getDirection();
	if (!isAlive())
		return;
	if (checkTick() == false && shouldIncrement())
	{
		incrementTick();
		return;
	}
	Actor* a;
	//Attempt to pick up a goodie
	a = getWorld()->isAt(getX(), getY(), 4);
	if (a != nullptr && getGood() == nullptr)
	{
		if (randInt(1, 10) == 1)
		{
			setPickedUpGoodie(a);
			a->setVisible(false);
			a->setGoodie(false);
			getWorld()->goodiePickedUp(a);
			getWorld()->playSound(SOUND_ROBOT_MUNCH);
			return;
		}
	}
	if (disBeforeTurn() > 0)
	{
		switch (dir)
		{
		case right:
		{
			a = getWorld()->isAt(getX() + 1, getY(), 5);
			if (a == nullptr || a->canBeWalkedOn())
			{
				moveTo(getX() + 1, getY());
				decDistance();
				return;
			}
			break;
		}
		case left:
		{
			a = getWorld()->isAt(getX() - 1, getY(), 5);
			if (a == nullptr || a->canBeWalkedOn())
			{
				moveTo(getX() - 1, getY());
				decDistance();
				return;
			}
			break;
		}
		case up:
		{
			a = getWorld()->isAt(getX(), getY() + 1, 5);
			if (a == nullptr || a->canBeWalkedOn())
			{
				moveTo(getX(), getY() + 1);
				decDistance();
				return;
			}
			break;
		}
		case down:
		{
			a = getWorld()->isAt(getX(), getY() - 1, 5);
			if (a == nullptr || a->canBeWalkedOn())
			{
				moveTo(getX(), getY() - 1);
				decDistance();
				return;
			}
			break;
		}
		}
	}
	//If reaches here, robot has traveled distanceBeforeTurning or there is an obstruction
	int newdir = findNewDirection();
	if (newdir != 0)
	{
		switch (newdir) {
		case 1:
			setDirection(right);
			break;
		case 2:
			setDirection(left);
			break;
		case 3:
			setDirection(up);
			break;
		case 4:
			setDirection(down);
			break;
		}
	}
	
}

int ThiefBot::findNewDirection()
{
	setDistance(randInt(1, 6));
	int directions[4];
	int size = 0;
	int newdir;
	while (size != 4)
	{
		bool again = false;
		newdir = randInt(1, 4);
		for (int i = 0; i < size; i++)
		{
			if (newdir == directions[i])
				again = true;
		}
		if (again)
		{
			again = false;
			continue;
		}
		Actor* a;
		switch (newdir)
		{
		case 1:
		{
			a = getWorld()->isAt(getX() + 1, getY(), 5);
			if (a == nullptr || a->canBeWalkedOn())
			{
				setDirection(right);
				moveTo(getX() + 1, getY());
				return 0;
			}
			else
			{
				directions[size] = newdir;
				size++;
			}
			break;
		}
		case 2:
		{
			a = getWorld()->isAt(getX() - 1, getY(), 5);
			if (a == nullptr || a->canBeWalkedOn())
			{
				setDirection(left);
				moveTo(getX() - 1, getY());
				return 0;
			}
			else
			{
				directions[size] = newdir;
				size++;
			}
			break;
		}
		case 3:
		{
			a = getWorld()->isAt(getX(), getY() + 1, 5);
			if (a == nullptr || a->canBeWalkedOn())
			{
				setDirection(up);
				moveTo(getX(), getY() + 1);
				return 0;
			}
			else
			{
				directions[size] = newdir;
				size++;
			}
			break;
		}
		case 4:
		{
			a = getWorld()->isAt(getX(), getY() - 1, 5);
			if (a == nullptr || a->canBeWalkedOn())
			{
				setDirection(down);
				moveTo(getX(), getY() - 1);
				return 0;
			}
			else
			{
				directions[size] = newdir;
				size++;
			}
			break;
		}
		}
	}
	return newdir;
}

void MeanThiefBot::doSomething()
{
	if (!isAlive())
		return;
	if (checkTick() == false)
	{
		incrementTick();
		return;
	}
	int dir = getDirection();
	StudentWorld* sw = getWorld();
	if (shootPea())
		return;
	else
		ThiefBot::doSomething(); //Deals with moving and stealing goodies
}

void ThiefBotFactory::doSomething()
{
	int origX = getX();
	int origY = getY();
	int count = 0;
	Actor* a;
	for (int x = origX - 3; x <= origX + 3; x++)
	{
		for (int y = origY - 3; y <= origY + 3; y++)
		{
			a = getWorld()->isAt(x, y);
			if (a != nullptr && a->countsInFactoryCensus())
				count++;
		}
	}
	//Check if thiefbot is on top of factory
	a = getWorld()->isAt(origX, origY, 2);
	if (count < 3 && (a == nullptr || a == this))
	{
		if (randInt(1, 50) == 1)
		{
			if (m_typeOfBot == 0) //Producing normal Thiefbots
				getWorld()->addThiefBot(origX, origY);
			else if (m_typeOfBot == 1) //Mean Thiefbots
				getWorld()->addThiefBot(origX, origY, 1);
		}
	}
}