# cat-file

From: https://wyag.thb.lt/#objects

## Commands

`git cat-file` - prints an existing git object to standard output.

## Git Objects

Git is a "content-addressed filesystem". The name of a file is not important, as they are mathematically derived from their contents.
Why is this important: it helps to identify file changes
"Objects" refer to files in the git repository, whose paths are determined by their contents.

## How can file changes be identified?
The file path is computed by calculating SHA-1 hash of its contents. The hash is a hexadecimal string with two parts:
- first two characters: for the directory name
- rest of the characters: as the file name

With this approach, Git creates 256 possible intermediate directories, dividing the average number of files per directory by 256

## Storage formats

An object starts with a header that specifies its type:
1. blob
2. commit
3. tag
4. tree

## What does Git use objects to store?
1. The actual files it keeps in version control