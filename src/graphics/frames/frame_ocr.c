#include "../frame.h"
#include "../gui_elements/rectangle.h"
#include "../gui_elements/text.h"
#include "../gui_elements/button.h"
#include "../gui_elements/text_area.h"
#include "../image_graphics.h"
#include "../globals.h"
#include "frames.h"
#include "common_elements.h"
#include "../../ocr/useful/globals_ocr.h"
#include "../../ocr/useful/utils.h"
#include "../../ocr/useful/matrix.h"
#include "../../ocr/neural_network/save.h"
#include "../../ocr/process_img/transform.h"

#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>

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

    Text *txt1 = create_text(
        "OCR parameters:",
        DEFAULT_SPACE_W,
        DEFAULT_SPACE_W,
        FONT_SIZE4,
        White
    );

    res = add_text(res, txt1);

    Text *txt2 = create_text(
        "Insert an image here",
        0.5,
        0.5,
        FONT_SIZE5,
        Black
    );

    res = add_text(res, txt2);

    return res;
}


static void button_preprocessed_funtion(void **data)
{
    printf("See preprocessed image\n");
}

static void button_normal_funtion(void **data)
{
    printf("See normal image\n");
}

static void button_launch_funtion(void **data)
{
	Frame *frame = data[0];
	char *save = "saves/save.ocr"; //data[1] TODO

	if (!frame->image) return;

    global_ocr = load_ocr(save);

	int *sudoku = launch_ocr(frame->image, global_ocr);
	if (!sudoku)
	{
		destroy_ocr(global_ocr);
		return;
	}

	int *empty_sudoku = malloc(sizeof(int) * 81);
	for (int i = 0; i < 81; i++)
		empty_sudoku[i] = sudoku[i];

    destroy_ocr(global_ocr);
    global_ocr = NULL;

	printf("Before\n");
	print_matrix_values(sudoku, 9, 9);

	solve_sudoku(sudoku);

	printf("After\n");
	print_matrix_values(sudoku, 9, 9);

	modify_sudoku_image(frame->image, empty_sudoku, sudoku);

	SDL_DestroyTexture(frame->texture);
	frame->texture = image_to_texture(Renderer, frame->image);
	
	free(sudoku);
	free(empty_sudoku);

	printf("Launch OCR\n");
}

static void button_save_funtion(void **data)
{
    printf("Save image\n");
}

static Button *init_buttons(Frame *frame)
{
    Button *res = NULL;

    Text *text1 = create_text("See preprocessed image", 0, 0, FONT_SIZE4, White);
    Rectangle *rect1 = create_rectangle(DEFAULT_SPACE_W, 0.4, 0.3, 0.08, DEFAULT_BORDER_W, Blue);
    Button *btn1 = create_button(1, text1, rect1);
    btn1->function = button_preprocessed_funtion;

    res = add_button(res, btn1);

    Text *text2 = create_text("See normal image", 0, 0, FONT_SIZE4, White);
    Rectangle *rect2 = create_rectangle(DEFAULT_SPACE_W, 0.5, 0.3, 0.08, DEFAULT_BORDER_W, Blue);
    Button *btn2 = create_button(1, text2, rect2);
    btn2->function = button_normal_funtion;

    res = add_button(res, btn2);

    Text *text3 = create_text("Launch OCR", 0, 0, FONT_SIZE4, White);
    Rectangle *rect3 = create_rectangle(DEFAULT_SPACE_W, 0.6, 0.3, 0.08, DEFAULT_BORDER_W, Blue);
    Button *btn3 = create_button(1, text3, rect3);
    btn3->function = button_launch_funtion;
	
	btn3->args = malloc(sizeof(void *) * 2);
	btn3->args[0] = frame;
	btn3->args[1] = NULL;


	res = add_button(res, btn3);

    Text *text4 = create_text("Save Image", 0, 0, FONT_SIZE4, White);
    Rectangle *rect4 = create_rectangle(DEFAULT_SPACE_W, 0.7, 0.3, 0.08, DEFAULT_BORDER_W, Blue);
    Button *btn4 = create_button(1, text4, rect4);
    btn4->function = button_save_funtion;

    res = add_button(res, btn4);

	Button *btn5 = create_options_button(frame);

    res = add_button(res, btn5);

    return res;
}

static TextArea *init_text_areas()
{
    TextArea *res = NULL;

    Text *text1 = create_text("Area1", 0, 0, 0.05, Black);
    Rectangle *rect1 = create_rectangle(DEFAULT_SPACE_W, 0.2, 0.3, 0.1, DEFAULT_BORDER_W, White);

    TextArea *txta1 = create_text_area(text1, rect1);

    res = add_text_area(res, txta1);

    return res;
}

static Cursor *init_cursors()
{
    Cursor *res = NULL;

    //Cursor *crs1 = create_cursor(DEFAULT_SPACE_W, 0.8, 0.15, 0.03, 0, 100.);

    //res = add_cursor(res, crs1);

    return res;
}

static void quit_function(void **data)
{
	Frame *frame = data[0];

	clear_frame(frame);
}

void init_frame_ocr(Frame *frame)
{
	frame->name = strdup("frame_ocr_launch");

	frame->drop_zone = 1;

	frame->rectangles = init_rectangles();
	frame->texts = init_texts();
	frame->buttons = init_buttons(frame);	
	frame->text_areas = init_text_areas();
	frame->cursors = init_cursors();

	frame->quit_function = quit_function;
	frame->quit_args = malloc(sizeof(void *) * 2);
	frame->quit_args[0] = frame;
	frame->quit_args[1] = NULL;
}
