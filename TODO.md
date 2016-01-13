# TODO
- [ ] Game over: 
Energy Cubes killed: 32 / 4 / 0
Stars collected: 5
Total time: 0:32
Points: 124500
- [ ] add matching explosions for every cube type
- [ ] fix all dialogs
- [ ] try HUGE clock in the center
- [ ] add wave/cube which is big -> moves for a certain time and then stops and rotates towards player and starts moving again
- [ ] border color flashing when explosion
- [ ] get number of stars to generate from cube definition in MainGameState
- [ ] try to make inner line of border a bit darker
- [ ] try different numbers of bombs

8.1.2016
- [x] rename EnergyBalls to Cubes

5.1.2016
- [x] check game settings -> everything still needed?

16.12.2015
- [x] different behaviors
- [x] each level means 100 + 50 * level balls
- [x] show flashing Level X message
- [x] ball emitter does not calculate emitt frequency correctly

8.12.2015
- [x] rotate bomb according to velocity
- [x] add wandering behavior - SIMPLE_MOVE_BIT
- [x] when creating a gate -> popup animation

5.12.2015
- [x] change color of bomb when grabbed

4.12.2014
- [x] gate explosion - remove balls and check player
- [x] add played time timer
- [x] WONTFIX: switch to game over state when player dies (but keep Game running)
- [x] WONTFIX: extract stuff from MainGameState into separate class

3.12.2015
- [x] use huge numbers for scoring / timer / level but with high transparency
- [x] implement push behavior -> player can control bomb by pressing right button and drag it. When releasing button bomb will be pushed in opposite direction
- [x]  stars will give player time (add seconds)
- [x] add timer - starting at 1:00 minute - picked up stars will give you time
- [x] add settings dialog
- [x] ball spawner
- [x] ball and player interception
- [x] ball - make behaviors configurable

02.12.2015
- [x] player should follow mouse pos
- [x] show mouse cursor
- [x] move stars towards player if in radius
- [x] add event buffer so balls, gates and stars can communicate

01.12.2015
- [x] add stars to collect
