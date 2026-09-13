#include "ocr/neural_network/nn.h"
#include "common/globals.h"
#include "ocr/process_img/image.h"
#include "ocr/useful/matrix.h"
#include "ocr/useful/utils.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

void relu(Layer *l)
{
    int cs = l->current_size;
    
	for (int i = 0; i < cs; i++)
	{
        if (l->output[i] < 0)
		{
            l->output[i] = 0;
        }
    }
}

void update_learning_coeff(int c_steps, double *learning_coeff)
{
    if (c_steps > 10000)
	{
        if (c_steps % 3000 == 0) 
		{
            *learning_coeff *= 0.98;
            if (*learning_coeff < 0.0005)
				*learning_coeff = 0.0005;
        }
    }
}

void soft_max(Layer *l)
{
    int cs = l->current_size;
    double max = l->output[0];

    for (int i = 1; i < cs; i++)
	{
        if (max < l->output[i])
            max = l->output[i];
    }

    double sum = 0;

    for (int i = 0; i < cs; i++)
        sum += exp(l->output[i] - max);

    for (int i = 0; i < cs; i++)
        l->output[i] = exp(l->output[i] - max) / sum;
}

void sigmoid(Layer *l)
{
    int cs = l->current_size;

    for (int i = 0; i < cs; i++)
        l->output[i] = 1./(1. + exp(-(l->output)[i]));
}

void forward(Layer *l, int training)
{
	if (!l) return;

	Layer *prev = l;
	Layer *curr = l->next;
	
	while (curr)
	{
		int cs = curr->current_size;
		int ps = curr->previous_size;
		double sum = 0;

		for (int i = 0; i < cs; i++)
		{
			sum = curr->biases[i];

			for (int j = 0; j < ps; j++)
				sum += prev->output[j] * curr->weights[i*ps + j];

			curr->z[i] = sum;
			curr->output[i] = sum;

		}

		if (curr->next)			
			relu(curr);

		prev = curr;
		curr = curr->next;
	}
}

double relu_prime(double v)
{
    if (v <= 0)
        return 0;

    return 1.0;
}

void backward(Layer *l, int training)
{
    if (!l) return;

    Layer *curr = l;
    Layer *prev = l->prev;

    while (curr)
    {
		int cs = curr->current_size;
		int ps = curr->previous_size;

		if (!prev->delta) return;

		double s = 0;

		for (int j = 0; j < ps; j++)
		{
			s = 0.0;

            if (training == 1)
            {
				prev->delta[j] = 0;
                
				continue;
            }
			
			for (int i = 0; i < cs; i++)
				s += curr->weights[i*ps + j] * curr->delta[i];

			if (prev->z) 
			{
				prev->delta[j] = s * relu_prime(prev->z[j]);
			}
			else 
			{
				prev->delta[j] = s;
			}
		}
		
		curr = prev;
		prev = prev->prev;
	}
}

void compute_gradients(Layer *l, int training)
{
    if (!l) return;

    Layer *curr = l;
    Layer *prev = l->prev;

    while (curr)
    {
        int cs = curr->current_size;
        int ps = curr->previous_size;

        if (!prev) return;

        for (int i = 0; i < cs; i++)
        {
			if (training == 1)
			{
				for (int j = 0; j < ps; j++)
				{
					int idx = i * ps + j;

					curr->grad_weights[idx] = 0;
				}

                curr->grad_biases[i] = 0;
			}
			
			else
			{			
				for (int j = 0; j < ps; j++)
				{
					int idx = i * ps + j;

					curr->grad_weights[idx] = curr->delta[i] * prev->output[j];
				}

				curr->grad_biases[i] = curr->delta[i];
			}
		}

        curr = prev;
        prev = prev->prev;
    }
}

void update_SGD(Layer *l, double learning_coeff)
{
    if (!l) return;

    Layer *curr = l;

    while (curr)
    {
        if (!curr->weights) return;

        int cs = curr->current_size;
        int ps = curr->previous_size;

        double lambda = 1e-4;

        for (int i = 0; i < cs; i++)
        {
            for (int j = 0; j < ps; j++)
            {
                int idx = i * ps + j;

                curr->weights[idx] -= learning_coeff * (curr->grad_weights[idx] + lambda * curr->weights[idx]);
            }

            curr->biases[i] -= learning_coeff * curr->grad_biases[i];
        }

        curr = curr->prev;
    }
}
