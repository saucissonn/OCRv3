#include "ocr/useful/matrix.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

static uint8_t **create_matrix(int w, int h)
{
    uint8_t **matrix = malloc(h * sizeof(uint8_t *));

    for (int y = 0; y < h; y++)
    {
        matrix[y] = malloc(w * sizeof(uint8_t));

        for (int x = 0; x < w; x++)
            matrix[y][x] = 0;
    }

    return matrix;
}

static void free_matrix(uint8_t **matrix, int h)
{
    if (!matrix)
        return;

    for (int y = 0; y < h; y++)
        free(matrix[y]);

    free(matrix);
}

static void assert_matrix_equal(
    uint8_t **matrix,
    const uint8_t *expected,
    int w,
    int h)
{
    for (int y = 0; y < h; y++)
    {
        for (int x = 0; x < w; x++)
        {
            assert(matrix[y][x] == expected[y * w + x]);
        }
    }
}

void test_shift_matrix_right(void)
{
    uint8_t **m = create_matrix(3, 3);

    uint8_t values[] = {
        1, 2, 3,
        4, 5, 6,
        7, 8, 9
    };

    for (int y = 0; y < 3; y++)
        for (int x = 0; x < 3; x++)
            m[y][x] = values[y * 3 + x];

    shift_matrix(m, 3, 3, 1, 0);

    uint8_t expected[] = {
        0, 1, 2,
        0, 4, 5,
        0, 7, 8
    };

    assert_matrix_equal(m, expected, 3, 3);

    free_matrix(m, 3);

    printf("test_shift_matrix_right: OK\n");
}

void test_shift_matrix_left(void)
{
    uint8_t **m = create_matrix(3, 3);

    uint8_t values[] = {
        1, 2, 3,
        4, 5, 6,
        7, 8, 9
    };

    for (int y = 0; y < 3; y++)
        for (int x = 0; x < 3; x++)
            m[y][x] = values[y * 3 + x];

    shift_matrix(m, 3, 3, -1, 0);

    uint8_t expected[] = {
        2, 3, 0,
        5, 6, 0,
        8, 9, 0
    };

    assert_matrix_equal(m, expected, 3, 3);

    free_matrix(m, 3);

    printf("test_shift_matrix_left: OK\n");
}

void test_shift_matrix_down(void)
{
    uint8_t **m = create_matrix(3, 3);

    uint8_t values[] = {
        1, 2, 3,
        4, 5, 6,
        7, 8, 9
    };

    for (int y = 0; y < 3; y++)
        for (int x = 0; x < 3; x++)
            m[y][x] = values[y * 3 + x];

    shift_matrix(m, 3, 3, 0, 1);

    uint8_t expected[] = {
        0, 0, 0,
        1, 2, 3,
        4, 5, 6
    };

    assert_matrix_equal(m, expected, 3, 3);

    free_matrix(m, 3);

    printf("test_shift_matrix_down: OK\n");
}

void test_shift_matrix_up(void)
{
    uint8_t **m = create_matrix(3, 3);

    uint8_t values[] = {
        1, 2, 3,
        4, 5, 6,
        7, 8, 9
    };

    for (int y = 0; y < 3; y++)
        for (int x = 0; x < 3; x++)
            m[y][x] = values[y * 3 + x];

    shift_matrix(m, 3, 3, 0, -1);

    uint8_t expected[] = {
        4, 5, 6,
        7, 8, 9,
        0, 0, 0
    };

    assert_matrix_equal(m, expected, 3, 3);

    free_matrix(m, 3);

    printf("test_shift_matrix_up: OK\n");
}

void test_shift_matrix_diagonal(void)
{
    uint8_t **m = create_matrix(3, 3);

    uint8_t values[] = {
        1, 2, 3,
        4, 5, 6,
        7, 8, 9
    };

    for (int y = 0; y < 3; y++)
        for (int x = 0; x < 3; x++)
            m[y][x] = values[y * 3 + x];

    shift_matrix(m, 3, 3, 1, 1);

    uint8_t expected[] = {
        0, 0, 0,
        0, 1, 2,
        0, 4, 5
    };

    assert_matrix_equal(m, expected, 3, 3);

    free_matrix(m, 3);

    printf("test_shift_matrix_diagonal: OK\n");
}

void test_shift_matrix_no_shift(void)
{
    uint8_t **m = create_matrix(2, 2);

    m[0][0] = 1;
    m[0][1] = 2;
    m[1][0] = 3;
    m[1][1] = 4;

    shift_matrix(m, 2, 2, 0, 0);

    uint8_t expected[] = {
        1, 2,
        3, 4
    };

    assert_matrix_equal(m, expected, 2, 2);

    free_matrix(m, 2);

    printf("test_shift_matrix_no_shift: OK\n");
}

void test_expand_matrix(void)
{
    uint8_t **m = create_matrix(2, 2);

    m[0][0] = 1;
    m[0][1] = 2;
    m[1][0] = 3;
    m[1][1] = 4;

    m = expand_matrix(m, 2, 2, 4, 3);

    uint8_t expected[] = {
        1, 2, 0, 0,
        3, 4, 0, 0,
        0, 0, 0, 0
    };

    assert_matrix_equal(m, expected, 4, 3);

    free_matrix(m, 3);

    printf("test_expand_matrix: OK\n");
}

void run_matrix_tests()
{
    printf("=== Matrix tests ===\n");

    test_shift_matrix_right();
    test_shift_matrix_left();
    test_shift_matrix_up();
    test_shift_matrix_down();
    test_shift_matrix_diagonal();
    test_shift_matrix_no_shift();

    test_expand_matrix();

    printf("=== All tests passed ===\n");
}
