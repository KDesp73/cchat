#include "data.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

void test_data_to_string(){
    struct Data data = {
        .id = 0,
        .user = "kostasqweq",
        .status = MESSAGE,
        .message = "Hello World",
        .time = 123456
    };

    char* str = data_to_string(data); 

    assert(strcmp(str, "0|kostasqweq|Hello World|0|123456") == 0);

    struct Data data1 = {
        .id = 0,
        .user = "aaaaaaaaaaaaaaaandfjsnfsd8ds",
        .status = COMMAND,
        .message = "\\whisper server hi",
        .time = 123456
    };

    char* str1 = data_to_string(data1); 

    assert(strcmp(str1, "0|aaaaaaaaaaaaaaaandfjsnfsd8ds|\\whisper server hi|4|123456") == 0);
}

int main(void){
    test_data_to_string();

    printf("All tests passed\n");

    return 0;
}
