#!/bin/bash

first_line=true
echo "const char* base_funcs=\"\\" > json.inc
while IFS= read -r line; do
    if $first_line; then
        first_line=false
        continue
    fi
    echo "$line\\n\\" >> json.inc
done < "json.head.template"
echo "\";" >> json.inc
gcc -g -finput-charset=UTF-8 -funsigned-char -Werror -O3 -D__version__="\"`date`\""  jsonc.c -o jsonc
rm -f json.inc