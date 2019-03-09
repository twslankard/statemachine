#ifndef LCC_STATE_H
#define LCC_STATE_H

#include "Immutable.h"

struct StateName : ImmutableString<StateName> {
    StateName(const std::string & name) : ImmutableString(name) {} 
};

struct State;
typedef std::shared_ptr<State> NextState;

class State {

    const StateName _name; 

public:
    State(StateName name) : _name(name) {}
    State(const std::string & name) : _name(name) {}

    virtual NextState run(void) = 0;

    StateName getName() const {
        return _name;
    }
};

struct AbortState : public State {
    std::exception_ptr exception;
    AbortState(std::exception_ptr e) : State("AbortState"), exception(e) {}
    NextState run(void) override { return nullptr; }
};

// #ifdef LCC_STATE_H
#endif

