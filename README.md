# GLVolumeViewer
An Volume Viewer for .raw Files
Inspired from - https://github.com/SuboptimalEng/volume-rendering/tree/main

This is a volume renderer using:
-OpenGL  
-SDL
-GLM
-IMGUI

This is a rendering technique where we intersect with volume bounding box, ray march a step size and reading from a 3D Texture 
glTexImage3D(GL_TEXTURE_3D, 0, GL_R8, WIDTH, HEIGHT, DEPTH, 0, GL_RED, GL_UNSIGNED_BYTE, volumeData.data());

Also included is Blinn Phong and a simple camera system.

Future Updates: 
-Add parameters for color and the resizing of the AABB
