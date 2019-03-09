A simple state machine library
==============================

Tom Slankard <tom.slankard@here.com>

Overview
--------

This is a very simple state machine lib. The goal was to see whether I could implement something very simple with minimal frills, but with the following design goals:

* Exceptions can be propagated from the thread running the state machine to the main thread.
* State logic is encapsulated to force reduced coupling between states.
* Use C++11

