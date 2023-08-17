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
#include <error.h>
#include <errno.h>
#include <string.h>
#include <stddef.h>
#include "todo.h"


size_t file_get_size(FILE* file) {
    size_t current_pos = 0;
    size_t size = 0;

    if (file) {
        current_pos = ftell(file);
        fseek(file, 0, SEEK_END);
        size = ftell(file);
        fseek(file, current_pos, SEEK_SET);        
    }

    return size;
}


/*
Allocates memory to store a new TODO in it.

Returns a pointer to TODO.
*/
Todo* todo_new(uint32_t text_len, char* text) {
    Todo* new_todo = malloc(sizeof(Todo));
    if (new_todo) {
        new_todo->text_len = text_len;
        new_todo->text = text;
    }

    return new_todo;
}


/*
Frees memory allocated for storing a todo. 

Sets pointer to todo to NULL; does nothing if NULL is received
*/
void todo_delete(Todo** todo) {
    if (!todo) {
        return;
    } else if (!*todo) {
        return;
    }

    free((*todo)->text);
    free(*todo);

    *todo = NULL;
    return;
}


/*
Writes a provided todo to a file at current position in binary representation.

Returns EXIT_FAILURE if an error has occured. 
*/
int todo_write(FILE* file, const Todo* todo) {
    size_t items_written = 0;

    if (!file) {
        return EXIT_FAILURE;
    }

    // Append to the end of the file
    fseek(file, 0, SEEK_END);
    
    // Text length
    items_written = fwrite(&todo->text_len, sizeof(uint32_t), 1, file);
    if (items_written != 1) {
        return EXIT_FAILURE;
    }

    // Text
    items_written = fwrite(todo->text, sizeof(char), todo->text_len, file);
    if (items_written != todo->text_len) {
        return EXIT_FAILURE;
    }

    // Done flag
    items_written = fwrite(&todo->done, sizeof(uint8_t), 1, file);
    if (items_written != 1) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}


/*
Reads the next TODO in provided file, puts it in todo.

Returns EXIT_FAILURE if an error has occured.
*/
int todo_read(FILE* file, Todo* todo) {
    size_t read_items = 0;

    if (!file || !todo) {
        return EXIT_FAILURE;
    }

    // Read text length
    read_items = fread(&todo->text_len, 1, sizeof(uint32_t), file);
    if (read_items != sizeof(uint32_t)) {
        return EXIT_FAILURE;
    }

    // Read text
    todo->text = malloc(todo->text_len);
    read_items = fread(todo->text, 1, todo->text_len, file);
    if (read_items != todo->text_len) {
        return EXIT_FAILURE;
    }

    // Read whether it's been done or not
    read_items = fread(&todo->done, 1, sizeof(uint8_t), file);
    if (read_items != 1) {
        return EXIT_FAILURE;
    }


    if (feof(file)) {
        return EXIT_SUCCESS;
    }

    return EXIT_SUCCESS;
}


/*
Tries to parse TODOs from a given TODO FILE. 

todos is a pointer to the beginning of a future TODO array and todos_read 
is the amount of TODOs read from file. 

Returns EXIT_FAILURE if an error occured somewhere.
*/
int todos_read(FILE* file, Todo*** todos, size_t* todos_read) {
    *todos_read = 0;
    *todos = NULL;

    if (!file) {
        return EXIT_FAILURE;
    }

    while (1) {
        Todo* read_todo = todo_new(0, "");
        int result = todo_read(file, read_todo); 
        if (result == EXIT_FAILURE && *todos_read == 0) {
            return EXIT_FAILURE;
        } else if (result == EXIT_FAILURE && feof(file)) {
            break;
        }

        *todos_read = (*todos_read) + 1;
        *todos = realloc(*todos, sizeof(Todo) * (*todos_read));
        (*todos)[(*todos_read)-1] = read_todo;
    }

    return EXIT_SUCCESS;
}


//TODO
int todo_mark_done() {
    return EXIT_SUCCESS;
}