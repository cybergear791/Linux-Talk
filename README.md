# Linux Talk
Acts just like the UNIX Talk.

# Setup
1. Run the makefile by typing `make`
2. run `./server.x` to start the host client.
3. run `./client.x [IP]` where `[IP]` is your local ip.

#### For Linux Users Only:
Add `#include<pthread.h>` to both server.cpp and client.cpp and add the parameter `-pthread` in the compilation of client.x and server.x in the makefile.

## Host's View
![](images/host.png)
## Client's View
![](images/client.png)
