#include "save.h"
#include "../useful/globals_ocr.h"

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

/*
Save format:

Magic number: "SOCR" (4 bytes) // Super OCR
Vesrion: 0 (4 bytes)
Learning coeff: 0 (8 bytes)

C steps: 0 (4 bytes)

Nb layers: 0 (4 bytes)
Size of each layer: {0, ...} (4 bytes * (Nb layers))

Longest streak: (4 bytes)
Sum guesses: (8 bytes)
Sum guesses 100: (8 bytes)
Sum certainty: (8 bytes)
Sum certainty100: (8 bytes)

W1 (8 bytes per elements)
B1
W2
B2
...
*/

void save_ocr(const char *path, Ocr *ocr)
{
	FILE *file = fopen(path, "wb");

	if (!file)
	{
		perror("save_ocr: fopen");
		return;
	}

	char magic_number[4] = "SOCR";
	fwrite(magic_number, sizeof(char), 4, file);

	int version = 2;
	fwrite(&version, sizeof(int), 1, file);

	fwrite(&ocr->learning_coeff, sizeof(double), 1, file);

	fwrite(&ocr->c_steps, sizeof(int), 1, file);

	fwrite(&ocr->nb_layers, sizeof(int), 1, file);

	fwrite(ocr->longest_streak, sizeof(int), 1, file);
	fwrite(ocr->sum_guesses, sizeof(double), 1, file);
	fwrite(ocr->sum_guesses_100, sizeof(double), 1, file);
	fwrite(ocr->sum_certainty, sizeof(double), 1, file);
	fwrite(ocr->sum_certainty_100, sizeof(double), 1, file);

	fwrite(ocr->size_layers, sizeof(int), ocr->nb_layers, file);

	Layer *curr = ocr->nn->next;

	while (curr)
	{
		fwrite(curr->weights, sizeof(double), curr->current_size * curr->previous_size, file);
		fwrite(curr->biases, sizeof(double), curr->current_size, file);
		
		curr = curr->next;
	}
	
	fclose(file);
}

Ocr *load_ocr(const char *path)
{
    FILE *file = fopen(path, "rb");

    if (!file)
    {
        perror("load_ocr: fopen");
        return NULL;
    }

    char magic_number[4];

    if (fread(magic_number, sizeof(char), 4, file) != 4)
        goto error;

    if (memcmp(magic_number, "SOCR", 4) != 0)
    {
        fprintf(stderr, "load_ocr: invalid magic number\n");
        goto error;
    }

    int version;

    if (fread(&version, sizeof(int), 1, file) != 1)
        goto error;

    if (version != 2)
    {
        fprintf(stderr, "load_ocr: unsupported version %d\n", version);
        goto error;
    }

    double learning_coeff;
    int c_steps;
    int nb_layers;

    if (fread(&learning_coeff, sizeof(double), 1, file) != 1)
        goto error;

    if (fread(&c_steps, sizeof(int), 1, file) != 1)
        goto error;

    if (fread(&nb_layers, sizeof(int), 1, file) != 1)
        goto error;

    if (nb_layers < 2)
    {
        fprintf(stderr, "load_ocr: invalid number of layers\n");
        goto error;
    }

    int longest_streak;
    double sum_guesses;
    double sum_guesses_100;
    double sum_certainty;
    double sum_certainty_100;

    if (fread(&longest_streak, sizeof(int), 1, file) != 1)
        goto error;

    if (fread(&sum_guesses, sizeof(double), 1, file) != 1)
        goto error;

    if (fread(&sum_guesses_100, sizeof(double), 1, file) != 1)
        goto error;

    if (fread(&sum_certainty, sizeof(double), 1, file) != 1)
        goto error;

    if (fread(&sum_certainty_100, sizeof(double), 1, file) != 1)
        goto error;

    int *size_layers = malloc(sizeof(int) * nb_layers);

    if (!size_layers)
        goto error;

    if (fread(size_layers, sizeof(int), nb_layers, file) != nb_layers)
    {
        free(size_layers);
        goto error;
    }

    Ocr *ocr = create_ocr(learning_coeff, nb_layers, size_layers);
    //free(size_layers);

    if (!ocr)
        goto error;

    *ocr->c_steps = c_steps;
    *ocr->longest_streak = longest_streak;
    *ocr->sum_guesses = sum_guesses;
    *ocr->sum_guesses_100 = sum_guesses_100;
    *ocr->sum_certainty = sum_certainty;
    *ocr->sum_certainty_100 = sum_certainty_100;

    Layer *curr = ocr->nn->next;

    while (curr)
    {
        size_t weights_count = (size_t)curr->current_size * curr->previous_size;

        if (fread(curr->weights, sizeof(double), weights_count, file) != weights_count)
        {
            destroy_ocr(ocr);
            goto error;
        }

        if (fread(curr->biases, sizeof(double), curr->current_size, file) != (size_t)curr->current_size)
        {
            destroy_ocr(ocr);
            goto error;
        }

        curr = curr->next;
    }

    fclose(file);
    return ocr;

error:
    fprintf(stderr, "load_ocr: invalid or corrupted file\n");
    fclose(file);
    return NULL;
}
