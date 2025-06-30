# Developing STM32 on Ubuntu
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
- `gdb-multiarch`: c:ross-platform debugging
### Extension for Visual Studio Code
- [`C/C++`](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools): core language intelligence
- [`C/C++ Extension Pack`](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools-extension-pack): complete embedded development suite
- [`CMake Tools`](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cmake-tools): seamless CMake integration
- [`Cortex-Debug`](https://marketplace.visualstudio.com/items?itemName=marus25.cortex-debug): specialized ARM Cortex-M debugging
## Configuration
### STM32CubeMX
`Project Manager`/`Toolchan / IDE`: `CMake`
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
            "name": "STM32 Debug (OpenOCD)",
            "cwd": "${workspaceFolder}",
            "executable": "${workspaceFolder}/build/Debug/executable.elf", // replace with your ELF file path
            "request": "launch",
            "type": "cortex-debug",
            "runToEntryPoint": "main",
            "servertype": "openocd",
            "device": "STM32F103C8", // replace with your MCU model
            "interface": "swd", // debug interface
            "configFiles": [
                "/usr/share/openocd/scripts/interface/stlink.cfg", // STLink interface configuration
                "/usr/share/openocd/scripts/target/stm32f1x.cfg" // MCU target configuration
            ],
            "svdFile": "STM32F103.svd", // optional: for peripheral check
            // "showDevDebugOutput": "raw",
            // "showDevDebugTimestamps": true,
```
## Start Debug Session
1. open your project then run `CMake: Build`
2. `Run and Debug`