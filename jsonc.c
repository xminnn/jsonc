#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define loge(fmt, args...) printf("\033[31m[%s:%d] " fmt "\033[0m\n", __FILE__, __LINE__, ##args)
#define logd(fmt, args...) printf("[%s:%d] " fmt "\n", __FILE__, __LINE__, ##args)
#define putl(tar, fmt, args...) \
    if (tar) fprintf(tar, fmt "\n", ##args);
#define return_error_exit                                                                                       \
    ctx->scan->err_exit = -1;                                                                                   \
    loge("\033[31mparse error, at %d:%d\033[0m", ctx->scan->line + 1, ctx->scan->i - ctx->scan->last_line_end); \
    return 0;

struct scanner {
    char *text;
    int text_len;
    int i;
    int line;
    int last_line_end;
    char *tok;
    int tok_len;
    int err_exit;
};

static char *scanner_get_match_text(const struct scanner *scan) {
    static char buff[1024] = {0};
    memcpy(buff, scan->tok, scan->tok_len);
    buff[scan->tok_len] = 0;
    return buff;
}

static void scanner_cpy_match_text(const struct scanner *scan, char *out) {
    memcpy(out, scan->tok, scan->tok_len);
    out[scan->tok_len] = 0;
}

static int scanner_match_skip(struct scanner *scan) {
    int j = scan->i;
    int comment = 0;
    for (; scan->i < scan->text_len; scan->i++) {
        char c = scan->text[scan->i];
        if (c == '\n') {
            scan->last_line_end = scan->i;
            scan->line++;
            comment = 0;
            continue;
        }
        if (comment) {
            continue;
        }
        if (c == ' ' || c == '\t' || c == '\r') {
            continue;
        }
        if (c == '/' && scan->i + 1 < scan->text_len && scan->text[scan->i + 1] == '/') {
            comment = 1;
            continue;
        }
        break;
    }
    return scan->i > j || scan->i == scan->text_len;
}

static void scanner_match_skip_until(struct scanner *scan, char u) {
    for (; scan->i < scan->text_len; scan->i++) {
        char c = scan->text[scan->i];
        if (c == '\n') {
            scan->last_line_end = scan->i;
            scan->line++;
        }
        if (c == u) {
            scan->i++;
            return;
        }
    }
}

static int scanner_match_token(struct scanner *scan, const char *match) {
    struct scanner back = *scan;
    scanner_match_skip(scan);
    int count = strlen(match);
    if (scan->i + count > scan->text_len) {
        *scan = back;
        return 0;
    }
    for (int k = 0; k < count; k++) {
        if (scan->text[scan->i++] != match[k]) {
            *scan = back;
            return 0;
        }
    }
    return 1;
}

static char not_word[] = "~`!@#$%^&*()_+-={}[]:\"|;'\\<>?,./ \r\t\n";
static int scanner_match_word(struct scanner *scan, const char *match) {
    struct scanner back = *scan;
    scanner_match_skip(scan);
    int count = strlen(match);
    if (scan->i + count > scan->text_len) {
        *scan = back;
        return 0;
    }
    for (int k = 0; k < count; k++) {
        if (scan->text[scan->i++] != match[k]) {
            *scan = back;
            return 0;
        }
    }
    if (scan->i < scan->text_len) {
        char c = scan->text[scan->i];
        for (char m = 0; m < sizeof(not_word) - 1; m++) {
            if (not_word[m] == c) {
                return 1;
            }
        }
        *scan = back;
        return 0;
    }
    return 1;
}

static int scanner_match_name(struct scanner *scan) {
    struct scanner back = *scan;
    scanner_match_skip(scan);
    char c = scan->text[scan->i];
    if ((c < 'a' || c > 'z') && (c < 'A' || c > 'Z') && c != '_') {
        *scan = back;
        return 0;
    }
    scan->tok = scan->text + scan->i;
    scan->tok_len = 1;
    for (++scan->i; scan->i < scan->text_len; scan->i++) {
        c = scan->text[scan->i];
        if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || (c == '_')) {
            scan->tok_len++;
        } else {
            break;
        }
    }
    return 1;
}

static int scanner_match_number(struct scanner *scan) {
    struct scanner back = *scan;
    scanner_match_skip(scan);

    char c = scan->text[scan->i];
    if ((c < '0' || c > '9') && c != '-') {
        *scan = back;
        return 0;
    }

    scan->tok = scan->text + scan->i;
    scan->tok_len = 1;
    for (++scan->i; scan->i < scan->text_len; scan->i++) {
        c = scan->text[scan->i];
        int ctn = c >= '0' && c <= '9';
        if (!ctn) {
            break;
        }
        scan->tok_len++;
    }
    return 1;
}

struct context {
    struct scanner *scan;
    FILE *encode;
    FILE *decode;
    FILE *head;
    int seq;
};

