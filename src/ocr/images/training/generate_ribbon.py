import os
import random
import string
from PIL import Image, ImageDraw, ImageFont


IMAGE_HEIGHT = 28
LETTER_WIDTH = 28
LETTER_COUNT = 100
IMAGE_WIDTH = LETTER_WIDTH * LETTER_COUNT

RIBBON_DIR = "ribbon"
ANSWERS_DIR = "ribbon_ans"

MIN_FONT_SIZE = 16
MAX_FONT_SIZE = 24

MAX_X_OFFSET = 1
MAX_Y_OFFSET = 1

LETTERS = string.ascii_uppercase


def get_random_font(size):
    fonts = [
        # DejaVu
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        "/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf",
        "/usr/share/fonts/truetype/dejavu/DejaVuSans-Oblique.ttf",
        "/usr/share/fonts/truetype/dejavu/DejaVuSans-BoldOblique.ttf",
        "/usr/share/fonts/truetype/dejavu/DejaVuSansCondensed.ttf",
        "/usr/share/fonts/truetype/dejavu/DejaVuSansCondensed-Bold.ttf",
        "/usr/share/fonts/truetype/dejavu/DejaVuSerif.ttf",
        "/usr/share/fonts/truetype/dejavu/DejaVuSerif-Bold.ttf",
        "/usr/share/fonts/truetype/dejavu/DejaVuSerif-Italic.ttf",
        "/usr/share/fonts/truetype/dejavu/DejaVuSerifCondensed.ttf",
        "/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf",
        "/usr/share/fonts/truetype/dejavu/DejaVuSansMono-Bold.ttf",

        # Liberation
        "/usr/share/fonts/truetype/liberation2/LiberationSans-Regular.ttf",
        "/usr/share/fonts/truetype/liberation2/LiberationSans-Bold.ttf",
        "/usr/share/fonts/truetype/liberation2/LiberationSans-Italic.ttf",
        "/usr/share/fonts/truetype/liberation2/LiberationSans-BoldItalic.ttf",
        "/usr/share/fonts/truetype/liberation2/LiberationSerif-Regular.ttf",
        "/usr/share/fonts/truetype/liberation2/LiberationSerif-Bold.ttf",
        "/usr/share/fonts/truetype/liberation2/LiberationSerif-Italic.ttf",
        "/usr/share/fonts/truetype/liberation2/LiberationSerif-BoldItalic.ttf",
        "/usr/share/fonts/truetype/liberation2/LiberationMono-Regular.ttf",
        "/usr/share/fonts/truetype/liberation2/LiberationMono-Bold.ttf",
        "/usr/share/fonts/truetype/liberation2/LiberationMono-Italic.ttf",

        # Ubuntu
        "/usr/share/fonts/truetype/ubuntu/Ubuntu-R.ttf",
        "/usr/share/fonts/truetype/ubuntu/Ubuntu-B.ttf",
        "/usr/share/fonts/truetype/ubuntu/Ubuntu-L.ttf",
        "/usr/share/fonts/truetype/ubuntu/Ubuntu-M.ttf",
        "/usr/share/fonts/truetype/ubuntu/Ubuntu-RI.ttf",
        "/usr/share/fonts/truetype/ubuntu/Ubuntu-BI.ttf",
        "/usr/share/fonts/truetype/ubuntu/UbuntuMono-R.ttf",
        "/usr/share/fonts/truetype/ubuntu/UbuntuMono-B.ttf",

        # Noto
        "/usr/share/fonts/truetype/noto/NotoSans-Regular.ttf",
        "/usr/share/fonts/truetype/noto/NotoSans-Bold.ttf",
        "/usr/share/fonts/truetype/noto/NotoSans-Italic.ttf",
        "/usr/share/fonts/truetype/noto/NotoSans-Bold.ttf",
        "/usr/share/fonts/truetype/noto/NotoSerif-Regular.ttf",
        "/usr/share/fonts/truetype/noto/NotoSerif-Bold.ttf",
        "/usr/share/fonts/truetype/noto/NotoSerif-Italic.ttf",
        "/usr/share/fonts/truetype/noto/NotoSerif-BoldItalic.ttf",
        "/usr/share/fonts/truetype/noto/NotoMono-Regular.ttf",

        # FreeFont
        "/usr/share/fonts/truetype/freefont/FreeSans.ttf",
        "/usr/share/fonts/truetype/freefont/FreeSansBold.ttf",
        "/usr/share/fonts/truetype/freefont/FreeSansOblique.ttf",
        "/usr/share/fonts/truetype/freefont/FreeSansBoldOblique.ttf",
        "/usr/share/fonts/truetype/freefont/FreeSerif.ttf",
        "/usr/share/fonts/truetype/freefont/FreeSerifBold.ttf",
        "/usr/share/fonts/truetype/freefont/FreeSerifItalic.ttf",
        "/usr/share/fonts/truetype/freefont/FreeMono.ttf",
        "/usr/share/fonts/truetype/freefont/FreeMonoBold.ttf",

        # Lato
        "/usr/share/fonts/truetype/lato/Lato-Regular.ttf",
        "/usr/share/fonts/truetype/lato/Lato-Bold.ttf",
        "/usr/share/fonts/truetype/lato/Lato-Black.ttf",
        "/usr/share/fonts/truetype/lato/Lato-Light.ttf",
        "/usr/share/fonts/truetype/lato/Lato-Light.ttf",
        "/usr/share/fonts/truetype/lato/Lato-Heavy.ttf",
    ]

    fonts = [font for font in fonts if os.path.exists(font)]

    if not fonts:
        raise RuntimeError("Aucune police trouvée.")

    return ImageFont.truetype(
        random.choice(fonts),
        size
    )


