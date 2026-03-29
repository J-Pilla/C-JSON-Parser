# JSON Parser
A lightweight JSON parser for C<br>
<i>This module does not export to C++</i>
## What does it do
The JSON parser can take any correctly formatted JSON file up to any size so long as the memory is available<br>
It returns a JSONList, in which each node contains a JSON, which the user can use to break down into strings
## How it works
JSON can be read as either a JSONObject or JSONArray<br>
JSONObjects use index maps, allowing the user to use keys to retrieve values<br>
JSONArrays use linked lists, requiring an index to be used to retrieve values<br>
Both types include collections of JSONObjects and JSONArrays, and a reference the their parent<br>
The usable values are contained in collections of strings (lists and maps)<br>
The user is required to parse into number types (including booleans)
## Example
```C
#include "JSON.h"

#include <assert.h>
#include <malloc.h>
#include <stdio.h>
#include <string.h>

typedef struct Weapons
{
	char** rHand;
	int rHandCount;
	char** lHand;
	int lHandCount;
} Weapons;

typedef struct Armor
{
	char* helm;
	char* armor;
	char* gauntlets;
	char* leggings;
} Armor;

typedef struct Attributes
{
	int vigor;
	int attunement;
	int endurance;
	int vitality;
	int strength;
	int dexterity;
	int intelligence;
	int faith;
	int luck;
} Attributes;

typedef struct DSIIINPC
{
	int id;
	char* name;
	Weapons weapons;
	Armor armor;
	char** arrows;
	int arrowCount;
	char** bolts;
	int boltCount;
	char** spells;
	int spellCount;
	char** items;
	int itemCount;
	int level;
	Attributes attributes;
} DSIIINPC;

int main()
{
	JSONList list = JSONParse("Dark Souls 3 NPCs.json");

	if (list.length == 0)
	{
		puts("ERROR: Failed to parse JSON");
		printf("press enter to exit . . . ");
		int key = getchar();
		fflush(stdin);
		return 1;
	}

	DSIIINPC* npcs = NULL;
	int length = list.length;
	npcs = (DSIIINPC*)calloc(length, sizeof(DSIIINPC));
	assert(npcs);

	for (int index = 0; index < length; index++)
	{
		DSIIINPC* npc = &npcs[index];

		// object pointers
		const JSONObject* currentObject = JSONListGet(&list, index);
		const JSONObject* weapons = JSONMapGet(&currentObject->objects, "Weapons");
		const JSONObject* armorObject = JSONMapGet(&currentObject->objects, "Armor");
		const JSONObject* attributes = JSONMapGet(&currentObject->objects, "Attributes");

		// array pointers
		const JSONArray* rHand = JSONMapGet(&weapons->arrays, "R-hand Weapons");
		const JSONArray* lHand = JSONMapGet(&weapons->arrays, "L-hand Weapons");
		const JSONArray* arrows = JSONMapGet(&currentObject->arrays, "Arrows");
		const JSONArray* bolts = JSONMapGet(&currentObject->arrays, "Bolts");
		const JSONArray* spells = JSONMapGet(&currentObject->arrays, "Spells");
		const JSONArray* items = JSONMapGet(&currentObject->arrays, "Items");

		// string pointers
		const char* name = SMGetString(&currentObject->values, "Name");
		const char* helm = SMGetString(&armorObject->values, "Helm");
		const char* armor = SMGetString(&armorObject->values, "Armor");
		const char* gauntlets = SMGetString(&armorObject->values, "Gauntlets");
		const char* leggings = SMGetString(&armorObject->values, "Leggings");

		// integer assignment
		npc->id = atoi(SMGetString(&currentObject->values, "ID"));
		npc->level = atoi(SMGetString(&currentObject->values, "Name"));

		// array count assignment
		npc->weapons.rHandCount = rHand->values.length;
		npc->weapons.lHandCount = lHand->values.length;
		npc->arrowCount = arrows->values.length;
		npc->boltCount = bolts->values.length;
		npc->spellCount = spells->values.length;
		npc->itemCount = items->values.length;

		// array allocation
		npc->weapons.rHand = (char**)calloc(npc->weapons.rHandCount, sizeof(char*));
		assert(npc->weapons.rHand);

		npc->weapons.lHand = (char**)calloc(npc->weapons.lHandCount, sizeof(char*));
		assert(npc->weapons.lHand);

		npc->arrows = (char**)calloc(npc->arrowCount, sizeof(char*));
		assert(npc->arrows);

		npc->bolts = (char**)calloc(npc->boltCount, sizeof(char*));
		assert(npc->bolts);

		npc->spells = (char**)calloc(npc->spellCount, sizeof(char*));
		assert(npc->spells);

		npc->items = (char**)calloc(npc->itemCount, sizeof(char*));
		assert(npc->items);

		// attribute assignment
		npc->attributes.vigor = atoi(SMGetString(&attributes->values, "VIG"));
		npc->attributes.attunement = atoi(SMGetString(&attributes->values, "ATT"));
		npc->attributes.endurance = atoi(SMGetString(&attributes->values, "END"));
		npc->attributes.vitality = atoi(SMGetString(&attributes->values, "VIT"));
		npc->attributes.strength = atoi(SMGetString(&attributes->values, "STR"));
		npc->attributes.dexterity = atoi(SMGetString(&attributes->values, "SKL"));
		npc->attributes.intelligence = atoi(SMGetString(&attributes->values, "INT"));
		npc->attributes.faith = atoi(SMGetString(&attributes->values, "FTH"));
		npc->attributes.luck = atoi(SMGetString(&attributes->values, "LCK"));

		// string assignments
		// Name
		size_t size = strlen(name) + 1;
		npc->name = malloc(size);
		assert(npc->name);
		strcpy_s(npc->name, size, name);

		// Armor
		size = strlen(helm) + 1;
		npc->armor.helm = malloc(size);
		assert(npc->armor.helm);
		strcpy_s(npc->armor.helm, size, helm);

		size = strlen(armor) + 1;
		npc->armor.armor = malloc(size);
		assert(npc->armor.armor);
		strcpy_s(npc->armor.armor, size, armor);

		size = strlen(gauntlets) + 1;
		npc->armor.gauntlets = malloc(size);
		assert(npc->armor.gauntlets);
		strcpy_s(npc->armor.gauntlets, size, gauntlets);

		size = strlen(leggings) + 1;
		npc->armor.leggings = malloc(size);
		assert(npc->armor.leggings);
		strcpy_s(npc->armor.leggings, size, leggings);

		// R-hand Weapons
		for (int index = 0; index < npc->weapons.rHandCount; index++)
		{
			const char* weapon = SLGetString(&rHand->values, index);

			size = strlen(weapon) + 1;
			npc->weapons.rHand[index] = malloc(size);
			assert(npc->weapons.rHand[index]);
			strcpy_s(npc->weapons.rHand[index], size, weapon);
		}

		// L-hand Weapons
		for (int index = 0; index < npc->weapons.lHandCount; index++)
		{
			const char* weapon = SLGetString(&lHand->values, index);

			size = strlen(weapon) + 1;
			npc->weapons.lHand[index] = malloc(size);
			assert(npc->weapons.lHand[index]);
			strcpy_s(npc->weapons.lHand[index], size, weapon);
		}

		// Arrows
		for (int index = 0; index < npc->arrowCount; index++)
		{
			const char* arrow = SLGetString(&arrows->values, index);

			size_t size = strlen(arrow) + 1;
			npc->arrows[index] = malloc(size);
			assert(npc->arrows[index]);
			strcpy_s(npc->arrows[index], size, arrow);
		}

		// Bolts
		for (int index = 0; index < npc->boltCount; index++)
		{
			const char* bolt = SLGetString(&bolts->values, index);

			size_t size = strlen(bolt) + 1;
			npc->bolts[index] = malloc(size);
			assert(npc->bolts[index]);
			strcpy_s(npc->bolts[index], size, bolt);
		}

		// Spells
		for (int index = 0; index < npc->spellCount; index++)
		{
			const char* spell = SLGetString(&spells->values, index);

			size_t size = strlen(spell) + 1;
			npc->spells[index] = malloc(size);
			assert(npc->spells[index]);
			strcpy_s(npc->spells[index], size, spell);
		}

		// Items
		for (int index = 0; index < npc->itemCount; index++)
		{
			const char* item = SLGetString(&items->values, index);

			size_t size = strlen(item) + 1;
			npc->items[index] = malloc(size);
			assert(npc->items[index]);
			strcpy_s(npc->items[index], size, item);
		}
	}

	JSONFree(&list);

	for (int index = 0; index < length; index++)
	{
		DSIIINPC* npc = &npcs[index];

		printf("ID: %d\n%s\nRight Hand Weapons:\n", npc->id, npc->name);
		for (int index = 0; index < npc->weapons.rHandCount; index++)
		{
			printf("%s\n", npc->weapons.rHand[index]);
		}

		puts("Left Hand Weapons:");
		for (int index = 0; index < npc->weapons.lHandCount; index++)
		{
			printf("%s\n", npc->weapons.lHand[index]);
		}

		printf(
			"Armor:\n%s\n%s\n%s\n%s\n",
			npc->armor.helm,
			npc->armor.armor,
			npc->armor.gauntlets,
			npc->armor.leggings
		);

		puts("Arrows:");
		for (int index = 0; index < npc->arrowCount; index++)
		{
			printf("%s\n", npc->arrows[index]);
		}

		puts("Bolts:");
		for (int index = 0; index < npc->boltCount; index++)
		{
			printf("%s\n", npc->bolts[index]);
		}

		puts("Spells:");
		for (int index = 0; index < npc->spellCount; index++)
		{
			printf("%s\n", npc->spells[index]);
		}

		puts("Items:");
		for (int index = 0; index < npc->itemCount; index++)
		{
			printf("%s\n", npc->items[index]);
		}

		puts("Attributes:");
		printf("Vigor: %d\n", npc->attributes.vigor);
		printf("Attunement: %d\n", npc->attributes.attunement);
		printf("Endurance: %d\n", npc->attributes.endurance);
		printf("Vitality: %d\n", npc->attributes.vitality);
		printf("Strength: %d\n", npc->attributes.strength);
		printf("Dexterity: %d\n", npc->attributes.dexterity);
		printf("Intelligence: %d\n", npc->attributes.intelligence);
		printf("Faith: %d\n", npc->attributes.faith);
		printf("Luck: %d\n\n", npc->attributes.luck);
	}

	puts("program ended successfully");
	printf("press enter to exit . . . ");
	getchar();
	return 0;
}
```
## Output
```
ID: 24803
Greirat the Thief
Right Hand Weapons:
Bandit's Knife + 2
Left Hand Weapons:
Iron Round Shield +2
Armor:
Thrall Hood
Deserter Armor
Soldier's Gauntlets
Deserter Trousers
Arrows:
Standard Arrow
Standard Arrow
Bolts:
Standard Bolt
Standard Bolt
Spells:
Items:
Throwing Knife
Attributes:
Vigor: 21
Attunement: 10
Endurance: 12
Vitality: 20
Strength: 20
Dexterity: 22
Intelligence: 9
Faith: 11
Luck: 5

ID: 29540
Kirk, Knight of Thorns
Right Hand Weapons:
Barbed Straight Sword + 2
Left Hand Weapons:
Spiked Shield +2
Armor:
Helm of Thorns
Armor of Thorns
Gauntlets of Thorns
Leggings of Thorns
Arrows:
Standard Arrow
Standard Arrow
Bolts:
Standard Bolt
Standard Bolt
Spells:
Items:
Estus Flask x2

Undead Hunter Charm
Dual Charm
Devine Blessing x2
Attributes:
Vigor: 25
Attunement: 18
Endurance: 25
Vitality: 21
Strength: 45
Dexterity: 16
Intelligence: 9
Faith: 12
Luck: 9

ID: 28040
Daughter of Crystal Kriemhild
Right Hand Weapons:
Sage's Crystal Staff +5
Crystal Sage's Rapier +5
Left Hand Weapons:
Parrying Dagger +5
Armor:

Fire Keeper Robe
Fire Keeper Gloves
Fire Keeper Skirt
Arrows:
Standard Arrow
Standard Arrow
Bolts:
Standard Bolt
Standard Bolt
Spells:
Crystal Soul Spear
Homing Crystal Soulmass
Crystal Hail
Items:
Estus Flask x2
Attributes:
Vigor: 35
Attunement: 18
Endurance: 22
Vitality: 1
Strength: 21
Dexterity: 23
Intelligence: 60
Faith: 1
Luck: 1

ID: 26209
[UNKNOWN]
Right Hand Weapons:
Storm Ruler
Long Sword +4
Left Hand Weapons:
Silver Eagle Kite Shield +4
Buckler Shield +4
Armor:
Knight Helm
Knight Armor
Knight Gauntlets
Knight Leggings
Arrows:
Standard Arrow x30
Poison Arrow x30
Bolts:
Standard Bolt x30
Wood Bolt x30
Spells:
Items:
Estus Flask x7
Ashen Flask x3
Throwing Knife
Firebomb
Alluring Skull
Charcoal Pine Resin
Blooming Purple Moss Clump
Ember
Attributes:
Vigor: 18
Attunement: 15
Endurance: 18
Vitality: 19
Strength: 26
Dexterity: 18
Intelligence: 10
Faith: 12
Luck: 11

program ended successfully
press enter to exit . . .
```
