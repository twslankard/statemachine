#include <chrono>
#include <mutex>
#include <thread>
#include <unistd.h>
#include <iostream>
#include "Immutable.h"


struct StateName : ImmutableString<StateName> {
    StateName(const std::string & name) : ImmutableString(name) {} 
};

template <typename T>
struct Callable {
    virtual T run(void) = 0;
};

struct State;
typedef std::shared_ptr<State> NextState;

class State : public Callable<NextState> {

    const StateName _name; 

public:
    State(StateName name) : _name(name) {}
    State(const std::string & name) : _name(name) {}

    virtual bool isTerminalState() const {
        return false;
    }

    StateName getName() const {
        return _name;
    }
};

struct EndState : public State {
    EndState() : State("EndState") {}
    NextState run(void) override { return nullptr; }
    bool isTerminalState() const override { return true; }
};

struct BarState : public State {
    BarState() : State("BarState") {}
    NextState run() override {
        std::cout << "Now in " << getName() << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1)); 
        throw std::runtime_error("oopsie!");
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

struct AbortState : public State {
    AbortState() : State("AbortState") {}
    NextState run(void) override { return nullptr; }
    bool isTerminalState() const override { 
        std::cout << "AbortState::isTerminalState() " << std::endl;
        return true;
    }
};

class StateManager : Callable<void> {

    NextState _current_state;
    mutable std::mutex _mutex;

    void setCurrentState(NextState next) {
        std::lock_guard<std::mutex> lock(_mutex);
        _current_state = next;
    }

public:
    StateManager(std::unique_ptr<State> start_state) : _current_state(std::move(start_state)), _mutex() {}

    std::string getStateName() const {
        std::lock_guard<std::mutex> lock(_mutex);
        return std::string(_current_state->getName().getValue());
    }

    bool isRunning() const {
        std::lock_guard<std::mutex> lock(_mutex);
        return !_current_state->isTerminalState();
    }


    void run(void) {
        while(isRunning()) {
            try {
                auto next_state = _current_state->run();
                setCurrentState(next_state);
            } catch (...) {
                std::cout << "Caught an exception in " << _current_state->getName() << std::endl;
                std::cout << "StateManager terminating" << std::endl;
                setCurrentState(NextState(new AbortState()));
                return;
            }
        }
    }
};

int main() {
    StateManager state_manager(std::unique_ptr<State>(new StartState));
    std::thread thread(&StateManager::run, &state_manager);
    while(state_manager.isRunning()) {
        std::cout << state_manager.getStateName() << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(250)); 
    }
    thread.join();
}


