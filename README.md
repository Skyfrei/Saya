# Simplified Saya version with DQN and PPO
Sandbox Saya without any graphics. Trying to code a way to use reinforcement learning and MTCS in Real time strategy games.

Game was initially though **NOT** to contain graphs. This quickly changed as I needed a way to visualize
AI movement, I am using SDL3. At the end of the project the AI will be able to go toe to toe with a human enemy and make the best
possible decisions to win the game.

# How can you collaborate
## MAYBE USE A CIRCULAR BUFFER INSTEAD OF A VECTOR 
- Bug finding in the game in case i have missed something such as times when Id get undefined behaviours.
- Probably nulll pointer mem access here as i have implemented null pointers for actions.
- Ideas on how to continue the Reinforcement learning and stuff i could add to it. Maybe additions to algorithms DQN and PPO and maybe ideas about adding SARSA too.
- Better writing of structs and enums (this would actually be really helpful)
- Structuing of header files and where classes go to, specifically the unit header. 


## What has currently been implemented
### A DAMN UNIT TESTING FRAMEWORK
- Game is fully finished and playable although not very interesting without graphics as its a real time strategy game.
- Algorithm for reinforcement learning has been implemented
- PyTorch has been sucessfully started running.
- Currently changing the input of the game so it fits in my tensor model.


-   https://harichada.github.io/osbox/posts/reinforcement-learning-for-real-time-strategy-games/



fix cooldown of movement

remove from array upon deat



## PROGRESS

    Replay system using both string and binary

    Replay system of binary went from 90 second for 1 million transitions to 14 seconds 

    Done using profiling tools such as perf and flamegraph

    Replay system is on O(1) since its using deque instead of vector for handling the states themselves,
    and optimizing for speed since im resizing the vectors that initially store
    the binaries instead of push backing.


### SDL3 next
    SDL_RenderDrawPoint
