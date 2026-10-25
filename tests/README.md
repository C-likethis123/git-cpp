# Tests

Tests are written using CTest.

The test set up uses a separate git repository as a fixture (git-cpp-test-fixture) that is tracked as a Git submodule.
## Updating test fixture
1. Create separate files/commits/tags accordingly in the test fixture git repo
2. Push the changes up
3. Update the git submodule using `git -C tests/gitrepo fetch origin`
