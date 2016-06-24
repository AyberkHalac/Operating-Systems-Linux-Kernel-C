#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

/**
 *  argCheck method checks the arguments of main.
 *  If arguments are wrong, method prints error message and exit from program.
 */
void argCheck(int argc, char *argv[]);

/**
 *  filecpy method copies buffer size bytes from source file to destination file.
 *  Method takes 2 arguments. Both arguments are for checking the correctness of opening files.
 *  Method gives signal every 2 minutes.
 */
void filecpy(int source, int destination);


/**
 *  alarmHandler sets alarm() method's handler.
 */
void alarmHandler();

/**
 *  checkPath method checks arguments. If arguments are correct path then program continues,
 *  if arguments are not correct path then program create folders to make that path true.
 *  Returns True path.
 */

int checkPath(char *argv);


int checkPath(char *argv) { // 1 True 0 False
    char subString[128] = {};
    int i;
    int check;

    for (i = strlen(argv) - 1; i > 0; i--)
        if (argv[i] == '/') {
            check = i;
            break;
        }

    for (i = 0; i < check; i++)
        subString[i] = argv[i];

    char buffer[128];

    if (subString[0] != '/')
        // Adding '/' to paths head.
        snprintf(buffer, sizeof buffer, "%s%s", "/", subString);
    else
        strcpy(buffer, subString);

    struct stat st;
    if (stat(buffer, &st) == 0) {
        printf("%s is found in directory.\n", buffer);
        return 1;
    }
    else {
        printf("%s -> Path Error\n ---> [%s]\n", buffer, strerror(errno));
        int mkd = mkdir(buffer, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
        if (mkd == -1) {
            printf("Folder creation failed. [%s]\n", strerror(errno));
            exit(1);
        }
        else {
            printf("Folder Created.");
            return 1;
        }
    }
}


void argCheck(int argc, char *argv[]) {
    if (argc != 3) { // If user gives more than 3 arguments.
        if (argc == 2) { // If user gives 2 arguments.
            if (strcmp(argv[1], "--help") == 0) {
                printf("Argument list must be like --> ./main Arg1 Arg2\n");
                printf("Arg1 -> Source File Path.\n");
                printf("Arg2 -> Destination File Path.\n");
                printf("Example Path -> /home/root/XXXXXX.YYY\n");
                exit(1);
            }
            printf("Argument list is wrong.\n"
                           "Please check --help for true argument combination.\n");
            exit(1);
        }
        printf("Argument list is wrong.\n"
                       "Argument list cannot be more than 3.\n"
                       "Please check --help for true argument combination.\n");
        exit(1);
    }
}

void alarmHandler() {
    fprintf(stderr, "Copying, Please Wait...\n");
    signal(SIGALRM, alarmHandler);
    alarm(2);
}

short CONTROL = 0;
long TOTALCOPIEDFILE = 0;

void filecpy(int source, int destination) {

    ssize_t sizeSource, sizeDestination;
    char BUFFER[4096];

    while (sizeSource = read(source, &BUFFER, 4096)) {
        sizeDestination = write(destination, &BUFFER, sizeSource);
        printf("Destination = %lu Byte/Character copied\n", sizeDestination);
        TOTALCOPIEDFILE += sizeDestination;
        if (sizeDestination == -1) {
            printf("Failed with error [%s]\n", strerror(errno));
            exit(1);
        }
        else if (sizeSource == -1 && sizeSource != sizeDestination) {
            printf("Failed with error [%s]\n", strerror(errno));
            if (CONTROL == 5)
                exit(1);
            CONTROL++;
            filecpy(source, destination); // Restart
        }
    }
    printf("File is successfully copied.\n");
    printf("TOTAL = %lu Byte/Character copied\n", TOTALCOPIEDFILE);
    close(source);
    close(destination);
}


int main(int argc, char *argv[]) {

    argCheck(argc, argv);  // Check Arguments.
    int checkArg2 = checkPath(argv[2]); // Check Path 2.
    signal(SIGALRM, alarmHandler);
    alarm(2);

    if (checkArg2) {

        char path1[128];
        char path2[128];

        if (argv[1][0] != '/') {
            // Adding '/' to paths head.
            snprintf(path1, sizeof path1, "%s%s", "/", argv[1]);
        }
        else {
            strcpy(path1, argv[1]);
        }

        if (argv[2][0] != '/') {
            // Adding '/' to paths head.
            snprintf(path2, sizeof path2, "%s%s", "/", argv[2]);
        }
        else {
            strcpy(path2, argv[2]);
        }


        // Open source file.
        int s = open(path1, O_RDWR);
        if (s == -1) {
            printf("Source file failed with error [%s]\n", strerror(errno));
            exit(1);
        }
        else
            printf("Source is Successfully opened.\n");

        // Create and open destination file, full control to all.(Read-Write permission for User, Group, Others).
        int d = open(path2, O_RDWR | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO);
        if (d == -1) {
            printf("Destination file failed with error [%s]\n", strerror(errno));
            close(s); // s is closed because we opened it before d.
            exit(1);
        }
        else
            printf("%s File Successfully Created.\n", path2);
        filecpy(s, d);

    }
    else exit(1);

    return 0;
}

