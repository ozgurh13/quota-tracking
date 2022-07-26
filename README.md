
# quota tracking

a prototype implementation for a quota tracking system


#### features

 * interface based  
 choose which interface (`em0`, `wlp2s0`) to listen to for traffic

 * interactive  
 start and stop listening to an interface in real time by sending commands through it's shell

 * ip and mac support  
 select whether to track using ip or mac address



#### usage

make the project by running `make.sh`. this will build the project and the interactive shell.  
```
# sh make.sh
```
the main project is written in C and the shell is written in Haskell so you will need both `gcc` and `ghc`.

after this run the main program
```
# ./main
```
this will start the tracker, however it won't be doing anything.

to start listening to an interface you need to send the command through the shell
```
# ./shell
```
**NB: you need to run both `main` and `shell` with root privileges so that `main` can listen to the interfaces and `shell` can send commands to `main`.**

for the shell commands, type `help`
```
# ./shell
~> help
```
this will list all commands available and their respective usages  
***note**: the output of the commands sent will not appear in the shell, it will be printed by the main program to stdout*



#### benchmark

`iperf3` says this program can handle 30Gbits/sec (tested using the loopback interface)

