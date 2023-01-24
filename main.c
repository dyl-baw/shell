//
// Created by David Lee on 2023-01-24.
//


#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

#define MAX_ARGS 100

//created by David Lee
//Assignment 1 - Unix Shell
//There are some bugs :(
//January 23, 2023.

void parseCommand(char *input, char **args, char **file)
{
    int i = 0;
    char *arg;

    arg = strtok(input, " "); //tokenizing input string by spaces
    while (arg != NULL)
    {
        if (strcmp(arg, ">") == 0)
        {
            file[0] = strtok(NULL, " ");
            break;
        } else if (strcmp(arg, ">>") == 0) {
            file[0] = strtok(NULL, " ");
            file[2] = "append";
            break;
        } else if (strcmp(arg, "<") == 0)
        {
            file[1] = strtok(NULL, " ");
        } else
        {
            args[i] = arg;
            i++;
        }
        arg = strtok(NULL, " ");
    }
    args[i] = NULL;
}


int main()
{
    char input[100];
    char *args[MAX_ARGS];
    char *file[3] = {NULL, NULL,NULL};
    int fd_in, fd_out, fd;
    int shell_running = 1;

    while (shell_running)
    {


        printf("David - shell $"); // prompt user to enter their command
        fgets(input, sizeof(input), stdin); //get user input from STDIN
        input[strcspn(input, "\n")] = 0; // this checks if the user input is an enter key. If so shell continues.
        if (strcmp(input, "") == 0)
        {
            continue;
        }
        parseCommand(input, args, file);

        if (strcmp(args[0], "cat") == 0)
        {
            fd = open(args[1], O_RDONLY);
            if (fd == -1)
            {
                perror("");
                continue;
            }
        }

        if (strcmp(args[0], "cd") == 0)
        {
            if (args[1] == NULL)
            {
                const char *home_env = getenv("HOME");
                if (home_env == NULL)
                {
                    perror("");
                    continue;
                }
                chdir(home_env);
            } else if (strcmp(args[1], "~") == 0) // check if symbol after cd is a tilda
            {
                const char *home_env = getenv("HOME");
                if (home_env == NULL)
                {
                    perror("");
                    continue;
                } else
                {
                    chdir(home_env); // if everything is passes, send them to the home.
                    continue;
                }
            }
            if(chdir(args[1]) != 0) { // if chdir fails, error out.
                perror("");
            }
            continue;
        }
        if (strcmp(args[0], "exit") == 0) {
            exit(EXIT_SUCCESS);
        }

        if (file[0] != NULL)
        {
            if (file[2] == "append")
            {
                fd_out = open(file[0], O_CREAT | O_WRONLY | O_APPEND, 0644); // on success = 1
            } else
            {
                fd_out = open(file[0], O_CREAT | O_WRONLY | O_TRUNC, 0644);
            }
            if (fd_out == -1) //open fails. fd_out = -1
            {
                perror("File Error:");
                continue;
            }
            dup2(fd_out, 1);
            if (close(fd_out) == -1)
            {
                perror("File Error:");
                continue;
            }
        }

        if (file[1] != NULL)
        {
            fd_in = open(file[1], O_RDONLY);
            if (fd_in == -1)
            {
                perror("File Error:");
            }
            dup2(fd_in, 0);
            if (close(fd_in) == -1)
            {
                perror("File Error:");
                continue;
            }
        }

        int pid = fork(); // create child process here.
        fflush(stdout);
        if (pid == 0)
        {
            execvp(args[0], args); //execute the command in args[0]
            perror("command not found"); // check if command is valid, if not print perror
            exit(1);
        } else if (pid < 0)
        {
            perror("Fork Error:"); //forking failure
            exit(1);
        } else
        {
            int status;
            wait(&status); //wait for the child process to complete its
        }
    }
    return 0;
}