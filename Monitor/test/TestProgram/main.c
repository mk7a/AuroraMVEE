#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>

int main(int argc, char *argv[]) {

    getchar();

    char buf[10];
    syscall(SYS_getrandom, buf, 10, 0);

    // print bytes hexdump
    printf("Bytes: ");
    for (int i = 0; i < 10; i++) {
        printf("%02x ", (unsigned char) buf[i]);
    }
    printf("\n");

    return 0;


}

//int main(int argc, char *argv[]) {
//    if (argc < 2) {
//        printf("Usage: %s <filename>\n", argv[0]);
//        return 1;
//    }
//
//    char *filename = argv[1];
//    FILE *file = fopen(filename, "a");
//    if (!file) {
//        printf("Could not open file %s\n", filename);
//        return 1;
//    }
//
//    fprintf(file, "hi");
//
//    fclose(file);
//    return 0;
//}
//int main(int argc, char *argv[]) {
//
//    // print arguments
//    printf("Arguments: (%d)", argc);
//    for (int i = 0; i < argc; i++) {
//        printf("%s ", argv[i]);
//    }
//
//    if (argc < 2) {
//        printf("Usage: %s <filename>\n", argv[0]);
//        return 1;
//    }
//
//    char *filename = argv[1];
//    FILE *file = fopen(filename, "a");
//    if (!file) {
//        printf("Could not open file %s\n", filename);
//        return 1;
//    }
//
//    time_t now = time(NULL);
//    struct tm *time = localtime(&now);
//    char timestamp[100];
//    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", time);
//
//    fprintf(file, "%s\n", timestamp);
//    printf("Appended timestamp: %s\n", timestamp);
//
//    fclose(file);
//    return 0;
//}
