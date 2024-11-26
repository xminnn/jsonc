## jsonc

一个可以直接解析c语言头文件中的struct生成对应的json序列化反序列化代码的工具。生成后的代码不依赖第三方库。

## 示例

定义： [example/example.h](./example/example.h)
```c
#define k_add_source_num 10

struct student {
    struct {
        int math;
        int other[100];
        int other_count_;
        int add[k_add_source_num];
        int add_count_;
    } source;

    struct {
        char name[32];
        int souce;
        float source2;
        double source3;
        long long d;
    } freinds[10];
    int freinds_count_;
};

#define u_pos_b 2

struct classroom {
    int type;
    union {
        u_pos(1) int a;
        u_pos(u_pos_b) int b;
    } info;
    int info_select_;
    struct student std;
};
```

编译：
```
jsonc example_json example.h
```

使用：[example/main.c](./example/main.c)
```c
#include "example_json.h"

int main(int argc, char const* argv[]) {
    int size = 0;
    char* json = fs_read("./example.json", &size);
    json[size] = 0;

    // 反序列化
    struct classroom room = {0};
    int ret = json_parse_classroom(&room, json, size);
    free(json);
    if (ret <= 0) {
        extern char example_json_parse_error[1024];
        printf("prase error: %s\n", example_json_parse_error);
        return -1;
    }

    printf("info_select_=%d, info.a=%d, std.freinds[0].source2=%g\n", room.info_select_, room.info.a, room.std.freinds[0].source2);
    
    // 序列化
    char stringfy[10240];
    int n = json_stringfy_classroom(&room, stringfy, sizeof(stringfy));
    stringfy[n] = 0;
    printf("%s\n", stringfy);
    return 0;
}
```

## 注意事项

1. 为了简化解析逻辑, 头文件中只允许存在**单行宏定义**、**结构体定义**、**单行注释**。不支持有函数等其他定义，以及多行注释
2. **以`_`结尾的属性将被忽略**
3. **数组必须存在 `xx_count_` 来对数组计数**。
4. **`union` 必须存在 `xx_select_` 来记录选中结构**。可以使用 `u_pos` 来自定义位置，否则默认按照字段顺序
4. 不支持指针
5. **`json_stringfy_xx` 函数默认不会补充最后一个0**, 需自行补 (因为一些协议场景不需要0，所以这里不默认补)。
6. 不支持动态键值, 无法解析的键将默认忽略。 编译jsonc添加 `-DNOT_ALLOW_UNKOWN_KEY` 可以禁止未知键

> jsonc代码，仅1000行，修改容易，非常容易扩展自己需要的功能。

## 编译

安装gcc编译器，在bash环境下运行：

```sh
bash build.sh
```

## 性能测试

这里测试解析一个配置文件 [./test/config.json](./test/config.json) ，对比 `yyjson` 的性能。测试代码 [test/test.c](./test/test.c) 。

时间单位是us, prase表示解析, value_get是获取一个int值。在`-O3`下解析性能接近 `yyjson`，获取值不在有性能损耗。

如果需要解析json后并且载入内存struct映射，这种情况下`jsonc`相比于`yyjson`有约30%性能优势。
```txt
-O0:
    yyjson time cost: prase=418510, value_get=338121,  total=756631
    jsonc  time cost: prase=524719, value_get=2942,    total=527661
-O2:
    yyjson time cost: prase=149733, value_get=91115,   total=240848
    jsonc  time cost: prase=173808, value_get=608,     total=174416
-O3:
    yyjson time cost: prase=141656, value_get=78204,   total=219860
    jsonc  time cost: prase=168563, value_get=13,      total=168576
```