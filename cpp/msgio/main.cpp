#include<stdio.h>
#include<uuid/uuid.h>

typedef char

int main(int argc, char *argv[]){
    char a[] = "abcde";
    char *b = "abcde";
    printf("sizeof(a):%d\n", sizeof(a));
    printf("sizeof(*a):%d\n", sizeof(*a));
    printf("sizeof(b):%d\n", sizeof(b));
    printf("sizeof(*b):%d\n", sizeof(*b));
    printf("%s\n", a);
}