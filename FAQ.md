# Why not a modern language like Go, Dart, Rust?

These languages have much of what Unit provides and don't work easily everywhere like C does.

# Why not in C++?

C++ was also considered as it is as well supported as C and provides better object controls.
Since Unit uses a much simpler object model it was felt that C++ would encourage application
developers to use a mixed style that would not make better applications.

# Why no inheritance?

Unit has been designed using composition instead of inheritance. This is helped by making objects
as light weight as possible.

# Can I make a dynamic library of Unit?

It would be technically possible to compile Unit as a dynamic library, however the supported way of
using it is to compile it into your application instead. A dynamic library would be less efficient
as it would contain code that is not used and be less optimised for the particular way the
application uses it.

# Do you support other free operating systems?

Unit is defined as a Linux toolkit, however changes to support other similar operating systems (BSD,
Fuschia etc) will be accepted as long as they can be reasonably maintained.

# Do you support Windows / macOS?

It's not the goal of Unit to be cross platform, but this is an interesting case if it can be done in
a way that is manageable.

# What do I do if there is a security bug in Unit?

You need to monitor Unit for bugs like this and rebuild and redeploy your software if this occurs.
This is the cost of using a source based toolkit and relies on application developers being able to
respond quickly.

# How can I help?

The best way to contribute it to use Unit, feedback any issues/ideas through the GitHub project and
make pull requests for changes.