static int parse_struct_body(struct context *ctx, const char *bname, int struct_type);

static int parse_type(struct context *ctx, int *type_which) {
    struct scanner back = *ctx->scan;
    if (scanner_match_word(ctx->scan, "unsigned")) {
    }
    if (scanner_match_word(ctx->scan, "int64")) {
        *type_which = 1;
        return 1;
    }
    if (scanner_match_word(ctx->scan, "long")) {
        scanner_match_word(ctx->scan, "long");
        *type_which = 1;
        return 1;
    }
    if (scanner_match_word(ctx->scan, "int")) {
        *type_which = 2;
        return 1;
    }
    if (scanner_match_word(ctx->scan, "short")) {
        *type_which = 3;
        return 1;
    }
    if (scanner_match_word(ctx->scan, "char")) {
        *type_which = 4;
        return 1;
    }
    if (scanner_match_word(ctx->scan, "float")) {
        *type_which = 5;
        return 1;
    }
    if (scanner_match_word(ctx->scan, "double")) {
        *type_which = 6;
        return 1;
    }
    if (scanner_match_word(ctx->scan, "bool")) {
        *type_which = 7;
        return 1;
    }
    if (scanner_match_name(ctx->scan)) {
        *type_which = 8;
        char typename[1024];
        scanner_cpy_match_text(ctx->scan, typename);
        int slen = strlen(typename);
        if (memcmp(typename + (slen - 4), "_ptr", 4) == 0) {
            return 1;
        }
    }
    *ctx->scan = back;
    return 0;
}

static int parse_struct_inner(struct context *ctx, int *type_which, char *name, char *bname) {
    struct scanner back = *ctx->scan;
    int struct_type = 1;
    if (scanner_match_word(ctx->scan, "struct")) {
        struct_type = 1;
    } else if (scanner_match_word(ctx->scan, "union")) {
        struct_type = 2;
    } else {
        return 0;
    }
    if (parse_struct_body(ctx, bname, struct_type)) {
        *type_which = 11;
        return 1;
    }
    if (scanner_match_name(ctx->scan)) {
        *type_which = 12;
        scanner_cpy_match_text(ctx->scan, name);
        return 1;
    }
    *ctx->scan = back;
    return 0;
}

static int parse_union_position(struct context *ctx, char *pos) {
    struct scanner back = *ctx->scan;
    if (!scanner_match_word(ctx->scan, "u_pos")) {
        *ctx->scan = back;
        return 0;
    }

    if (!scanner_match_token(ctx->scan, "(")) {
        *ctx->scan = back;
        return 0;
    }

    if (scanner_match_number(ctx->scan)) {
    } else if (scanner_match_name(ctx->scan)) {
    } else {
        *ctx->scan = back;
        return 0;
    }

    scanner_cpy_match_text(ctx->scan, pos);
    if (!scanner_match_token(ctx->scan, ")")) {
        *ctx->scan = back;
        return 0;
    }
    return 1;
}

