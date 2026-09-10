#include "train.h"
#include "../useful/globals_ocr.h"
#include "../process_img/image.h"
#include "../process_img/detection.h"
#include "../useful/matrix.h"
#include "nn.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>


double start_to_loss(int *input, Ocr *ocr, int expected)
{
    if (!input)
    {
        printf("Error: start_to_loss, no input\n");
        return 0;
    }

	if (!ocr)
	{
		printf("Error: start_to_loss, no OCR\n");
		pthread_mutex_unlock(&mutex_global_ocr);
		return 0;
	}

	Layer *input_layer = ocr->nn;

    put_in_input(input, 28, 28, input_layer);

    forward(input_layer, ocr->training);

    Layer *output_layer = get_output_layer(input_layer);

    soft_max(output_layer);

    double p = output_layer->output[expected];

    if (p < 1e-8) p = 1e-8;
    double loss = -log(p);
    int idx_max_output = index_max_output(output_layer);
    int result = 0;

    if (idx_max_output == expected)
        result = 1;

	pthread_mutex_lock(&mutex_global_ocr);
    pthread_mutex_lock(&mutex_char);

	update_stats_ocr(ocr, result, output_layer->output[expected]);

    printf("loss: %f, value: %f, steps: %d, "
            "expected: %d, get: %d, result: %d, lr: %f\n",
            loss, output_layer->output[expected], *ocr->c_steps,
            expected, idx_max_output, result, *ocr->learning_coeff);

    pthread_mutex_unlock(&mutex_char);
	pthread_mutex_unlock(&mutex_global_ocr);

    //print_outputs(output_layer);

    for (int i = 0; i < output_layer->current_size; i++)
        output_layer->delta[i] = output_layer->output[i];

    output_layer->delta[expected] -= 1.0; //cross entropy

    return loss;
}

double browse(int *input, Ocr *ocr, int expected)
{
    double loss = start_to_loss(input, ocr, expected);

    Layer *output_layer = get_output_layer(ocr->nn);
    backward(output_layer, ocr->training);

    compute_gradients(output_layer, ocr->training);

    update_SGD(output_layer, *ocr->learning_coeff);

    update_learning_coeff(*ocr->c_steps, ocr->learning_coeff);

    return loss;
}

void average_gradients(Layer **outputs, int nb_threads)
{
    Layer *currs[nb_threads];

    for (int th = 0; th < nb_threads; th++)
        currs[th] = outputs[th];

    while (currs[0] && currs[0]->weights)
    {
        int cs = currs[0]->current_size;
        int ps = currs[0]->previous_size;

        for (int i = 0; i < cs; i++)
        {
            double sum_bias = 0.0;

            for (int th = 0; th < nb_threads; th++)
                sum_bias += currs[th]->grad_biases[i];

            currs[0]->grad_biases[i] =
                sum_bias / nb_threads;

            for (int j = 0; j < ps; j++)
            {
                int idx = i * ps + j;
                double sum = 0.0;

                for (int th = 0; th < nb_threads; th++)
                    sum += currs[th]->grad_weights[idx];

                currs[0]->grad_weights[idx] =
                    sum / nb_threads;
            }
        }

        for (int th = 0; th < nb_threads; th++)
            currs[th] = currs[th]->prev;
    }
}

void *train_thread(void *arg)
{
    void **args = arg;

    Image *img = args[0];
    Ocr *ocr = args[1];
    int *ans_list = args[3];

    int start = *(int *)args[4];
    int end = *(int *)args[5];

    int nb_threads = *(int *)args[6];

    Layer **outputs = args[7];

    for (int i = start; i < end; i++)
    {
        int expected = ans_list[i];

        int *square = coordinates_to_matrix(img, 28 * i, 0, 28 * (i + 1) - 1, 27);

        int *save = square;

        int x0 = 0;
        int y0 = 0;
        int x1 = 0;
        int y1 = 0;

        int *bbox = get_bbox(square, 28, 28, &x0, &y0, &x1, &y1);

		square = matrix_to_28x28(bbox, (x1 - x0 + 1), (y1 - y0 + 1));

        free(save);
        free(bbox);

        start_to_loss(square, ocr, expected);

        Layer *output = get_output_layer(ocr->nn);

        backward(output, ocr->training);
        compute_gradients(output, ocr->training);

        int ret = pthread_barrier_wait(&barrier_char); // Wait for every threads to read one char

        if (ret == PTHREAD_BARRIER_SERIAL_THREAD) // One thread updates shared biases and weights
        {
            average_gradients(outputs, nb_threads);

            update_SGD(outputs[0], *ocr->learning_coeff);
		 }

        pthread_barrier_wait(&barrier_char); // Wait for the SGD update

        free(square);
    }

    return NULL;
}

void train(size_t nb_file, Ocr *ocr, int nb_threads)
{
	if (!ocr)
	{
		perror("no ocr");
		return;
	}

	if (!ocr->nn)
	{
		perror("no neural network");
		return;
	}

    if (nb_threads < 1)
        nb_threads = 1;

    if (nb_threads > NB_THREADS_MAX)
        nb_threads = NB_THREADS_MAX;

	double learning_coeff = *ocr->learning_coeff;

    char file_img_name[255];
    char file_ans_name[255];

    sprintf(file_img_name, "ocr/images/training/ribbon/%ld.png", nb_file);
    sprintf(file_ans_name, "ocr/images/training/ribbon_ans/%ld.txt", nb_file);

    FILE *ans = fopen(file_ans_name, "r");

    if (!ans)
    {
        perror(file_ans_name);
        return;
    }

    fseek(ans, 0, SEEK_END);
    int file_size = ftell(ans);
    rewind(ans);

    int *ans_list = malloc(file_size * sizeof(int));

    for (int i = 0; i < file_size; i++)
        ans_list[i] = fgetc(ans) - '0';

    Image *img = load_png(file_img_name);

    pthread_t threads[nb_threads];

    void *args[nb_threads][9];

    int starts[nb_threads];
    int ends[nb_threads];

    Ocr *ocrs[nb_threads];
    Layer *outputs[nb_threads];

    int step = file_size / nb_threads;

    pthread_barrier_init(&barrier_char, NULL, nb_threads);

    for (int th = 0; th < nb_threads; th++)
    {
        ocrs[th] = create_ocr_clone(ocr);
        outputs[th] = get_output_layer(ocrs[th]->nn);
    }

    for (int th = 0; th < nb_threads; th++)
    {
        starts[th] = th * step;
        ends[th] = starts[th] + step;

        args[th][0] = img;
        args[th][1] = ocrs[th];
		args[th][2] = NULL;
        args[th][3] = ans_list;
        args[th][4] = &starts[th];
        args[th][5] = &ends[th];
        args[th][6] = &nb_threads;
        args[th][7] = outputs;

        pthread_create(&threads[th], NULL, train_thread, args[th]);
    }

    for (int th = 0; th < nb_threads; th++)
        pthread_join(threads[th], NULL);

    for (int th = 0; th < nb_threads; th++)
		destroy_ocr_clone(ocrs[th]);

    pthread_barrier_destroy(&barrier_char);

	int remainder_start = step * nb_threads;

	for (int i = remainder_start; i < file_size; i++)
	{
		int expected = ans_list[i];

		int *square = coordinates_to_matrix(img, 28 * i, 0, 28 * (i + 1) - 1, 27);

		browse(square, ocr, expected);

		free(square);
	}

	free(ans_list);
    free_image(img);

    fclose(ans);
}

