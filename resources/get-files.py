import os, zipfile, urllib.request, shutil

URL = "https://github.com/FortAwesome/Font-Awesome/archive/refs/heads/6.x.zip"
ZIP_FILE = "fa.zip"
EXTRACT_DIR = "Font-Awesome-6.x"
TARGET_DIR = r"C:\Users\stupi\Desktop\Projects\Image-Editor\resources\icons"

ICON_MAP = {
    "open.svg": "svgs/solid/folder-open.svg",
    "save.svg": "svgs/solid/floppy-disk.svg",
    "undo.svg": "svgs/solid/rotate-left.svg",
    "redo.svg": "svgs/solid/rotate-right.svg",
    "rotate_right.svg": "svgs/solid/rotate.svg",
    "flip.svg": "svgs/solid/arrows-left-right.svg",
    "crop.svg": "svgs/solid/crop.svg",
    "brush.svg": "svgs/solid/paintbrush.svg",
    "eraser.svg": "svgs/solid/eraser.svg",
    "text.svg": "svgs/solid/font.svg",
    "brightness.svg": "svgs/solid/sun.svg",
    "contrast.svg": "svgs/solid/adjust.svg",
    "zoom_in.svg": "svgs/solid/magnifying-glass-plus.svg",
    "zoom_out.svg": "svgs/solid/magnifying-glass-minus.svg",
    "app_icon.svg": "svgs/solid/image.svg"
}

if not os.path.exists(ZIP_FILE):
    print("Downloading...")
    urllib.request.urlretrieve(URL, ZIP_FILE)

if not os.path.exists(EXTRACT_DIR):
    print("Extracting...")
    with zipfile.ZipFile(ZIP_FILE, 'r') as z:
        z.extractall(".")

os.makedirs(TARGET_DIR, exist_ok=True)

for target, rel in ICON_MAP.items():
    src = os.path.join(EXTRACT_DIR, rel)
    dst = os.path.join(TARGET_DIR, target)
    if os.path.exists(src):
        shutil.copy(src, dst)
        print("Copied", target)
    else:
        print("MISSING:", rel)
