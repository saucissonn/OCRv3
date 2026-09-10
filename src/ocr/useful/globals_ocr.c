#include "globals_ocr.h"

pthread_barrier_t barrier_char;
pthread_barrier_t barrier_global_ocr_stop1;
pthread_barrier_t barrier_global_ocr_stop2;

pthread_mutex_t mutex_char = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_SGD = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_global_ocr = PTHREAD_MUTEX_INITIALIZER;

int NB_THREADS_MAX = 1;

Ocr *global_ocr = NULL;

int SGD_ready = 1;

