#ifndef LAYER_H
#define LAYER_H

typedef struct Layer
{
    char *name;
    int previous_size;
    int current_size;

    double *weights; //matrix (output_size × input_size)
    double *grad_weights;
    double *biases; //output_size
    double *grad_biases;
    double *output; //activations
    double *delta; //gradients
    double *z;

    struct Layer *prev;
    struct Layer *next;
} Layer;

Layer *create_layer(char *name, int previous_size, int current_size);
Layer *init_layers();
Layer *create_layer_clone(Layer *l);

void free_layer(Layer *l);
void free_layers(Layer *l);
void free_layer_clone(Layer *l);

Layer *get_output_layer(Layer *l);

void print_outputs(Layer *l);
void put_in_input(int *input, int W, int H, Layer *input_layer);
int index_max_output(Layer *l);

#endif
