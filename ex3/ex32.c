
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <memory.h>
#include <wait.h>
#include <sys/time.h>

#define MAX_PATH_LEN 150
#define MAX_LINE_LEN 150
#define ERROR -1
#define MAX_RUNTIME 3


/**
 * function that write the current students result to a file
 * @param resFile the file to write to
 * @param studName the student name
 * @param grade the grade
 */
void writeToResFile(int resFile, char *studName, int grade) {

    char *details = malloc((strlen(studName) + MAX_LINE_LEN) * sizeof(char));
    strcat(details, studName);
    switch (grade) {
        case 100:
            strcat(details, ",EXCELLENT,100\n");
            break;
        case 75:
            strcat(details, ",SIMILAR,75\n");
            break;
        case 50:
            strcat(details, ",WRONG,50\n");
            break;
        case 20:
            strcat(details, ",TIMEOUT,20\n");
            break;
        case 10:
            strcat(details, ",COMPILATION_ERROR,10\n");
            break;
        case 0:
            strcat(details, ",NO_C_FILE,0\n");
            break;
        default:
            break;
    }
    /*writing to the file*/
    write(resFile, details, strlen(details));
    /*free the details line memory allocation*/
    free(details);
}

int isFileNameEndingWithDotC(const char *name) {
    int len = (int) strlen(name);
    if (name[len - 1] == 'c' && name[len - 2] == '.') {
        return 1;
    }
    return 0;
}

/**
 * search for c file in the given directory
 * @param HomeDir the given directory
 * @return the name of the c file or empty string if the is not c file in the directory
 */
char *searchDirForCFiles(char *studentsFolderLoc, char *currDirName) {
    char temp[MAX_PATH_LEN] = {0};
    char *fullPath = strcat(temp, studentsFolderLoc);
    strcat(fullPath, "/");
    strcat(fullPath, currDirName);

    DIR *currentDir = opendir(fullPath);
    struct dirent *currentStruct;
    if (currentDir == NULL) {
        perror("open dir failed");   /* dir open failed */
        exit(-1);
    }
    // for each student directory send it to search function
    while ((currentStruct = readdir(currentDir)) != NULL) {
        if (strcmp(currentStruct->d_name, "..") == 0 || strcmp(currentStruct->d_name, ".") == 0) {
            continue;
        }
        if (currentStruct->d_type == DT_DIR) {
            continue;
        }
        if (isFileNameEndingWithDotC(currentStruct->d_name)) {
            return currentStruct->d_name;
        }
    }
    return NULL;
}

void copyLocationToBuffer(const char *buffer, char *dest, int *bufIndex) {
    int i = 0;
    while (buffer[*bufIndex] != '\n') {
        dest[i] = buffer[*bufIndex];
        (*bufIndex)++;
        i++;
    }
    dest[i] = '\0';
}

int compileFile(char *studentFolderPath, char *studentFileName) {
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork failed");   /*  fork did not happened */
        exit(-1);
    }

    /*the son compile the c file*/
    if (pid == 0) {
        char temp[MAX_PATH_LEN] = {0};
        char *fullPath = strcat(temp, studentFolderPath);
        strcat(fullPath, "/");
        strcat(fullPath, studentFileName);

        char *argsGcc[] = {"gcc", "-o", "studentFile.out", fullPath, NULL};
        int val = execvp("gcc", argsGcc);
        if (val == ERROR) {
            perror("Compilation failed");
            exit(-1);
        }
    }
        /*the father checking for compilation error*/
    else {
        int status;
        wait(&status);
        if (WEXITSTATUS(status) != 0) {
            return 10;
        }
    }
    return 0;
}

int runFile(char *cwd, char *inputFileLoc) {
    char temp[MAX_PATH_LEN] = {0};
    char *fullPath = strcat(temp, cwd);
    strcat(fullPath, "/studentFile.out");
    // start measure time
    struct timeval start;
    gettimeofday(&start, NULL);

    int pid = fork();
    if (pid < 0) {
        perror("fork failed");   /*  fork did not happened */
        exit(-1);
    }
    /*the son running the program*/
    if (pid == 0) {
        char *args_for_compiled[] = {fullPath, inputFileLoc, NULL};
        int val = execvp(fullPath, args_for_compiled);
        if (val == ERROR) {
            perror("Error in running user file");
            exit(-1);
        }
    }
        /*father checks if son takes more than 3 seconds*/
    else {
        wait(NULL);
        // stop measure time
        struct timeval end;
        gettimeofday(&end, NULL);
        long runningTime = end.tv_sec - start.tv_sec;

        remove(fullPath);
        if (runningTime > MAX_RUNTIME) {
            return 20;
        }
    }
    return 0;
}

