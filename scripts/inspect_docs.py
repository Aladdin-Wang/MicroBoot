"""Inventory source documents and image references without changing their content."""
import hashlib
import json
from pathlib import Path
import re

ROOT = Path(__file__).resolve().parents[1]
DOCS = ROOT / 'docs'

if __name__ == '__main__':
    pages = {p.relative_to(DOCS).as_posix(): p.read_text(encoding='utf-8-sig') for p in DOCS.rglob('*.md')}
    images = []
    for path in sorted((DOCS / 'images').rglob('*')):
        if not path.is_file():
            continue
        relative = path.relative_to(DOCS).as_posix()
        refs = []
        for page, source in pages.items():
            for line in source.splitlines():
                if path.name in line:
                    refs.append({'page': page, 'line': line.strip()[:250]})
        images.append({'path': relative, 'bytes': path.stat().st_size, 'sha256': hashlib.sha256(path.read_bytes()).hexdigest(), 'references': refs})
    output = ROOT / 'maintenance/docs-organization'
    output.mkdir(parents=True, exist_ok=True)
    inventory_path = output / ('inventory-current.json' if (output / 'inventory-before.json').exists() else 'inventory-before.json')
    inventory_path.write_text(json.dumps({'documents': [{'path': path, 'title': next((line.strip('# ') for line in text.splitlines() if line.startswith('#')), ''), 'bytes': len(text.encode('utf-8'))} for path, text in pages.items()], 'images': images}, ensure_ascii=False, indent=2), encoding='utf-8')
    for image in images:
        print(image['path'] + ' | ' + ', '.join(dict.fromkeys(ref['page'] for ref in image['references'])))
    print(f'{len(pages)} documents; {len(images)} image/source assets')
