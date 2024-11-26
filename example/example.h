#pragma once
#define k_add_source_num 10
#define u_pos(pos)

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