"""Audit documentation migration, generate a browsable source-asset catalogue."""
import argparse
from collections import defaultdict
import csv
import hashlib
from html import escape
from html.parser import HTMLParser
import json
from pathlib import Path
import posixpath
import re
from urllib.parse import unquote, urlsplit, quote
from zipfile import ZipFile
import markdown
from PIL import Image
import yaml

ROOT = Path(__file__).resolve().parents[1]
REPORT = ROOT / 'maintenance/docs-organization'


class Links(HTMLParser):
    def __init__(self):
        super().__init__()
        self.links, self.images, self.ids = [], [], set()

    def handle_starttag(self, tag, attrs):
        attrs = dict(attrs)
        if 'id' in attrs: self.ids.add(attrs['id'])
        if tag == 'a' and 'name' in attrs: self.ids.add(attrs['name'])
        for key in ('src', 'href'):
            if key in attrs:
                self.links.append(attrs[key])
        if tag == 'img' and 'src' in attrs: self.images.append(attrs['src'])


def parse(text):
    parser = Links()
    parser.feed(text)
    return parser


def html_path(path):
    return path[:-3] + '/index.html' if not path.endswith('index.md') else path[:-3] + '.html'


def fenced_blocks(text):
    blocks, current, fence = [], [], None
    for line in text.splitlines():
        marker = re.match(r'^\s*(`{3,}|~{3,})', line)
        if fence:
            current.append(line)
            if marker and marker[1][0] == fence[0] and len(marker[1]) >= len(fence):
                blocks.append('\n'.join(current)); current = []; fence = None
        elif marker:
            fence = marker[1]; current = [line]
    return blocks


