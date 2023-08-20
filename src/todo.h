/*
    clido - Command Line Interface to DO program
    Copyright (C) 2023 Kasyanov Nikolay Alexeyevich (Unbewohnte)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/


#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>


// TODO representation
typedef struct Todo {
    uint32_t text_len;
    char* text;
    uint8_t done;
} Todo;


/*
Allocates memory to store a new TODO in it.

Returns a pointer to TODO.
*/
Todo* todo_new(uint32_t text_len, char* text); 


/*
Frees memory allocated for storing a todo. 

Sets pointer to todo to NULL; does nothing if NULL is received.
*/
void todo_delete(Todo** todo);


/*
Writes a provided todo to a file at the current position in binary representation.

Returns EXIT_FAILURE if an error has occured. 
*/
int todo_write(FILE* file, const Todo* todo);


/*
Writes a provided todo to a file at the end of the file in binary representation.

Returns EXIT_FAILURE if an error has occured. 
*/
int todo_write_end(FILE* file, const Todo* todo);


/*
Writes given TODOs to file. File cursor isn't changed in any way.

Returns EXIT_FAILURE in case of an error.
*/
int todos_write(FILE* file, Todo** todos, size_t todo_count);


/*
Reads the next TODO in provided file, puts it in todo.

Returns EXIT_FAILURE if an error has occured.
*/
int todo_read(FILE* file, Todo* todo);


/*
Tries to parse TODOs from a given TODO FILE. 

todos is a pointer to the beginning of a future TODO array and todos_read 
is the amount of TODOs read from file. 

Returns EXIT_FAILURE if an error occured somewhere.
*/
int todos_read(FILE* file, Todo*** todos, size_t* todos_read);


/*
Marks todo as done
*/
void todo_mark_done(Todo* todo);