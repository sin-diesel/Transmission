#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h> 
#include <unistd.h>
#include <assert.h>
#include <fcntl.h>
#include <errno.h>
#include <ctype.h>
#include <string.h>
#include <signal.h>


#define MAX_NAME_SZ 128
#define MAX_DATA_SZ 4096
#define TEST 1
#define PIPE 124

#ifdef TEST
#define DBG(a) if (TEST) {a;}
#else
#define DBG(a)
#endif

#ifdef TEST
#define ASSERT_OK(error) fprintf(stderr, "ERROR AT LINE %d, FUNCTION %s\nCALLING DUMP\n", __LINE__, __func__); \
                         dump(error);  
#else
#define ASSERT_OK(error)                                                                    
#endif 

char* file_name;

void usr1_handler(int signal) {
    DBG(fprintf(stderr,"Signal received! Getting data from fifo...\n"))
    int fifo = open("/Users/stassidelnikov/Documents/MIPT/Operating_systems/signals_transm/myfifo", O_RDONLY);
    assert(fifo > 0);
    char* data = (char*) calloc(MAX_DATA_SZ, sizeof(char));
    assert(data);

    int n_read = read(fifo, data, MAX_DATA_SZ);
    DBG(fprintf(stderr, "Bytes read from fifo:%d\n", n_read))

    char* end = strchr(data, '\0');
    DBG(fprintf(stderr, "Data received:%s\n", data))
    DBG(fprintf(stderr, "Bytes received:%d\n", n_read))

    int output = open(file_name, O_WRONLY | O_CREAT, 0666);
    assert(output > 0);
    int n_out_write = write(output, data, n_read);
    assert(n_out_write == n_read);
    DBG(fprintf(stderr, "Data transferred\n"))
    close(fifo);
}

int main(int argc, char** argv) {

    signal(SIGUSR1, &usr1_handler);
    
    assert(argc == 2);
    char* file = argv[1];
    unsigned fname_sz = strlen(file);
    assert(fname_sz > 0);

    file_name = (char*) calloc(MAX_NAME_SZ, sizeof(char));
    assert(file_name);
    strcpy(file_name, file); // copy to global variable
    DBG(fprintf(stderr, "File name copied:%s\n", file_name))

    // mkfifo("/Users/stassidelnikov/Documents/MIPT/ \
    //Operating_systems/signals_transm/internal_fifo", 0666);
    // int fifo = open("/Users/stassidelnikov/Documents/MIPT/ \
    // Operating_systems/signals_transm/internal_fifo", O_WRONLY);
    
    // int n_write = write(fifo, file, fname_sz);
    // DBG(fprintf(stderr," written to fifo:%d\n", n_write))
    // assert(n_write == fname_sz);
    // close(fifo);

    while (1) {
        sleep(10);
    }

    return 0;
}