static int parse_struct_feild(struct context *ctx, int bseq, const char *bname, int *feild_pos, int struct_type) {
    int type_which = -1;
    int is_array = 0;

    struct scanner back = *ctx->scan;    // 备份会临时回滚使用

    // 暂停输出
    struct context back_ctx = *ctx;
    ctx->decode = 0;
    ctx->encode = 0;
    ctx->head = 0;

    char pos_attr[1024];
    int pos_attr_use = parse_union_position(ctx, pos_attr);

    char inner_struct_type_name[1024] = {0};
    if (parse_type(ctx, &type_which)) {
    } else if (parse_struct_inner(ctx, &type_which, inner_struct_type_name, "")) {
    }

    // 恢复输出
    ctx->decode = back_ctx.decode;
    ctx->encode = back_ctx.encode;
    ctx->head = back_ctx.head;

    if (ctx->scan->err_exit || type_which == -1) {
        return 0;
    }

    char name[1024] = {0};
    char len[1024] = {0};
    if (!scanner_match_name(ctx->scan)) {
        return_error_exit;
    }

    scanner_cpy_match_text(ctx->scan, name);

    if (scanner_match_token(ctx->scan, ":")) {
        if (!scanner_match_number(ctx->scan)) {
            return_error_exit;
        }
    }

    if (scanner_match_token(ctx->scan, "[")) {
        is_array = 1;
        if (!scanner_match_number(ctx->scan) && !scanner_match_name(ctx->scan)) {
            return_error_exit;
        }
        scanner_cpy_match_text(ctx->scan, len);
        if (!scanner_match_token(ctx->scan, "]")) {
            return_error_exit;
        }
    }
    if (!scanner_match_token(ctx->scan, ";")) {
        return_error_exit;
    }

    if (name[strlen(name) - 1] == '_' || type_which == 8) {    // 直接忽略
        return 1;
    }

    char prename[1024];
    strcpy(prename, bname);
    if (prename[strlen(prename) - 1] == '>') {
        prename[strlen(prename) - 2] = 0;
    } else {
        prename[strlen(prename) - 1] = 0;
    }

    struct scanner back_success = *ctx->scan;    // 备份会临时回滚使用
    *ctx->scan = back;
    putl(ctx->decode, "if (strcmp(key%d, \"%s\") == 0) {", bseq, name);

    if (struct_type == 2) {
        if (pos_attr_use) {
            putl(ctx->encode, "if (%s_select_ == %s) {", prename, pos_attr);
        } else {
            putl(ctx->encode, "if (%s_select_ == %d) {", prename, *feild_pos);
        }
    }

    if (!is_array) {
        if (type_which == 1 || type_which == 2 || type_which == 3 || type_which == 4) {
            putl(ctx->encode, "if (%s%s != 0) {", bname, name);
        } else if (type_which == 5 || type_which == 6) {
            putl(ctx->encode, "if (%s%s != 0) {", bname, name);
        } else if (type_which == 7) {
        } else if (type_which == 11) {
        } else if (type_which == 12) {
        }
    } else {
        if (type_which == 4) {
            putl(ctx->encode, "if (%s%s[0] != 0) {", bname, name);
        } else {
            putl(ctx->encode, "if (%s%s_count_ != 0) {", bname, name);
        }
    }

    if (*feild_pos != 1 && struct_type == 1) {
        putl(ctx->encode, "if (str[k-1] != '{' && str[k-1] != ',') {");
        putl(ctx->encode, "if (k + 1 >= len) {");
        putl(ctx->encode, "ERROR(\"len overflow\");");
        putl(ctx->encode, "return -1;");
        putl(ctx->encode, "}");
        putl(ctx->encode, "str[k++] = ',';");
        putl(ctx->encode, "}");
    }

    int name_str_len_add = strlen(name) + 3;
    putl(ctx->encode, "if (k + %d >= len) {", name_str_len_add);
    putl(ctx->encode, "ERROR(\"len overflow\");");
    putl(ctx->encode, "return -1;");
    putl(ctx->encode, "}");
    putl(ctx->encode, "memcpy(str + k, \"\\\"%s\\\":\", %d);", name, name_str_len_add);
    putl(ctx->encode, "k += %d;", name_str_len_add);

    if (!is_array) {
        if (type_which == 1 || type_which == 2 || type_which == 3 || type_which == 4) {
            putl(ctx->decode, "long long num = 0;");
            putl(ctx->decode, "if (match_integer(text, len, &k, &num) < 0) {");
            putl(ctx->decode, "ERROR(\"expect interget number at %%d\", k);");
            putl(ctx->decode, "return -1;");
            putl(ctx->decode, "}");
            putl(ctx->decode, "%s%s = num;", bname, name);

            putl(ctx->encode, "count = snprintf(str + k, len - k, \"%%lld\", (long long int)%s%s);", bname, name);
        } else if (type_which == 5 || type_which == 6) {
            putl(ctx->decode, "double num = 0;");
            putl(ctx->decode, "if (match_float(text, len, &k, &num) < 0) {");
            putl(ctx->decode, "ERROR(\"expect float number at %%d\", k);");
            putl(ctx->decode, "return -1;");
            putl(ctx->decode, "}");
            putl(ctx->decode, "%s%s = num;", bname, name);

            putl(ctx->encode, "count = snprintf(str + k, len - k, \"%%g\", (double)%s%s);", bname, name);
        } else if (type_which == 7) {
            putl(ctx->decode, "char num= 0;");
            putl(ctx->decode, "if (match_word(text, len, &k, \"true\") >= 0) {");
            putl(ctx->decode, "num = 1;");
            putl(ctx->decode, "}");
            putl(ctx->decode, "else if (match_word(text, len, &k, \"false\") >= 0) {");
            putl(ctx->decode, "num = 0;");
            putl(ctx->decode, "} else {");
            putl(ctx->decode, "ERROR(\"expect bool at %%d\", k);");
            putl(ctx->decode, "return -1;");
            putl(ctx->decode, "}");
            putl(ctx->decode, "%s%s = num;", bname, name);

            putl(ctx->encode, "if (%s%s) {", bname, name);
            putl(ctx->encode, "count = snprintf(str + k, len - k, \"true\");");
            putl(ctx->encode, "} else {");
            putl(ctx->encode, "count = snprintf(str + k, len - k, \"false\");");
            putl(ctx->encode, "}");
        } else if (type_which == 11) {
            char fix_name[1024];
            sprintf(fix_name, "%s%s.", bname, name);
            int type_which_temp = 0;
            parse_struct_inner(ctx, &type_which_temp, inner_struct_type_name, fix_name);
        } else if (type_which == 12) {
            int type_which_temp = 0;
            parse_struct_inner(ctx, &type_which_temp, inner_struct_type_name, "");

            int tseq = ++ctx->seq;

            putl(ctx->decode, "int rst%d = 0;", tseq);
            putl(ctx->decode, "{");
            putl(ctx->decode, "int json_parse_%s(struct %s*, const char*, int);", inner_struct_type_name, inner_struct_type_name);
            putl(ctx->decode, "rst%d = json_parse_%s(&%s%s, text + k, len);", tseq, inner_struct_type_name, bname, name);
            putl(ctx->decode, "}");
            putl(ctx->decode, "if (rst%d < 0) {", tseq);
            putl(ctx->decode, "ERROR(\"not match %s object\");", inner_struct_type_name);
            putl(ctx->decode, "return -1;");
            putl(ctx->decode, "}");
            putl(ctx->decode, "k += rst%d;", tseq);

            putl(ctx->encode, "{");
            putl(ctx->encode, "int json_stringfy_%s(struct %s*, char*, int);", inner_struct_type_name, inner_struct_type_name);
            putl(ctx->encode, "count = json_stringfy_%s(&%s%s, str + k, len - k);", inner_struct_type_name, bname, name);
            putl(ctx->encode, "}");
        }

        if (type_which != 11) {
            putl(ctx->encode, "if (count < 0 || k + count >= len) {");
            putl(ctx->encode, "ERROR(\"len overflow\");");
            putl(ctx->encode, "return -1;");
            putl(ctx->encode, "}");
            putl(ctx->encode, "k += count;");
        }
    } else {
        if (type_which == 4) {
            int seq = ++ctx->seq;
            putl(ctx->decode, "if (match_string(text, len, &k, %s%s, %s) < 0) {", bname, name, len);
            putl(ctx->decode, "ERROR(\"expect string at %%d\", k);");
            putl(ctx->decode, "return -1;");
            putl(ctx->decode, "}");

            putl(ctx->encode, "count = flush_string(str + k, len - k, %s%s);", bname, name);
            putl(ctx->encode, "if (count < 0 || k + count >= len) {");
            putl(ctx->encode, "ERROR(\"len overflow\");");
            putl(ctx->encode, "return -1;");
            putl(ctx->encode, "}");
            putl(ctx->encode, "k += count;");
        } else {
            putl(ctx->encode, "if (k + 1 >= len) {");
            putl(ctx->encode, "ERROR(\"len overflow\");");
            putl(ctx->encode, "return -1;");
            putl(ctx->encode, "}");
            putl(ctx->encode, "str[k++] = '[';");

            int seq = ++ctx->seq;
            putl(ctx->encode, "for (int i%d = 0; i%d < %s%s_count_; i%d++) {", seq, seq, bname, name, seq);
            putl(ctx->encode, "if (i%d != 0) {", seq);
            putl(ctx->encode, "if (k + 1 >= len) {");
            putl(ctx->encode, "ERROR(\"len overflow\");");
            putl(ctx->encode, "return -1;");
            putl(ctx->encode, "}");
            putl(ctx->encode, "str[k++] = ',';");
            putl(ctx->encode, "}");

            putl(ctx->decode, "%s%s_count_ = 0;", bname, name);
            putl(ctx->decode, "if (match_char(text, len, &k, '[') < 0) {");
            putl(ctx->decode, "ERROR(\"expect char '[' at %%d\", k);");
            putl(ctx->decode, "return -1;");
            putl(ctx->decode, "}");
            putl(ctx->decode, "if (match_char(text, len, &k, ']') >= 0) {");
            putl(ctx->decode, "continue;");
            putl(ctx->decode, "}");
            putl(ctx->decode, "int first%d = 1;", seq);
            putl(ctx->decode, "while (1) {");
            putl(ctx->decode, "if (!first%d) {", seq);
            putl(ctx->decode, "if (match_char(text, len, &k, ',') < 0) {");
            putl(ctx->decode, "break;");
            putl(ctx->decode, "}");
            putl(ctx->decode, "} else {");
            putl(ctx->decode, "first%d = 0;", seq);
            putl(ctx->decode, "}");
            if (type_which == 1 || type_which == 2 || type_which == 3 || type_which == 4) {
                putl(ctx->decode, "long long num = 0;");
                putl(ctx->decode, "if (match_integer(text, len, &k, &num) < 0) {");
                putl(ctx->decode, "ERROR(\"expect interget number at %%d\", k);");
                putl(ctx->decode, "return -1;");
                putl(ctx->decode, "}");
                putl(ctx->decode, "if (%s%s_count_ >= %s) {", bname, name, len);
                putl(ctx->decode, "ERROR(\"array count overflow\");");
                putl(ctx->decode, "return -1;");
                putl(ctx->decode, "}");
                putl(ctx->decode, "%s%s[%s%s_count_++] = num;", bname, name, bname, name);

                putl(ctx->encode, "count = snprintf(str + k, len - k, \"%%lld\", (long long int)%s%s[i%d]);", bname, name, seq);
                putl(ctx->encode, "if (count < 0 || k + count >= len) {");
                putl(ctx->encode, "ERROR(\"len overflow\");");
                putl(ctx->encode, "return -1;");
                putl(ctx->encode, "}");
                putl(ctx->encode, "k += count;");
            } else if (type_which == 5 || type_which == 6) {
                putl(ctx->decode, "double num= 0;");
                putl(ctx->decode, "if (match_float(text, len, &k, &num) < 0) {");
                putl(ctx->decode, "ERROR(\"expect float number at %%d\", k);");
                putl(ctx->decode, "return -1;");
                putl(ctx->decode, "}");
                putl(ctx->decode, "if (%s%s_count_ >= %s) {", bname, name, len);
                putl(ctx->decode, "ERROR(\"array count overflow\");");
                putl(ctx->decode, "return -1;");
                putl(ctx->decode, "}");
                putl(ctx->decode, "%s%s[%s%s_count_++] = num;", bname, name, bname, name);

                putl(ctx->encode, "count = snprintf(str + k, len - k, \"%%g\", (double)%s%s[i%d]);", bname, name, seq);
                putl(ctx->encode, "if (count < 0 || k + count >= len) {");
                putl(ctx->encode, "ERROR(\"len overflow\");");
                putl(ctx->encode, "return -1;");
                putl(ctx->encode, "}");
                putl(ctx->encode, "k += count;");
            } else if (type_which == 7) {
                putl(ctx->decode, "char num= 0;");
                putl(ctx->decode, "if (match_word(text, len, &k, \"true\") >= 0) {");
                putl(ctx->decode, "num = 1;");
                putl(ctx->decode, "}");
                putl(ctx->decode, "else if (match_word(text, len, &k, \"false\") >= 0) {");
                putl(ctx->decode, "num = 0;");
                putl(ctx->decode, "} else {");
                putl(ctx->decode, "ERROR(\"expect bool at %%d\", k);");
                putl(ctx->decode, "return -1;");
                putl(ctx->decode, "}");

                putl(ctx->decode, "if (%s%s_count_ >= %s) {", bname, name, len);
                putl(ctx->decode, "ERROR(\"array count overflow\");");
                putl(ctx->decode, "return -1;");
                putl(ctx->decode, "}");
                putl(ctx->decode, "%s%s[%s%s_count_++] = num;", bname, name, bname, name);

                putl(ctx->encode, "if (%s%s[i%d]) {", bname, name, seq);
                putl(ctx->encode, "count = snprintf(str + k, len - k, \"true\");");
                putl(ctx->encode, "} else {");
                putl(ctx->encode, "count = snprintf(str + k, len - k, \"false\");");
                putl(ctx->encode, "}");
                putl(ctx->encode, "if (count < 0 || k + count >= len) {");
                putl(ctx->encode, "ERROR(\"len overflow\");");
                putl(ctx->encode, "return -1;");
                putl(ctx->encode, "}");
                putl(ctx->encode, "k += count;");
            } else if (type_which == 11) {
                putl(ctx->decode, "if (%s%s_count_ >= %s) {", bname, name, len);
                putl(ctx->decode, "ERROR(\"array count overflow\");");
                putl(ctx->decode, "return -1;");
                putl(ctx->decode, "}");
                putl(ctx->decode, "int i%d = %s%s_count_++;", seq, bname, name);

                char fix_name[1024];
                sprintf(fix_name, "%s%s[i%d].", bname, name, seq);
                int type_which_temp = 0;
                parse_struct_inner(ctx, &type_which_temp, inner_struct_type_name, fix_name);
            } else if (type_which == 12) {
                putl(ctx->decode, "if (%s%s_count_ >= %s) {", bname, name, len);
                putl(ctx->decode, "ERROR(\"array count overflow\");");
                putl(ctx->decode, "return -1;");
                putl(ctx->decode, "}");

                int type_which_temp = 0;
                parse_struct_inner(ctx, &type_which_temp, inner_struct_type_name, "");

                int tseq = ++ctx->seq;
                putl(ctx->decode, "int rst%d;", tseq);
                putl(ctx->decode, "{");
                putl(ctx->decode, "int json_parse_%s(struct %s*, const char*, int);", inner_struct_type_name, inner_struct_type_name);
                putl(ctx->decode, "rst%d = json_parse_%s(&%s%s[%s%s_count_++], text + k, len);", tseq, inner_struct_type_name, bname, name, bname, name);
                putl(ctx->decode, "}");
                putl(ctx->decode, "if (rst%d < 0) {", tseq);
                putl(ctx->decode, "ERROR(\"expect object type parse failed: %s\");", inner_struct_type_name);
                putl(ctx->decode, "return -1;");
                putl(ctx->decode, "}");
                putl(ctx->decode, "k += rst%d;", tseq);

                putl(ctx->encode, "{");
                putl(ctx->encode, "int json_stringfy_%s(struct %s*, char*, int);", inner_struct_type_name, inner_struct_type_name);
                putl(ctx->encode, "count = json_stringfy_%s(&%s%s[i%d], str + k, len - k);", inner_struct_type_name, bname, name, seq);
                putl(ctx->encode, "}");
                putl(ctx->encode, "if (count < 0 || k + count >= len) {");
                putl(ctx->encode, "ERROR(\"len overflow\");");
                putl(ctx->encode, "return -1;");
                putl(ctx->encode, "}");
                putl(ctx->encode, "k += count;");
            }
            putl(ctx->decode, "}");
            putl(ctx->decode, "if (match_char(text, len, &k, ']') < 0) {");
            putl(ctx->decode, "ERROR(\"expect char ']' at %%d\", k);");
            putl(ctx->decode, "return -1;");
            putl(ctx->decode, "}");

            putl(ctx->encode, "}");
            putl(ctx->encode, "if (k + 1 >= len) {");
            putl(ctx->encode, "ERROR(\"len overflow\");");
            putl(ctx->encode, "return -1;");
            putl(ctx->encode, "}");
            putl(ctx->encode, "str[k++] = ']';");
        }
    }

    if (!is_array) {
        if (type_which == 1 || type_which == 2 || type_which == 3 || type_which == 4) {
            putl(ctx->encode, "}");
        } else if (type_which == 5 || type_which == 6) {
            putl(ctx->encode, "}");
        } else if (type_which == 7) {
        } else if (type_which == 11) {
        } else if (type_which == 12) {
        }
    } else {
        if (type_which == 4) {
            putl(ctx->encode, "}");
        } else {
            putl(ctx->encode, "}");
        }
    }

    if (struct_type == 2) {
        if (pos_attr_use) {
            putl(ctx->decode, "%s_select_ = %s;", prename, pos_attr);
        } else {
            putl(ctx->decode, "%s_select_ = %d;", prename, *feild_pos);
        }
        putl(ctx->encode, "}");
    }

    putl(ctx->decode, "continue;");
    putl(ctx->decode, "}");

    (*feild_pos) += 1;
    *ctx->scan = back_success;
    return 1;
}

