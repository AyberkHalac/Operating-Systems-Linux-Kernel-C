#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <sys/wait.h>

int checkLength(char argv3[]) { // Input olarak girilen Stringing kac kelimeden olustugunu bulan fonksiyon.
    int counter = 1;
    char *d = strtok(argv3, " \n");
    for (d = strtok(NULL, " \n"); d; d = strtok(NULL, " \n")) {
        counter++;
    }
    return counter;
}

int main() {

    printf("yeniShell is Starting...\n"); // Start

    char argv[1024];
    char argv2[1024];
    char argv3[1024];
    pid_t processId;
    while (1) { // kapat girilene kadar calisir.

        printf("yenishell>"); // Konsol start
        fgets(argv, 1024, stdin); // Get Input
        strcpy(argv2, argv);
        strcpy(argv3, argv);
        strtok(argv2, "\n");

        int length = checkLength(argv3);
        char *p = strtok(argv, " \n"); // EOF ve bosluklara ayrılır.

        if (length <= 10 && length > 1) { // 11 kelimeden az ve 1 den fazla olmali
            if (strcmp(p, "baslat") == 0) { // shell baslat komutu
                char *proc[10]; // 10 kelime tutacak char* array
                int counter = 0;
                for (p = strtok(NULL, " \n"); p != NULL; p = strtok(NULL, " \n")) { // girilen kelimeleri parse edilir. Girilen inputu string kelimelere cevirir
                    proc[counter] = p;
                    counter++;
                }
                proc[counter] = NULL; // son elemana null basilir.
                pid_t pid;
                pid = fork();
                if (pid < 0)
                    perror("fork() error");
                else if (pid > 0){
                    sleep(1);
                }
                else if (pid == 0) {
                    processId = getpid();
                    printf("%d No’lu is baslatildi.\n", processId);
                    int e = execvp(proc[0], proc);
                    if (e == -1)
                        printf("execvp error\n");
                }
            }
            else if (strcmp(p, "yurut") == 0) { // shell yurut komutu
                int status;
                char *proc[10];
                int counter = 0;
                for (p = strtok(NULL, " \n"); p != NULL; p = strtok(NULL, " \n")) { // girilen inputu string kelimelere cevirir
                    proc[counter] = p;
                    counter++;
                }
                proc[counter] = NULL;
                pid_t pid;
                pid = fork();
                if (pid < 0)
                    perror("fork() error");
                else if (pid > 0) {
                    waitpid(pid, &status, 0);
                    if (status == 0)
                        printf("child dogru bir sekilde kapatildi.\n");
                    else if (status == 1)
                        printf("child kapatilirken hata olustu.\n");
                }
                else {
                    printf("%d No’lu is baslatildi.\n", getpid());
                    if (execvp(proc[0], proc) == 0)
                        printf("%d No’lu is baslatildi.\n", getpid());
                    else {
                        printf("failed with error [%s]\n", strerror(errno));
                        kill(atoi(p), SIGKILL) != 0;
                    }
                }
            }
            else if (strcmp(p, "durdur") == 0) { // durdur komutu
                for (p = strtok(NULL, " \n"); p; p = strtok(NULL, " \n")) { // girilen inputu string kelimelere cevirir
                    if (kill(atoi(p), SIGSTOP) != 0) {	//ardisik veya tek verilen pid durdurur
                        printf("failed with error [%s]\n", strerror(errno));
                    } else printf("pid = [%d] Durduruldu.\n", atoi(p));
                }
            } else if (strcmp(p, "surdur") == 0) { // surdur komutu
                for (p = strtok(NULL, " \n"); p; p = strtok(NULL, " \n")) { // girilen inputu string kelimelere cevirir
                    if (kill(atoi(p), SIGCONT) != 0) {	 //ardisik veya tek verilen pid surdurur
                        printf("failed with error [%s]\n", strerror(errno));
                    } else printf("pid = [%d] Surduruluyor.\n", atoi(p));
                }
            } else if (strcmp(p, "oldur") == 0) { // oldur komutu
                for (p = strtok(NULL, " \n"); p; p = strtok(NULL, " \n")) { // girilen inputu string kelimelere cevirir
                    if (kill(atoi(p), SIGKILL) != 0) { // ardisik veya tek verilen pid oldurur
                        printf("failed with error [%s]\n", strerror(errno));
                    } else printf("pid = [%d] Olduruldu.\n", atoi(p));
                }
            }
        }
        else if (length == 1) { // Eger 1 kelimeden olusuyor ise
            if (strcmp(argv2, "kapat") == 0) {  // shell cikis.
                printf("Program kapatiliyor...");
                exit(0);
            }
            else if (strcmp(argv2, "beklet")) { // shell beklet komutu
                int status;
                processId = wait(&status);
                if (processId != -1)
                    printf("Process %ld 'den cikiliyor status =  %d\n", (long) processId, status);
            }
        }
        else if (length > 10) // 10 kelimeden fazla ise hata
            printf("Shell script 10 kelimeden uzun stringler desteklemez.\n");
    }

}
