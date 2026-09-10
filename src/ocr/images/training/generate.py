import os
import random
from PIL import Image, ImageDraw, ImageFont

# =========================
# CONFIGURATION
# =========================

IMAGE_SIZE = 504
GRID_SIZE = 9
CELL_SIZE = IMAGE_SIZE // GRID_SIZE

SUDOKU_DIR = "sudoku"
ANSWERS_DIR = "sudoku_ans"

MIN_EMPTY_CELLS = 40
MAX_EMPTY_CELLS = 60


# =========================
# SUDOKU
# =========================

def is_valid(grid, row, col, num):
    for i in range(9):
        if grid[row][i] == num:
            return False

        if grid[i][col] == num:
            return False

    box_row = (row // 3) * 3
    box_col = (col // 3) * 3

    for y in range(box_row, box_row + 3):
        for x in range(box_col, box_col + 3):
            if grid[y][x] == num:
                return False

    return True


def fill_grid(grid):
    cells = [(row, col) for row in range(9) for col in range(9) if grid[row][col] == 0]

    if not cells:
        return True

    row, col = cells[0]

    numbers = list(range(1, 10))
    random.shuffle(numbers)

    for num in numbers:
        if is_valid(grid, row, col, num):
            grid[row][col] = num

            if fill_grid(grid):
                return True

            grid[row][col] = 0

    return False


def generate_sudoku():
    grid = [[0 for _ in range(9)] for _ in range(9)]

    fill_grid(grid)

    empty_cells = random.randint(MIN_EMPTY_CELLS, MAX_EMPTY_CELLS)

    positions = [(row, col) for row in range(9) for col in range(9)]

    random.shuffle(positions)

    for i in range(empty_cells):
        row, col = positions[i]
        grid[row][col] = 0

    return grid


# =========================
# IMAGE
# =========================

def get_font(size):
    font_paths = [
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        "/usr/share/fonts/truetype/liberation2/LiberationSans-Regular.ttf",
        "/usr/share/fonts/truetype/freefont/FreeSans.ttf",
    ]

    for path in font_paths:
        if os.path.exists(path):
            return ImageFont.truetype(path, size)

    return ImageFont.load_default()


def draw_sudoku(grid, output_path):
    image = Image.new("RGB", (IMAGE_SIZE, IMAGE_SIZE), "white")

    draw = ImageDraw.Draw(image)

    # Lines of the grid
    for i in range(10):
        position = i * CELL_SIZE

        if i % 3 == 0:
            width = 4
        else:
            width = 1

        draw.line((position, 0, position, IMAGE_SIZE), fill="black", width=width)

        draw.line((0, position, IMAGE_SIZE, position), fill="black", width=width)

    font = get_font(int(CELL_SIZE * 0.65))

    # Digits
    for row in range(9):
        for col in range(9):
            value = grid[row][col]

            if value == 0:
                continue

            text = str(value)

            bbox = draw.textbbox((0, 0), text, font=font)

            text_width = bbox[2] - bbox[0]
            text_height = bbox[3] - bbox[1]

            x = (col * CELL_SIZE + (CELL_SIZE - text_width) / 2)

            y = (row * CELL_SIZE + (CELL_SIZE - text_height) / 2 - bbox[1])

            draw.text((x, y), text, fill="black", font=font)

    image.save(output_path, "PNG")


# =========================
# ANSWER
# =========================

def sudoku_to_string(grid):
    return "".join(str(grid[row][col]) for row in range(9) for col in range(9))


def save_answer(grid, output_path):
    answer = sudoku_to_string(grid)

    with open(output_path, "w") as file:
        file.write(answer)


# =========================
# GENERATION
# =========================

def generate_dataset(n):
    os.makedirs(SUDOKU_DIR, exist_ok=True)
    os.makedirs(ANSWERS_DIR, exist_ok=True)

    generated = set()

    i = 0

    while i < n:
        grid = generate_sudoku()

        answer = sudoku_to_string(grid)

        if answer in generated:
            continue

        generated.add(answer)

        image_path = os.path.join(SUDOKU_DIR, f"{i}.png")

        answer_path = os.path.join(ANSWERS_DIR, f"{i}.txt")


        draw_sudoku(grid, image_path)
        save_answer(grid, answer_path)

        i += 1

        print(f"{i}/{n} generated")


# =========================
# MAIN
# =========================

if __name__ == "__main__":
    n = int(input("Nb of sudokus to generate: "))

    generate_dataset(n)
