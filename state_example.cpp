#include <chrono>
#include <thread>
#include <iostream>
#include <state/State.h>
#include <state/StateManager.h>

struct EndState : public State {
    EndState() : State("EndState") {}
    StatePtr run(void) override { return nullptr; }
};

struct BarState : public State {
    BarState() : State("BarState") {}
    StatePtr run() override {
        std::cout << "Now in " << getName() << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1)); 
        return StatePtr(new EndState());
    }
};

struct FooState : public State {
    FooState() : State("FooState") {}
    StatePtr run() override {
        std::cout << "Now in " << getName() << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1)); 
        std::cout << "Going to BarState" << std::endl;
        return StatePtr(new BarState());
    }
};

struct StartState : public State {
    StartState() : State("StartState") {}
    StatePtr run() override {
        std::cout << "Now in " << getName() << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1)); 
        std::cout << "Going to FooState" << std::endl;
        return StatePtr(new FooState());
    }
};

void stateStarted(StatePtr state) {
    std::cout << "State started: " << state->getName() << std::endl;
}

void stateCompleted(StatePtr state, std::exception_ptr exception) {
    std::cout << "State completed: " << state->getName() << std::endl;
}


int main() {
    StateManager state_manager(std::unique_ptr<State>(new StartState), stateStarted, stateCompleted);
    std::thread thread(&StateManager::run, &state_manager);
    thread.join();
}


