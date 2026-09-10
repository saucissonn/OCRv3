#include "../frame.h"
#include "../gui_elements/text_area.h"
#include "../image_graphics.h"
#include "../globals.h"
#include "frames.h"
#include "common_elements.h"
#include "../../ocr/neural_network/ocr.h"
#include "../../ocr/useful/globals_ocr.h"
#include "../../ocr/neural_network/save.h"
#include "../../ocr/neural_network/train.h"
#include "../../ocr/process_img/fs.h"
#include "../update.h"
#include "../utils.h"

#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <pthread.h>
#include <stdio.h>
#include <dirent.h>

static void call_thread_ocr(void **data);
static void reset_frame_stats(void **data);

// Setup frames layout
static Rectangle *init_rectangles()
{
    Rectangle *rectangles = NULL;

    Rectangle *left_bar = create_rectangle(0, 0, BAR_W, 1, 0, Gray);
    rectangles = add_rectangle(rectangles, left_bar);

    Rectangle *default_background = create_rectangle(BAR_W, 0, 1 - BAR_W, 1, 0, White);
    rectangles = add_rectangle(rectangles, default_background);

    return rectangles;
}

static Text *init_texts()
{
    Text *res = NULL;

    Text *txt1 = create_text("OCR training:", DEFAULT_SPACE_W, DEFAULT_SPACE_W, FONT_SIZE5, White);
    res = add_text(res, txt1);

    Text *txt2 = create_text("Number of input neurons: 784", DEFAULT_SPACE_W, 0.07, FONT_SIZE3, White);
    res = add_text(res, txt2);

    Text *txt3 = create_text("Number of output neurons: 10", DEFAULT_SPACE_W, 0.1, FONT_SIZE3, White);
    txt3->y = txt2->y + (float)txt2->rect->h / DELTA_H;
    txt3->rect->y = txt2->rect->y + txt2->rect->h;
    res = add_text(res, txt3);

	Text *txt4 = create_text("Number of hidden neurons per layer", DEFAULT_SPACE_W, 0.1, FONT_SIZE3, White);
	txt4->y = 0.2 - (float)txt4->rect->h / DELTA_H;
	txt4->rect->y = 0.2 * DELTA_H - (float)txt4->rect->h;
	res = add_text(res, txt4);

    Text *txt5 = create_text("Number of threads", DEFAULT_SPACE_W, 0.1, FONT_SIZE3, White);
    txt5->y = 0.3 - (float)txt5->rect->h / DELTA_H;
    txt5->rect->y = 0.3 * DELTA_H - (float)txt5->rect->h;
    res = add_text(res, txt5);

    Text *txt6 = create_text("Number of images to read (100 char / image)", DEFAULT_SPACE_W, 0.1, FONT_SIZE3, White);
    txt6->y = 0.4 - (float)txt6->rect->h / DELTA_H;
    txt6->rect->y = 0.4 * DELTA_H - (float)txt6->rect->h;
    res = add_text(res, txt6);

    Text *txt7 = create_text("Name of the save file (.ocr will be added)", DEFAULT_SPACE_W, 0.1, FONT_SIZE3, White);
    txt7->y = 0.5 - (float)txt7->rect->h / DELTA_H;
    txt7->rect->y = 0.5 * DELTA_H - (float)txt7->rect->h;
    res = add_text(res, txt7);

    return res;
}

static Button *init_buttons(Frame *frame)
{
    Button *res = NULL;

	Button *btn1 = create_options_button(frame);

    res = add_button(res, btn1);

    Text *text2 = create_text("Launch & Save", 0, 0, FONT_SIZE4, White);
    Rectangle *rect2 = create_rectangle(DEFAULT_SPACE_W, 0.85, 0.25, 0.1, DEFAULT_BORDER_W, Blue);
    Button *btn2 = create_button(1, text2, rect2);
    btn2->function = call_thread_ocr;
	btn2->args = malloc(sizeof(void *) * 2);
    btn2->args[0] = frame;
    btn2->args[1] = NULL;

	res = add_button(res, btn2);

    Text *text3 = create_text("Stop & not Save", 0, 0, FONT_SIZE4, White);
    Rectangle *rect3 = create_rectangle(DEFAULT_SPACE_W, 0.7, 0.25, 0.1, DEFAULT_BORDER_W, Red);
    Button *btn3 = create_button(1, text3, rect3);
    btn3->function = reset_frame_stats;
    btn3->args = malloc(sizeof(void *) * 2);
    btn3->args[0] = frame;
    btn3->args[1] = NULL;

    res = add_button(res, btn3);

    return res;
}

