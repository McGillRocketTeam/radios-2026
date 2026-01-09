# Preamble

`tx.cpp` - uses one radio to simulate data atomics coming from the flight computer.

`rx.cpp` - uses automatically generated `frame_printer`s to print the atomic data to the Serial monitor.

`main.cpp` - not used.

# Usage

Use the dedicated PlatformIO project tasks (accessible from the left VS Code sidebar) to build and upload the TX or RX scripts to each Teensy.

# Requirements

The project assumes that the required Telemetry header and source code files share the same parent directory as this project:

```
ParentFolder\
    Telemetry_2026\
        telemetry\
            fixed\
            gen\
    Radios_2026\
        EthernetTesting\
        GroundStation\
        FC_Mocker\
```

To change this default behaviour:
1. Update the `config.telemetry_repo` variable in `platformio.ini` to contain the path of the Telemetry_2026 directory
2. Configure VS Code IntelliSense by updating the `includePath` in `.vscode\c_cpp_properties.json`.