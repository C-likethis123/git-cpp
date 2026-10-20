#ifndef TCLAPPARSESCOPE_H
#define TCLAPPARSESCOPE_H

#include "tclap/CmdLine.h"

// TCLAP tracks positional definitions globally, even for distinct CmdLine objects.
class TclapParseScope {
public:
  TclapParseScope()
      : previous(TCLAP::OptionalUnlabeledTracker::alreadyOptional()) {
    TCLAP::OptionalUnlabeledTracker::alreadyOptional() = false;
  }
  ~TclapParseScope() {
    TCLAP::OptionalUnlabeledTracker::alreadyOptional() = previous;
  }
  TclapParseScope(const TclapParseScope &) = delete;
  TclapParseScope &operator=(const TclapParseScope &) = delete;

private:
  bool previous;
};

#endif // TCLAPPARSESCOPE_H
