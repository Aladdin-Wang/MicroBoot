"""Build a static homepage plus MkDocs for the existing Nginx server."""
import argparse
import json
from pathlib import Path
import shutil
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[1]
OUTPUT = ROOT / "dist"


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--website-only", action="store_true", help="Skip MkDocs (documentation links require a previous full build)")
    args = parser.parse_args()
    # Generated output must stay within this checkout, even if a directory is linked.
    if OUTPUT.resolve() != OUTPUT or (OUTPUT / "assets").resolve() != OUTPUT / "assets" or (OUTPUT / "docs").resolve() != OUTPUT / "docs":
        raise SystemExit("Refusing to write through a linked output directory")
    from PIL import Image, ImageOps
    OUTPUT.mkdir(exist_ok=True)
    (OUTPUT / "assets").mkdir(exist_ok=True)
    for name in ("index.html", "styles.css", "app.js", "content.js", "favicon.svg", "robots.txt", "sitemap.xml", "home-sitemap.xml"):
        shutil.copy2(ROOT / "website" / name, OUTPUT / name)
    assets = json.loads((ROOT / "website/assets.json").read_text(encoding="utf-8"))
    for name, relative in assets.items():
        source = ROOT / relative
        with Image.open(source) as original:
            optimized = ImageOps.exif_transpose(original).convert("RGB")
            # Keep source pixels: no invented detail, retouching or upscaling.
            optimized.save(OUTPUT / "assets" / f"{name}.webp", "WEBP", quality=90, lossless=(name.startswith("case-") or name == "sine-wave"), method=6)
    print(f"Website built: {OUTPUT}", flush=True)
    if not args.website_only:
        subprocess.run([sys.executable, "-m", "mkdocs", "build", "--strict", "--config-file", "mkdocs.website.yml"], cwd=ROOT, check=True)
    subprocess.run([sys.executable, str(ROOT / "scripts/check_website.py"), *(["--website-only"] if args.website_only else [])], check=True)


if __name__ == "__main__":
    main()