static TextArea *init_text_areas()
{
    TextArea *res = NULL;

    for (int i = 0; i < 3; i++)
    {
		Text *text1 = create_text("", 0, 0, FONT_SIZE3, Black);
		Rectangle *rect1 = create_rectangle(DEFAULT_SPACE_W + 0.1 * i, 0.2, 0.09, 0.05, DEFAULT_BORDER_W, White);
		TextArea *txta1 = create_text_area(text1, rect1);
		txta1->alpha = 0;

		res = add_text_area(res, txta1);
	}

	Text *text2 = create_text("", 0, 0, FONT_SIZE3, Black);
	Rectangle *rect2 = create_rectangle(DEFAULT_SPACE_W, 0.3, 0.09, 0.05, DEFAULT_BORDER_W, White);
	TextArea *txta2 = create_text_area(text2, rect2);
	txta2->alpha = 0;

    res = add_text_area(res, txta2);

    Text *text3 = create_text("", 0, 0, FONT_SIZE3, Black);
    Rectangle *rect3 = create_rectangle(DEFAULT_SPACE_W, 0.4, 0.09, 0.05, DEFAULT_BORDER_W, White);
    TextArea *txta3 = create_text_area(text3, rect3);
    txta3->alpha = 0;

	res = add_text_area(res, txta3);

    Text *text4 = create_text("save", 0, 0, FONT_SIZE3, Black);
    Rectangle *rect4 = create_rectangle(DEFAULT_SPACE_W, 0.5, 0.3, 0.05, DEFAULT_BORDER_W, White);
    TextArea *txta4 = create_text_area(text4, rect4);

    res = add_text_area(res, txta4);

    return res;
}

static Cursor *init_cursors() {
	return NULL;
}

// End of setup frames layout

// Start of setup additional features

static void quit_function(void **data)
{
    Frame *frame = data[0];

	void **data2 = malloc(sizeof(void *) * 2);
	data2[0] = frame;
	data2[1] = NULL;

	reset_frame_stats(data2);

	// Because reset delete then create the frame stats so we delete it once again
    destroy_frame(frame->additional_display_args[0]); // Frame stats
    free(frame->additional_display_args);

	free(data2);

	frame->additional_display_args = NULL;

	clear_frame(frame);
}

static void additional_display_function(void **data) // To update OCR stats easily. Also destroys OCR at the end
{
    Frame *frame_stats = data[0];
	
	resize_texts(Renderer, frame_stats->texts); // Lazy but working

    int c_steps;
    int longest_streak;
    int current_streak;
    double average_guesses_all;
	double average_guesses_100;
    double average_certainty_all;
	double average_certainty_100;

    pthread_mutex_lock(&mutex_global_ocr);

    if (!global_ocr)
    {
        pthread_mutex_unlock(&mutex_global_ocr);
        display_frame(frame_stats);
        return;
    }
	
	pthread_mutex_lock(&mutex_char);

    c_steps = *global_ocr->c_steps;
    longest_streak = *global_ocr->longest_streak;
    current_streak = *global_ocr->current_streak;

	pthread_mutex_unlock(&mutex_global_ocr);

    average_guesses_all = c_steps ? *global_ocr->sum_guesses / (double)c_steps : 0;
	average_guesses_100 = c_steps ? *global_ocr->sum_guesses_100 / (double)(((c_steps - 1) % 100 + 1)) : 0;

    average_certainty_all = c_steps ? *global_ocr->sum_certainty / (double)c_steps : 0;
	average_certainty_100 = c_steps ? *global_ocr->sum_certainty_100 / (double)(((c_steps - 1) % 100 + 1)) : 0;
	
	pthread_mutex_unlock(&mutex_char);

    Text *curr = frame_stats->texts->next;

    set_text_int(Renderer, curr, c_steps); // steps
    curr = curr->next;

    set_text_int(Renderer, curr, longest_streak); // longest streak
    curr = curr->next;

    set_text_int(Renderer, curr, current_streak); // current streak
    curr = curr->next;

    set_text_double(Renderer, curr, average_guesses_all); // average guesses all
    curr = curr->next;

	set_text_double(Renderer, curr, average_guesses_100);// average guesses last 100 char
	curr = curr->next;

    set_text_double(Renderer, curr, average_certainty_all); // average certainty all
	curr = curr->next;

	set_text_double(Renderer, curr, average_certainty_100); // average certainty last 100 char

    display_frame(frame_stats);

	pthread_mutex_lock(&mutex_global_ocr);

	if (global_ocr->stop_flag)
	{
	    destroy_ocr(global_ocr);
		global_ocr = NULL;
	}

	pthread_mutex_unlock(&mutex_global_ocr);
}

