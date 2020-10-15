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

int main() {
    return 0;
}