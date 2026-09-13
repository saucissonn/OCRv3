#ifndef TRAIN_H
#define TRAIN_H

#include "ocr.h"

#include <stddef.h>

double start_to_loss(int *input, Ocr *ocr, int expected);

double browse(int *input, Ocr *ocr, int expected);
void average_gradients(Layer **outputs, int nb_threads);

void *train_thread(void *arg);
void train(size_t nb_file, Ocr *ocr, int nb_threads);

#endif
