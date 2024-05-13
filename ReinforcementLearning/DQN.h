#ifndef DQN_H
#define DQN_H

class DQN {

  private:

    float discount = 0.35f;
    void Train();
    void SelectAction();
    void OptimizeModel();
    
};



#endif