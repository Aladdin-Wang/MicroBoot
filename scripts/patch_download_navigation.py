"""Patch only deployed download navigation, preserving unrelated online content.

Dry run by default. Explicit --apply requires a backup directory outside the webroot.
This does not touch download service configuration or any automatic update files.
"""
import argparse
from datetime import datetime, timezone
import hashlib
import json
from pathlib import Path
import re
import shutil
from urllib.parse import urljoin, urlsplit

TARGET = 'https://download.microkeenai.com/'
ANCHOR = re.compile(r'<a\b[^>]*>.*?</a>', re.S | re.I)
HREF = re.compile(r'\bhref\s*=\s*([\'"])(.*?)\1', re.S | re.I)


def patch(text, page_url, homepage=False):
    count = 0
    def anchor(match):
        nonlocal count
        html = match.group(0)
        tag = html[:html.index('>') + 1]
        nav = re.search(r'\bclass\s*=\s*[\'"][^\'"]*\bmd-(?:nav|tabs)__link\b', tag)
        home = homepage and re.search(r'\bdata-i18n\s*=\s*[\'"](?:downloads|getDownloads)[\'"]', tag)
        label = re.sub(r'<[^>]*>', '', html[html.index('>') + 1:]).strip()
        if nav and label not in ('资料下载', '下载', '下载中心'):
            return html
        if not (nav or home):
            return html
        href = HREF.search(tag)
        if not href:
            return html
        resolved = urlsplit(urljoin(page_url, href.group(2)))
        if href.group(2) == TARGET:
            return html
        if resolved.hostname not in ('microkeenai.com', 'www.microkeenai.com') or not resolved.path.rstrip('/').endswith('/mklink/support/downloads'):
            return html
        count += 1
        return html[:href.start(2)] + TARGET + html[href.end(2):]
    text = ANCHOR.sub(anchor, text)
    # Material renders a mobile TOC label before the active-page anchor. Once
    # that anchor points off-site, remove only its redundant intercepting label.
    # Keep the real anchor, TOC input and all TOC markup untouched.
    if urlsplit(page_url).path.rstrip('/').endswith('/mklink/support/downloads/index.html'):
        pattern = re.compile(r'(<label\b[^>]*\bfor=[\'"]__toc[\'"][^>]*>.*?</label>)(\s*)(<a\b[^>]*>.*?</a>)', re.S | re.I)
        def active_label(match):
            nonlocal count
            label = re.sub(r'<[^>]*>', '', match.group(1)).strip()
            href = HREF.search(match.group(3).split('>', 1)[0])
            if label in ('资料下载', '下载', '下载中心') and href and href.group(2) == TARGET:
                count += 1
                return match.group(2) + match.group(3)
            return match.group(0)
        text = pattern.sub(active_label, text)
    return text, count


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--root', type=Path, required=True)
    parser.add_argument('--backup-dir', type=Path)
    parser.add_argument('--apply', action='store_true')
    args = parser.parse_args()
    root = args.root.resolve()
    assert (root / 'index.html').is_file() and (root / 'docs/index.html').is_file(), 'Unexpected website layout'
    changes = []
    files = [root / 'index.html', *sorted((root / 'docs').rglob('*.html'))]
    for path in files:
        assert not path.is_symlink(), f'Refusing symlink: {path}'
        relative = path.relative_to(root)
        page = 'https://microkeenai.com/' + str(relative)
        old = path.read_text(encoding='utf-8')
        new, count = patch(old, page, relative == Path('index.html'))
        if count:
            changes.append((path, old, new, count))
    report = {'files': len(changes), 'anchors': sum(x[3] for x in changes), 'applied': False}
    if args.apply:
        assert args.backup_dir, '--backup-dir is required for apply'
        backup = args.backup_dir.resolve()
        assert backup != root and root not in backup.parents, 'Backup must be outside public root'
        assert changes, 'No matching old navigation found; inspect before retry'
        backup = backup / ('download-nav-' + datetime.now(timezone.utc).strftime('%Y%m%dT%H%M%S%fZ'))
        backup.mkdir(parents=True, exist_ok=False, mode=0o700)
        manifest = []
        for path, old, new, count in changes:
            relative = path.relative_to(root)
            saved = backup / relative
            saved.parent.mkdir(parents=True, exist_ok=True)
            shutil.copy2(path, saved)
            manifest.append({'path': str(relative), 'beforeSha256': hashlib.sha256(path.read_bytes()).hexdigest(), 'anchors': count})
        (backup/'manifest.json').write_text(json.dumps(manifest, ensure_ascii=False, indent=2))
        for path, old, new, count in changes:
            assert path.read_text(encoding='utf-8') == old, f'File changed during backup: {path}'
        for path, old, new, count in changes:
            temporary = path.with_name(path.name+'.download-nav.tmp')
            assert not temporary.exists(), f'Temporary path already exists: {temporary}'
            temporary.write_text(new, encoding='utf-8')
            shutil.copymode(path, temporary)
            temporary.replace(path)
        report.update(applied=True, backup=str(backup))
    print(json.dumps(report, ensure_ascii=False))


if __name__ == '__main__':
    main()
