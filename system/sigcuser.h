#ifndef SIGCUSER_H
#define SIGCUSER_H

#include <sigc++/sigc++.h>

#define SIGCTRACKABLE virtual private sigc::trackable


//a function with no arguments and no return
typedef sigc::signal<void> SimpleSignal;

#endif // SIGCUSER_H
