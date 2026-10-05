# Testing Rules

## Do NOT automatically test or run files after making changes

- Never run tests automatically after code changes
- Never execute binaries (like ./app/gyt, ./tests/tests) after modifications  
- Never run build commands (like make, cmake, ninja) unless explicitly requested
- Ask for permission before running any commands
- Only suggest commands - let the user decide when to run them

## Exception
- Only run commands if the user explicitly asks "test this" or "run this"
