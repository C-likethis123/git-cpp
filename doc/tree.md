# tree

## What's in a tree?

A tree describes the content of the current folder by associating blobs to paths.
It's a table with 3 columns: file mode, file path, SHA-1.
Each subfolder will be represented by its own tree object.

## Tree format

A tree is a concatenation of records of the format:
```
[mode] space [path] 0x00 [sha-1]
```

## How does Git store version history in the worktree?

1. Each branch is associated with one worktree object
2. Current versions of the worktree is associated with a blob object

## How to parse a tree object?

1. Not sure if there's a format header
2. Follow the tree format.
3. It can be parsed into:
- a mapping of a path to a file mode, and a SHA-1 hash
- when needed, the SHA-1 hash could be read

# What do we do with trees?
1. Every commit object stores a reference to the tree, which represents the working object.
2. Need to modify the paths in the working tree.
3. Need to add to the working tree.
4. Delete from the working tree
5. Show the mode, sha1, object type, from a given file path

Since I'm using `git add`, `git rm` in terms of file paths, I will use the file path as a key and represent it in an unordered_map.