#include "ocr/neural_network/ocr.h"
#include "common/globals.h"
#include "ocr/process_img/transform.h"
#include "ocr/useful/matrix.h"
#include "ocr/neural_network/nn.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdlib.h>

Ocr *create_ocr(double learning_coeff, int nb_layers, int *size_layers)
{
	Ocr *res = malloc(sizeof(Ocr));

	int *c_steps = malloc(sizeof(int));
	*c_steps = 0;
	res->c_steps = c_steps;

    double *addr_learning_coeff = malloc(sizeof(double));
    *addr_learning_coeff = learning_coeff;
	res->learning_coeff = addr_learning_coeff;

	res->nb_layers = nb_layers;
    res->size_layers = size_layers;

    int *longest_streak = malloc(sizeof(int));
	*longest_streak = 0;
	res->longest_streak = longest_streak;

    int *current_streak = malloc(sizeof(int));
    *current_streak = 0;
    res->current_streak = current_streak;

    double *sum_guesses = malloc(sizeof(double));
	*sum_guesses = 0;
	res->sum_guesses = sum_guesses;

    double *sum_guesses_100 = malloc(sizeof(double));
    *sum_guesses_100 = 0;
    res->sum_guesses_100 = sum_guesses_100;

	double *sum_certainty = malloc(sizeof(double));
	*sum_certainty = 0;
	res->sum_certainty = sum_certainty;

    double *sum_certainty_100 = malloc(sizeof(double));
    *sum_certainty_100 = 0;
    res->sum_certainty_100 = sum_certainty_100;

	res->nb_threads = 0; // Default
	res->stop_flag = 0; // Default

	res->training = 1; // Default

	Layer *nn = create_layer("input", size_layers[0], size_layers[0]);
	Layer *curr = create_layer("hidden", size_layers[0], size_layers[1]);
	nn->next = curr;
	curr->prev = nn;	

	for (int i = 1; i < nb_layers - 2; i++)
	{
		Layer *temp = create_layer("hidden", size_layers[i], size_layers[i + 1]);
		curr->next = temp;
		temp->prev = curr;
		
		curr = temp;
	}

	Layer *temp2 = create_layer("output", size_layers[nb_layers - 2], size_layers[nb_layers - 1]);
    curr->next = temp2;
    temp2->prev = curr;

    res->nn = nn;

	return res;
}

Ocr *init_ocr()
{
	int nb_layers = 4;
	int *size_layers = malloc(sizeof(int) * 4);
	size_layers[0] = 28 * 28;
    size_layers[1] = 256;
    size_layers[2] = 128;
    size_layers[3] = NB_OUTPUTS_OCR;

	Ocr *res = create_ocr(0.01, nb_layers, size_layers);

	return res;
}

Ocr *create_ocr_clone(Ocr *ocr)
{
    if (!ocr)
        return NULL;

    Ocr *clone = malloc(sizeof(Ocr));
    if (!clone)
        return NULL;

    clone->c_steps = ocr->c_steps;
    clone->learning_coeff = ocr->learning_coeff;
    clone->nb_layers = ocr->nb_layers;

    clone->size_layers = malloc(ocr->nb_layers * sizeof(int));

    if (!clone->size_layers)
    {
        free(clone);
        return NULL;
    }

    for (int i = 0; i < ocr->nb_layers; i++)
        clone->size_layers[i] = ocr->size_layers[i];

	
    clone->longest_streak = ocr->longest_streak;
	clone->current_streak = ocr->current_streak;
    clone->sum_guesses = ocr->sum_guesses;
	clone->sum_guesses_100 = ocr->sum_guesses_100;
    clone->sum_certainty = ocr->sum_certainty;
	clone->sum_certainty_100 = ocr->sum_certainty_100;

	clone->training = 0;

    clone->nn = create_layer_clone(ocr->nn);

    if (!clone->nn)
    {
        free(clone->size_layers);
        free(clone);
        return NULL;
    }

    return clone;
}

void destroy_ocr_clone(Ocr *ocr)
{
    if (!ocr)
        return;

    free(ocr->size_layers);
    free_layer_clone(ocr->nn);
    free(ocr);
}

void destroy_ocr(Ocr *ocr)
{
	if (!ocr) return;

	free(ocr->c_steps);
	free(ocr->learning_coeff);
	free(ocr->size_layers);

	free(ocr->longest_streak);
	free(ocr->current_streak);
	free(ocr->sum_guesses);
	free(ocr->sum_guesses_100);
	free(ocr->sum_certainty);
	free(ocr->sum_certainty_100);

	free_layers(ocr->nn);
	free(ocr);
}

void update_stats_ocr(Ocr *ocr, int result, double certainty)
{
    (*ocr->c_steps)++;

	if (*ocr->c_steps % 100 == 1)
	{
		*ocr->sum_guesses_100 = 0;
		*ocr->sum_certainty_100 = 0;
	}

    *ocr->sum_guesses += result;
    *ocr->sum_guesses_100 += result;
    *ocr->sum_certainty += certainty;
    *ocr->sum_certainty_100 += certainty;

    if (result)
    {
        (*ocr->current_streak)++;

        if (*ocr->current_streak > *ocr->longest_streak)
            *ocr->longest_streak = *ocr->current_streak;
    }
    else
    {
        *ocr->current_streak = 0;
    }
}

int *launch_ocr(Image *img, Ocr *ocr)
{
    ocr->training = 0;

	
	//free(img->puzzle_rectangles);
    //free(img->word_rectangles);    

//	double h_angle = hough_angle(img, 0.1);
//	printf("angle: %lf\n", h_angle);
//	rotate_image(img, -h_angle);
    process_image(img);

    return NULL;
}