static int parse_struct_body(struct context *ctx, const char *bname, int struct_type) {
    if (!scanner_match_token(ctx->scan, "{")) {
        return 0;
    }

    int bseq = ++ctx->seq;

    putl(ctx->encode, "if (k + 1 >= len) {");
    putl(ctx->encode, "ERROR(\"len overflow\");");
    putl(ctx->encode, "return -1;");
    putl(ctx->encode, "}");
    putl(ctx->encode, "str[k++] = '{';");

    putl(ctx->decode, "if (match_char(text, len, &k, '{') < 0) {");
    putl(ctx->decode, "ERROR(\"expect char '{' at %%d\", k);");
    putl(ctx->decode, "return -1;");
    putl(ctx->decode, "}");
    putl(ctx->decode, "int first%d = 1;", bseq);
    putl(ctx->decode, "while (1) {");
    putl(ctx->decode, "char key%d[1024];", bseq);
    putl(ctx->decode, "if (!first%d) {", bseq);
    putl(ctx->decode, "if (match_char(text, len, &k, ',') < 0) {");
    putl(ctx->decode, "break;");
    putl(ctx->decode, "}");
    putl(ctx->decode, "if (match_string(text, len, &k, key%d, 1024) < 0) {", bseq);
    putl(ctx->decode, "ERROR(\"expect string key at %%d\", k);");
    putl(ctx->decode, "return -1;");
    putl(ctx->decode, "}");
    putl(ctx->decode, "} else {");
    putl(ctx->decode, "first%d = 0;", bseq);
    putl(ctx->decode, "if (match_string(text, len, &k, key%d, 1024) < 0) {", bseq);
    putl(ctx->decode, "break;");
    putl(ctx->decode, "}");
    putl(ctx->decode, "}");
    putl(ctx->decode, "if (match_char(text, len, &k, ':') < 0) {");
    putl(ctx->decode, "ERROR(\"expect char ':' at %%d\", k);");
    putl(ctx->decode, "return -1;");
    putl(ctx->decode, "}");

    int feild_pos = 1;
    while (parse_struct_feild(ctx, bseq, bname, &feild_pos, struct_type)) {
    }

    if (!scanner_match_token(ctx->scan, "}")) {
        return_error_exit;
    }

    putl(ctx->encode, "if (k + 1 >= len) {");
    putl(ctx->encode, "ERROR(\"len overflow\");");
    putl(ctx->encode, "return -1;");
    putl(ctx->encode, "}");
    putl(ctx->encode, "str[k++] = '}';");

#ifdef NOT_ALLOW_UNKOWN_KEY
    putl(ctx->decode, "ERROR(\"unkown key: %%s\", key1);");
    putl(ctx->decode, "return -1;");
#else
    putl(ctx->decode, "if (skip_value(text, len, &k) < 0) {");
    putl(ctx->decode, "return -1;");
    putl(ctx->decode, "}");
#endif

    putl(ctx->decode, "}");
    putl(ctx->decode, "if (match_char(text, len, &k, '}') < 0) {");
    putl(ctx->decode, "ERROR(\"expect char '}' at %%d\", k);");
    putl(ctx->decode, "return -1;");
    putl(ctx->decode, "}");
    return 1;
}