static Text *init_texts_additional_display()
{
    Text *res = NULL;

	Text *txt1 = create_text("OCR stats:", DEFAULT_SPACE_W + BAR_W, DEFAULT_SPACE_W, FONT_SIZE5, Black);
	res = add_text(res, txt1);

	Text *txt2 = create_text("Number char read: ", DEFAULT_SPACE_W + BAR_W, 0.1, FONT_SIZE3, Black);
	txt2->y = 0.2 - (float)txt2->rect->h / DELTA_H;
	txt2->rect->y = 0.2 * DELTA_H - (float)txt2->rect->h;
	txt2->len_max = 1000;
	res = add_text(res, txt2);

	Text *txt3 = create_text("Longest streak: ", DEFAULT_SPACE_W + BAR_W, 0.1, FONT_SIZE3, Black);
	txt3->y = 0.3 - (float)txt3->rect->h / DELTA_H;
	txt3->rect->y = 0.3 * DELTA_H - (float)txt3->rect->h;
	txt3->len_max = 1000;
	res = add_text(res, txt3);

	Text *txt4 = create_text("Current streak: ", DEFAULT_SPACE_W + BAR_W, 0.1, FONT_SIZE3, Black);
	txt4->y = 0.4 - (float)txt4->rect->h / DELTA_H;
	txt4->rect->y = 0.4 * DELTA_H - (float)txt4->rect->h;
	txt4->len_max = 1000;
	res = add_text(res, txt4);

	Text *txt5 = create_text("Average of guesses (all): ", DEFAULT_SPACE_W + BAR_W, 0.1, FONT_SIZE3, Black);
	txt5->y = 0.5 - (float)txt5->rect->h / DELTA_H;
	txt5->rect->y = 0.5 * DELTA_H - (float)txt5->rect->h;
	txt5->len_max = 1000;
	res = add_text(res, txt5);

    Text *txt6 = create_text("Average of guesses (last 100 char): ", DEFAULT_SPACE_W + BAR_W, 0.1, FONT_SIZE3, Black);
    txt6->y = 0.6 - (float)txt6->rect->h / DELTA_H;
    txt6->rect->y = 0.6 * DELTA_H - (float)txt6->rect->h;
    txt6->len_max = 1000;
    res = add_text(res, txt6);

	Text *txt7 = create_text("Average of certainty (all): ", DEFAULT_SPACE_W + BAR_W, 0.1, FONT_SIZE3, Black);
	txt7->y = 0.7 - (float)txt7->rect->h / DELTA_H;
	txt7->rect->y = 0.7 * DELTA_H - (float)txt7->rect->h;
	txt7->len_max = 1000;
	res = add_text(res, txt7);

    Text *txt8 = create_text("Average of certainty (last 100 char): ", DEFAULT_SPACE_W + BAR_W, 0.1, FONT_SIZE3, Black);
    txt8->y = 0.8 - (float)txt8->rect->h / DELTA_H;
    txt8->rect->y = 0.8 * DELTA_H - (float)txt8->rect->h;
    txt8->len_max = 1000;
    res = add_text(res, txt8);

    return res;
}

static void init_additional_display(Frame *frame) // Need to display additional features for stats
{
    frame->additional_display_function = additional_display_function;
    frame->additional_display_args = malloc(sizeof(void *) * 2);

	// Possible to only create texts but it's better if more gui needs to be added later on
	Frame *frame_stats = create_frame(NULL); // No parent = No conflict
	
	frame_stats->texts = init_texts_additional_display();

	frame->additional_display_args[0] = frame_stats;
	frame->additional_display_args[1] = NULL;
}

