# CPP 3D Renderer

Please note this is an amature project in progress. I've been working on this code for a while, however I've had to figure out the magical workings of github. If you find any mathematical errors please contact me.

This project is intended to be a 3d fluid simulation demonstrating how particles move in convection currents under a temperature gradient in a confined space. I've written based of reading I've done about SPH (smoothed particle hydrodynamics). In my physics engine I utilise the poly6 kernel for density, the spiky gradient kernel for pressure and the viscosity laplacian kernel for viscous damping. My main engine allows for particle interaction, the clicking and dragging was mainly for my own sake in the earlier stages of this topic and isn't completely necessary. I want to refine my rendering engine more, I haven't neatly commented my code yet and I need to thoroughly review all my workings and notes but this is it for now. 

## Dependencies
- OpenGL
- GLUT / freeglut
- C++23


## Controls
| Input | Action |
|-------|--------|
| Left drag (empty space) | Rotates camera |
| Left drag (particle) | Grabs and throws the particle |

## License
MIT