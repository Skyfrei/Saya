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


