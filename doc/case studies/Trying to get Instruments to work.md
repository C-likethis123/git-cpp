# Background

Trying to use Instruments to profile the app -- as a start to optimisation.
Instruments app was running the program, but without the stack trace.

## Were debug symbols missing?

Initial suspicion was that the debug symbols were missing because I tried:

`dwarfdump [binary]` and the debug section was missing.

But from this stackoverflow thread: https://stackoverflow.com/questions/56688359/debug-symbols-not-included-in-gcc-compiled-c
- macOS compilation leaves the debug information in the .o files and writes a debug map into the binary
- The debug map is stripped when stripping the binary
- Check for the presence of the debug map with: `nm -ap <PATH_TO_BINARY> | grep OSO`

## Picking up which traces

When initialising a trace profile, there is an option to choose reporting threshold - set it to the minimum reporting threshold
