# Simplified Saya version with DQN and PPO
Sandbox Saya without any graphics. Trying to code a way to use reinforcement learning and MTCS in Real time strategy games.

Game was initially though **NOT** to contain graphs. This quickly changed as I needed a way to visualize
AI movement, I am using SDL3. At the end of the project the AI will be able to go toe to toe with a human enemy and make the best
possible decisions to win the game.

# How can you collaborate

## What has currently been implemented
- A DAMN UNIT TESTING FRAMEWORK
- Game finally playable and decided to put some graphis with sdl3 currently
- Algorithm for reinforcement learning has been implemented (both dqn and ppo)
- PyTorch has been sucessfully started running.
- Currently changing the input of the game so it fits in my tensor model.
- Replay system works both to serialize and serialize in 2 different formats or binary (.bay)
and string (.say)
- Units have their own cooldowns but this probably needs something smarter to make it a bit
more performant instead of slapping each unit with 3 timers.
- Both algorithm have their training loops implemented but tiny issues here and there
- Replay system has been implemented with graphics where you can see in quite performant
time how the algorithms have moved with their actions.

## Tools
To compile please stick to strickly using cmake with the -G flag and strictly use the provided
./compile.sh script to do any compilation as its quicker.

To check for any perfromance issues please use the ./profiler.sh script which follows 
some dependencies such as flamegraph in the build/ sections. To check the results of the
profiler please use the provided .svg and use ur prefered vector app the view the results.

If you are on nixos its as simple to get dependencies as just using the shell provided in
the flake and you are good to go. For other OS's please clone the additional repositories
and figure out how to add them on the CMakeLists.txt by yourself or just open an issue here. 
Additionally, you will need to put the dependencies in their respective places, specifically
flamegraph in the builds/, sdl3, catch2 and any other library i forget to mention here sits
either on the src/ or on the parent directory. Additionally, to run the replay gui and see
how the algorithm plays, please use the included font in the repository.

## Testing

Please stick to catch2 as the testing framework and please follow whatever format I have
specified for my tests, unless you just hate it i guess and have a better way at it. 


## Hierarchy of the repository

```txt
    src/:
    gui  Living.cpp  Living.h  main.cpp  Race  ReinforcementLearning  State  Tools
    
    src/gui:
    start.cpp  Window.cpp  Window.h
    
    src/Race:
    Spells  Structure  Unit
    
    src/Race/Spells:
    Banish.h  Blizzard.h  BrillanceAura.h  FlameStrike.h  MassTeleport.h  SiphonMana.h  Spell.h
    
    src/Race/Structure:
    Barrack.cpp  Barrack.h  Farm.cpp  Farm.h  Structure.cpp  Structure.h  TownHall.cpp  TownHall.h
    
    src/Race/Unit:
    Action.cpp  Action.h  Footman.cpp  Footman.h  Hero  Peasant.cpp  Peasant.h  Unit.cpp  Unit.h
    
    src/Race/Unit/Hero:
    ArchMage.h  BloodMage.h  Hero.cpp  Hero.h
    
    src/ReinforcementLearning:
    DQN.cpp  PPO.cpp  Reward.cpp  RlManager.cpp  Tensor.cpp  Transition.cpp  ValueNetwork.cpp
    DQN.h    PPO.h    Reward.h    RlManager.h    Tensor.h    Transition.h    ValueNetwork.h
    
    src/State:
    Graph.cpp  Manager.cpp  Map.cpp  Player.cpp  Terrain.cpp  WinMapColored.h
    Graph.h    Manager.h    Map.h    Player.h    Terrain.h
    
    src/Tools:
    Binary.h  Enums.h  Macro.h  Vec2.h
```
