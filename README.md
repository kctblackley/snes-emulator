# snes-emulator

Set-up steps:
1. Clone via `git clone github.com/kctblackley/snes-emulator`;
2. Due to copyright restrictions, the SNES SPC-700 IPL ROM will need to be manually provided. This can be found online. It is a 64-byte file;
3. Download the IPL ROM. In the emulator's root directory, there is a folder, `ipl`. Place the IPL ROM file in there. Name the file `ipl.rom`;
4. Via the terminal, locate to the root directory for the emulator. Build in Release mode via: `cmake -B build -DCMAKE_BUILD_TYPE=Release ; cmake --build build`
5. This completes the set-up.

Usage:
1. To use, download a USA/Japan ROM for a game of your choosing. Place the downloaded file in the `rom` folder;
2. Rename the ROM to a simple name (e.g. `Zelda - A Link to the Past.sfc` should become `zelda.sfc`)
3. To run the emulator, navigate to the root directory. Run `./build/snes_emulator ___.sfc` (for example: `./build/snes_emulator zelda.sfc`)

