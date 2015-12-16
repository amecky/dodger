# TODO

- [ ] fix game over dialog: Balls killed / Stars collected / Level / Time played

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
