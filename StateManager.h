#ifndef LCC_STATEMANAGER_H
#define LCC_STATEMANAGER_H

#include <mutex>
#include "State.h"

class StateManager {

    NextState _current_state;
    NextState _last_state;
    mutable std::mutex _mutex;

    void setCurrentState(NextState next) {
        std::lock_guard<std::mutex> lock(_mutex);
        _last_state = _current_state;
        _current_state = next;
    }

public:
    StateManager(std::unique_ptr<State> start_state) : _current_state(std::move(start_state)), _mutex() {}

    std::string getStateName() const {
        std::lock_guard<std::mutex> lock(_mutex);
        return std::string(_current_state->getName().getValue());
    }

    NextState getLastState() const {
        std::lock_guard<std::mutex> lock(_mutex);
        return _last_state;
    }

    bool isRunning() const {
        std::lock_guard<std::mutex> lock(_mutex);
        return _current_state != nullptr;
    }


    void run(void) {
        while(isRunning()) {
            try {
                auto next_state = _current_state->run();
                setCurrentState(next_state);
            } catch (const std::exception & e) {
                std::cout << "Caught an exception in " << _current_state->getName() << std::endl;
                setCurrentState(NextState(new AbortState(std::current_exception())));
            }
        }
        std::cout << "StateManager terminating" << std::endl;
    }
};

// #ifdef LCC_STATEMANAGER_H
#endif

