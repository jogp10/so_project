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
#define PHRASESIZE 256

char (*readTransformation(const char* f))[WORD_SIZE] {
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

int replace_str(char *word1, char *word2, char *target) {

    char buffer[1024] = { 0 };
    char *insert_point = &buffer[0];
    const char *tmp = target;
    char *replacement;
    size_t needle_len, repl_len;
    size_t word1_len = strlen(word1);
    size_t word2_len = strlen(word2);

    while (1) {
        /** Pointers to first occurrence of words */
        const char *p1 = strstr(tmp, word1);
        const char *p2 = strstr(tmp, word2);
        const char *p;

        /* p -> pointer which holds first occurrence of any word */
        if ((p1 == NULL && p2 == NULL)) p = NULL;
        else if (p2 == NULL) p = p1;
        else if (p1 == NULL) p = p2;
        else p = p1 > p2 ? p2 : p1;

        /* walked past last occurrence of needle; copy remaining part */
        if (p == NULL) {
            strcpy(insert_point, tmp);
            break;
        }

        if (p == p1) { needle_len = word1_len; repl_len = word2_len; replacement = word2; }
        else { needle_len = word2_len; repl_len = word1_len; replacement = word1; }

        /* copy part before needle */
        memcpy(insert_point, tmp, p - tmp);
        insert_point += p - tmp;

        /* copy replacement string */
        memcpy(insert_point, replacement, repl_len);
        insert_point += repl_len;

        /* adjust pointers, move on */
        tmp = p + needle_len;
    }

    /* write altered string back to target */
    strcpy(target, buffer);
    return 0;
}

int main() {
    int fd_parent[2], fd_child[2];
    pid_t pid;
    int n_bytes;

    char (*transformer)[WORD_SIZE] = readTransformation("cypher.txt"); /** array with transformer words */

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
    } else if (pid > 0) {
        /** parent */
        close(fd_parent[READ_END]);
        close(fd_child[WRITE_END]);

        char phrase[PHRASESIZE]; //To store the words

        while ( fgets(phrase, PHRASESIZE, stdin) != NULL ) {
            /** write to child */
            if (write(fd_parent[WRITE_END], phrase, strlen(phrase)) < 0) {
                fprintf( stderr, "Unable to write to pipe: %s\n", strerror(errno));
            }

            /** read from child */
            memset(phrase, 0, PHRASESIZE);
            if ( (n_bytes = read(fd_child[READ_END], phrase, PHRASESIZE)) < 0) {
                fprintf(stderr, "Unable to read from pipe: %s\n", strerror(errno));
                break;
            } else if(n_bytes == 0) break;
            printf("%s", phrase);
        }

        close(fd_parent[WRITE_END]);
        close(fd_child[READ_END]);

        /* wait for child and exit */
        if (waitpid(pid, NULL, 0) < 0) {
            fprintf(stderr, "Cannot wait for child: %s\n", strerror(errno));
        }

        exit(EXIT_SUCCESS);
    } else {
        /** child */
        close(fd_parent[WRITE_END]);
        close(fd_child[READ_END]);

        char newToken[PHRASESIZE];

        while(1) {
            memset(newToken, 0, PHRASESIZE);

            /** read from parent */
            if ((n_bytes = read(fd_parent[READ_END], newToken, PHRASESIZE)) < 0) {
                fprintf(stderr, "Unable to read from pipe: %s\n", strerror(errno));
                break;
            } else if(n_bytes == 0) break;

            for (int i = 0; i < NUM_WORDS; i+=2) {
                replace_str(transformer[i], transformer[i + 1],  newToken); /* replace words in transformers */
            }

            /** write to parent */
            if ( write(fd_child[WRITE_END], newToken, strlen(newToken)) < 0) {
                fprintf(stderr, "Unable to write to pipe: %s\n", strerror(errno));
                break;
            }
        }

        close(fd_parent[READ_END]);
        close(fd_child[WRITE_END]);

        exit(EXIT_SUCCESS);
    }
}