static void reset_frame_stats(void **data) // Stops OCR and creates a new one
{
    Frame *frame = data[0];

    pthread_mutex_lock(&mutex_global_ocr);

    if (!global_ocr)
    {
        pthread_mutex_unlock(&mutex_global_ocr);
        return;
    }

    //int nb_threads = global_ocr->nb_threads;

    int ret = pthread_barrier_init(&barrier_global_ocr_stop1, NULL, 2);

    if (ret)
    {
        pthread_mutex_unlock(&mutex_global_ocr);
        return;
    }

    global_ocr->stop_flag = 1;

    pthread_mutex_unlock(&mutex_global_ocr);

    pthread_barrier_wait(&barrier_global_ocr_stop1);

    destroy_frame(frame->additional_display_args[0]); // Frame stats
    free(frame->additional_display_args);
    init_additional_display(frame); // Init the OCR here

    printf("reset stats and clear OCR\n");
}

static void *setup_and_train_ocr(void *data) // Needs to be called with pthread_create()
{
    void **args = data;

	int nb_layers = *(int *)args[0];
	int *size_layers = args[1];
	int nb_threads = *(int *)args[2];
	int c_steps_local = *(int *)args[3];
	char *save_file = args[4];
	Frame *frame = args[5];

    Button *launch_btn = frame->buttons->next;

    launch_btn->interactible = 0; // Disable button click to prevent multiple launches

	if (nb_threads < 1)
		nb_threads = 1;
	if (nb_threads > NB_THREADS_MAX)
		nb_threads = NB_THREADS_MAX;

	global_ocr = create_ocr(0.01, nb_layers, size_layers);
	global_ocr->nb_threads = nb_threads;
	global_ocr->training = 1;

	pthread_barrier_init(&barrier_char, NULL, nb_threads);

    double learning_coeff = 0.01;

    for (int i = 0; i < c_steps_local; i++)
    {
        size_t nb_file = random_file("ocr/images/training/ribbon");

		pthread_mutex_lock(&mutex_global_ocr);

		if (global_ocr->stop_flag)
		{
			pthread_mutex_unlock(&mutex_global_ocr);

			free(args[0]);
			free(args[2]);
			free(args[3]);
			free(args);

			pthread_barrier_wait(&barrier_global_ocr_stop1);

			pthread_mutex_lock(&mutex_global_ocr);

			destroy_ocr(global_ocr);
			global_ocr = NULL;

			pthread_mutex_unlock(&mutex_global_ocr);

			pthread_barrier_destroy(&barrier_char);
			pthread_barrier_destroy(&barrier_global_ocr_stop1);

			launch_btn->interactible = 1;

			return NULL;
		}

		pthread_mutex_unlock(&mutex_global_ocr);

        train(nb_file, global_ocr, nb_threads);
    }

	char buff[255];
	snprintf(buff, sizeof(buff), "saves/%s.ocr", save_file);
    save_ocr(buff, global_ocr);

	pthread_mutex_lock(&mutex_global_ocr);

	global_ocr->stop_flag = 1;

    free(args[0]);
	//free(args[1]);
    free(args[2]);
    free(args[3]);
	//free(args[4]);
    free(args);

	pthread_mutex_unlock(&mutex_global_ocr);

	pthread_barrier_destroy(&barrier_char);

	launch_btn->interactible = 1;

	return NULL;
}

