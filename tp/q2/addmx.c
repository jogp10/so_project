#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <string.h>
#include <sys/wait.h>

void* create_shared_memory(size_t size) {
    // Our memory buffer will be readable and writable:
    int protection = PROT_READ | PROT_WRITE;

    // The buffer will be shared (meaning other processes can access it), but
    // anonymous (meaning third-party processes cannot obtain an address for it),
    // so only this process and its children will be able to use it:
    int visibility = MAP_SHARED | MAP_ANONYMOUS;

    // The remaining parameters to `mmap()` are not important for this use case,
    // but the manpage for `mmap` explains their purpose.
    return mmap(NULL, size, protection, visibility, -1, 0);
}

int* readData(char* f, int* n, int* m) {
    FILE *ft;
    if ((ft = fopen(f, "r")) == NULL) {
        printf("Unable to open file: %s", f);
        return NULL;
    }

    int *ptr;
    char trash;
    int i=0;

    while(!feof(ft)) {
        if (i==0){
            fscanf(ft, "%d", n);
            fscanf(ft, "%c", &trash);
            fscanf(ft, "%d", m);
            i=1;

            ptr = calloc((*n)*(*m), sizeof(int));
        }
        else if (fscanf(ft, "%d", &ptr[i-1])) {
            i++;
        }
        else
            fscanf(ft, "%c", &trash);
    }

    fclose(ft);
    return ptr;
}

int main(int argc, char *argv[]) {
    int n1, n2, m1, m2;

    if(argc!=3) {
        puts("Invalid number of arguments");
        return 1;
    }

    /** Open files & Read Data */
    int* matrix1 = readData(argv[1], &n1, &m1);
    int* matrix2 = readData(argv[2], &n2, &m2);
    int* matrix3 = calloc(n1 * m1, sizeof(int));

    if(n1!=n2 || m1!=m2) {
        puts("Matrices with different dimensions. Unable to add them!");
        return 1;
    }

    int size = (int) (n1*m1*sizeof(int));
    void* shmem = create_shared_memory(size);
    memcpy(shmem, matrix3, size);

    pid_t pid;
    for(int i=0; i<m1; i++) {
        pid = fork();
        if (pid == 0) {
            /** child process */
            for (int j=0; j<n1; j++)
                ((int*)shmem)[j * m1 + i] = matrix1[j * m1 + i] + matrix2[j * m1 + i]; /** sum */
            exit(0);
        } else {
            /** parent process */
        }
    }

    int status;
    while (wait(&status) > 0); // wait for all childs to finish

    /** stdout resultant matrix from matrix1 + matrix2 */
    printf("%dx%d", n1, m1);
    for (int i=0; i<n1*m1; i++) {
        if(i%m1==0)
            printf("\n");
        printf("%d ", ((int*)shmem)[i]);
    }
    printf("\n");
    return 0;
}
