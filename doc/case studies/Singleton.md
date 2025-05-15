# Parser and singleton

`TagParser` is an example of a singleton class.
It was largely adapted from: https://stackoverflow.com/questions/75249277/is-singleton-with-static-unique-ptr-a-good-practice

It stores a reference of itself as a static member. The constructor constructs and sets the CmdLine arguments.

Some learning points:
- CmdLine, Arg classes has no default constructor, so they have to be constructed during the construction of `TagParser`, not after.
- Using a `static` reference of itself.