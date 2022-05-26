#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PHRASE_SIZE 500 // tamanho da frase
#define NUM_PHRASES 1000 // no de frases

int main(int argc, char *argv[]) {
    enum {
        COUNT_MODE, LINE_MODE // no de frases; print frase por frase com índice
    } mode = COUNT_MODE;
    int opt;

    FILE *ft;
    char phrases[NUM_PHRASES][PHRASE_SIZE];
    int num_phrases;

    /** Iterate through arguments options */
    while ((opt = getopt(argc, argv, "l")) != -1) { // = -1 se l não existir  
        switch (opt) {
            case 'l':
                puts("Activate Print Lines Mode"); // l de lines
                mode = LINE_MODE;
                break;
            default:
                puts("Invalid option");
                return 1;
        }
    }

    /** Open file */
    int index = optind; // optind, resultante do getopt, é igual ao índice do próximo argumento a ser iterado
    char *f = (char *) malloc(100 * sizeof(char));
    strcpy(f, argv[index]); // string copy de argv[index] (só o nome do ficheiro) para f
    if ((ft = fopen(f, "r")) == NULL) { // se der erro; fopen retorna apontador para f; ft = apontador para início do ficheiro f
        puts(f); // puts faz sempre tab
        puts("Unable to open file");
        return 1;
    }

    /** Get size of file */
    int sz;
    fseek(ft, 0, SEEK_END); // muda o apontador ft para o final do ficheiro (SEEK_END); 0  -> offset
    sz = ftell(ft); // posição do apontador (índice do final do ficheiro = tamanho do ficheiro)
    rewind(ft); // voltar ao início do ficheiro


    int phrase = 0;
    int j = 0; /* position in phrase */
    for (int i = 0; i < sz; i++) { /** Iterate through all file */
        char ch;
        fscanf(ft, "%c", &ch); // LER um caracter a cada iteração; ft ficheiro, "%c" tipo que vou ler, &ch onde vou guardar o que li

        if (ch == '\n') ch = ' '; /* substitute new line for space */
        phrases[phrase][j] = ch; 
        j++;

        /** End of phrase */
        if (strchr(".!?", ch)) { // se encontrar ch na string ".!?"
            while (1) { // reticencias, varios pontos
                fscanf(ft, "%c", &ch);
                if (!strchr(".!?", ch)) break;
                else {
                    phrases[phrase][j] = ch;
                    j++;
                }
            }
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
            printf("%d\n", num_phrases); // d de dígito
            break;
    }

    return EXIT_SUCCESS;
}
