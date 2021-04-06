# TerrainGeneration_OpenGL
This is a school project: an OpenGL game with procedural terrain generation and portals.

Gameplay:

The goal of the game is to look around, find the portal and go through it.
Each time you go through it or crash (skybox or ground), the terrain is recreated procedurally.
There are 4 different environments to see depending on terrain height.

Controls:

Use the arrow-up key or w to move forward, and the arrow-side keys or the mouse (preferred) to turn 
as you advance.
Press p to pause and look around with the arrow keys only (no mouse in pause mode).
Press r to go back to first position.
Press n to generate a new terrain.

Problems:

I wanted to make a game out of it but could not because I did not manage to use VBOs correctly.
Grassland and Forest terrains have multiple objects which takes to long to process at each frame.
I tried using Instances but did not manage to.
The camera was supposed to advance on its own and the user would have guided it to the portal,
with speed increasing each level. But the frame rate difference between terrains made it impossible.
So this is not really a game in the end.

Source:
Youtube videos mostly for the terrain generation with perlin.

Screenshots:
![desert](/screenshots/screenshot_desert.png)
![mountains](/screenshots/screenshot_mountains.png)
![plain](/screenshots/screenshot_plain.png)
![forest](/screenshots/screenshot_forest.png)
![portal](/screenshots/screenshot_portal.png)
