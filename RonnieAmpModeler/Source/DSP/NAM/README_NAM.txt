Ronnie Amp Modeler - NAM Integration Instructions
===================================================

The project is currently configured with a "Mock" NAM integration to allow building and testing the UI/Signal Chain without external heavy dependencies.

To enable the full Neural Amp Modeler Core:

1. Download the `NeuralAmpModelerCore` source code:
   https://github.com/sdatkinson/NeuralAmpModelerCore

2. Download `Eigen` (Linear Algebra Library):
   https://eigen.tuxfamily.org/

3. Replace `Source/DSP/NAM/NamLoader.h` with the actual inclusion of NAM headers.

4. Update `NamModelProcessor.h` to instantiate the real `nam::DSP` object instead of `nam::MockModel`.

5. Update `CMakeLists.txt` to include the NAM core source files and the Eigen include path.

The current `NamLoader.h` provides a structure compatible with the real loader (`nam::get_dsp`), so minimal code changes are required in `NamModelProcessor`.
