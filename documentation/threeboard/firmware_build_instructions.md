# threeboard firmware build instructions

1. Install Bazel using the instructions for your OS/distro as described in the [Bazel installation guide](https://docs.bazel.build/versions/4.0.0/install.html).
2. Install the packages that the threeboard firmware depends on:  
   ```
   sudo apt-get install build-essential gcc-avr avr-libc git
   ```
3. If you don't already have a C++17 compatible compiler, install GCC version 8:  
   ```
   sudo apt-get install g++8
   ``` 
3. Clone the threeboard git repository:  
   ```
   git clone git@github.com:taylorconor/threeboard.git
   ```
4. Navigate to the Bazel workspace root of the threeboard project:  
   ```
   cd threeboard/firmware
   ``` 
5. Compile the threeboard firmware into a `.hex` file that can be flashed to hardware:  
   ```
   bazel build //src:threeboard_hex
   ```  
   The firmware hex file will be written to `threeboard/firmware/bazel-bin/src/threeboard_hex.hex`.