def random_weight():
    return random.choices(
        [0, 1],
        weights=[9, 1]
    )[0]


def draw_text_weight(draw, x, y, text, font, weight):
    if weight == 0:
        draw.text(
            (x, y),
            text,
            fill=0,
            font=font
        )
        return

    for dx in range(-weight, weight + 1):
        for dy in range(-weight, weight + 1):

            if dx * dx + dy * dy <= weight * weight:
                draw.text(
                    (x + dx, y + dy),
                    text,
                    fill=0,
                    font=font
                )


def add_local_noise(img):
    pixels = img.load()

    width, height = img.size

    text_pixels = []

    # Recherche les pixels appartenant à la lettre
    for y in range(height):
        for x in range(width):

            if pixels[x, y] < 220:
                text_pixels.append((x, y))

    if not text_pixels:
        return

    # Petits amas de bruit autour de la lettre
    for _ in range(random.randint(2, 6)):

        cx, cy = random.choice(text_pixels)

        for _ in range(random.randint(1, 5)):

            x = cx + random.randint(-3, 3)
            y = cy + random.randint(-3, 3)

            if (
                0 <= x < width
                and
                0 <= y < height
            ):

                r = random.random()

                if r < 0.3:
                    # Tache sombre
                    pixels[x, y] = random.randint(0, 80)

                elif r < 0.6:
                    # Pixel éclairci
                    pixels[x, y] = random.randint(180, 255)


def degrade_resolution(img):
    scale = random.uniform(0.4, 0.9)

    down_method = random.choice([
        Image.Resampling.BILINEAR,
        Image.Resampling.BICUBIC,
        Image.Resampling.LANCZOS,
    ])

    up_method = random.choice([
        Image.Resampling.NEAREST,
        Image.Resampling.BILINEAR,
        Image.Resampling.BICUBIC,
    ])

    width, height = img.size

    small_width = max(
        1,
        int(width * scale)
    )

    small_height = max(
        1,
        int(height * scale)
    )

    small = img.resize(
        (small_width, small_height),
        down_method
    )

    return small.resize(
        (width, height),
        up_method
    )


def draw_letter(draw, cell_x, letter):
    font_size = random.randint(
        MIN_FONT_SIZE,
        MAX_FONT_SIZE
    )

    font = get_random_font(font_size)

    bbox = draw.textbbox(
        (0, 0),
        letter,
        font=font
    )

    letter_width = bbox[2] - bbox[0]
    letter_height = bbox[3] - bbox[1]

    x = (
        cell_x
        + (LETTER_WIDTH - letter_width) / 2
        - bbox[0]
        + random.randint(
            -MAX_X_OFFSET,
            MAX_X_OFFSET
        )
    )

    y = (
        (IMAGE_HEIGHT - letter_height) / 2
        - bbox[1]
        + random.randint(
            -MAX_Y_OFFSET,
            MAX_Y_OFFSET
        )
    )

    draw_text_weight(
        draw,
        x,
        y,
        letter,
        font,
        random_weight()
    )


def draw_ribbon(letters, output_path):
    img = Image.new(
        "L",
        (IMAGE_WIDTH, IMAGE_HEIGHT),
        255
    )

    draw = ImageDraw.Draw(img)

    for i, letter in enumerate(letters):

        cell_x = i * LETTER_WIDTH

        # Dessine directement la lettre majuscule
        draw_letter(
            draw,
            cell_x,
            letter
        )

        # Extrait uniquement cette case
        crop = img.crop(
            (
                cell_x,
                0,
                cell_x + LETTER_WIDTH,
                IMAGE_HEIGHT
            )
        )

        # Bruit local
        if random.random() < 0.3:
            add_local_noise(crop)

        # Dégradation résolution
        if random.random() < 0.3:
            crop = degrade_resolution(crop)

        # Replace la case modifiée
        img.paste(
            crop,
            (cell_x, 0)
        )

    img.save(output_path)


def generate_dataset(count):
    os.makedirs(
        RIBBON_DIR,
        exist_ok=True
    )

    os.makedirs(
        ANSWERS_DIR,
        exist_ok=True
    )

    for i in range(count):

        letters = [
            random.choice(LETTERS)
            for _ in range(LETTER_COUNT)
        ]

        image_path = (
            f"{RIBBON_DIR}/{i}.png"
        )

        answer_path = (
            f"{ANSWERS_DIR}/{i}.txt"
        )

        draw_ribbon(
            letters,
            image_path
        )

        with open(
            answer_path,
            "w"
        ) as file:

            file.write(
                "".join(letters)
            )

        print(
            f"{i + 1}/{count}"
        )


if __name__ == "__main__":
    count = int(
        input(
            "Number of ribbons to generate: "
        )
    )

    generate_dataset(count)