def main():
    args_parser = argparse.ArgumentParser(description=__doc__)
    args_parser.add_argument('--migration', action='store_true', help='Compare original code blocks with local pre-migration backup')
    args_parser.add_argument('--site-dir', default='site')
    args = args_parser.parse_args()
    site = ROOT / args.site_dir
    assert (site / 'index.html').is_file(), 'Build MkDocs first'
    mapping = json.loads((REPORT / 'path-map.json').read_text(encoding='utf-8'))
    inverse = {new:old for old,new in mapping.items()}
    errors, warnings = [], []
    hashes = json.loads((REPORT / 'binary-hashes.json').read_text(encoding='utf-8'))
    for path, expected in hashes.items():
        if not (ROOT / path).is_file() or hashlib.sha256((ROOT / path).read_bytes()).hexdigest() != expected:
            errors.append('Asset changed or missing: ' + path)
    config = yaml.safe_load((ROOT / 'mkdocs.yml').read_text(encoding='utf-8'))
    redirects = next(p['redirects']['redirect_maps'] for p in config['plugins'] if isinstance(p, dict) and 'redirects' in p)
    for old, new in redirects.items():
        path = site / html_path(old)
        if not path.is_file() or 'location.href=' not in path.read_text(encoding='utf-8'):
            errors.append('Missing redirect: ' + old)
    for old, new in mapping.items():
        if old.startswith('docs/images/') and new.startswith('docs/images/'):
            alias = site / old[5:]
            if not alias.is_file() or hashlib.sha256(alias.read_bytes()).hexdigest() != hashlib.sha256((ROOT / new).read_bytes()).hexdigest():
                errors.append('Missing/changed legacy image alias: ' + old)
    built = {p.relative_to(site).as_posix():parse(p.read_text(encoding='utf-8')) for p in site.rglob('*.html')}
    all_files = {p.relative_to(site).as_posix() for p in site.rglob('*') if p.is_file()}
    checked = 0
    for page, parser in built.items():
        for link in parser.links:
            parts = urlsplit(link)
            if parts.scheme or parts.netloc: continue
            target = posixpath.normpath(posixpath.join(posixpath.dirname(page), unquote(parts.path))) if parts.path else page
            if parts.path.startswith('/'): target = unquote(parts.path).lstrip('/')
            if parts.path.endswith('/') or target == '.': target = posixpath.join(target, 'index.html').removeprefix('./')
            if target not in all_files:
                errors.append(f'Broken built link: {page} -> {link}')
                continue
            checked += 1
            if parts.fragment and target in built and unquote(parts.fragment) not in built[target].ids:
                warnings.append(f'Unknown anchor: {page} -> {link}')
    if args.migration:
        with ZipFile(REPORT / 'before-2026-09-10.zip') as archive:
            for path in archive.namelist():
                if path.endswith('.md'):
                    current = ROOT / mapping.get(path, path)
                    if fenced_blocks(archive.read(path).decode('utf-8-sig')) != fenced_blocks(current.read_text(encoding='utf-8-sig')):
                        errors.append('Code block changed: ' + str(current))
    references = defaultdict(set)
    for page in (ROOT / 'docs').rglob('*.md'):
        text = page.read_text(encoding='utf-8')
        rendered = markdown.markdown(text, extensions=['pymdownx.superfences','tables','admonition'])
        for url in parse(rendered).images:
            parts = urlsplit(url)
            if parts.scheme or parts.netloc: continue
            resolved = (page.parent / unquote(parts.path)).resolve()
            if resolved.is_relative_to(ROOT): references[resolved.relative_to(ROOT).as_posix()].add(page.relative_to(ROOT).as_posix())
    assets = []
    for path in sorted((ROOT / 'docs/images').rglob('*')):
        if not path.is_file(): continue
        relative = path.relative_to(ROOT).as_posix()
        width, height = '', ''
        if path.suffix.lower() in ['.png','.jpg','.jpeg','.gif','.webp','.ico']:
            with Image.open(path) as im: width, height = im.size
        assets.append({'path':relative,'original':inverse.get(relative, relative),'width':width,'height':height,'bytes':path.stat().st_size,'sha256':hashlib.sha256(path.read_bytes()).hexdigest(),'references':'; '.join(sorted(references[relative]))})
    duplicate_groups = defaultdict(list)
    for asset in assets: duplicate_groups[asset['sha256']].append(asset['path'])
    duplicates = [paths for paths in duplicate_groups.values() if len(paths) > 1]
    with (REPORT / 'assets.csv').open('w', encoding='utf-8-sig', newline='') as handle:
        writer = csv.DictWriter(handle, fieldnames=list(assets[0])); writer.writeheader(); writer.writerows(assets)
    with (REPORT / 'path-map.csv').open('w', encoding='utf-8-sig', newline='') as handle:
        writer = csv.writer(handle); writer.writerow(['旧路径','新路径']); writer.writerows(mapping.items())
    (REPORT / 'duplicates.json').write_text(json.dumps(duplicates, ensure_ascii=False, indent=2), encoding='utf-8')
    cards = []
    for item in assets:
        url = '../../' + quote(item['path'])
        picture = f'<img loading="lazy" src="{url}" alt="{escape(Path(item["path"]).name)}">' if item['width'] else '<span class="source">可编辑源文件</span>'
        pending = 'pending-review/' in item['path']
        refs = item['references'] or '当前文档未引用（保留）'
        cards.append(f'<article data-pending="{str(pending).lower()}"><a target="_blank" href="{url}">{picture}</a><h2>{escape(Path(item["path"]).name)}</h2><p>{escape(item["path"])}</p><p>原名：{escape(item["original"])}</p><p>{item["width"]} × {item["height"]} · {round(item["bytes"]/1024)} KB</p><p class="refs">{escape(refs)}</p></article>')
    gallery = '''<!doctype html><html lang="zh-CN"><meta charset="utf-8"><meta name="viewport" content="width=device-width,initial-scale=1"><title>MicroKeen 素材索引</title><style>body{font:15px/1.6 system-ui,sans-serif;margin:32px;background:#f5f5f7;color:#1d1d1f}h1{font-size:30px}header{max-width:960px}input{font:inherit;padding:12px;min-width:250px;max-width:90%;border:1px solid #ccc;border-radius:8px}button{font:inherit;padding:12px;margin:8px;border:1px solid #ccc;border-radius:8px;background:white;cursor:pointer}button[aria-pressed=true]{background:#0066cc;color:white}main{display:grid;grid-template-columns:repeat(auto-fill,minmax(280px,1fr));gap:20px;margin-top:24px}article{background:white;padding:16px;border-radius:12px;overflow-wrap:anywhere}article[hidden]{display:none}article>a{display:flex;align-items:center;justify-content:center;height:190px;background:#fafafa}img{max-width:100%;max-height:190px;object-fit:contain}h2{font-size:16px}p{font-size:12px;color:#666}.refs{color:#333}.source{color:#777}header p{font-size:15px}</style><header><h1>MicroKeen 素材索引</h1><p>点击图片查看原图。保留原始内容；“待确认”表示还需要补充用途说明。</p><input id="search" aria-label="搜索素材" placeholder="搜索文件名、分类或引用文档"><button id="pending" aria-pressed="false">只看待确认</button><span id="count" aria-live="polite"></span></header><main>''' + ''.join(cards) + '''</main><script>const search=document.querySelector('#search'),button=document.querySelector('#pending'),cards=[...document.querySelectorAll('article')];function filter(){const q=search.value.toLowerCase(),pending=button.getAttribute('aria-pressed')==='true';let n=0;for(const card of cards){card.hidden=!(card.textContent.toLowerCase().includes(q)&&(!pending||card.dataset.pending==='true'));if(!card.hidden)n++}document.querySelector('#count').textContent=n+' 项'}search.addEventListener('input',filter);button.addEventListener('click',()=>{button.setAttribute('aria-pressed',button.getAttribute('aria-pressed')==='true'?'false':'true');filter()});filter();</script></html>'''
    (REPORT / 'assets.html').write_text(gallery, encoding='utf-8')
    summary = {'moved_files':len(mapping),'original_documents':71,'drafts':len(list((ROOT/'docs/drafts').rglob('*.md')))-1,'source_assets':len(assets),'binary_hashes_checked':len(hashes),'document_redirects':len(redirects),'local_links_checked':checked,'unreferenced_assets':sum(not a['references'] for a in assets),'identical_asset_groups':len(duplicates),'pending_assets':sum('pending-review/' in a['path'] for a in assets),'errors':sorted(set(errors)),'anchor_warnings':sorted(set(warnings))}
    (REPORT / 'audit.json').write_text(json.dumps(summary, ensure_ascii=False, indent=2), encoding='utf-8')
    print(json.dumps(summary, ensure_ascii=False, indent=2))
    if errors: raise SystemExit(1)


if __name__ == '__main__':
    main()
