Kai Lindskog, Zachary Wilkins-Olson, Zachary Sherman, Shane Bliss, Dillon Stickler
Oregon State University
CS 462

A-Life simulation project. Agents move around a grid, consume resources, reproduce, and evolve over time. Built in C++17.

## Build & Run

g++ -std=c++17 -I include test/test_main.cpp src/resource_node.cpp -o test_executable
./test_executable

## Persistence (save/load system)

Requires PostgreSQL. On macOS: `brew install postgresql@14`

Set up the database once:
```
psql -U <your_user> -d postgres -c "CREATE DATABASE alife_sim;"
psql -U <your_user> -d alife_sim -f db/schema.sql
```

Set env vars (or add to ~/.zshrc):
```
export ALIFE_DB_HOST=localhost
export ALIFE_DB_PORT=5432
export ALIFE_DB_NAME=alife_sim
export ALIFE_DB_USER=<your_user>
```

Build the persistence module:
```
mkdir -p build && cd build
cmake ../persistence
make -j4
```

Run persistence tests:
```
./test_persistence
./test_auto_save
```

## CLI & Autosave

Build and run the simulation from the project root:
```
mkdir -p build && cd build
cmake ..
make -j4
./main --ticks 100 --autosave 25
```

Options:
| Flag | Default | Description |
|------|---------|-------------|
| `--ticks N` | 10 | Number of simulation ticks |
| `--autosave K` | 0 (off) | Save state history every K ticks |
| `--buffer-size N` | 1000 | Circular buffer capacity |
| `--save-dir DIR` | `saves/` | Output directory for autosave files |
| `--help` | | Show usage |

## Project structure

```
include/        headers (circular buffer, fitness, resource node, save system)
src/            implementations
db/             PostgreSQL schema
persistence/    CMakeLists.txt for the save/load module
test/           integration tests
decision_center/ brain / neural decision making
```



