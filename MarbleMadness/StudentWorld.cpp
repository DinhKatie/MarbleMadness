#include "StudentWorld.h"
#include "GameConstants.h"
#include "Actor.h"
#include <string>
#include <sstream>
#include <iomanip>
#include <iostream>
using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
    return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h, and Actor.cpp

StudentWorld::StudentWorld(string assetPath)
    : GameWorld(assetPath)
{
    m_player = nullptr;
    m_timebonus = 1000;
    m_levelFinished = false;
}

StudentWorld::~StudentWorld() {
    cleanUp();
}

int StudentWorld::PlayerX() const { return m_player->getX();  }

int StudentWorld::PlayerY() const { return m_player->getY();  }

void StudentWorld::restoreHealth() { m_player->restoreHealth(); }

bool StudentWorld::checkForCrystals() const 
{
    for (int i = 0; i < m_actors.size(); i++)
    {
        if (m_actors[i]->countsForLevelFinish())
            return true;
    }
    return false;
}

Actor* StudentWorld::isAt(double x, double y, int type) const
{
    if (PlayerX() == x && PlayerY() == y)
        return m_player;
    if (type == 1) //Check for actors that peas can collide with
    {
        for (int i = 0; i < m_actors.size(); i++)
        {
            if (m_actors[i]->getX() == x && m_actors[i]->getY() == y && m_actors[i]->isCollide())
                return m_actors[i];
        }
    }
    //General check for actors
    else if (type == 2)
    {
        for (int i = 0; i < m_actors.size(); i++)
        {
            if (m_actors[i]->getX() == x && m_actors[i]->getY() == y)
                return m_actors[i];
        }
    }
    else if (type == 4) //Check for goodies only
    {
        for (int i = 0; i < m_actors.size(); i++)
        {
            if (m_actors[i]->getX() == x && m_actors[i]->getY() == y && m_actors[i]->isGoodie())
                return m_actors[i];
        }
    }
    else if (type == 5) //Check for actors that cannot be walked on
    {
        for (int i = 0; i < m_actors.size(); i++)
        {
            if (m_actors[i]->getX() == x && m_actors[i]->getY() == y && !m_actors[i]->canBeWalkedOn())
                return m_actors[i];
        }
    }
    return nullptr;
}

Actor* StudentWorld::isAtForPit(int x, int y) const
{
    for (int i = 0; i < m_actors.size(); i++)
    {
        if (m_actors[i]->getX() == x && m_actors[i]->getY() == y && m_actors[i]->canBePushed())
            return m_actors[i];
    }
    return nullptr;
}

bool StudentWorld::ObstacleToRight(int x, int y)
{
    vector<Actor*>::iterator it;
    it = m_actors.begin();
    while (it != m_actors.end())
    {
        //If actor is in the same row and between the player and robot
        if ((*it)->getY() == y && (*it)->getX() < PlayerX() && (*it)->getX() > x &&
            (*it)->isCollide() == true)
        {
            if (*it != m_player)
                return true;
        }
        it++;
    }
    return false;

}

bool StudentWorld::ObstacleToLeft(int x, int y)
{
    vector<Actor*>::iterator it;
    it = m_actors.begin();
    while (it != m_actors.end())
    {
        //If actor is in the same row and between the player and robot on the left
        if ((*it)->getY() == y && (*it)->getX() > PlayerX() && (*it)->getX() < x &&
            (*it)->isCollide() == true)
        {
            if (*it != m_player)
                return true;
        }
        it++;
    }
    return false;

}

bool StudentWorld::ObstacleToBottom(int x, int y)
{
    vector<Actor*>::iterator it;
    it = m_actors.begin();
    while (it != m_actors.end())
    {
        //If actor is in the same column and blocking player and robot, return true
        if ((*it)->getX() == x && (*it)->getY() > PlayerY() && (*it)->getY() < y &&
            (*it)->isCollide() == true)
        {
            if (*it != m_player)
                return true;
        }
        it++;
    }
    return false;
}

bool StudentWorld::ObstacleToTop(int x, int y)
{
    vector<Actor*>::iterator it;
    it = m_actors.begin();
    while (it != m_actors.end())
    {
        //If actor is in the same column and blocking player and robot, return true
        if ((*it)->getX() == x && (*it)->getY() < PlayerY() && (*it)->getY() > y &&
            (*it)->isCollide() == true)
        {
            if (*it != m_player)
                return true;
        }
        it++;
    }
    return false;
}

void StudentWorld::goodiePickedUp(Actor* a)
{
    for (int i = 0; i < m_actors.size(); i++)
    {
        if (m_actors[i] == a)
            m_actors.erase(m_actors.begin() + i);
    }
}

void StudentWorld::addPea(int x, int y, int dir, bool shouldWait)
{
    Pea* p = new Pea(x, y, dir, this);
    if (shouldWait)
        p->setWait();
    m_actors.push_back(p);
}

void StudentWorld::addThiefBot(int x, int y, int type)
{
    if (type == 0)
    {
        RegularThiefBot* tb = new RegularThiefBot(x, y, this, getLevel());
        m_actors.insert(m_actors.begin(), tb);
    }
    else if (type == 1)
    {
        MeanThiefBot* tb = new MeanThiefBot(x, y, this, getLevel());
        m_actors.insert(m_actors.begin(), tb);
    }
    playSound(SOUND_ROBOT_BORN);
}

