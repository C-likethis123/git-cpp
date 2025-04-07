# init

`git init` creates a repository.

## Command usage

- `./adder_app init` - creates an empty repository
- `./adder_app init -p [path]` - creates an empty repository on the specified path

## Implementation details

1. App representation: Initialise a `GitRepository` object, which is an in-memory representation of an actual git repository
2. This object will create actual files that represent a git repository

## Git repository

1. work tree - files in version control
2. git directory - where git stores its own metadata
    - objects: the object store
    - refs: reference store
        - heads
        - tags
    - reference to the current HEAD
    - config: repository's config file
    - description: freeform description of the repo's contents
