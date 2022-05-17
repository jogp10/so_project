#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PHRASE_SIZE 100
#define TEXT_SIZE 200

int main(int argc, char *argv[]) {
    enum {
        COUNT_MODE, LINE_MODE
    } mode = COUNT_MODE;
    int opt;

    FILE *ft;
    char phrases[TEXT_SIZE][PHRASE_SIZE];
    int num_phrases = 0;

    /** Iterate through arguments options */
    while ((opt = getopt(argc, argv, "l")) != -1) {
        switch (opt) {
            case 'l':
                puts("Activate Print Lines Mode");
                mode = LINE_MODE;
                break;
            default:
                puts("Invalid option");
                return 1;
        }
    }

    /** Open file */
    int index = optind;
    char *f = (char *) malloc(100 * sizeof(char));
    strcpy(f, argv[index]);
    if ((ft = fopen(f, "r")) == NULL) {
        puts(f);
        puts("Unable to open file");
        return 1;
    }

    /** Get size of file */
    int sz;
    fseek(ft, 0, SEEK_END);
    sz = ftell(ft);
    rewind(ft);


    int phrase = 0;
    int j = 0; /* position in phrase */
    for (int i = 0; i < sz; i++) { /** Iterate through all file */
        char ch;
        fscanf(ft, "%c", &ch);

        if (ch == '\n') ch = ' '; /* substitute new line for space */
        phrases[phrase][j] = ch;
        j++;

        /** End of phrase */
        if (strchr(".!?", ch)) {
            fscanf(ft, "%c", &ch);
            phrase++;
            j = 0;
        }
    }
    fclose(ft); /** Close file */

    num_phrases = phrase + 1;

    // Print to console
    switch (mode) {
        case LINE_MODE:
            for (int i = 0; i < num_phrases; i++) {
                printf("[%d] ", i + 1);
                puts(phrases[i]);
            }
            break;
        case COUNT_MODE:
            printf("%d\n", num_phrases);
            break;
    }

    return EXIT_SUCCESS;
}