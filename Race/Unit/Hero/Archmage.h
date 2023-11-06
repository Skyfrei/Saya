#include "Hero.h"
#include "../../Spells/Blizzard.h"
#include "../../Spells/BrillanceAura.h"
#include "../../Spells/MassTeleport.h"

// Level	Strength	Hit Points	HP Regen Rate	Agility	(In-game)	Cooldown	Intelligence	Mana    Mana Regen Rate	Attack (Ground/Air)
// 1	    18	        550	        1.15 HP/sec.	14	     (2)	    1.28 sec.	19	            285	    0.96 Mana/sec.	21-27 (24 avg)
// 2	    20	        600	        1.25 HP/sec.	15	     (3)	    1.26 sec.	22	            330	    1.11 Mana/sec.	24-30 (27 avg)
// 3	    22	        650	        1.35 HP/sec.	16	     (3)	    1.24 sec.	25	            375	    1.26 Mana/sec.	27-33 (30 avg)
// 4	    24	        700	        1.45 HP/sec.	17	     (3)	    1.22 sec.	28	            420	    1.41 Mana/sec.	30-36 (33 avg)
// 5	    26	        750	        1.55 HP/sec.	18	     (3)	    1.21 sec.	31	            465	    1.56 Mana/sec.	33-39 (36 avg)
// 6	    28	        800	        1.65 HP/sec.	19	     (4)	    1.19 sec.	34	            510	    1.71 Mana/sec.	36-42 (39 avg)
// 7	    30	        850	        1.75 HP/sec.	20	     (4)	    1.17 sec.	37	            555	    1.86 Mana/sec.	39-45 (42 avg)
// 8	    32	        900	        1.85 HP/sec.	21	     (4)	    1.15 sec.	40	            600	    2.01 Mana/sec.	42-48 (45 avg)
// 9	    34	        950	        1.95 HP/sec.	22	     (5)	    1.14 sec.	43	            645	    2.16 Mana/sec.	45-51 (48 avg)
// 10	    36	        1000        2.05 HP/sec.	23	     (5)	    1.12 sec.	46	            690	    2.31 Mana/sec.	48-54 (51 avg)


namespace Warcraft::Units::Heroes
{
    class Archmage : public Hero
    {
        public:
            Archmage()
            {
                health = 550;
                hpRegen = 1.15f;
                mana = 285;
                manaRegen = 0.96f;
                maxMana += 285;

                goldCold = 535;
                foodCost = 5;
                buildTime = 55;
                attack = 24;
                attackCooldown = 1.28;

                strength = 18;
                agility = 14;
                intelligence = 19;

                primaryAttribute = INTELLIGENCE;

                spells.push_back(new Blizzard());
                spells.push_back(new BrillanceAura());
                spells.push_back(new MassTeleport());
            }

        std::string GetDescription() override {return "Archmage hero";}

    };
}


