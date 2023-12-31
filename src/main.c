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

#include <stdio.h>
#include <stddef.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "todo.h"


#define CLIDO_VERSION "0.1.1"


#ifdef _WIN32
#define TODO_FILE_PATH "TODOS.bin"

#else
#define TODO_FILE_PATH "/usr/local/share/clido/TODOS.bin"

#endif


/*
Closes a todo file and frees up memory allocated to store todos, basically
a clean up function.
*/
void clean_up(FILE* todo_file, Todo** todos, size_t todo_count) {
    if (todo_file) {
        fclose(todo_file);
    }

    if (!todos) {
        return;
    }

    for (size_t i = 0; i < todo_count; i++) {
        todo_delete(&todos[i]);
    }    
}


/*
Opens a TODO file for reading and writing at todo_file_path. If an error occurs - creates a new 
file instead. If provided todo_file points to an already opened file - closes it.

Returns EXIT_FAILURE if creating a new file happens to be unsuccessful or one of the arguments are NULL.
*/
int open_todo_file(const char* todo_file_path, FILE** todo_file) {
    if (!todo_file_path || !todo_file) {
        return EXIT_FAILURE;
    }

    if (*todo_file) {
        fclose(*todo_file);
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
    char* todo_file_path = TODO_FILE_PATH;
    size_t todo_count = 0;
    Todo** todos = NULL;
    char* arg = NULL;
    FILE* todo_file = NULL;
    int i = 0;


    if (argc < 2) {
        printf(
            "A command is required!\
            \nRun clido help to look at usage overview\n"
        );
        clean_up(todo_file, todos, todo_count);
        return EXIT_FAILURE;
    }


    // Collect and work out arguments
    while (i+1 < argc) {
        i++;
        arg = argv[i];
        

        // Help command
        if (strcmp(arg, "help") == 0 && i == 1) {
            printf(
                "clido [COMMAND]\
                \n\n[COMMAND]:\
                \nhelp -> Prints this message and exists\
                \nversion -> Prints version information and exits\
                \nadd [todo] -> Writes a new TODO to a default TODO file\
                \nshow -> Outputs current TODOs\
                \nshow-done -> Outputs TODOs which were done previously\
                \ndone [index]... -> Marks specified TODO(s) as done\
                \ntodo-path [path] -> Specifies another path pointing to the todo file\
                \n\nExamples:\
                \nclido add Do the cooking today\
                \nclido add Read a book\
                \nclido show\
                \nclido done 0 1\
                \nclido show-done\
                \n"
            );
            clean_up(todo_file, todos, todo_count);
            return EXIT_SUCCESS;
        }


        // Version command
        if (strcmp(arg, "version") == 0) {
            printf(
                "clido v%s- Command Line Interface to DO program\
                \n(c) 2023 Kasyanov Nikolay Alexeyevich (Unbewohnte)\
                \n", CLIDO_VERSION
            );
            clean_up(todo_file, todos, todo_count);
            return EXIT_SUCCESS;
        }


        // Todo-path command
        if (strcmp(arg, "todo-path") == 0) {
            i++;
            if (i >= argc) {
                fprintf(stderr, "No path was provided!\n");
                clean_up(todo_file, todos, todo_count);
                return EXIT_FAILURE;
            }

            // Try to open the specified one
            todo_file_path = argv[i];
            if (open_todo_file(todo_file_path, &todo_file) == EXIT_FAILURE) {
                return EXIT_FAILURE;
            }
        }


        // Add command
        if (strcmp(arg, "add") == 0) {
            // Open|Create|Reopen TODO file
            if (open_todo_file(todo_file_path, &todo_file) == EXIT_FAILURE) {
                return EXIT_FAILURE;
            }

            i++;
            if (i >= argc) {
                // No actual text was given
                fprintf(stderr, "No TODO text was given!\n");
                clean_up(todo_file, todos, todo_count);
                return EXIT_FAILURE;
            }
            arg = argv[i];

            
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
            if (todo_write_end(todo_file, new_todo) == EXIT_FAILURE) {
                fprintf(stderr, "[ERR] Failed to write a new todo: %s\n", strerror(errno));
                clean_up(todo_file, todos, todo_count);
                return EXIT_FAILURE;
            }

            printf("Added!\n");
            
            clean_up(todo_file, todos, todo_count);
            return EXIT_SUCCESS;
        }


        // Show command
        if (strcmp(arg, "show") == 0) {
            // Open|Create|Reopen TODO file
            if (open_todo_file(todo_file_path, &todo_file) == EXIT_FAILURE) {
                return EXIT_FAILURE;
            }

            int result = todos_read(todo_file, &todos, &todo_count);
            if (result == EXIT_FAILURE && todo_count == 0) {
                printf("No TODOs yet!\n");
                return EXIT_SUCCESS;
            }

            size_t printed = 0;
            for (size_t j = 0; j < todo_count; j++) {
                if (todos[j]->done) {
                    continue;
                }
                printf("[%ld] %s\n", j, todos[j]->text);
                printed++;
            }

            if (printed == 0) {
                printf("All is done!\n");
            }
            
            clean_up(todo_file, todos, todo_count);
            return EXIT_SUCCESS;
        }


        // Show-done command
        if (strcmp(arg, "show-done") == 0) {
            // Open|Create|Reopen TODO file
            if (open_todo_file(todo_file_path, &todo_file) == EXIT_FAILURE) {
                return EXIT_FAILURE;
            }

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
            
            clean_up(todo_file, todos, todo_count);
            return EXIT_SUCCESS;
        }


        // Done command
        if (strcmp(arg, "done") == 0) {
            // Open|Create|Reopen TODO file
            if (open_todo_file(todo_file_path, &todo_file) == EXIT_FAILURE) {
                return EXIT_FAILURE;
            }

            int result = todos_read(todo_file, &todos, &todo_count);
            if (result == EXIT_FAILURE && todo_count == 0) {
                printf("No TODOs yet!\n");
                return EXIT_SUCCESS;
            }

            i++;            
            if (i >= argc) {
                printf("Not one index was specified!\n");
                clean_up(todo_file, todos, todo_count);
                return EXIT_SUCCESS;
            }
            
            for (size_t j = 0; j < todo_count; j++) {
                if (todos[j]->done) {
                    continue;
                }

                for (size_t arg_index = i; arg_index < argc; arg_index++) {
                    size_t index = (size_t) atoll(argv[arg_index]);
                    if (index >= todo_count) {
                        continue;
                    }

                    if (j == index) {
                        // Mark as done
                        todo_mark_done(todos[j]);
                        printf("Marked \" %s\" as done!\n", todos[j]->text);
                    }
                }
            }
            
            // Re-write TODO file (Truncation is no needed as only one bit has changed)
            fseek(todo_file, 0, SEEK_SET);
            if (todos_write(todo_file, todos, todo_count) == EXIT_FAILURE) {
                fprintf(stderr, "[ERR] Failed to write updated todos to a todo file: %s\n", strerror(errno));
                clean_up(todo_file, todos, todo_count);
                return EXIT_FAILURE;
            }

            clean_up(todo_file, todos, todo_count);
            return EXIT_SUCCESS;
        }
    }

    printf(
        "No valid sequence of commands was specified!\
        \nRun clido help to look at usage overview\n"
    );
    clean_up(todo_file, todos, todo_count);

    return EXIT_FAILURE;
}