static void call_thread_ocr(void **data) // Calls setup_and_train_ocr()
{
	Frame *frame = data[0];	

	if (data[1])
	{
		pthread_t thread_ocr;

		pthread_create(&thread_ocr, NULL, setup_and_train_ocr, data[1]);

		pthread_detach(thread_ocr); // to not wait for the threads to end
		return;
	}

	void **data2 = malloc(sizeof(void *) * 7); // args of the function called

	TextArea *curr = frame->text_areas;
	
	int nb_layers = 2;
    for (int i = 0; i < 3; i++)
    {
        if (atoi(curr->text->text))
			nb_layers += 1;

        curr = curr->next;
    }

	if (nb_layers < 3) // OCR is invalid
	{
		free(data2);
		printf("No hidden layers\n");
		return;
	}

	int *size_layers = malloc(sizeof(int) * nb_layers);
	size_layers[0] = 28 * 28;

	curr = frame->text_areas;

	int c = 1;
	for (int i = 0; i < 3; i++)
	{
		if (atoi(curr->text->text))
		{
			size_layers[c] = atoi(curr->text->text);
			c += 1;
		}		

		curr = curr->next;
	}

	size_layers[c] = 10;

	int *addr_nb_layers = malloc(sizeof(int));
	*addr_nb_layers = nb_layers;

	int *addr_nb_threads = malloc(sizeof(int));
	*addr_nb_threads = atoi(curr->text->text);
	
	curr = curr->next;

	int *addr_steps = malloc(sizeof(int));
	*addr_steps = atoi(curr->text->text);

	curr = curr->next;
	
	data2[0] = addr_nb_layers;
	data2[1] = size_layers;
	data2[2] = addr_nb_threads;
	data2[3] = addr_steps;
	data2[4] = curr->text->text;
	data2[5] = data[0]; // frame
	data2[6] = NULL;

    pthread_t thread_ocr;

    pthread_create(&thread_ocr, NULL, setup_and_train_ocr, data2);

	pthread_detach(thread_ocr); // to not wait for the threads to end
}
/*
static void call_all_trains(Frame *frame)
{
    for (int i = 1; i < 5 * 5 * 5; i++)
    {
        void **data = malloc(sizeof(void *) * 7);

        int *addr_nb_layers = malloc(sizeof(int));
        int *size_layers = malloc(sizeof(int) * 5);
        int *addr_nb_threads = malloc(sizeof(int));
        int *addr_steps = malloc(sizeof(int));

        char *pattern = itoa_base_n(i, 5);

        int len = strlen(pattern);
        int base = 16;

        size_layers[0] = 28 * 28;

        for (int j = 0; j < len; j++)
            size_layers[j + 1] = base << (pattern[j] - '0');

        size_layers[len + 1] = 10;

        *addr_nb_layers = len + 2;
        *addr_nb_threads = 10;
        *addr_steps = 1000;

        char *save = malloc(255);

        if (len == 1)
            sprintf(save, "save_layers_%d", size_layers[1]);
        else if (len == 2)
            sprintf(save, "save_layers_%d_%d",
                    size_layers[1], size_layers[2]);
        else
            sprintf(save, "save_layers_%d_%d_%d",
                    size_layers[1], size_layers[2], size_layers[3]);

        data[0] = addr_nb_layers;
        data[1] = size_layers;
        data[2] = addr_nb_threads;
        data[3] = addr_steps;
        data[4] = save;
        data[5] = frame;
        data[6] = NULL;

        pthread_t thread;

        pthread_create(&thread, NULL, setup_and_train_ocr, data);
        pthread_join(thread, NULL);

        free(pattern);
    }
}

static int get_best_training(char *dir_name)
{
    DIR *dir = opendir(dir_name);
    if (!dir)
    {
        perror("opendir");
        return -1;
    }

    struct dirent *entry;

	char *res = NULL;
	int maxi = 0;

    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        char buff[255];
        snprintf(buff, sizeof(buff), "%s/%s", dir_name, entry->d_name);
        printf("%s\n", buff);

		Ocr *ocr = load_ocr(buff);

		if (!ocr)
			continue;

		if (maxi < *ocr->longest_streak)
		{
			maxi = *ocr->longest_streak;
			free(res);
			res = strdup(buff);
		}

		destroy_ocr(ocr);
    }

	printf("res = %s\n", res);

	free(res);

    closedir(dir);
    return 0;
}
*/
void init_frame_training(Frame *frame)
{
    frame->rectangles = init_rectangles();
    frame->texts = init_texts();
    frame->buttons = init_buttons(frame);
    frame->text_areas = init_text_areas();
    frame->cursors = init_cursors();

    frame->quit_function = quit_function;
    frame->quit_args = malloc(sizeof(void *) * 2);
    frame->quit_args[0] = frame;
    frame->quit_args[1] = NULL;

	init_additional_display(frame);

	//call_all_trains(frame); // Be careful when you call it it's really long
	//get_best_training("saves");
}