static int parse_struct(struct context *ctx) {
    int struct_type = 1;
    if (scanner_match_word(ctx->scan, "struct")) {
        struct_type = 1;
    } else {
        return 0;
    }
    if (!scanner_match_name(ctx->scan)) {
        return_error_exit;
    }

    while (1) {
        if (scanner_match_name(ctx->scan)) {
        } else {
            break;
        }
    }

    ctx->seq = 0;
    char name[1024] = {0};
    scanner_cpy_match_text(ctx->scan, name);

    putl(ctx->encode, "\nint json_stringfy_%s(struct %s* self, char* str, int len) {", name, name);
    putl(ctx->encode, "int k = 0;");
    putl(ctx->encode, "int count = 0;");
    putl(ctx->encode, "ERROR_CLEAR;");

    putl(ctx->decode, "\nint json_parse_%s(struct %s* self, const char* text, int len) {", name, name);
    putl(ctx->decode, "int k = 0;");
    putl(ctx->decode, "ERROR_CLEAR;");

    if (!parse_struct_body(ctx, "self->", struct_type)) {
        return_error_exit;
    }
    if (!scanner_match_token(ctx->scan, ";")) {
        return_error_exit;
    }

    putl(ctx->encode, "return k;");
    putl(ctx->encode, "}");

    putl(ctx->decode, "return k;");
    putl(ctx->decode, "}");

    putl(ctx->head, "");
    putl(ctx->head, "int json_stringfy_%s(struct %s* self, char* str, int len);", name, name);
    putl(ctx->head, "int json_parse_%s(struct %s* self, const char* text, int len);", name, name);
    return 1;
}

