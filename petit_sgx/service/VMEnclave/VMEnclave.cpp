#include <stdarg.h>
#include <stdio.h>      /* vsnprintf */
#include <string.h>      /* vsnprintf */

#include "VMEnclave_t.h"
#include "../LibEnclave/LibEnclave.h"

void printf(const char *fmt, ...)
{
    char buf[BUFSIZ] = {'\0'};
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buf, BUFSIZ, fmt, ap);
    va_end(ap);
    ocall_print_vm(buf);
}

void ecall_get_mr_enclave() {
    get_mr_enclave();
}

uint32_t ecall_init() {
    create_session();
    return 0;
}

void ecall_test() {
    printf("hello world from enclave!\n");
}