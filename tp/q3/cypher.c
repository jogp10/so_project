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
#define LINESIZE 10000 //não pode dar asneira?

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
    int fd_parent[2], fd_child[2];
    pid_t pid;
    
    char word[255]; //To store the words
     
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
        close(fd_parent[READ_END]);
        close(fd_child[WRITE_END]);
        /* parent */

        

        while (!feof(stdin)) {
            scanf("%s",word);
            fflush(stdin);
            

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

            /* write to child */
            if (write(fd_parent[WRITE_END], word, 255) < 0) {
                fprintf(stderr, "Unable to write to pipe: %s\n", strerror(errno));
            }
            //printf("%s\n",word);

        }

        if (write(fd_parent[WRITE_END], "\0", 255) < 0) {
            fprintf(stderr, "Unable to write to pipe: %s\n", strerror(errno));
        }


        close(fd_parent[WRITE_END]);

        /* wait for child and exit */
        if (waitpid(pid, NULL, 0) < 0) {
            fprintf(stderr, "Cannot wait for child: %s\n", strerror(errno));
        }

        while (strcmp(word,"\0")) {
            /* read from child */
            if (read(fd_child[READ_END], word, 255) < 0) {
                fprintf(stderr, "Unable to read from pipe: %s\n", strerror(errno));
            }
            printf("%s ", word);
        }


        for(int i = 0; i < 255; i++){
            word[i] = '\0';
        }

        
        close(fd_child[READ_END]);

        exit(EXIT_SUCCESS);
    } else {
        close(fd_parent[WRITE_END]);
        close(fd_child[READ_END]);
        /*-----------child-----------*/
        char newToken[255];

        char (*transformer)[WORD_SIZE] = readTransformation("cypher.txt"); /** array with transformer words */

        while(strcmp(newToken,"\0")){
            for(int i = 0; i < 255; i++){
                newToken[i] = '\0';
            }

            /* read from parent */
            if (read(fd_parent[READ_END], newToken, 255) < 0) {
                fprintf(stderr, "Unable to read from pipe: %s\n", strerror(errno));
            }

            printf("\nchild: %s", newToken);
            

            for (int i = 0; i < NUM_WORDS; i++) {
                if (!strcmp(transformer[i], newToken)) {
                    if (i % 2 == 0) i = i + 1;
                    else i = i - 1;
                    strcpy(newToken, transformer[i]);
                }
            } 

            /* write to parent */
            if (write(fd_child[WRITE_END], newToken, 255) < 0) {
                fprintf(stderr, "Unable to write to pipe: %s\n", strerror(errno));
            }

        }

        close(fd_parent[READ_END]);
        close(fd_child[WRITE_END]);

        exit(EXIT_SUCCESS);
    }
}
