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
Writes a provided todo to a file at the current position in binary representation.

Returns EXIT_FAILURE if an error has occured. 
*/
int todo_write(FILE* file, const Todo* todo) {
    size_t items_written = 0;

    if (!file) {
        return EXIT_FAILURE;
    }
    
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
Writes a provided todo to a file at the end of the file in binary representation.

Returns EXIT_FAILURE if an error has occured. 
*/
int todo_write_end(FILE* file, const Todo* todo) {
    if (!file) {
        return EXIT_FAILURE;
    }

    // Append to the end of the file
    fseek(file, 0, SEEK_END);
    
    return todo_write(file, todo);
}


/*
Writes given TODOs to file. File cursor isn't changed in any way.

Returns EXIT_FAILURE in case of an error.
*/
int todos_write(FILE* file, Todo** todos, size_t todo_count) {
    if (!file || !todos) {
        return EXIT_FAILURE;
    }
    if (!(*todos)) {
        return EXIT_FAILURE;
    } 

    for (size_t i = 0; i < todo_count; i++) {
        if (!todos[i]) {
            continue;
        }

        if (todo_write(file, todos[i]) == EXIT_FAILURE) {
            return EXIT_FAILURE;
        }
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


/*
Marks todo as done
*/
void todo_mark_done(Todo* todo) {
    if (!todo) {
        return;
    }

    todo->done = 1;
    return;
}