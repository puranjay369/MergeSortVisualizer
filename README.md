
# Merge Sort Visualizer (SFML)

A graphical visualizer for the Merge Sort algorithm using C++ and SFML.

## Features
- Step-by-step animation of Merge Sort
- Adjustable speed and array size
- Statistics: comparisons, array accesses, merge operations
- Interactive controls

## Requirements
- **C++17 or newer**
- **SFML 2.x** (Simple and Fast Multimedia Library)
	- Libraries: `sfml-graphics`, `sfml-window`, `sfml-system`
- Windows (tested), but should work on Linux/macOS with minor changes

## Setup Instructions
1. **Install SFML**
	 - Download SFML from: https://www.sfml-dev.org/download.php
	 - Follow the official guide for your OS: https://www.sfml-dev.org/tutorials/2.6/
	 - Place the SFML DLLs (`sfml-graphics-2.dll`, `sfml-window-2.dll`, `sfml-system-2.dll`) in the same folder as the executable or add them to your system PATH.
2. **Build the Project**
	 - Open a terminal in the `final` directory and run:
		 ```sh
		 g++ -std=c++17 merge_sort_visualizer.cpp -o merge_sort_visualizer.exe -lsfml-graphics -lsfml-window -lsfml-system
		 ```
	 - Or use the provided VS Code build task (`build SFML`).
3. **Run the Visualizer**
	 - In the `final` folder, run:
		 ```sh
		 ./merge_sort_visualizer.exe
		 ```

## Controls
- **SPACE**: Play/Pause animation
- **R**: Reset to beginning
- **UP/DOWN arrows**: Adjust speed
- **N**: Generate new random array (same size)
- **ESC**: Exit program

## Troubleshooting
- If you see errors about missing DLLs, download them from the SFML website and place them next to the `.exe`.
- If you get linker errors, make sure SFML is installed and the library paths are set up correctly.

## License
This project is for educational purposes. See SFML's license for DLL redistribution terms.

---
Feel free to fork, modify, and share!
