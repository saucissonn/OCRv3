#ifndef GLOBALS_OCR_H
#define GLOBALS_OCR_H

#include <pthread.h>

#include "../neural_network/ocr.h"

extern pthread_barrier_t barrier_char;
extern pthread_barrier_t barrier_global_ocr_stop1;
extern pthread_barrier_t barrier_global_ocr_stop2;

extern pthread_mutex_t mutex_char;
extern pthread_mutex_t mutex_SGD;
extern pthread_mutex_t mutex_global_ocr;

extern int NB_THREADS_MAX;

extern Ocr *global_ocr;

extern int SGD_ready;

#endif
