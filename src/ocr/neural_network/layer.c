#include "ocr/neural_network/layer.h"
#include "ocr/useful/utils.h"
#include "common/globals.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stddef.h>

Layer *create_layer(char *name, int previous_size, int current_size)
{
    Layer *res = malloc(sizeof(Layer));
    res->previous_size = previous_size;
    res->current_size = current_size;
    res->name = name;

    res->output = malloc(current_size * sizeof(double));
    res->prev = NULL;
    res->next = NULL;

    if (name[0] == 'i') // It's the first Layer (name == "input")
    {
        res->weights = NULL;
        res->grad_weights = NULL;
        res->biases = NULL;
        res->grad_biases = NULL;
        res->delta = NULL;
        res->z = NULL;
	}
    else
    {
        double scale = sqrt(6.0 / (previous_size + current_size));
        res->weights = malloc(previous_size * current_size * sizeof(double));
        for (int i = 0; i < previous_size * current_size; i++)
            res->weights[i] = rand_uniform() * scale;

        res->grad_weights = malloc(previous_size * current_size * sizeof(double));

        res->biases = malloc(current_size * sizeof(double));
        for (int i = 0; i < current_size; i++)
            res->biases[i] = 0.01;

        res->grad_biases = malloc(current_size * sizeof(double));

        res->delta = malloc(current_size * sizeof(double));
        res->z = malloc(current_size * sizeof(double));
		
	}

    return res;
}

Layer *init_layers()
{
    Layer *res = NULL;

    int nb_input = 28 * 28;
    int nb_hidden = 256;
    int nb_output = NB_OUTPUTS_OCR; // A-Z

    Layer *input = create_layer("input", nb_input, nb_input); // We use a 28 * 28 image

    Layer *hidden1 = create_layer("hidden", nb_input, nb_hidden);
    input->next = hidden1;
    hidden1->prev = input;

    Layer *hidden2 = create_layer("hidden", nb_hidden, nb_hidden/2);
    hidden1->next = hidden2;
    hidden2->prev = hidden1;

    Layer *output = create_layer("output", nb_hidden/2, nb_output);
    hidden2->next = output;
    output->prev = hidden2;

    res = input;

    return res;
}

Layer *create_layer_clone(Layer *l)
{
    if (!l)
        return NULL;

    Layer *curr = l;
    Layer *first = NULL;
    Layer *prev_clone = NULL;

    while (curr)
    {
        Layer *clone = malloc(sizeof(Layer));
        if (!clone)
        {
            free_layers(first);
            return NULL;
        }

        clone->previous_size = curr->previous_size;
        clone->current_size = curr->current_size;
        clone->name = curr->name;

        clone->weights = curr->weights;
        clone->biases = curr->biases;

        clone->grad_weights = curr->weights ? malloc(clone->previous_size * clone->current_size * sizeof(double)) : NULL;

        clone->grad_biases = curr->biases ? malloc(clone->current_size * sizeof(double)) : NULL;

        clone->output = malloc(clone->current_size * sizeof(double));

        if (curr->delta)
            clone->delta = malloc(clone->current_size * sizeof(double));
        else
            clone->delta = NULL;

        if (curr->z)
            clone->z = malloc(clone->current_size * sizeof(double));
        else
            clone->z = NULL;

        clone->prev = prev_clone;
        clone->next = NULL;

        if (prev_clone)
            prev_clone->next = clone;
        else
            first = clone;

        prev_clone = clone;
        curr = curr->next;
    }

    return first;
}

void free_layer(Layer *l)
{
    if (!l) return;
    free(l->weights);
    free(l->biases);
    free(l->output);
    free(l->delta);
    free(l->z);
    free(l->grad_biases);
    free(l->grad_weights);
}

void free_layers(Layer *l)
{
    Layer *curr = l;

    while (curr)
    {
        Layer *tmp = curr;
        free_layer(curr);
        curr = curr->next;
        free(tmp);
    }
}

void free_layer_clone(Layer *l)
{
    while (l)
    {
        Layer *next = l->next;

        free(l->output);
        free(l->delta);
        free(l->z);
        free(l->grad_biases);
        free(l->grad_weights);

        free(l);

        l = next;
    }
}

Layer *get_output_layer(Layer *l)
{
    Layer *curr = l;

    while (curr->next)
        curr = curr->next;

    return curr;
}

void print_outputs(Layer *l)
{
    int cs = l->current_size;

    for (int i = 0; i < cs; i++)
        printf("%d: %lf\n", i, (l->output)[i]);
}

void put_in_input(int *input, int W, int H, Layer *input_layer)
{
    for (int y = 0; y < H; y++)
    {
        for (int x = 0; x < W; x++)
            (input_layer->output)[y*W + x] = (double)input[y*W + x];
    }
}

int index_max_output(Layer *l)
{
    int cs = l->current_size;
    int maxi = 0;

    for (int i = 0; i < cs; i++) {
        if (l->output[maxi] < l->output[i])
            maxi = i;
    }

    return maxi;
}


