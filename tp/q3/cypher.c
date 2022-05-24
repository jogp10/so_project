#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>

#define NUM_WORDS 8
#define WORD_SIZE 20

#define READ_END 0
#define WRITE_END 1
#define LINESIZE 10000

char (*readTransformation(char* f))[WORD_SIZE] {
    FILE *ft;
    static char transformation[NUM_WORDS][WORD_SIZE];

    /** Open file */
    if ((ft = fopen(f, "r")) == NULL) {
        puts(f);
        puts("Unable to open file");
        return NULL;
    }

    /** Get size of file */
    int sz;
    fseek(ft, 0, SEEK_END);
    sz = (int) ftell(ft);
    rewind(ft);


    int word = 0;
    int j = 0; /* position in phrase */
    for (int i = 0; i < sz; i++) { /** Iterate through all file */
        char ch;
        fscanf(ft, "%c", &ch);

        /** End of word */
        if (ch == ' ' || ch == '\n') {
            word++;
            j = 0;
        } else {
            transformation[word][j] = ch;
            j++;
        }
    }
    fclose(ft); /** Close file */

    return transformation;
}

int main(int argc, char *argv[]) {
    char (*transformer)[WORD_SIZE] = readTransformation("cypher.txt"); /** array with transformer words */

    int nbytes_parent, nbytes_child, fd_parent[2], fd_child[2];
    pid_t pid;
    char line[LINESIZE];

    /** Create pipes*/
    if (pipe(fd_child) < 0) {
        perror("pipe error");
        exit(EXIT_FAILURE);
    }
    if (pipe(fd_parent) < 0) {
        perror("pipe error");
        exit(EXIT_FAILURE);
    }

    /** Create child process */
    if ((pid = fork()) < 0) {
        perror("fork error");
        exit(EXIT_FAILURE);
    }
    else if (pid > 0) {
        close(fd_parent[READ_END]);
        close(fd_child[WRITE_END]);
        /* parent */

        /* write to child */
        if ( (nbytes_parent = write(fd_parent[WRITE_END], "Hey!!", 5)) < 0) {
            fprintf(stderr, "Unable to write to pipe: %s\n", strerror(errno));
        }
        close(fd_parent[WRITE_END]);

        /* wait for child and exit */
        if ( waitpid(pid, NULL, 0) < 0) {
            fprintf(stderr, "Cannot wait for child: %s\n", strerror(errno));
        }

        /* read from child */
        if ( (nbytes_parent = read(fd_child[READ_END], line, LINESIZE)) < 0) {
            fprintf(stderr, "Unable to read from pipe: %s\n", strerror(errno));
        }
        close(fd_child[READ_END]);

        printf("\n%s", line);

        exit(EXIT_SUCCESS);
    } else {
        close(fd_parent[WRITE_END]);
        close(fd_child[READ_END]);
        /* child */

        char line[LINESIZE];

        /* read from parent */
        if ( (nbytes_parent = read(fd_parent[READ_END], line, LINESIZE)) < 0) {
            fprintf(stderr, "Unable to read from pipe: %s\n", strerror(errno));
        }
        close(fd_parent[READ_END]);

        /* write to parent */
        if ( (nbytes_parent = write(fd_child[WRITE_END], "Hey parent!!", 12)) < 0) {
            fprintf(stderr, "Unable to write to pipe: %s\n", strerror(errno));
        }
        close(fd_child[WRITE_END]);

        printf("\n%s", line);
        
        exit(EXIT_SUCCESS);
    }
}
