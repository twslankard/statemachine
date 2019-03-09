#include <mutex>
#include <thread>
#include <unistd.h>
#include <iostream>
#include "Immutable.h"

struct NotImplementedException : public std::logic_error {
    NotImplementedException () : std::logic_error("Function not yet implemented.") {}
};

struct StateName : ImmutableString<StateName> {
    StateName(const std::string & name) : ImmutableString(name) {} 
};

template <typename T>
struct Callable {
    virtual T run(void) = 0;
};

struct State;
typedef std::shared_ptr<State> NextState;

class State : Callable<NextState> {

    const StateName _name; 

public:
    State(StateName name) : _name(name) {}
    State(const std::string & name) : _name(name) {}

    virtual NextState run(void) {
       throw NotImplementedException(); 
    };

    virtual bool isTerminalState() const {
        return false;
    }

    StateName getName() const {
        return _name;
    }
};

struct EndState : public State {
    EndState() : State("EndState") {}
    bool isTerminalState() const { return true; }
};

struct BarState : public State {
    BarState() : State("BarState") {}
    NextState run() {
        std::cout << "Now in " << getName() << std::endl;
        sleep(2);
        throw std::runtime_error("oopsie!");
        return NextState(new EndState());
    }
};

struct FooState : public State {
    FooState() : State("FooState") {}
    NextState run() {
        std::cout << "Now in " << getName() << std::endl;
        sleep(2);
        std::cout << "Going to BarState" << std::endl;
        return NextState(new BarState());
    }
};

struct StartState : public State {
    StartState() : State("StartState") {}
    NextState run() {
        std::cout << "Now in " << getName() << std::endl;
        sleep(2);
        std::cout << "Going to FooState" << std::endl;
        return NextState(new FooState());
    }
};

struct AbortState : public State {
    AbortState() : State("AbortState") {}
    bool isTerminalState() const { 
        std::cout << "AbortState::isTerminalState() " << std::endl;
        return true;
    }
};

class StateManager : Callable<void> {

    NextState _current_state;
    std::mutex _mutex;

public:
    StateManager(State * start_state) : _current_state(start_state), _mutex() {}

    std::string getStateName() {
        std::lock_guard<std::mutex> lock(_mutex);
        return std::string(_current_state->getName().getValue());
    }

    bool isRunning() {
        std::lock_guard<std::mutex> lock(_mutex);
        return !_current_state->isTerminalState();
    }

    void setCurrentState(NextState next) {
        std::lock_guard<std::mutex> lock(_mutex);
        _current_state = next;
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
    StateManager state_manager(new StartState);
    std::thread thread(&StateManager::run, &state_manager);
    while(state_manager.isRunning()) {
        std::cout << state_manager.getStateName() << std::endl;
        usleep(250000);
    }
    thread.join();
}


