#ifndef NN_H
#define NN_H

#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>

#include "layer.h"

void relu(Layer *l);

void update_learning_coeff(int c_steps, double *learning_coeff);
void soft_max(Layer *l);
void sigmoid(Layer *l);

void forward(Layer *l, int training);
void backward(Layer *l, int training);
void compute_gradients(Layer *l, int training);
void update_SGD(Layer *l, double learning_coeff);

void print_outputs(Layer *l);
int index_max_output(Layer *l);
void put_in_output(SDL_Surface *rgba);

#endif
