#ifndef LCC_STATEMANAGER_H
#define LCC_STATEMANAGER_H

#include <mutex>
#include "State.h"

class StateManager {

    StatePtr _current_state;
    StatePtr _last_state;
    void (*_stateStarted)(StatePtr);
    void (*_stateCompleted)(StatePtr, std::exception_ptr);
    mutable std::mutex _mutex;

    void setCurrentState(StatePtr next) {
        std::lock_guard<std::mutex> lock(_mutex);
        _last_state = _current_state;
        _current_state = next;
    }

public:
    StateManager(std::unique_ptr<State> start_state, void(*stateStarted)(StatePtr), void(*stateCompleted)(StatePtr, std::exception_ptr))
        : _current_state(std::move(start_state)),
        _stateStarted(stateStarted),
        _stateCompleted(stateCompleted),
        _mutex() {
    }

    std::string getStateName() const {
        std::lock_guard<std::mutex> lock(_mutex);
        return std::string(_current_state->getName().getValue());
    }

    StatePtr getLastState() const {
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
                _stateStarted(_current_state);
                auto next_state = _current_state->run();
                _stateCompleted(_current_state, nullptr);
                setCurrentState(next_state);
            } catch (const std::exception & e) {
                std::cout << "Caught an exception in " << _current_state->getName() << std::endl;
                _stateCompleted(_current_state, std::current_exception());
                _current_state = nullptr;
            }
        }
        std::cout << "StateManager terminating" << std::endl;
    }
};

// #ifdef LCC_STATEMANAGER_H
#endif

