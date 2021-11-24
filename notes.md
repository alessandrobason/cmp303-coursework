# client
- has same basic objects, but they are devoid of function, 
  they just render the sprite and animations
- the player send the direction its going in every time it changes
- the player receives the position every time it changes
- the player receives the current anim
- the player receives this data every time it changes:
  - position
  - animation
  - state
- if the position change is too big, the sprite interpolates between 
  the old position and the new one
- the current map is choosen by the server and sent to every map


## How a normal game goes

### Server side
- server starts and opens a connection
- waits for all the player to connect
- when everybody is connected, the server sends the position of all 
  the crates to every player
- it sends all the players names
- it sends all the players' positions and whose the main player 
  to every connection
- it sends a MSG_READY message with the map id to load

By now the game has actually started, on a normal frame the server:
- updates and renders the game on one thread
- waits for messages on the other

### Client side

One of the main problems to solve was syncing the network
thread with the main thread every time something new had to be added.
To fix this I made extensive use of my own mutexes.
The colla repo is a collection of small libraries that are fairly simple
but avoid a lot of rewriting. It is also 100% portable between linux and
windows. The mutexes on windows are implemented using critical sections, which are less
flexible but should be much faster when the mutex isn't locked. 

### other

Every time a player does something, the server broadcasts the 
consequences to every other player.
For example, if a player puts down a bomb, every player will 
receive a OBJ_ADD message with the id = Bomb::id() (every objects has
its own id) and the position.

Most of the messages are fairly simple, whenever something is added to
the map the server sends a OBJ_ADD message, whenever something is 
removed, the server sends a OBJ_REM message.

Whenever a message is sent, it gets converted to the net endianess using
hton(s, l, ll) family of functions.

every message is sent with a templated packed struct called Message.
Message has two variables: the type (a uint8_t) and data (templated).

These are the type of messages:
- MSG_ERR: error value, never used
- MSG_READY: sent with the map_id when the server is ready to start
- MSG_NEW_PLAYER: sent when a player is added with its id and a bool
  that says if this player is the controlled one
- MSG_PLAYER_NAME: sent when a player is added with its id and a string for its name
- MSG_DIR: sent by a client every time the direction changes
- MSG_POS: sent by the server every time a position changes
- MSG_ANIM: sent by the server every time an animation changes
- MSG_STATE: sent by the server every time a state changes
- MSG_BOMB_REQ: sent by a client whenever the player tries to put down
  a bomb
- MSG_OBJ_ADD: sent by the server every time an object is added
- MSG_OBJ_REM: sent by the server every time an object is removed
- MSG_POW_ADD: sent by the server every time a player picks up a powerup

# server