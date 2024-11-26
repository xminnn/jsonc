#include <stdio.h>
#include <stdlib.h>

#include "example_json.h"

char* fs_read(const char* path, int* out_size) {
    FILE* file = fopen(path, "rb");
    if (!file) {
        return 0;
    }
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);
    char* buff = malloc(size + 1);
    fread(buff, 1, size, file);
    buff[size] = 0;
    fclose(file);
    if (out_size) {
        *out_size = size;
    }
    return buff;
}

int main(int argc, char const* argv[]) {
    int size = 0;
    char* json = fs_read("./example.json", &size);
    json[size] = 0;
    struct classroom room = {0};
    int ret = json_parse_classroom(&room, json, size);
    free(json);
    if (ret <= 0) {
        extern char example_json_parse_error[1024];
        printf("prase error: %s\n", example_json_parse_error);
        return -1;
    }
    printf("info_select_=%d, info.a=%d, std.freinds[0].source2=%g\n", room.info_select_, room.info.a, room.std.freinds[0].source2);
    
    char stringfy[10240];
    int n = json_stringfy_classroom(&room, stringfy, sizeof(stringfy));
    stringfy[n] = 0;
    printf("%s\n", stringfy);
    return 0;
}

/**
output:

info_select_=1, info.a=1, std.freinds[0].source2=1.12
{"type":1,"info":{"a":1},"std":{"source":{"math":1,"other":[1,2]},"freinds":[{"name":"123","source2":1.12,"d":212232}]}}
*/