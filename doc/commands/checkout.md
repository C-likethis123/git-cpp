# checkout

## What it does

`git checkout [commit]` checks out the current commit into the repository.
`git checkout [branch name]` takes the HEAD commit associated with the branch name, and checks out that commit into the repository.

## Internal state changes

A branch name is associated with a commit.
A commit is associated with a worktree.
Checking out a commit means:
- comparing both commits for the files that needs to be changed
- if it's a file, replace the contents with what's in the new commit. Change the file mode if need be.
- if it's a directory:
   - create the directory
   - scan through the tree contents and create files
   - if it's a directory, create a directory as well and read the file contents, etc.

## Error handling and general questions

What if there are untracked changes in tracked files?
- These changes will be overridden, so it should return with an ERROR

Besides file mode, would other file metadata be stored?
- Eg: when the file is last updated, when the file was created.
- Did a check: the last updated date/created date would be different from the commit date.