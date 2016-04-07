# GL4Framework

GL4Framework is a modern OpenGL 4.x framwork with Python interfaces. In order to support OS X, features that require OpenGL 4.2 or higher are not adopted. 

#Features
* Deferred Renderering

<img src="https://raw.githubusercontent.com/v3c70r/GL4Framework/master/screenshots/deferredRendering.png" width="300">
* Forward Renderering
* Screen Space Renderering for Particle System
* GPU based skeleton animation

<img src="https://raw.githubusercontent.com/v3c70r/GL4Framework/master/screenshots/anim.gif" width="300">
* Python APIs 
* Raytracer works with boxes from [this tutorial](https://github.com/LWJGL/lwjgl3-wiki/wiki/2.6.1.-Ray-tracing-with-OpenGL-Compute-Shaders-%28Part-I%29). 
* CUDA SPH System
* Joystick Support 

<img src="https://github.com/v3c70r/GL4Framework/raw/master/screenshots/joystick.gif" width="300">

#TODOs
* Deferred Rendering with SSAO
* Surface Editing using Laplacine with rotation from this [Paper](http://igl.ethz.ch/projects/Laplacian-mesh-processing/Laplacian-mesh-editing/laplacian-mesh-editing.pdf). (I have done this before in 691F using [Graphite](http://alice.loria.fr/index.php?option=com_content&view=article&id=22). But it's a good idea to incoperate this feature in my engine) (THOUGHT: Maybe I can use OpenMesh?)
* Eliminate all `stars`. Using smart pointers to manage resources







