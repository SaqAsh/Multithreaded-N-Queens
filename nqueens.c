// from the Cilk manual: http://supertech.csail.mit.edu/cilk/manual-5.4.6.pdf
#include <pthread.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

atomic_int number_of_solutions = 0;

typedef struct {
  int j;
  int n;
} location;

void *threadFn(void *arg);

int safe(char *config, int i, int j) {
  int r, s;

  for (r = 0; r < i; r++) {
    s = config[r];
    if (j == s || i - r == j - s || i - r == s - j)
      return 0;
  }
  return 1;
}

// removed unused global count

void nqueens(char *config, int n, int i) {
  char *new_config;
  int j;

  if (i == n) {
    atomic_fetch_add(&number_of_solutions, 1);
    return;
  }

  /* try each possible position for queen <i> */
  for (j = 0; j < n; j++) {
    /* allocate a temporary array and copy the config into it */
    new_config = malloc((i + 1) * sizeof(char));
    memcpy(new_config, config, i * sizeof(char));
    if (safe(new_config, i, j)) {
      new_config[i] = j;
      nqueens(new_config, n, i + 1);
    }
    free(new_config);
  }
  return;
}

int main(int argc, char *argv[]) {
  int n = 0;
  pthread_t *t;
  location *thread_args;

  if (argc < 2) {
    printf("%s: number of queens required\n", argv[0]);
    return 1;
  }

  n = atoi(argv[1]);
  if (n <= 0) {
    printf("%s: n must be positive\n", argv[0]);
    return 1;
  }
  t = (pthread_t *)malloc(n * sizeof(pthread_t));
  thread_args = (location *)malloc(n * sizeof(location));

  for (int i = 0; i < n; i++) {
    thread_args[i].j = i;
    thread_args[i].n = n;
    pthread_create(&t[i], NULL, threadFn, &thread_args[i]);
  }

  printf("running queens %d\n", n);

  for (int i = 0; i < n; i++) {
    pthread_join(t[i], NULL);
  }

  printf("# solutions: %d\n", atomic_load(&number_of_solutions));

  free(thread_args);
  free(t);
  return 0;
}

void *threadFn(void *arg) {
  location *_location = (location *)arg;
  int n = _location->n;
  int j = _location->j;
  char *config = malloc(n * sizeof(char));
  config[0] = j;
  nqueens(config, n, 1);
  free(config);
  return NULL;
}