static int parse_file(struct context *ctx) {
    int count = 0;
    while (1) {
        if (scanner_match_token(ctx->scan, "__")) {
            scanner_match_skip_until(ctx->scan, '\n');
            continue;
        }
        if (scanner_match_token(ctx->scan, "#")) {
            scanner_match_skip_until(ctx->scan, '\n');
            continue;
        }
        if (scanner_match_word(ctx->scan, "static const int")) {
            scanner_match_skip_until(ctx->scan, '\n');
            continue;
        }
        if (parse_struct(ctx)) {
            count++;
            continue;
        }
        break;
    }
    if (ctx->scan->err_exit) {
        loge("parse failed");
        return 0;
    }
    logd("success: %d", count);
    return 1;
}

static void append_file(FILE *tar, FILE *source) {
    fseek(source, 0, SEEK_END);
    int len = ftell(source);
    fseek(source, 0, SEEK_SET);
    char *buff = malloc(len);
    int n = fread(buff, 1, len, source);
    if (n != len) {
        loge("read error, %d, %d", n, len);
        exit(-1);
    }

    int tab = 0;
    for (int i = 0; i < len; i++) {
        if (buff[i] == '{') {
            fwrite(buff + i, 1, 1, tar);
            if (i + 1 < len && buff[i + 1] == ' ' || buff[i + 1] == '\n' || buff[i + 1] == '\r' || buff[i + 1] == '\t') {
                tab += 4;
                if (buff[i + 1] == ' ') {
                    fwrite("\n", 1, 1, tar);
                    for (int k = 0; k < tab; k++) {
                        fwrite(" ", 1, 1, tar);
                    }
                }
            }
        } else if (buff[i] == '}') {
            if (i - 1 >= 0 && (buff[i - 1] == ' ' || buff[i - 1] == '\n' || buff[i - 1] == '\r' || buff[i - 1] == '\t')) {
                tab -= 4;
                for (int k = 0; k < tab; k++) {
                    fwrite(" ", 1, 1, tar);
                }
                fwrite(buff + i, 1, 1, tar);
            } else {
                fwrite(buff + i, 1, 1, tar);
            }
        } else if (buff[i] == '\n') {
            fwrite(buff + i, 1, 1, tar);
            if (i + 1 < len && buff[i + 1] != '}') {
                for (int k = 0; k < tab; k++) {
                    fwrite(" ", 1, 1, tar);
                }
            }
        } else {
            fwrite(buff + i, 1, 1, tar);
        }
    }

    free(buff);
}

