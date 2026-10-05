"""Check generated homepage resources, anchors and documentation destinations."""
import argparse
import json
from html.parser import HTMLParser
from pathlib import Path
from urllib.parse import unquote, urlsplit

ROOT = Path(__file__).resolve().parents[1]
OUTPUT = ROOT / "dist"


class Links(HTMLParser):
    def __init__(self):
        super().__init__()
        self.urls = []
        self.ids = set()
        self.i18n = set()

    def handle_starttag(self, tag, attrs):
        attrs = dict(attrs)
        if "id" in attrs:
            if attrs["id"] in self.ids:
                raise ValueError(f"Duplicate id: {attrs['id']}")
            self.ids.add(attrs["id"])
        if "data-i18n" in attrs:
            self.i18n.add(attrs["data-i18n"])
        for attribute in ("src", "href"):
            if attribute in attrs:
                self.urls.append(attrs[attribute])


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--website-only", action="store_true")
    args = parser.parse_args()
    page = Links()
    page.feed((OUTPUT / "index.html").read_text(encoding="utf-8"))
    for url in page.urls:
        parts = urlsplit(url)
        if parts.scheme or parts.netloc:
            continue
        if not parts.path:
            assert not parts.fragment or parts.fragment in page.ids, f"Missing anchor: {url}"
            continue
        if args.website_only and parts.path.startswith("/docs/"):
            continue
        destination = OUTPUT / unquote(parts.path).lstrip("/")
        if parts.path.endswith("/"):
            destination /= "index.html"
        assert destination.is_file(), f"Missing destination: {url}"
    for name in json.loads((ROOT / "website/assets.json").read_text(encoding="utf-8")):
        assert (OUTPUT / "assets" / f"{name}.webp").is_file(), f"Missing asset: {name}"
    if not args.website_only:
        doc = (OUTPUT / "docs/index.html").read_text(encoding="utf-8")
        assert 'https://microkeenai.com/docs/' in doc, "Self-hosted documentation canonical URL missing"
    print(f"PASS: {len(page.urls)} homepage URLs, {len(page.ids)} IDs, all generated images" + (" (MkDocs skipped)" if args.website_only else ", /docs/ destinations and canonical URL"))


if __name__ == "__main__":
    main()
