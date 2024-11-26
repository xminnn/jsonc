#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <threads.h>
#include <time.h>
#include <unistd.h>

#include "config_json.h"
#include "yyjson.h"

long long time_curruent_us() {
    long long now;
    struct timeval tv;
    gettimeofday(&tv, 0);
    now = tv.tv_sec;
    now = now * 1000000;
    now += tv.tv_usec;
    return now;
}

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
    printf("start: \n");
    int file_size;
    char* config_text = fs_read("./config.json", &file_size);
    config_text[file_size] = 0;

    long long prase_time_total = 0;
    long long view_time_total = 0;
    long long total_time_total = 0;
    for (int k = 0; k < 1000; k++) {
        long long start_time = time_curruent_us();
        yyjson_doc* doc = yyjson_read(config_text, file_size, 0);
        yyjson_val* root = yyjson_doc_get_root(doc);
        long long prase_time = time_curruent_us();
        for (int i = 0; i < 2000; i++) {    // 配置中int大概2000个，模拟一次性获取
            yyjson_val* alliance_param = yyjson_obj_get(root, "alliance_param");
            yyjson_val* one = yyjson_arr_get(alliance_param, 0);
            yyjson_val* id = yyjson_obj_get(one, "id");
            int a = (int)yyjson_get_int(id);
            if (a == 1000000) {    // 防止优化掉
                prase_time_total = -1;
            }
        }
        long long end_time = time_curruent_us();
        prase_time_total += prase_time - start_time;
        view_time_total += end_time - prase_time;
        total_time_total += end_time - start_time;
    }
    printf("yyjson time cost: prase=%lld, value_get=%lld, total=%lld \n", prase_time_total, view_time_total, total_time_total);

    prase_time_total = 0;
    view_time_total = 0;
    total_time_total = 0;
    struct res_config res = {0};
    for (int k = 0; k < 1000; k++) {
        long long start_time = time_curruent_us();
        json_parse_res_config(&res, config_text, file_size);
        long long prase_time = time_curruent_us();
        for (int i = 0; i < 2000; i++) { // 无意义，仅对掉 for 2000 性能消耗
            int a = res.alliance_param->id;
            if (a == 1000000) {    // 防止优化掉
                prase_time_total = -1;
            }
        }
        long long end_time = time_curruent_us();
        prase_time_total += prase_time - start_time;
        view_time_total += end_time - prase_time;
        total_time_total += end_time - start_time;
    }
    printf("jsonc  time cost: prase=%lld, value_get=%lld, total=%lld \n", prase_time_total, view_time_total, total_time_total);
    return 0;
}

/**
-O0:
    yyjson time cost: prase=418510, value_get=338121,  total=756631
    jsonc  time cost: prase=524719, value_get=2942,    total=527661
-O2:
    yyjson time cost: prase=149733, value_get=91115,   total=240848
    jsonc  time cost: prase=173808, value_get=608,     total=174416
-O3:
    yyjson time cost: prase=141656, value_get=78204,   total=219860
    jsonc  time cost: prase=168563, value_get=13,      total=168576
*/