#ifndef __version__
#define __version__ "unkown"
#endif

int main(int argc, char const *argv[]) {
    if (argc < 3) {
        printf("version: " __version__ "\n");
        printf("usage: jsonc outname struct1.h struct2.h struct3.h ...\n");
        return 0;
    }

#include "./json.inc"

    char file_head[1024] = {0};
    char file_source[1024] = {0};
    char file_tmp_decode[1024] = {0};
    char file_tmp_encode[1024] = {0};

    snprintf(file_head, 1024, "%s.h", argv[1]);
    snprintf(file_source, 1024, "%s.c", argv[1]);
    snprintf(file_tmp_decode, 1024, "%s.decode.c.tmp", argv[1]);
    snprintf(file_tmp_encode, 1024, "%s.encode.c.tmp", argv[1]);

    struct context ctx = {0};
    ctx.decode = fopen(file_tmp_decode, "wb+");
    if (!ctx.decode) {
        printf("file open error: %s", file_tmp_decode);
        return -1;
    }
    ctx.encode = fopen(file_tmp_encode, "wb+");
    if (!ctx.encode) {
        printf("file open error: %s", file_tmp_encode);
        return -1;
    }
    ctx.head = fopen(file_head, "w");
    if (!ctx.head) {
        printf("file open error: %s", file_head);
        return -1;
    }
    FILE *source = fopen(file_source, "w");
    if (!source) {
        printf("file open error: %s", file_source);
        return -1;
    }

    for (int i = 2; i < argc; i++) {
        putl(source, "#include \"%s\"", argv[i]);
    }

    char file_name[128];
    int i = strlen(argv[1]) - 1;
    for (; i > 0; i--) {
        if (argv[1][i] == '/') {
            i++;
            break;
        }
    }
    strcpy(file_name, argv[1] + i);

    putl(source, "");
    putl(source, "char %s_parse_error[1024];", file_name);
    putl(source, "");
    putl(source, "#pragma GCC diagnostic ignored \"-Wformat-truncation=\"");
    putl(source, "#pragma GCC diagnostic ignored \"-Wrestrict\"");
    putl(source, "#pragma GCC diagnostic ignored \"-Wunused-variable\"");
    putl(source, "#pragma GCC diagnostic ignored \"-Wunused-function\"");
    putl(source, "#define ERROR(fmt, args...) snprintf(%s_parse_error, sizeof(%s_parse_error), \"%%s [%%s:%%d] \" fmt, %s_parse_error, __FILE__, __LINE__, ##args);", file_name, file_name, file_name);
    putl(source, "#define ERROR_CLEAR %s_parse_error[0] = 0;", file_name);
    putl(source, "%s", base_funcs);

    putl(ctx.head, "#pragma once");
    putl(ctx.head, "");
    for (int i = 2; i < argc; i++) {
        putl(ctx.head, "#include \"%s\"", argv[i]);
    }

    for (int i = 2; i < argc; i++) {
        logd("parse %s", argv[i]);
        FILE *file = fopen(argv[i], "rb");
        if (!file) {
            loge("file not exist: %s", argv[i]);
            return -1;
        }
        fseek(file, 0, SEEK_END);
        int len = ftell(file);
        fseek(file, 0, SEEK_SET);
        char *buff = malloc(len);
        fread(buff, 1, len, file);
        fclose(file);

        struct scanner scan = {0};
        scan.text = buff;
        scan.text_len = len;
        ctx.scan = &scan;

        int rst = parse_file(&ctx);

        free(buff);

        fflush(ctx.decode);
        fflush(ctx.encode);
        fflush(ctx.head);
        fflush(source);
        if (rst) {
            append_file(source, ctx.decode);
            append_file(source, ctx.encode);
        } else {
            loge("file parse error: %s", argv[i]);
            remove(file_tmp_decode);
            remove(file_tmp_encode);
            remove(file_head);
            remove(file_source);
            exit(-1);
        }
        fclose(ctx.decode);
        fclose(ctx.encode);
        ctx.decode = fopen(file_tmp_decode, "wb+");
        ctx.encode = fopen(file_tmp_encode, "wb+");
    }

    fclose(ctx.decode);
    fclose(ctx.encode);
    remove(file_tmp_decode);
    remove(file_tmp_encode);

    fflush(ctx.head);
    fflush(source);
    fclose(ctx.head);
    fclose(source);
    return 0;
}
