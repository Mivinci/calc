#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include "stack.h"

#define DEFAULT_BUF_SIZE 128

struct stack stk;

// 从后缀表达式中读出一条数字
int read_num(const char *p, int *i) {
    int r = 0;
    while (isdigit(*(p+*i))) {
        r = r * 10 + (*(p+*i) - '0');
        (*i)++;
    }
    return r;
}

bool isop(char op) {
    return (op == '+' || op == '-' || op == '*' || op == '/') ? true : false;
}

int prior(char op) {
    switch (op) {
        case '\0': return -1;
        case '(': return 0;
        case '+':
        case '-': return 1;
        case '*':
        case '/': return 2;
        default: return -1;
    }
}

void exiterr(const char *s) {
    fputs(s, stderr);
    exit(EXIT_FAILURE);
}

void help() {
    fprintf(stdout, 
        "Usage: calc [options] [1*(2+3)]\n\n"
        "Options:\n"
        "-c\toutput the reverse polish expression\n"
        "-h\thelp\n"
    );
}

int operate(char op) {
    int b = stack_pop(&stk);
    int a = stack_pop(&stk);
    int r = 0;
    switch (op) {
        case '+': r = a+b; break;
        case '-': r = a-b; break;
        case '*': r = a*b; break;
        case '/': r = a/b; break;
        default: exiterr("unknown operator");
    }
    stack_push(&stk, r);
    return r;
}

// 计算后缀表达式
int calculate(const char *s) {
    int i = 0;
    char c;
    while (*(s+i) != '\0') {
        c = *(s+i);
        if (isdigit(c)) {
            stack_push(&stk, read_num(s, &i));
        } else if (isspace(c)) {
            i++;
        } else {
            operate(c);
            i++;
        }
    }
    return stack_pop(&stk);
}

int chcat(char *s, char c, int i) {
    *(s+i) = c;
    return ++i;
}

// 中缀表达式转后缀表达式
char *convert(const char *s, char *dst) {
    struct stack sk;
    stack_init(&sk);
    stack_push(&sk, '\0');
    size_t n = strlen(s);
    int j = 0;
    char c;
    for (int i = 0; i < n; ++i) {
        c = *(s+i);
        if (isdigit(c)) {
            j = chcat(dst, c, j);
        } else if (c == '(') {
            stack_push(&sk, c);
        } else if (c == ')') {
            while (stack_top(&sk) != '(') {
                j = chcat(dst, (char)stack_pop(&sk), j);
            }
            stack_pop(&sk);
        } else if (isop(c)) {
            j = chcat(dst, ' ', j);
            while (prior(c) <= prior((char)stack_top(&sk))) {
                j = chcat(dst, (char)stack_pop(&sk), j);
            }
            stack_push(&sk, c);
        }
    }
    while (!stack_empty(&sk)) {
        j = chcat(dst, (char)stack_pop(&sk), j);
    }
    return dst;
}

int main(int argc, char **argv) {
    stack_init(&stk);
    char buf[DEFAULT_BUF_SIZE];
    memset(buf, '\0', sizeof(buf));

    // 输入模式
    if (argc == 1) {
        char s[DEFAULT_BUF_SIZE];
        while (strcmp(s, "exit") != 0) {
            printf(">> ");
            scanf("%s", s);
            convert(s, buf);
            printf("%d\n", calculate(buf));
        }
        printf("You have exited.\n");
        return 0;
    }

    
    // 命令模式
    int output = 0; // 是否显示后缀表达式
    char opt;
    for (int i = 1; i < argc; i++)
    {
        if (*argv[i] == '-') {
            opt = *(argv[i]+1);
            switch(opt) {
                case 'h': help(); return EXIT_SUCCESS;
                case 'c': output = 1; break;
                default: fprintf(stdout, "Unknown option: %c\n", opt);
            }
        } else {
            convert(argv[i], buf);
            if (output) {
                printf("%s\n", buf);
            }
            printf("%d\n", calculate(buf));
        }
    }
    
    return EXIT_SUCCESS;
}
