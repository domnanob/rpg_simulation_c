#ifndef _ENTITY_H_
#define	_ENTITY_H_

#include <stdio.h>
#include <stdlib.h> 
#include <sys/time.h> 

//------- Structs --------

enum Entity_type {Ranged = 1, Meele = 2, Troll = 3, Dragon = 4, Bandit = 5, Dummy = 99};
typedef struct ENTITY
{
    enum Entity_type type;
    int health;
    int power;
    int accuracy;
}ENTITY;
typedef struct ENEMY
{
    union {
        ENTITY entity;
    } base;
} ENEMY;
typedef struct CHARACTER
{
    union {
        ENTITY entity;
    } base;
} CHARACTER;
//-----------------------

//------- Macros --------

#define GETENTITY(self) (&(self)->base.entity)
#define type(self) (GETENTITY(self)->type)
#define power(self) (GETENTITY(self)->power)
#define health(self) (GETENTITY(self)->health)
#define accuracy(self) (GETENTITY(self)->accuracy)
#define printEntity(self) (printf("%s (health: %i, power: %i, accuracy: %i)\n", typename(type(self)), health(self), power(self), accuracy(self))) 
#define aim(self) (accuracy(self) <= random_number(100))
#define alive(self) (health(self) > 0)
#define attack(who, whom) {\
    GETENTITY(whom)->health -= GETENTITY(who)->power; \
    printf("\t~ %s attacked %s!\n", typename(type(who)), typename(type(whom))); \
    if (type(who) == 1 || type(who) == 2) { \
        accuracy(who) += 2; \
        printf("\t~ %s accuracy buffed by 2!\n", typename(type(who))); \
    } \
}

//-------------------------------
char* typename(int type)
{
    switch (type)
    {
      case 1:
        return "Ranged";
        break;
      case 2:
        return "Meele";
        break;
      case 3:
        return "Troll";
        break;
      case 4:
        return "Dragon";
        break;
      case 5:
        return "Bandit";
        break;
    }
    return "Entity";
}

int random_number(int max){
    srand(time(NULL));
    return rand() % max+1;
}
int random_number_between(int min, int max)
{
    srand(time(NULL));
    return rand() % (max + 1 - min) + min;
}
void resetCharacterAccuracy(CHARACTER* c) {
    if (type(c) == Ranged)
    {
      accuracy(c) = 55;
    } else if (type(c) == Meele)
    {
      accuracy(c) = 40;
    }
    printf("\t~ %s accuracy reset to default!\n", typename(type(c)));
}
void healCharacter(CHARACTER* c, int amount) {
    if (health(c) + amount <= 100)
    {
      health(c) = health(c) + amount;
    } else {
      health(c) = 100;
    }
    printf("\t~ %s healed by %d%%!\n", typename(type(c)), amount);
}
//------- "Constructors" --------

CHARACTER CreateRangedCharacter() { 
  CHARACTER rc;
  type(&rc) = Ranged;
  health(&rc) = 100;
  power(&rc) = 10;
  accuracy(&rc) = 55;
  return rc;
}
CHARACTER CreateMeeleCharacter() { 
  CHARACTER mc;
  type(&mc) = Meele;
  health(&mc) = 100;
  power(&mc) = 15;
  accuracy(&mc) = 40;
  return mc;
}
ENEMY CreateTrollEnemy() { 
  ENEMY te;
  type(&te) = Troll;
  health(&te) = 50;
  power(&te) = 70;
  accuracy(&te) = 10;
  return te;
}
ENEMY CreateDragonEnemy() { 
  ENEMY de;
  type(&de) = Dragon;
  health(&de) = 40;
  power(&de) = 50;
  accuracy(&de) = 50;
  return de;
}
ENEMY CreateBanditEnemy() { 
  ENEMY be;
  type(&be) = Bandit;
  health(&be) = 20;
  power(&be) = 10;
  accuracy(&be) = 30;
  return be;
}
ENEMY CreateDummyEnemy() { 
  ENEMY de;
  type(&de) = Dummy;
  health(&de) = 1;
  power(&de) = 0;
  accuracy(&de) = 0;
  return de;
}

#endif

/*
    Referenciák:
    https://stackoverflow.com/questions/7109964/creating-your-own-header-file-in-c
    http://www.deleveld.dds.nl/inherit.htm
    https://stackoverflow.com/questions/3774193/constructor-for-structs-in-c
    https://stackoverflow.com/questions/39821164/how-do-you-define-a-multiline-macro-in-c
    https://stackoverflow-com.translate.goog/questions/14849866/c-rand-is-not-really-random?_x_tr_sl=en&_x_tr_tl=hu&_x_tr_hl=hu&_x_tr_pto=sc
    https://www.geeksforgeeks.org/pass-the-value-from-child-process-to-parent-process/
*/