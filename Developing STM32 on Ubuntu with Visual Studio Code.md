# Developing STM32 on Ubuntu with Visual Studio Code
## Prerequisite
### Toolchain
```bash
sudo apt update
sudo apt upgrade -y
sudo apt install -y gcc-arm-none-eabi cmake ninja-build openocd gdb-multiarch
```
- `gcc-arm-none-eabi`: ARM cross-compiler for STM32
- `cmake`: build configuration and project setup
- `ninja-build`: fast incremental build execution
- `openocd`: JTAG/SWD communication with hardware
- `gdb-multiarch`: cross-platform debugging
### Extension for Visual Studio Code
- [`C/C++`](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools): core language intelligence
- [`C/C++ Extension Pack`](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools-extension-pack): complete embedded development suite
- [`CMake Tools`](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cmake-tools): seamless CMake integration
- [`Cortex-Debug`](https://marketplace.visualstudio.com/items?itemName=marus25.cortex-debug): specialized ARM Cortex-M debugging
## Configuration
### STM32CubeMX
`Project Manager`>`Toolchan / IDE`>`CMake`
### VScode Extension `cortex-debug`
in *settings.json*
```json
    "cortex-debug.liveWatchRefreshRate": 250,
    "cortex-debug.armToolchainPath": "/usr/bin",
    "cortex-debug.gdbPath": "/usr/bin/gdb-multiarch",
    "cortex-debug.openocdPath": "/usr/bin/openocd",
```
### Edit *CMakeLists.txt*
```cmake
# Add sources to executable
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user sources here
)

# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined include paths
)
```
### Debug with *launch.json*
```json
        {
            "name": "Debug (OpenOCD)",
            "type": "cortex-debug",
            "request": "launch",
            "servertype": "openocd",
            "runToEntryPoint": "main",
            "liveWatch": {
                "enabled": true,
                "samplesPerSecond": 4
            },
            "executable": "./build/${workspaceFolderBasename}.elf", // ELF file path
            "device": "STM32F103C8", // MCU model
            "interface": "swd", // debug interface
            "configFiles": [
                "/usr/share/openocd/scripts/interface/stlink.cfg", // debugger interface configuration
                "/usr/share/openocd/scripts/target/stm32f1x.cfg" // MCU target configuration
            ],
            "svdFile": "STM32F103.svd", // optional: for peripheral check
            "serverArgs": [
                // default reset method unavailable for STM32H7 series        
                // "-c",
                // "reset_config none"
            ],
            // "showDevDebugOutput": "raw",
            // "showDevDebugTimestamps": true
        }
```
### *tasks.json*
```json
        {
            "label": "Program & Verify Flash (OpenOCD)",
            "type": "shell",
            "command": "openocd",
            "args": [
                "-f",
                "interface/stlink.cfg", // debugger interface configuration
                "-f",
                "target/stm32f1x.cfg", // MCU target configuration
                // default reset method unavailable for STM32H7 series        
                // "-c",
                // "reset_config none",
                "-c",
                "init",
                "-c",
                "reset halt",
                "-c", 
                "flash erase_sector 0 0 last", // erase command: flash erase_sector <bank> <first_sector> <last_sector>
                "-c",
                "program ./build/${workspaceFolderBasename}.elf verify reset exit",
            ],
            "problemMatcher": [
                "$gcc"
            ]
        },
        {
            "label": "Erase Flash (OpenOCD)",
            "type": "shell",
            "command": "openocd",
            "args": [
                "-f",
                "interface/stlink.cfg", // debugger interface configuration
                "-f",
                "target/stm32f1x.cfg", // MCU target configuration
                // default reset method unavailable for STM32H7 series        
                // "-c",
                // "reset_config none",
                "-c",
                "init",
                "-c",
                "reset halt",
                "-c",
                "flash erase_sector 0 0 last", // erase command: flash erase_sector <bank> <first_sector> <last_sector>
                "-c",
                "shutdown",
            ],
            "problemMatcher": [
                "$gcc"
            ]
        }
```
## Commom Operations
### Debug
1. Open your project then run `CMake: Build`
2. In `Run and Debug`, choose *Debug (OpenOCD)*
3. `Start Debugging (F5)`
### Program & Verify Flash
`Show and Run Commands`>`Tasks: Run Task`>`Program & Verify Flash (OpenOCD)`
### Erase Flash
`Show and Run Commands`>`Tasks: Run Task`>`Erase Flash (OpenOCD)`
## Attention *(FAQ)*
**GCC performs slightly differently from Clang**
### HardFault about FP
*GCC is more aggressive about keeping FP values in registers*  
expression as
```C
    *(float *)&data[] = /* FP calculation expression */;
```
may not work properly with FPU *(require strict align)*
#### Solution
use another variable to receive and assign value
```C
    *(float *)&data[] = temp = /* FP calculation expression */;
```