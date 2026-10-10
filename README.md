# frontier_vs_oc
## HOW TO RUN THE CODE

1. Install Visual Studio Code (VS Code) on your loacl pc.
2. Install the PlatformIO IDE extension in VS Code.
3. Open the GitHub repository: `riad2008004/frontier_vs_oc`.
4. Download or clone the repository to your local computer.
5. Open the downloaded project folder in VS Code. Ensure that `platformio.ini` is located in the project root directory.
6. Allow PlatformIO to install the required platform and library dependencies automatically.
7. Connect the Arduino Uno to your computer using a USB cable.
8. Build in PlatformIO: Build to compile the source code.
9. Generate HEX in .pio/build/uno/firmwire.HEX. For generating HEX open the powershell in local directory if not generated automatically.
10. Paste the following code with path modification if need.


$tool = Get-ChildItem "$env:USERPROFILE\.platformio\packages" -Filter arm-none-eabi-objcopy.exe -Recurse -ErrorAction SilentlyContinue | Select-Object -First 1 -ExpandProperty FullName

if ($tool) {
    & $tool -O ihex ".pio\build\uno\firmware.elf" ".pio\build\uno\firmware.hex"
} else {
    Write-Host "objcopy tool not found. Check PlatformIO installation."
}


## For Proteus Simulation:
1. Open the provided file from projects files ('Frontier_voltage_surge_and_oc.pdsprj').
2. Build or open the corresponding circuit in Proteus.
3. Build the firmware and patch the Arduino Uno HEX file .pio/build/uno/firmwire.HEX. 
4. Load the HEX file into the Arduino Uno component in Proteus.
5. Run the simulation and test normal operation, voltage surge, overcurrent, relay switching, and fault reset.