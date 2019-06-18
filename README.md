# CS:GO Multihack

![alt text](https://github.com/ps1337/r/blob/master/csgo-multihack/1.gif?raw=true)
![alt text](https://github.com/ps1337/r/blob/master/csgo-multihack/2.gif?raw=true)
![alt text](https://github.com/ps1337/r/blob/master/csgo-multihack/3.gif?raw=true)
![alt text](https://github.com/ps1337/r/blob/master/csgo-multihack/4.gif?raw=true)

## Features

- Bunnyhop Bot
- NoFlash
- Aimbot (360 degree but also a bit crappy)
- (Get enemy locations and health values)

## Building and Running

```
<Edit the offsets in main.cpp according to your CS:GO version (e.g. with hazedumper)>
mkdir build && cd build
cmake .. && make
sudo chown root:root ./build/bananabot
sudo chmod 100 ./build/bananabot
<Start CS:GO and join a game>
sudo ./build/bananabot
```

The `chown` and `chmod` commands are used to "protect" the bot from Valve Anti Cheat (VAC).