string formatString(int score, int level, int lives, double health, int ammo, int bonus)
{
    ostringstream oss;
    oss.setf(ios::fixed);
    oss << "Score: ";
    oss.fill('0');
    oss << setw(7) << score << "  ";
    oss << "Level: " << setw(2) << level << "  ";
    oss.fill(' ');
    oss << "Lives: " << lives << "  ";
    oss << setprecision(0);
    oss << "Health: " << setw(3) << health << "%  ";
    oss << "Ammo: " << ammo << "  ";
    oss << "Bonus: " << setw(4) << bonus << "  ";

    return oss.str();
}

void StudentWorld::setDisplayText()
{
    int score = getScore();
    int level = getLevel();
    int lives = getLives();
    double health = (m_player->currentHealth() / m_player->m_maxHealth) * 100; 
    int ammo = m_player->currentAmmo();
    int bonus = getBonus();
    string s = formatString(score, level, lives, health, ammo, bonus);
    setGameStatText(s);
}

string StudentWorld::getNextLevel()
{
    ostringstream oss;
    if (getLevel() > 99)
        return "level99";
    oss << "level";
    oss.fill('0');
    oss << setw(2) << getLevel() << ".txt";
    return oss.str();
}

int StudentWorld::init()
{
    m_timebonus = 1000;
    Level lev(assetPath());
    string s = getNextLevel();
    Level::LoadResult result = lev.loadLevel(s);
    if (result == Level::load_fail_file_not_found || s == "level99")
        return GWSTATUS_PLAYER_WON;
    else if (result == Level::load_fail_bad_format)
        return GWSTATUS_LEVEL_ERROR;
    else if (result == Level::load_success)
    {
        cerr << "Successfully loaded level\n";
        for (int x = 0; x < VIEW_HEIGHT; x++) {
            for (int y = 0; y < VIEW_WIDTH; y++)
            {
                Level::MazeEntry ge = lev.getContentsOf(x, y); // x=5 column 5, y=10 row 10
                switch (ge)
                {
                case Level::empty:
                    break;
                case Level::exit:
                {
                    Exit* exit = new Exit(x, y, this);
                    m_actors.push_back(exit);
                    break;
                }
                case Level::player:
                {
                    m_player = new Player(x, y, this);
                    break;
                }
                case Level::marble:
                {
                    Marble* m = new Marble(x, y, this);
                    m_actors.push_back(m);
                    break;
                }
                case Level::crystal:
                {
                    Crystal* c = new Crystal(x, y, this);
                    m_actors.push_back(c);
                    break;
                }
                case Level::pit:
                {
                    Pit* p = new Pit(x, y, this);
                    m_actors.push_back(p);
                    break;
                }
                case Level::ammo:
                {
                    AmmoGoodie* a = new AmmoGoodie(x, y, this);
                    m_actors.push_back(a);
                    break;
                }
                case Level::extra_life:
                {
                    ExtraLifeGoodie* el = new ExtraLifeGoodie(x, y, this);
                    m_actors.push_back(el);
                    break;
                }
                case Level::restore_health:
                {
                    RestoreHealth* rh = new RestoreHealth(x, y, this);
                    m_actors.push_back(rh);
                    break;
                }
                case Level::horiz_ragebot:
                {
                    HorizRageBot* h = new HorizRageBot(x, y, this, getLevel());
                    m_actors.push_back(h);
                    break;
                }
                case Level::vert_ragebot:
                {
                    VertRageBot* v = new VertRageBot(x, y, this, getLevel());
                    m_actors.push_back(v);
                    break;
                }
                case Level::thiefbot_factory:
                {
                    ThiefBotFactory* thf = new ThiefBotFactory(x, y, 0, this);
                    m_actors.push_back(thf);
                    break;
                }
                case Level::mean_thiefbot_factory:
                {
                    ThiefBotFactory* thf = new ThiefBotFactory(x, y, 1, this);
                    m_actors.push_back(thf);
                    break;
                }
                case Level::wall:
                    Wall* w = new Wall(x, y, this);
                    m_actors.push_back(w);
                    break;
                }

            }
        }
    }
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    setDisplayText();
    // Loop through all actors and do something
    for (int i = 0; i < m_actors.size(); i++)
    {
        m_actors[i]->doSomething();
        if (m_levelFinished)
        {
            increaseScore(m_timebonus);
            increaseScore(2000);
            playSound(SOUND_FINISHED_LEVEL);
            m_levelFinished = false;
            return GWSTATUS_FINISHED_LEVEL;
        }
    }
    m_player->doSomething();
    if (m_player->isAlive() == false)
    {
        playSound(SOUND_PLAYER_DIE);
        decLives();
        return GWSTATUS_PLAYER_DIED;
    }
    if (m_timebonus != 0)
        m_timebonus--;

    //Delete actors that die at the end of this tick
    for (int i = 0; i < m_actors.size(); i++)
    {
        if (m_actors[i]->isAlive() == false)
        {
            //When Thiefbots die, drop the goodie at their location
            if (m_actors[i]->countsInFactoryCensus() && m_actors[i]->getGood() != nullptr)
            {
                Actor* goodie = m_actors[i]->getGood();
                m_actors.push_back(goodie);
                goodie->moveTo(m_actors[i]->getX(), m_actors[i]->getY());
                goodie->setVisible(true);
            }
            delete m_actors[i];
            m_actors.erase(m_actors.begin() + i);
        }
    }

    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
    vector<Actor*>::iterator it;
    it = m_actors.begin();
    while (it != m_actors.end())
    {
        //If the actor is a thiefbot and holding a goodie, but it hasn't died yet to drop the goodie
        //Then make sure to delete that goodie as well
        if ((*it)->countsInFactoryCensus() && (*it)->getGood() != nullptr)
            delete (*it)->getGood();
        delete* it;
        it = m_actors.erase(it);
    }

    if (m_player != nullptr)
    {
        delete m_player;
        m_player = nullptr;
    }
}