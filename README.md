Unit is a no-dependency application C toolkit for Linux.
An application built using Unit compiles the Unit source into that
application and the resultant binary has no dynamic library dependencies.

Some of the thoughts that led to the development of Unit:
 - The protocols and formats in use are well established and therefore
   can be reimplemented without too much ongoing change.
 - A consistent style across all modules makes development faster and
   easier to debug.
 - Large projects can be effectively managed using services like GitHub.
 - Continuous integration allows quality to be kept high.
 - Bandwidth and storage is sufficiently large that code duplication
   between applications is not a major concern.
 - Processors and compilers are fast enough to compile much more code
   than is required.
 - Compilers will effectively optimize away any code that is not used.
 - Software can be easily distributed and updated if bugs are found in
   Unit.

Unit provides:
 - An object model that uses interfaces (no inheritence).
 - Asynchronous operations.
 - Character strings (UTF-8, UTF-16).
 - Lists and maps.
 - Image en/decoding (JPEG, PNG, TIFF, GIF).
 - Compression (gzip, zlib, LZW, DEFLATE).
 - Networking (IPv4, IPv6, HTTP, TCP, UDP, DNS, Unix sockets).
 - Serialization (Base64, protobuf, ASN.1 BER, JSON, XML)
 - Inter-process communication (DBus).
 - Windowing (Wayland, X11).

Unit is still in early development and not suitable as a mature toolkit.
Cases where Unit might be useful:
 - Developing on embedded devices.
 - Experimental applications.
 - Understanding protocols and formats.
