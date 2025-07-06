# Index files

## Staging area

The staging area holds the changes we want to change between the current files, and the HEAD.

## Why use index files

It's similar to a copy of the commit, but it holds extra information about files in the worktree, such as the modified time. We don't have to actually compare file contents in `git status`, just the difference between the file modification time and the one stored in the index file.

An index file can represent inconsistent states, like a merge conflict. Whereas a tree is always an unambiguous representation.

## on `git commit`

Git turns index files into a new commit:
1. Turn the current index file into a tree object:
  - create blob objects for files that changed
  - create tree objects for folders that changed
  - create a new tree object representing the worktree
2. create a commit object with
  - tree object
  - parent commit (the HEAD)
  - author
  - committer
  - pgp signature

## on `git status`
Iterate through all files in the worktree
1. For files in the work tree:
  - check if they are modified by comparing modification date
  - check if they are deleted
  - check if they are added
2. For files not tracked by the worktree, show them as untracked files


## on `git ls-files`
Displays the names of the files in the staging area.

## Format of index files

1. A header with the format version number and the number of entries the index holds.
2. A series of entries, sorted, each representing a file
