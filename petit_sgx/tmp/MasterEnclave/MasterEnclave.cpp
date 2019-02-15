/*
 * Copyright (C) 2011-2017 Intel Corporation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in
 *     the documentation and/or other materials provided with the
 *     distribution.
 *   * Neither the name of Intel Corporation nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <stdlib.h>
#include <math.h>

#include <stdarg.h>
#include <stdio.h>      /* vsnprintf */
#include <string.h>      /* vsnprintf */

#include "MasterEnclave.h"
#include "MasterEnclave_t.h"  /* print_string */

#include "sgx_trts.h"
#include "sgx_utils.h"
#include "sgx_eid.h"
#include "sgx_ecp_types.h"
#include "sgx_thread.h"
#include <map>
#include "dh_session_protocol.h"
#include "sgx_dh.h"
#include "sgx_tcrypto.h"

#include "error_codes.h"

#define MAX_SESSION_COUNT  16

//number of open sessions
uint32_t g_session_count = 0;

//Array of open session ids
session_id_tracker_t *g_session_id_tracker[MAX_SESSION_COUNT];

std::map<sgx_enclave_id_t, dh_session_t>g_dest_session_info_map;
std::map<sgx_enclave_id_t, dh_session_t>g_src_session_info_map;


/* 
 * printf: 
 *   Invokes OCALL to display the enclave buffer to the terminal.
 */
void printf(const char *fmt, ...)
{
    char buf[BUFSIZ] = {'\0'};
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buf, BUFSIZ, fmt, ap);
    va_end(ap);
    print_ocall(buf);
}

void test_ecall(sgx_enclave_id_t enclave_id) {
    printf("enclave_id = %u\n", (uint64_t)enclave_id);
}

int x, y, n, t, i, flag;
long int e[50], d[50], temp[50], j, m[50], en[50];
char msg[100];
int prime(long int);
void encryption_key();
long int cd(long int);
void encrypt();
void decrypt();

// http://www.trytoprogram.com/c-examples/c-program-to-encrypt-and-decrypt-string/
void test_ecall()
{
  printf("\nENTER FIRST PRIME NUMBER\n");
  scanf("%d", &x);
  flag = prime(x);
  if(flag == 0)
  {
    printf("\nINVALID INPUT\n");
    exit(0);
  }
  printf("\nENTER SECOND PRIME NUMBER\n");
  scanf("%d", &y);
  flag = prime(y);
  if(flag == 0 || x == y)
  {
    printf("\nINVALID INPUT\n");
    exit(0);
  }
  printf("\nENTER MESSAGE OR STRING TO ENCRYPT\n");

  scanf("%s",msg);
  for(i = 0; msg[i] != NULL; i++)
    m[i] = msg[i];
  n = x * y;
  t = (x-1) * (y-1);
  encryption_key();
  printf("\nPOSSIBLE VALUES OF e AND d ARE\n");
  for(i = 0; i < j-1; i++)
    printf("\n%ld\t%ld", e[i], d[i]);
  encrypt();
  decrypt();
  return 0;
}

int prime(long int pr)
{
  int i;
  j = sqrt(pr);
  for(i = 2; i <= j; i++)
  {
   if(pr % i == 0)
     return 0;
  }
  return 1;
 }

//function to generate encryption key
void encryption_key()
{
  int k;
  k = 0;
  for(i = 2; i < t; i++)
  {
    if(t % i == 0)
     continue;
    flag = prime(i);
    if(flag == 1 && i != x && i != y)
    {
     e[k] = i;
     flag = cd(e[k]);
    if(flag > 0)
    {
     d[k] = flag;
     k++;
    }
   if(k == 99)
    break;
   }
 }
}

long int cd(long int a)
{
  long int k = 1;
  while(1)
  {
    k = k + t;
    if(k % a == 0)
     return(k / a);
  }
}

//function to encrypt the message
void encrypt()
{
  long int pt, ct, key = e[0], k, len;
  i = 0;
  len = strlen(msg);
  while(i != len)
  {
    pt = m[i];
    pt = pt - 96;
    k = 1;
    for(j = 0; j < key; j++)
    {
     k = k * pt;
     k = k % n;
    }
   temp[i] = k;
   ct = k + 96;
   en[i] = ct;
   i++;
  }
  en[i] = -1;
  printf("\n\nTHE ENCRYPTED MESSAGE IS\n");
  for(i = 0; en[i] != -1; i++)
    printf("%c", en[i]);
}

//function to decrypt the message
void decrypt()
{
  long int pt, ct, key = d[0], k;
  i = 0;
  while(en[i] != -1)
  {
    ct = temp[i];
    k = 1;
    for(j = 0; j < key; j++)
    {
      k = k * ct;
      k = k % n;
    }
   pt = k + 96;
   m[i] = pt;
   i++;
  }
  m[i] = -1;
  printf("\n\nTHE DECRYPTED MESSAGE IS\n");
  for(i = 0; m[i] != -1; i++)
   printf("%c", m[i]);
  printf("\n");
}