int compareFiles(char *cwd, char *correctOutputFilePath) {
    char temp[MAX_PATH_LEN] = {0};
    char *userOutputFilePath = strcat(temp, cwd);
    strcat(userOutputFilePath, "/studentOutput.txt");

    int status = 0;
    /*fork for the comparing of the files*/
    int pid = fork();
    if (pid < 0) {
        perror("fork failed");   /*  fork did not happened */
        exit(-1);
    }
    /*the son compare files*/
    if (pid == 0) {
        char temp2[MAX_PATH_LEN] = {0};
        char *compOutFilePath = strcat(temp2, cwd);
        strcat(compOutFilePath, "/comp.out");

        char *args_for_compiled[] = {compOutFilePath, userOutputFilePath, correctOutputFilePath, NULL};
        int val = execvp(compOutFilePath, args_for_compiled);
        if (val == ERROR) {
            perror("Error in comparing files");
            exit(-1);
        }
    }
        /*the father gives the right grade to his son*/
    else {
        wait(&status);
        remove(userOutputFilePath);
    }
    return WEXITSTATUS(status);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "usage: %s output_file\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    /*saving the current dir path, so we could find later the comp.c file*/
    char currDirPath[MAX_PATH_LEN];
    if (getcwd(currDirPath, sizeof(currDirPath)) == NULL) {
        perror("saving current dir failed");
        exit(-1);
    }

    /*opening configuration file*/
    int configFile;
    configFile = open(argv[1], O_RDONLY);
    if (configFile == -1) {
        perror("open file failed");   /* file open failed */
        exit(-1);
    }

    int readOutcome;
    char studentsFolderLoc[MAX_LINE_LEN];
    char inputFileLoc[MAX_LINE_LEN];
    char correctOutputFile[MAX_LINE_LEN];
    char buffer[MAX_LINE_LEN * 3];
    readOutcome = read(configFile, buffer, sizeof(buffer) + 1);
    if (readOutcome <= 0) {
        perror("reading failed");   /* failed reading from the configuration file */
        exit(-1);
    }

    /*puting each file's adderess in the matching string*/
    int bufIndex = 0;
    copyLocationToBuffer(buffer, studentsFolderLoc, &bufIndex);

    bufIndex++;
    copyLocationToBuffer(buffer, inputFileLoc, &bufIndex);

    bufIndex++;
    copyLocationToBuffer(buffer, correctOutputFile, &bufIndex);

    /*open student dir*/
    DIR *studDir = opendir(studentsFolderLoc);
    if (studDir == NULL) {
        perror("open dir failed");   /* dir open failed */
        exit(-1);
    }

    int resultFile = open("results.csv", O_CREAT | O_WRONLY | O_APPEND, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (resultFile <= 0) {
        perror("create failed");   /*  file open did not take place */
        exit(-1);
    }

    struct dirent *studentDirent;
    /*for each student in the dir searching for its c file and deal with it*/
    while ((studentDirent = readdir(studDir)) != NULL) {
        if (strcmp(studentDirent->d_name, "..") == 0 || strcmp(studentDirent->d_name, ".") == 0) {
            continue;
        }
        if (studentDirent->d_type == DT_REG) {
            continue;
        }
        char *studentFileName = searchDirForCFiles(studentsFolderLoc, studentDirent->d_name);

        if (studentFileName == NULL) {
            writeToResFile(resultFile, studentDirent->d_name, 0);
            continue;
        }
        // replace standard input and output
        int in, out;

        in = open(inputFileLoc, O_RDONLY);
        dup2(in, 0);

        out = open("studentOutput.txt", O_CREAT | O_RDWR | O_APPEND | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
        dup2(out, 1);

        close(in);
        close(out);

        /*fork to compile student file*/
        char studentFolderPath[MAX_PATH_LEN] = {0};
        strcat(studentFolderPath, studentsFolderLoc);
        strcat(studentFolderPath, "/");
        strcat(studentFolderPath, studentDirent->d_name);

        int ret = compileFile(studentFolderPath, studentFileName);
        if (ret != 0) {
            writeToResFile(resultFile, studentDirent->d_name, 10);
            continue;
        }

        /*fork to run the compiled c file*/
        ret = runFile(currDirPath, inputFileLoc);
        if (ret != 0) {
            writeToResFile(resultFile, studentDirent->d_name, 20);
            continue;
        }
        /*compare the results*/
        ret = compareFiles(currDirPath, correctOutputFile);
        switch (ret) {
            case 1:
                writeToResFile(resultFile, studentDirent->d_name, 100);
                continue;
            case 2:
                writeToResFile(resultFile, studentDirent->d_name, 50);
                continue;
            case 3:
                writeToResFile(resultFile, studentDirent->d_name, 75);
                continue;
            default:
                break;
        }
    }
    return 0;
}
