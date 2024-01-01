# cat-file

From: https://wyag.thb.lt/#objects

## Objects
`hash-object` converts an existing file into a git object
`cat-file` prints an existing git object to standard output

Git is a "content-addressed filesystem". The name of a file is not important, as they are mathematically derived from their contents.
Why: helps to identify file changes
Objects are: files in the git repository, whose paths are determined by their contents.
The path is computed by calculating SHA-1 hash of its contents