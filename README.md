# Simplified Saya version with DQN and PPO
Sandbox Saya without any graphics. Trying to code a way to use reinforcement learning and MTCS in Real time strategy games.

Game **will** not have any graphics for a long time if ever because the point of the project is to create an RTS game which can be deeplearned with different
algorithms, following previously published licensed papers. At the end of the project the AI will be able to go toe to toe with a human enemy and make the best
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

cmake -D CMAKE_CXX_COMPILER=g++-10 CMakeLists.txt
export DYLD_LIBRARY_PATH=/opt/homebrew/opt/libomp/lib:$DYLD_LIBRARY_PATH
https://pytorch.org/tutorials/intermediate/reinforcement_ppo.html



fix cooldown of movement

remove from array upon deat


## Also another thing: I'd avoid serializing a std::variant as-is, because you don't know its memory layout (and I don't think the standard gives you any guarantees about it). You can use something simple like:

enum class MyBinaryDataType : uint8_t {
  Int = 0,
  Float = 1,
  Double = 2
};

union MyBinaryData {
  int64_t as_int;
  float as_float;
  double as_double;
};

#pragma pack(push, 1)
struct MyBinary {
  MyBinaryData data;
  MyBinaryDataType data_type; // Put values from MyBinaryDataType in here
};
#pragma pack(pop)
