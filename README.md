# STL_SpatialDecomposition
This repository contains utlities to spatially decompose an STL mesh onto a cartesian grid to speed up triangle lookup. It also generates flattened arrays to push the data for CUDA applications. Leaks in the algorithm are tested with a simple Discrete Element solver.

The video below(rendered using ray tracing) shows particles enclosed within an stl hollow sphere. 

![](ezgif-2-c2aefe3cbfc6.gif)

PS : The particle you may see flying out is because the stl mesh is not water tight. There is a missing triangle.

_Reference_ : https://fileadmin.cs.lth.se/cs/Personal/Tomas_Akenine-Moller/pubs/tribox.pdf
