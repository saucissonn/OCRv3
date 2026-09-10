import os
import random
from PIL import Image, ImageDraw, ImageFont

IMAGE_HEIGHT = 28
DIGIT_WIDTH = 28
DIGIT_COUNT = 100
IMAGE_WIDTH = DIGIT_WIDTH * DIGIT_COUNT

RIBBON_DIR = "ribbon"
ANSWERS_DIR = "ribbon_ans"

MIN_FONT_SIZE = 16
MAX_FONT_SIZE = 24
MAX_X_OFFSET = 1
MAX_Y_OFFSET = 1

def get_random_font(size):
    fonts = [
        # DejaVu
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",

        "/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf",
        "/usr/share/fonts/truetype/dejavu/DejaVuSerif.ttf",
        "/usr/share/fonts/truetype/dejavu/DejaVuSerif-Bold.ttf",
        "/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf",

        # Liberation
        "/usr/share/fonts/truetype/liberation2/LiberationSans-Regular.ttf",
        "/usr/share/fonts/truetype/liberation2/LiberationSans-Bold.ttf",
        "/usr/share/fonts/truetype/liberation2/LiberationSerif-Regular.ttf",
        "/usr/share/fonts/truetype/liberation2/LiberationSerif-Bold.ttf",
        "/usr/share/fonts/truetype/liberation2/LiberationMono-Regular.ttf",
        "/usr/share/fonts/truetype/liberation2/LiberationMono-Bold.ttf",

        # Ubuntu
        "/usr/share/fonts/truetype/ubuntu/Ubuntu-R.ttf",
        "/usr/share/fonts/truetype/ubuntu/Ubuntu-B.ttf",

        # Noto
        "/usr/share/fonts/truetype/noto/NotoSans-Regular.ttf",
        "/usr/share/fonts/truetype/noto/NotoSans-Bold.ttf",
        "/usr/share/fonts/truetype/noto/NotoSerif-Regular.ttf",
        "/usr/share/fonts/truetype/noto/NotoSerif-Bold.ttf",

        # FreeFont
        "/usr/share/fonts/truetype/freefont/FreeSans.ttf",
        "/usr/share/fonts/truetype/freefont/FreeSansBold.ttf",
        "/usr/share/fonts/truetype/freefont/FreeSerif.ttf",
        "/usr/share/fonts/truetype/freefont/FreeSerifBold.ttf",
        "/usr/share/fonts/truetype/freefont/FreeMono.ttf",
        "/usr/share/fonts/truetype/freefont/FreeMonoBold.ttf",

    ]

    fonts = [f for f in fonts if os.path.exists(f)]

    if not fonts:
        raise RuntimeError("Aucune police trouvée.")

    return ImageFont.truetype(random.choice(fonts), size)

def random_weight():
    #return 0;
    return random.choices([0,1], weights=[9,1])[0]

def draw_text_weight(draw, x, y, text, font, w):
    if w == 0:
        draw.text((x,y), text, fill=0, font=font)
        return
    for dx in range(-w, w+1):
        for dy in range(-w, w+1):
            if dx*dx+dy*dy <= w*w:
                draw.text((x+dx,y+dy), text, fill=0, font=font)

def add_local_noise(img):
    pixels = img.load()
    w, h = img.size

    # Récupère les pixels appartenant au chiffre
    text_pixels = []

    for y in range(h):
        for x in range(w):
            if pixels[x, y] < 220:  # fait partie du caractère
                text_pixels.append((x, y))

    if not text_pixels:
        return

    # Quelques petits amas de bruit autour du texte
    for _ in range(random.randint(2, 6)):
        cx, cy = random.choice(text_pixels)

        for _ in range(random.randint(1, 5)):
            x = cx + random.randint(-3, 3)
            y = cy + random.randint(-3, 3)

            if 0 <= x < w and 0 <= y < h:
                if random.random() < 0.3:
                    pixels[x, y] = random.randint(0, 80)      # tache noire
                elif random.random() < 0.3:
                    pixels[x, y] = random.randint(180, 255)   # éclaircissement

