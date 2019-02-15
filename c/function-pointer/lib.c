#include<stdio.h>

extern void do_main();

void _do_main() {
    void (*func)() = do_main;
    (*func)();
}