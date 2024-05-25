# init

Git init creates a repository.

## Command usage

- `./adder_app init` - creates an empty repository
- `./adder_app init -p [path]` - creates an empty repository on the specified path

## Implementation details

1. Initialise a GitRepository object

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
