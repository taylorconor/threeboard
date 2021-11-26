# threeboard simulator build instructions

1. Complete all of the steps in the [firmware build instructions](firmware_build_instructions.md) guide if you haven't already.
2. Install additional simulator dependencies:
   ```
   sudo apt-get install libelf-dev libncursesw5-dev freeglut3-dev
   ```
3. Clone the custom simavr fork, which includes additional threeboard compatabiliity functionaliity:
   ```
   git clone git@github.com:taylorconor/simavr.git
   ```
4. Check out the threeboard compatability branch:
   ```
   cd simavr && git checkout threeboard_compat
   ```
5. Install simavr:
   ``` 
   sudo make install RELEASE=1
   ```
6. Navigate to the `threeboard/firmware` directory of your threeboard project clone.
7. Run the threbeoard simulator:
   ```
   bazel run //simulator:simulator
   ```