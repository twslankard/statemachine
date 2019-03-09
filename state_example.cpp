#include <chrono>
#include <thread>
#include <iostream>
#include "State.h"
#include "StateManager.h"

struct EndState : public State {
    EndState() : State("EndState") {}
    NextState run(void) override { return nullptr; }
};

struct BarState : public State {
    BarState() : State("BarState") {}
    NextState run() override {
        std::cout << "Now in " << getName() << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1)); 
        return NextState(new EndState());
    }
};

struct FooState : public State {
    FooState() : State("FooState") {}
    NextState run() override {
        std::cout << "Now in " << getName() << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1)); 
        std::cout << "Going to BarState" << std::endl;
        return NextState(new BarState());
    }
};

struct StartState : public State {
    StartState() : State("StartState") {}
    NextState run() override {
        std::cout << "Now in " << getName() << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1)); 
        std::cout << "Going to FooState" << std::endl;
        return NextState(new FooState());
    }
};



int main() {
    StateManager state_manager(std::unique_ptr<State>(new StartState));
    std::thread thread(&StateManager::run, &state_manager);
    while(state_manager.isRunning()) {
        std::cout << state_manager.getStateName() << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(250)); 
    }
    std::cout << "Last state was " << state_manager.getLastState()->getName() << std::endl;
    thread.join();
}


