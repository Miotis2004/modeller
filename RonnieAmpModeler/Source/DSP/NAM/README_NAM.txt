Ronnie Amp Modeler - NAM Integration Instructions
===================================================

The project can build with a lightweight "Mock" NAM fallback, or with the full Neural Amp Modeler Core when you provide the dependencies locally.

To enable the full Neural Amp Modeler Core:

1. Download the `NeuralAmpModelerCore` source code:
   https://github.com/sdatkinson/NeuralAmpModelerCore

2. Download `Eigen` (Linear Algebra Library):
   https://eigen.tuxfamily.org/

3. Configure CMake with the NAM core and Eigen paths:
   - `-DRONNIE_USE_NAM_CORE=ON`
   - `-DNAM_CORE_PATH=/path/to/NeuralAmpModelerCore`
   - `-DEIGEN_PATH=/path/to/eigen`

When enabled, the build will add NAM core sources, include the NAM headers, and
enable the real `nam::get_dsp` loader. If these paths are not provided, the build
will use the lightweight mock path so the UI and DSP chain can still compile.
