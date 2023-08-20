/*
 The MIT License (MIT)

Copyright © 2023 Kasyanov Nikolay Alexeyevich (Unbewohnte)

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the “Software”), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
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