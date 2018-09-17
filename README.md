# GPU-Particle-Demo

This demo explores the functionality of Compute & Geometry shaders. The particle properties are updated in the compute shader and the 
output is copied to the vertex buffer. The geometry shader expands the vertex into a quad and also does billboarding. There are 3 emitters whose particles follow a spiral path.

# Performance

Able to render over 4 million particles at 60 fps on an AMD Radeon RX580. 

![alt text](https://github.com/RahulSVasudevan/GPU-Particle-Demo/blob/master/Screenshots/SpiralGalaxy.PNG)
