/*
 * Automaton.h - Reactive State Machine Framework for Arduino.
 * Published under the MIT License (MIT), Copyright (c) 2015-2016, J.P. van der Landen
 */

#pragma once

#include "Arduino.h"

class atm_serial_debug {  // It seems necessary to put this code in .h to keep it from being compiled in unnecessarily
 public:
  static void trace( Stream* stream, Machine& machine, const char label[], const char current[], const char next[], const char trigger[], uint32_t runtime,
                     uint32_t cycles ) {
    stream->print( millis() );
    stream->print( F(" Switch ") );
    stream->print( label );
    stream->print( F("@") );
    stream->print( (long)&machine, HEX );
    stream->print( F(" from ") );
    stream->print( current );
    stream->print( F(" to ") );
    stream->print( next );
    stream->print( F(" on ") );
    stream->print( trigger );
    stream->print( F(" (") );
    stream->print( cycles );
    stream->print( F(" cycles in ") );
    stream->print( runtime );
    stream->println( F(" ms)") );
  }
};
