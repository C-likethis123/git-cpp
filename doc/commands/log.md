# log

`git log` will take a commit, and generate a list of commits starting from that commit all the way to the root commit.

## Command usage
- `git log [commit hash]`

## Implementation details

1. Starting from the given commit, find and parse the commit metadata in ./git/objects
2. Use the metadata to figure out the commit hash of its parent
3. Keep repeating this until no parent can be found

## An interesting tidbit about PGP signatures

When I commit locally, these commits do not have PGP signatures.
When I pull commits remotely (for example to merge with the main branch), these commits have PGP signatures.
At what point are PGP signatures added?

Turns out that Github signs commits made to the web interface ([source](https://docs.github.com/en/authentication/managing-commit-signature-verification/about-commit-signature-verification)) - all the commits created by merging in feature branches to `main` are signed. Commits that are created locally and pushed to the `main` branch are not signed.

## Future work
- [ ] Make the commit argument optional, by defaulting the commit argument to the HEAD argument.
- [ ] Pipe the output to programs like `less` for repositories with a large set of commits
- [ ] Support more options, like `--oneline`