def draw_noise(draw, cell_x):
    for _ in range(random.randint(30,80)):
        draw.point((cell_x+random.randint(0,DIGIT_WIDTH-1),
                    random.randint(0,IMAGE_HEIGHT-1)),
                    fill=random.randint(0,180))

def draw_ribbon(digits, out):
    img = Image.new("L", (IMAGE_WIDTH, IMAGE_HEIGHT), 255)
    draw = ImageDraw.Draw(img)

    for i, d in enumerate(digits):
        cell_x = i * DIGIT_WIDTH

        if d == 0:
            shown = str(random.randint(1, 9))
            angle = random.choice([90, 180, 270])
        else:
            shown = str(d)
            angle = 0

        font = get_random_font(random.randint(MIN_FONT_SIZE, MAX_FONT_SIZE))

        if angle == 0:
            bbox = draw.textbbox((0, 0), shown, font=font)
            x = (
                cell_x
                + (DIGIT_WIDTH - (bbox[2] - bbox[0])) / 2
                + random.randint(-MAX_X_OFFSET, MAX_X_OFFSET)
            )
            y = (
                (IMAGE_HEIGHT - (bbox[3] - bbox[1])) / 2
                - bbox[1]
                + random.randint(-MAX_Y_OFFSET, MAX_Y_OFFSET)
            )

            draw_text_weight(draw, x, y, shown, font, random_weight())

        else:
            tmp = Image.new("L", (64, 64), 255)
            td = ImageDraw.Draw(tmp)

            bbox = td.textbbox((0, 0), shown, font=font)
            x = (64 - (bbox[2] - bbox[0])) / 2
            y = (64 - (bbox[3] - bbox[1])) / 2 - bbox[1]

            draw_text_weight(td, x, y, shown, font, random_weight())

            tmp = tmp.rotate(angle, expand=True, fillcolor=255)

            l = (tmp.width - DIGIT_WIDTH) // 2
            t = (tmp.height - IMAGE_HEIGHT) // 2
            crop = tmp.crop((l, t, l + DIGIT_WIDTH, t + IMAGE_HEIGHT))
            img.paste(crop, (cell_x, 0))

            if angle == 180:
                if shown == "6":
                    digits[i] = 9
                elif shown == "9":
                    digits[i] = 6
                elif shown == "8":
                    digits[i] = 8
                elif shown == "1":
                    digits[i] = 1

        # Toutes les augmentations se font uniquement sur ce chiffre
        crop = img.crop((cell_x, 0, cell_x + DIGIT_WIDTH, IMAGE_HEIGHT))

        # Bruit local
        if random.random() < 0.3:
            add_local_noise(crop)

        # Dégradation par downscale/upscale
        if random.random() < 0.3:
            scale = random.uniform(0.4, 0.9)

            down = random.choice([
                Image.Resampling.BILINEAR,
                Image.Resampling.BICUBIC,
                Image.Resampling.LANCZOS,
            ])

            up = random.choice([
                Image.Resampling.NEAREST,
                Image.Resampling.BILINEAR,
                Image.Resampling.BICUBIC,
            ])

            w, h = crop.size
            small = crop.resize(
                (max(1, int(w * scale)), max(1, int(h * scale))),
                down,
            )
            crop = small.resize((w, h), up)

        # Remet le chiffre modifié dans le ruban
        img.paste(crop, (cell_x, 0))

    img.save(out)

def generate_dataset(n):
    os.makedirs(RIBBON_DIR,exist_ok=True)
    os.makedirs(ANSWERS_DIR,exist_ok=True)
    for i in range(n):
        digits=[random.randint(0,9) for _ in range(DIGIT_COUNT)]
        draw_ribbon(digits,f"{RIBBON_DIR}/{i}.png")
        with open(f"{ANSWERS_DIR}/{i}.txt","w") as f:
            f.write("".join(map(str,digits)))
        print(f"{i+1}/{n}")

if __name__=="__main__":
    generate_dataset(int(input("Number of ribbons to generate: ")))
