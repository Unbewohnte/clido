/*
 The MIT License (MIT)

Copyright © 2023 Kasyanov Nikolay Alexeyevich (Unbewohnte)

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the “Software”), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <stdio.h>
#include <stddef.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "todo.h"


/*
Closes a todo file and frees up memory allocated to store todos, basically
a clean up function.
*/
void clean_up(FILE* todo_file, Todo** todos, size_t todo_count, char* todo_file_path) {
    fclose(todo_file);
    for (size_t i = 0; i < todo_count; i++) {
        todo_delete(&todos[i]);
    }
    free(todo_file_path);
}


/*
Opens a TODO file for reading and writing at todo_file_path. If an error occurs - creates a new 
file instead.

Returns EXIT_FAILURE if creating a new file happens to be unsuccessful or one of the arguments are NULL.
*/
int open_todo_file(const char* todo_file_path, FILE** todo_file) {
    if (!todo_file_path || !todo_file) {
        return EXIT_FAILURE;
    }

    *todo_file = fopen(todo_file_path, "r+");
    if (!(*todo_file)) {
        // Create a new one
        printf("[INFO] Failed to read TODO file at \"%s\": %s. Creating a new one...\n", todo_file_path, strerror(errno));
        
        *todo_file = fopen(todo_file_path, "w+");
        if (!(*todo_file)) {
            fprintf(stderr, "[ERR] Failed to create a new TODO file at the same path: %s\n", strerror(errno));    
            return EXIT_FAILURE;
        }

        printf("[INFO] Successfully created a new TODO file\n");
    }

    return EXIT_SUCCESS;
}


int main(int argc, char** argv) {
    char* todo_file_name = "TODOS.bin";
    char* todo_file_dir_path = "./";
    char* todo_file_path = malloc(sizeof(char) * (strlen(todo_file_dir_path)+strlen(todo_file_name)+1));
    todo_file_path = strcat(todo_file_path, todo_file_dir_path);
    todo_file_path = strcat(todo_file_path, todo_file_name);
    size_t todo_count = 0;
    Todo** todos = NULL;
    char* arg = NULL;
    FILE* todo_file = NULL;
    int i = 0;


    // Open TODO file
    open_todo_file(todo_file_path, &todo_file);


    if (argc < 2) {
        printf(
            "A command is required!\
            \nRun clido help to look at usage overview\n"
        );
        clean_up(todo_file, todos, todo_count, todo_file_path);
        return EXIT_FAILURE;
    }


    // Collect and work out arguments
    while (i < argc) {
        i++;
        arg = argv[i];
        

        // Help command
        if (strcmp(arg, "help") == 0 && i == 1) {
            printf(
                "clido [COMMAND]\
                \n\n[COMMAND]:\
                \nhelp -> Prints this message and exists\
                \nadd [todo] -> Writes a new TODO to a default TODO file\
                \nshow -> Outputs current TODOs\
                \nshow-done -> Outputs TODOs which were done previously\
                \ndone [index OR index1-index2 OR index1,index2] -> Marks specified TODO(s) as done\
                \n"
            );
            clean_up(todo_file, todos, todo_count, todo_file_path);
            return EXIT_SUCCESS;
        }


        // Add command
        if (strcmp(arg, "add") == 0) {
            i++;
            arg = argv[i];
            if (i >= argc) {
                // No actual text was given
                fprintf(stderr, "No TODO text was given!\n");
                clean_up(todo_file, todos, todo_count, todo_file_path);
                return EXIT_FAILURE;
            }
            
            // Todo text will be everything from now on until the end
            size_t todo_text_length = 0;
            char* todo_text = NULL;

            for (size_t j = i; j < argc; j++) {
                char* todo_text_part = argv[j];

                todo_text_length += strlen(todo_text_part) + 2;
                todo_text = (char*) realloc(todo_text, todo_text_length);
                
                todo_text = strcat(todo_text, todo_text_part);
                todo_text = strcat(todo_text, " ");
            }

            Todo* new_todo = todo_new(strlen(todo_text), todo_text);
            if (todo_write(todo_file, new_todo) == EXIT_FAILURE) {
                fprintf(stderr, "[ERR] Failed to write a new todo: %s\n", strerror(errno));
                clean_up(todo_file, todos, todo_count, todo_file_path);
                return EXIT_FAILURE;
            }

            printf("Added!\n");
            
            clean_up(todo_file, todos, todo_count, todo_file_path);
            return EXIT_SUCCESS;
        }


        // Show command
        if (strcmp(arg, "show") == 0) {
            int result = todos_read(todo_file, &todos, &todo_count);
            if (result == EXIT_FAILURE && todo_count == 0) {
                printf("No TODOs yet!\n");
                return EXIT_SUCCESS;
            }

            for (size_t j = 0; j < todo_count; j++) {
                if (todos[j]->done) {
                    continue;
                }
                printf("[%ld] %s\n", j, todos[j]->text);
            }
            
            clean_up(todo_file, todos, todo_count, todo_file_path);
            return EXIT_SUCCESS;
        }


        // Show-done command
        if (strcmp(arg, "show-done") == 0) {
            int result = todos_read(todo_file, &todos, &todo_count);
            if (result == EXIT_FAILURE && todo_count == 0) {
                printf("No TODOs yet!\n");
                return EXIT_SUCCESS;
            }

            size_t printed = 0;
            for (size_t j = 0; j < todo_count; j++) {
                if (!todos[j]->done) {
                    continue;
                }
                printf("[%ld] %s\n", j, todos[j]->text);
                printed++;
            }

            if (printed == 0) {
                printf("No TODOs were done yet!\n");
            }
            
            clean_up(todo_file, todos, todo_count, todo_file_path);
            return EXIT_SUCCESS;
        }


        // Done command TODO!!!!!
        if (strcmp(arg, "done") == 0) {
            int result = todos_read(todo_file, &todos, &todo_count);
            if (result == EXIT_FAILURE && todo_count == 0) {
                printf("No TODOs yet!\n");
                return EXIT_SUCCESS;
            }

            i++;            
            if (i >= argc) {
                // Not one index was specified
                // Mark the first one as DONE
                
            }
            arg = argv[i];


            for (size_t j = 0; j < todo_count; j++) {
                if (todos[j]->done) {
                    continue;
                }
                printf("[%ld] %s\n", j, todos[j]->text);
            }
            

            clean_up(todo_file, todos, todo_count, todo_file_path);
            return EXIT_SUCCESS;
        }
    }

    printf(
        "No valid sequence of commands was specified!\
        \nRun clido help to look at usage overview\n"
    );
    clean_up(todo_file, todos, todo_count, todo_file_path);

    return EXIT_FAILURE;
}