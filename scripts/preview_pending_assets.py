"""Make a temporary contact sheet for read-only review; originals stay intact."""
import json
from pathlib import Path
import tempfile
from PIL import Image, ImageDraw, ImageFont, ImageOps

ROOT = Path(__file__).resolve().parents[1]
if __name__ == '__main__':
    assets = json.loads((ROOT / 'maintenance/docs-organization/pending-review.json').read_text(encoding='utf-8'))
    width, height = 360, 235
    canvas = Image.new('RGB', (width * 4, height * ((len(assets) + 3) // 4)), 'white')
    draw = ImageDraw.Draw(canvas)
    font = ImageFont.truetype('C:/Windows/Fonts/arial.ttf', 16)
    for i, item in enumerate(assets):
        x, y = (i % 4) * width, (i // 4) * height
        with Image.open(ROOT / item['new']) as image:
            thumbnail = ImageOps.contain(image.convert('RGB'), (width - 20, height - 45))
            canvas.paste(thumbnail, (x + (width - thumbnail.width) // 2, y + 5))
        draw.text((x + 10, y + height - 35), f'{i + 1:02} {Path(item["new"]).name}', font=font, fill='black')
    output = Path(tempfile.gettempdir()) / 'microkeen-pending-assets.jpg'
    canvas.save(output, quality=92)
    print(output)
