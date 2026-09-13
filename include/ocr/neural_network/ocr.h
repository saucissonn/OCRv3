#ifndef OCR_H
#define OCR_H

#include "layer.h"
#include "../process_img/image.h"

typedef struct Ocr
{
    int *c_steps; // How many steps of training, default 0
    double *learning_coeff;

    int nb_layers; // Default 4
	int *size_layers;

	// Stats
	int *longest_streak;
	int *current_streak;
	double *sum_guesses;
	double *sum_guesses_100;
	double *sum_certainty;
	double *sum_certainty_100;
	int nb_threads;
	int stop_flag;

	int training; // State
	
    Layer *nn;
} Ocr;

Ocr *create_ocr(double learning_coeff, int nb_layers, int *size_layers);
Ocr *init_ocr();

Ocr *create_ocr_clone(Ocr *ocr);

void destroy_ocr(Ocr *ocr);
void destroy_ocr_clone(Ocr *ocr);

void update_stats_ocr(Ocr *ocr, int current_guess, double certainty);

int *launch_ocr(Image *img, Ocr *ocr);

#endif
