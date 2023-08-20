# clido
## Command Line Interface to DO program

clido helps to track and keep "to do" notes on the disk.


## Compilation

Simply `make` or as simply compile .c files together


## Installation

If you're on Linux - `sudo make install` **or** `sudo ./install.sh` **or if not** simply move the binary to wherever you desire.


## Usage

`clido [COMMAND]`

Commands:

- `help -> Prints help message and exists`
- `version -> Prints version information and exits`
- `add [todo]... -> Writes a new TODO to a default TODO file`
- `show -> Outputs current TODOs`
- `show-done -> Outputs TODOs which were done previously`
- `done [index]... -> Marks specified TODO(s) as done`
- `todo-path [path] -> Specifies another path pointing to the todo file`


## Examples

- `clido add Do the cooking today` -> appends a new todo with given text to a todo file
- `clido show` -> prints current todos
- `clido done 0 1` -> marks todos with indexes 0 and 1 as done
- `clido show-done` -> prints todos which are done at this point

## License
![GPLv3](https://www.gnu.org/graphics/gplv3-with-text-84x42.png "GPLv3 logo")

GPLv3