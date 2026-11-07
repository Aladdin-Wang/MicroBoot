"""Check generated homepage resources, anchors and documentation destinations."""
import argparse
import json
import re
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


def check_urls(output, page, extra_urls=(), website_only=False):
    """Validate static and case-switcher links, including destination anchors."""
    for url in [*page.urls, *extra_urls]:
        parts = urlsplit(url)
        if parts.scheme or parts.netloc:
            continue
        if not parts.path:
            assert not parts.fragment or parts.fragment in page.ids, f"Missing anchor: {url}"
            continue
        if website_only and parts.path.startswith("/docs/"):
            continue
        destination = output / unquote(parts.path).lstrip("/")
        if parts.path.endswith("/"):
            destination /= "index.html"
        assert destination.is_file(), f"Missing destination: {url}"
        if parts.fragment:
            target = Links()
            target.feed(destination.read_text(encoding="utf-8"))
            assert unquote(parts.fragment) in target.ids, f"Missing destination anchor: {url}"


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--website-only", action="store_true")
    args = parser.parse_args()
    page = Links()
    page.feed((OUTPUT / "index.html").read_text(encoding="utf-8"))
    # Case-switcher destinations are literal paths in the content module.
    case_urls = re.findall(r"guide\s*:\s*['\"]([^'\"]+)['\"]", (OUTPUT / "content.js").read_text(encoding="utf-8"))
    check_urls(OUTPUT, page, case_urls, args.website_only)
    for name in json.loads((ROOT / "website/assets.json").read_text(encoding="utf-8")):
        assert (OUTPUT / "assets" / f"{name}.webp").is_file(), f"Missing asset: {name}"
    if not args.website_only:
        doc = (OUTPUT / "docs/index.html").read_text(encoding="utf-8")
        assert 'https://microkeenai.com/docs/' in doc, "Self-hosted documentation canonical URL missing"
    print(f"PASS: {len(page.urls)} homepage URLs, {len(case_urls)} case-guide URLs, {len(page.ids)} IDs, all generated images" + (" (MkDocs skipped)" if args.website_only else ", /docs/ destinations, anchors and canonical URL"))


if __name__ == "__main__":
    main()
