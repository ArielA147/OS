
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <unistd.h>
#include <errno.h>
#include <wait.h>
#include <stdbool.h>
#include <values.h>
#include <stdlib.h>

#define MAXLEN 100

typedef struct {
  pid_t id;
  char command[MAXLEN];
}process;

/**
 * spliting function acourding to speace
 * @param input
 * @return
 */
char **parseInput(char *input) {
    int i = 0;
    int j = 0;
    char **parsed = malloc(sizeof(char *) * MAXLEN); //REMEMBER REMEMBER THE FREEING OF MALLOC-EVER

    bool look_for_word = true;
    while (input[i] != '\0') {
        if (input[i] == '"') {
            input[i] = '\0';
            char *next = strchr(input + i + 1, '"');
            if (!next) {
                // error
            }
            next[0] = '\0';
            parsed[j] = input + i + 1;
            i = (int) (next - input);
            j++;
        } else if (input[i] != ' ' && look_for_word) {
            parsed[j] = input + i;
            look_for_word = false;
            j++;
        } else if (input[i] == ' ') {
            input[i] = '\0';
            look_for_word = true;
        }
        i++;
    }

    parsed[j] = NULL;
    return parsed;
}

char *getInput() {
    //get input from user
    char *input = malloc(sizeof(char) * MAXLEN); //REMEMBER REMEMBER THE FREEING OF MALLOC-EVER
    fgets(input, MAXLEN, stdin);

    //remover all new lines
    char *newln = NULL;
    while ((newln = strrchr(input, '\n')) != NULL) {
        *newln = '\0';
    }

    return input;
}

// presenting all the bakground process which are running
void jobs(int *pids, char backgroundCommands[MAXLEN][MAXLEN], int backgroundIndex) {
    int i;
    for (i = 0; i < backgroundIndex; i++) {
        if (!waitpid(pids[i], NULL, WNOHANG)) { //is alive
            printf("%d %s RUNNING\n", pids[i], backgroundCommands[i]);
        }
    }
}

/**
 *
 * @param dir - the directoy application from the user
 * @param lastPath
 */
int changeDirectory(char **arg, char lastPath[PATH_MAX]) {
    // checking if there are mor than 2 inputs for cd command
    if (arg[1] != NULL && arg[2] != NULL) {
      printf("%d\n", getpid());
      fprintf(stderr, "Error: Too many arguments\n");
      return 0;
    }

    char* dir = arg[1];

    if(dir!=NULL)
    {
        if (!strcmp(dir, "~")) {
            dir = getenv("HOME");
        }
        // handling the condition of: cd ~/blablabla
        else if (strchr(dir, '~')!=NULL&& strchr(dir, '/')!=NULL){
            char tmp[strlen(dir)-1];
            strcpy(tmp,dir+1);
            dir = getenv("HOME");
            dir = strcat(dir,tmp);
        } else if (!strcmp(dir, "-")) {
            dir = lastPath;
      }
    } else {
        dir = getenv("HOME");
    }

    char localPath[PATH_MAX];
    getcwd(localPath, PATH_MAX);

    if (chdir(dir)) {
        fprintf(stderr, "Error in system call\n");
        return 0;
    }

    strcpy(lastPath, localPath);

    printf("%d\n", getpid());
    return 0;
}

// adding a new process to the history list
void addHistory(process* history,pid_t id,char* command){
  int i = 0;
  for(; i < MAXLEN; i++){
    if(history[i].command[0] == 0){
      history[i].id= id;
      strcpy(history[i].command, command);
      break;
    }
  }
}

// presenting the history on screen
void showHistory(process* history){
  int i = 0;
  for(; i < MAXLEN; i++){
    if(history[i].id == 0 ){break;}
    printf("%d %s",history[i].id,history[i].command);
    // check if job done or not ?
    if(waitpid(history[i].id,NULL,WNOHANG)>=0){
      printf(" RUNNING\n");
    } else if (history[i+1].id==0) {
      printf(" RUNNING\n");
    }else {
      printf(" DONE\n");
    }
  }
}

// running the commands if they are not one of (cd, exit, jobs, history)
int runProgram(const char *command, char *const argv[],process* history,
  char* original_input,bool foreground) {
    int pid = fork();

    if (pid > 0) { //is parent
      addHistory(history,pid,original_input);
        printf("%d\n", pid);
        if (foreground) { //wait
            waitpid(pid, NULL, 0);
        }
        return pid;
    } else if (pid < 0) {
        return -1;
    }

    execvp(command, argv);
    fprintf(stderr, "Error in system call\n"); //if we returned from execvp, there was an error!
    return -1;
}

int main() {
  process* history=(process*)malloc(sizeof(process)*MAXLEN);

    bool exited = false;
    int pids[MAXLEN];
    char backgroundCommands[MAXLEN][MAXLEN];
    char lastPath[PATH_MAX];
    getcwd(lastPath, PATH_MAX);
    int backgroundIndex = 0;

    while (!exited) {

        printf("> ");

        char *input = getInput();
        char *original_input = malloc(strlen(input) + 1);
        strcpy(original_input, input);

        char **argv = parseInput(input); // spliting the input

        const char *command = argv[0];

        if (command == NULL) {
            free(original_input);
            free(input);
            free(argv);
            continue;
        }

        if (!strcmp(command, "exit")) {
            exited = true;
            addHistory(history,getpid(),original_input);
            free(original_input);
            free(input);
            free(argv);
            break;
        } else if (!strcmp(command, "cd")) {
            addHistory(history,getpid(),original_input);
            changeDirectory(argv, lastPath);
        } else if (!strcmp(command, "jobs")) {
          addHistory(history,getpid(),original_input);
          jobs(pids, backgroundCommands, backgroundIndex);
        } else if (!strcmp(command, "history")) {
          addHistory(history,getpid(),original_input);
          showHistory(history);
        }

        else {
            bool foreground = true; // searching for &
            int i = 0;
            while (argv[i] != NULL) {
                ++i;
            }

            //check if the last argument is &
            if (!strcmp(argv[i - 1], "&")) {
                foreground = false;
                argv[i - 1] = NULL;
            }

            //run the command
            int pid = runProgram(command, argv,history,original_input, foreground);

            //check if command was background
            if (!foreground) {
                strcpy(backgroundCommands[backgroundIndex], original_input);
                pids[backgroundIndex] = pid;
                ++backgroundIndex;
            }
        }
        free(original_input);
        free(input);
        free(argv);
    }
    free(history);
    return 0;
}
