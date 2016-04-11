# Corona_C
Corona_C is a simple HTTP server written in C. It supports both http and https (with openssl). It can host multiple html web sites (similar to virtual hosts in Apache 2) with support for server side caching. It supports plug-in based extensions (.so files, written in C or CPP) which can be web applications, RESTful and non RESTful APIs etc... The server is written using basic Linux sockets and POSIX threads.

It was created simply as a entertainment exercise by me and does not even attempt to solve any practical shortcomings of any existing http servers. 
