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

#define TEST 1
#define PIPE 124

#ifdef TEST
#define DBG(a) if (TEST) {a;}
#else
#define DBG(a)
#endif

#define TRS "transmitter"

#ifdef TEST
#define ASSERT_OK(error) fprintf(stderr, "ERROR AT LINE %d, FUNCTION %s\nCALLING DUMP\n", __LINE__, __func__); \
                         dump(error);  
#else
#define ASSERT_OK(error)                                                                    
#endif

enum errors {
    NO_INPUT = -1,
    ARGS_OVERFLOW = -2,
    ARGS_UNDERFLOW = -5,
    BAD_ALLOC = -6,
    INP_FILE = -7,
    UNEXPECTED_ERROR = -100,
    INVALID_NUM = -8,
    WRITE_ERROR = -9,
    OPEN_ERROR = -10,
};

void dump(int error_type) {
    switch(error_type) {

        case NO_INPUT:
        fprintf(stderr, TRS " error: at least 1 argument expected\n");
        exit(NO_INPUT);
        break;

        case ARGS_OVERFLOW:
        fprintf(stderr, TRS " error: too many arguments passed\n");
        exit(ARGS_OVERFLOW);
        break;

        case ARGS_UNDERFLOW:
        fprintf(stderr, TRS " error: file does not exist\n");
        exit(ARGS_UNDERFLOW);
        break;

        case BAD_ALLOC:
        fprintf(stderr, TRS " error: could not allocate memory\n");
        exit(BAD_ALLOC);
        break;

        case INP_FILE:
        fprintf(stderr, TRS " error: could not open file\n");
        exit(INP_FILE);
        break;

        case INVALID_NUM:
        fprintf(stderr, TRS " error: invalid number\n");
        exit(INVALID_NUM);
        break;

        case OPEN_ERROR:
        fprintf(stderr, TRS " error: could not open fifo\n");
        exit(OPEN_ERROR);
        break;

        case WRITE_ERROR:
        fprintf(stderr, TRS " error: error writing to fifo\n");
        exit(WRITE_ERROR);
        break;
        case 0:
        break;

        default:
        fprintf(stderr, TRS " error: unexpected error\n");
        exit(UNEXPECTED_ERROR);
        break;
    }
}

int check_args(int argc, char** argv) {
    if (argc < 3) {
        return ARGS_UNDERFLOW;
    } else if (argc > 3) {
        return ARGS_OVERFLOW;
    }
    
    return 0;
}

int handle_input(int argc, char** argv) {

    int pid = strtol(argv[1], &argv[1], 10);
    if (pid == 0) {
        ASSERT_OK(INVALID_NUM)
    }

    return pid;
}



int main(int argc, char** argv) {

    int result = check_args(argc, argv);
    dump(result);
    int pid = handle_input(argc, argv);
    char* file = argv[2];

    DBG(fprintf(stderr, "PID: %d\nfile:%s\n", pid, file))
    int fname_size = strlen(file);
    DBG(fprintf(stderr, "File name size:%d\n", fname_size))

    int file_data = open(file, O_RDONLY);
    assert(file_data > 0);
    struct stat input;
    fstat(file_data, &input);
    int data_size = input.st_size;

    char* buf = (char*) calloc(data_size, sizeof(char));
    assert(buf);

    buf[data_size - 1] = '\0';

    int n_read = read(file_data, buf, data_size);
    assert(n_read == data_size);

    DBG(fprintf(stderr, "Bytes sent:%d\n", data_size))
    DBG(fprintf(stderr, "buf:%s\n", buf));


    kill(pid, SIGUSR1); // send signal for the receiver to get data from fifo

    DBG(fprintf(stderr, "Opening fifo...\n"))
    mkfifo("/Users/stassidelnikov/Documents/MIPT/Operating_systems/signals_transm/myfifo", 0666);
    int fifo = open("/Users/stassidelnikov/Documents/MIPT/Operating_systems/signals_transm/myfifo", O_WRONLY);
    assert(fifo > 0);
    DBG(fprintf(stderr, "Writing to fifo...\n"))

    int n_write = write(fifo, buf, data_size);
    if (n_write != data_size) {
        ASSERT_OK(WRITE_ERROR)
    }
    close(fifo);

    return 0;
}