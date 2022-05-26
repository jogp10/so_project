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
#define PHRASESIZE 255 //não pode dar asneira?

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

int replace_str(char to_replace[], char replacer[], char phrase[]) {
    return 0;
}

int main(int argc, char *argv[]) {
    int fd_parent[2], fd_child[2];
    pid_t pid;
    int n_bytes;

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
/*             char ch[1];
            for(int i = 0; i < 255; i++){
                scanf("%c",ch);
                word[i] = ch[1];
                if(strcmp(ch," ")){
                    printf("%s",word);
                    printf("\n1\n");
                    break;
                }else if(strcmp(ch,"\n")){
                    printf("%s",word);
                    printf("\n2\n");
                    break;
                }
                fflush(stdin);
            }
            fflush(stdin); */
            //fprintf(stderr, "write: %s\n", phrase);

            /** write to child */
            if (write(fd_parent[WRITE_END], phrase, strlen(phrase)) < 0) {
                fprintf( stderr, "Unable to write to pipe: %s\n", strerror(errno));
            }
        }
        close(fd_parent[WRITE_END]);

        /* wait for child and exit */
        if (waitpid(pid, NULL, 0) < 0) {
            fprintf(stderr, "Cannot wait for child: %s\n", strerror(errno));
        }

        while (1) {
            memset(phrase, 0, PHRASESIZE);

            /* read from child */
            if ( (n_bytes = read(fd_child[READ_END], phrase, PHRASESIZE)) < 0) {
                fprintf(stderr, "Unable to read from pipe: %s\n", strerror(errno));
                break;
            } else if(n_bytes == 0) break;
            printf("%s", phrase);
        }
        close(fd_child[READ_END]);

        exit(EXIT_SUCCESS);
    } else {
        /** child */
        close(fd_parent[WRITE_END]);
        close(fd_child[READ_END]);

        char newToken[PHRASESIZE];

        char (*transformer)[WORD_SIZE] = readTransformation("cypher.txt"); /** array with transformer words */

        while(1) {
            memset(newToken, 0, PHRASESIZE);

            /* read from parent */
            if ((n_bytes = read(fd_parent[READ_END], newToken, PHRASESIZE)) < 0) {
                fprintf(stderr, "Unable to read from pipe: %s\n", strerror(errno));
                break;
            } else if(n_bytes == 0) break;

            for (int i = 0; i < NUM_WORDS; i++) {
                int new_i;
                if (i%2==0) new_i = i + 1;
                else new_i = i - 1;
                replace_str(transformer[i], transformer[new_i], newToken);
            }

            /* write to parent */
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
