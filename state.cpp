#include <atomic>
#include <mutex>
#include <thread>
#include <cstdlib>
#include <unistd.h>
#include <iostream>
#include <memory>
#include <string>

template<typename T>
class AbstractImmutable {
    private:
        const T _value;
    public:
        AbstractImmutable(const T & value) : _value(value) {}
        const T & getValue() const { return _value; }

        inline friend bool operator<(const AbstractImmutable<T> & a, const AbstractImmutable<T> & b) {
            return a.getValue() < b.getValue();
        }

        inline friend bool operator>(const AbstractImmutable<T> & a, const AbstractImmutable<T> & b) {
            return a.getValue() > b.getValue();
        }

        inline friend bool operator==(const AbstractImmutable<T> & a, const AbstractImmutable<T> & b) {
            return a.getValue() == b.getValue();
        }

        const AbstractImmutable<T> operator+(const AbstractImmutable<T> & that) const {
            return AbstractImmutable<T>(getValue() + that.getValue());
        }

        const AbstractImmutable<T> operator-(const AbstractImmutable<T> & that) const {
            return AbstractImmutable<T>(getValue() - that.getValue());
        }

        const AbstractImmutable<T> operator/(const AbstractImmutable<T> & that) const {
            return AbstractImmutable<T>(getValue() / that.getValue());
        }

        const AbstractImmutable<T> operator*(const AbstractImmutable<T> & that) const {
            return AbstractImmutable<T>(getValue() * that.getValue());
        }

        inline friend std::ostream & operator<<(std::ostream & out, const AbstractImmutable<T> & that) {
            out << that.getValue();
            return out;
        }
};

template<typename T>
class ImmutableString : public AbstractImmutable<std::string> {
    using AbstractImmutable::AbstractImmutable;
};



struct NotImplementedException : public std::logic_error
{
    NotImplementedException () : std::logic_error("Function not yet implemented.") {}
};

struct StateName : ImmutableString<StateName> {
    StateName(const std::string & name) : ImmutableString(name) {
    
    } 
};

template <typename T>
struct Callable {
    virtual T run(void) = 0;
};

struct State;

typedef std::shared_ptr<State> NextState;

struct State : Callable<NextState> {

    const StateName _name; 

    State(StateName name) : _name(name) {

    }

    State(const std::string & name) : _name(name) {

    }

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
        std::cout << getName() << std::endl;
        sleep(2);
        throw std::runtime_error("oopsie!");
        return NextState(new EndState());
    }
};

struct FooState : public State {
    FooState() : State("FooState") {}
    NextState run() {
        std::cout << getName() << std::endl;
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

struct StateManager : Callable<void> {
    NextState _current_state;
    std::mutex _mutex;
    std::string _state_name;
    StateManager(State * start_state) : _current_state(start_state), _mutex() {
    }

    std::string getStateName() {
        std::lock_guard<std::mutex> lock(_mutex);
        return std::string(_current_state->getName().getValue());
    }

    bool isRunning() {
        std::lock_guard<std::mutex> lock(_mutex);
        std::cout << "isRunning: " << _current_state->getName() << " " << (!_current_state->isTerminalState()) << std::endl;;
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
        usleep(500000);
    }
    thread.join();
}


