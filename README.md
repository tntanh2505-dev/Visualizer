# Data Structure Visualizer
An interactive visualization tool for fundamental data structures, built with SFML (Simple and Fast Multimedia Library).

## Features
Supports visualization of four data structures:
- **Heap** — Min/max heap with dynamic insertion and removal
- **AVL Tree** — Self-balancing binary search tree with rotation visualization
- **Dijkstra's Algorithm** — Shortest path visualization on weighted graphs
- **Linked List** — Singly linked list with pointer visualization

### Operations
Each data structure supports the following operations:

| Operation | Description |
|-----------|-------------|
| Insert | Add a new element |
| Update | Modify an existing element |
| Delete | Remove an element |
| Search | Find an element and highlight it |
| Previous / Next | Step through elements sequentially |
| Random | Generate a random dataset |
| Clear All | Reset the structure |
| Load from File | Import a dataset from a `.txt` file |

## Installation

### Prerequisites
- [SFML](https://www.sfml-dev.org/download.php) — version 2.5 or later
- [CMake](https://cmake.org/download/) — version 3.15 or later
- A C++17-compatible compiler (GCC, Clang, or MSVC)

### Build Steps
```bash
# Clone the repository
git clone https://github.com/your-username/data-structure-visualizer.git
cd data-structure-visualizer

# Create a build directory
mkdir build && cd build

# Configure with CMake (adjust SFML path if needed)
cmake ..

# Build the project
cmake --build .

# Run the executable
./DataStructureVisualizer
```

> **Note:** Make sure SFML is installed and its path is correctly set in your environment or `CMakeLists.txt`.

## Project Structure
```
data-structure-visualizer/
├── src/
│   ├── linked_list/
│   ├── dijkstra/
│   ├── heap/
│   └── avl/
├── assets/
├── CMakeLists.txt
└── README.md
```

## Contributors

| Name | Component | GitHub |
|------|-----------|--------|
| Ly Trai Thai | Linked List | [@LyTranThai](https://github.com/LyTranThai) |
| Tran Dang Khoa | Dijkstra's Algorithm | [@trankhoa8581](https://github.com/trankhoa8581) |
| Tran Duc Toan | Heap | [@Toan-Tran-lab](https://github.com/Toan-Tran-lab) |
| Tran Ngoc Tuan Anh | AVL Tree | [@IrrationalRacoon2025](https://github.com/IrrationalRacoon2025) · [@tntanh2505-dev](https://github.com/tntanh2505-dev) |

> **Note:** Tuan Anh uses two GitHub accounts due to a Git Bash authenticator issue.

## License
This project is for educational purposes.
