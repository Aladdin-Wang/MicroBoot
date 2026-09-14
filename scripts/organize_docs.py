"""Apply the reviewed 2026-09-10 path map once, preserving a full local backup."""
import argparse
import hashlib
import json
import os
from pathlib import Path
import posixpath
import re
from urllib.parse import unquote, urlsplit, urlunsplit
from zipfile import ZipFile, ZIP_DEFLATED
import yaml

ROOT = Path(__file__).resolve().parents[1]
REPORT = ROOT / 'maintenance/docs-organization'


def safe_path(relative):
    path = ROOT / relative
    resolved = path.resolve()
    if not resolved.is_relative_to(ROOT) or resolved == ROOT:
        raise ValueError(f'Path escapes project: {relative}')
    return path


def rewrite_markdown(source, old_page, new_page, mapping):
    docs_map = {old[5:]: new[5:] for old, new in mapping.items() if old.startswith('docs/') and new.startswith('docs/')}
    docs_map['tools/microlink/先辑使用教程.md'] = 'mklink/hpm/overview.md'
    known = {p.relative_to(ROOT / 'docs').as_posix() for p in (ROOT / 'docs').rglob('*') if p.is_file()}
    old_dir = posixpath.dirname(old_page.removeprefix('docs/'))
    new_dir = posixpath.dirname(new_page.removeprefix('docs/'))

    def url(value):
        parts = urlsplit(value)
        if parts.scheme or parts.netloc:
            prefixes = ('https://microboot.readthedocs.io/zh-cn/latest/', 'https://microkeenai.com/docs/')
            prefix = next((p for p in prefixes if value.startswith(p)), None)
            if not prefix:
                return value
            relative = unquote(parts.path[len(urlsplit(prefix).path):])
            candidate = relative.rstrip('/') + '.md' if relative.endswith('/') else relative
            target = docs_map.get(candidate)
            if not target:
                return value
            if target.endswith('.md'):
                target = target[:-3] + '/'
                if target.endswith('/index/'):
                    target = target[:-6]
            return prefix + target + ('?' + parts.query if parts.query else '') + ('#' + parts.fragment if parts.fragment else '')
        if not parts.path:
            return value
        if parts.path.startswith('/'):
            return value
        resolved = posixpath.normpath(posixpath.join(old_dir, unquote(parts.path)))
        if resolved not in docs_map and resolved not in known:
            return value
        target = docs_map.get(resolved, resolved)
        relative = posixpath.relpath(target, new_dir or '.')
        return urlunsplit(('', '', relative, parts.query, parts.fragment))

    def markdown_destination(match):
        original = match.group(1)
        converted = url(original)
        if converted == original:
            # Support angle-wrapped destinations and optional quoted titles.
            target = re.fullmatch(r'(<[^>]+>|\S+)(\s+[\"\'].*[\"\'])?', original)
            if target:
                raw = target.group(1)
                angled = raw.startswith('<')
                result = url(raw[1:-1] if angled else raw)
                converted = ('<' + result + '>' if angled else result) + (target.group(2) or '')
        return '](' + converted + ')'

    def prose(text):
        text = re.sub(r'\]\(([^\n)]*)\)', markdown_destination, text)
        text = re.sub(r'(?P<prefix>\b(?:src|href)=[\"\'])(?P<url>[^\"\']+)(?P<end>[\"\'])', lambda m: m['prefix'] + url(m['url']) + m['end'], text)
        text = re.sub(r'(?m)^(\s*\[[^\]]+\]:\s*)(\S+)', lambda m: m[1] + url(m[2]), text)
        # Plain links to our own published docs also occur outside markdown links.
        text = re.sub(r'https://(?:microboot\.readthedocs\.io/zh-cn/latest|microkeenai\.com/docs)/[^\s<>）)\"\']+', lambda m: url(m[0]), text)
        return text

    result, buffer, fence = [], [], None
    for line in source.splitlines(keepends=True):
        marker = re.match(r'^\s*(`{3,}|~{3,})', line)
        if fence:
            result.append(line)
            if marker and marker[1][0] == fence[0] and len(marker[1]) >= len(fence):
                fence = None
        elif marker:
            result.append(prose(''.join(buffer))); buffer = []
            result.append(line); fence = marker[1]
        else:
            buffer.append(line)
    result.append(prose(''.join(buffer)))
    return ''.join(result)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--apply', action='store_true')
    args = parser.parse_args()
    mapping = json.loads((REPORT / 'path-map.json').read_text(encoding='utf-8'))
    original_pages = {p.relative_to(ROOT).as_posix(): p.read_text(encoding='utf-8-sig') for p in (ROOT / 'docs').rglob('*.md')}
    for old, new in mapping.items():
        src, dst = safe_path(old), safe_path(new)
        if not src.is_file():
            raise SystemExit(f'Source missing; migration may already be applied: {old}')
        if dst.exists():
            raise SystemExit(f'Destination already exists: {new}')
    if len({str(safe_path(p)).casefold() for p in mapping.values()}) != len(mapping):
        raise SystemExit('Case-insensitive destination collision')
    updated_pages = {mapping.get(old, old): rewrite_markdown(text, old, mapping.get(old, old), mapping) for old, text in original_pages.items()}
    if not args.apply:
        print(f'Preflight OK: {len(mapping)} moves and {len(updated_pages)} document rewrites. Use --apply to proceed.')
        return
    backup = REPORT / 'before-2026-09-10.zip'
    if backup.exists():
        raise SystemExit('Backup already exists; refusing to reapply migration')
    with ZipFile(backup, 'x', ZIP_DEFLATED) as archive:
        for path in sorted((ROOT / 'docs').rglob('*')):
            if path.is_file(): archive.write(path, path.relative_to(ROOT).as_posix())
        archive.write(ROOT / 'mkdocs.yml', 'mkdocs.yml')
    hashes = {new: hashlib.sha256(safe_path(old).read_bytes()).hexdigest() for old, new in mapping.items() if not old.endswith('.md')}
    for old, new in mapping.items():
        dst = safe_path(new)
        dst.parent.mkdir(parents=True, exist_ok=True)
        safe_path(old).rename(dst)
    for path, content in updated_pages.items():
        safe_path(path).write_text(content, encoding='utf-8')
    config_path = ROOT / 'mkdocs.yml'
    config_text = config_path.read_text(encoding='utf-8-sig')
    for old, new in sorted(mapping.items(), key=lambda pair: len(pair[0]), reverse=True):
        if old.endswith('.md') and old.startswith('docs/'):
            config_text = config_text.replace(old[5:], new[5:])
    redirects = {old[5:]:new[5:] for old,new in mapping.items() if old.endswith('.md') and new.startswith('docs/')}
    redirects['tools/microlink/先辑使用教程.md'] = 'mklink/hpm/overview.md'
    redirect_yaml = yaml.safe_dump({'redirect_maps':redirects}, allow_unicode=True, sort_keys=False)
    plugin_text = '  - redirects:\n' + ''.join('      ' + line + '\n' for line in redirect_yaml.splitlines())
    config_text = config_text.replace('  - awesome-pages\n', '  - awesome-pages\n' + plugin_text)
    config_text += '\n# Preserve previously published image URLs without duplicate source images.\nhooks:\n  - scripts/docs_asset_aliases.py\n'
    config_path.write_text(config_text, encoding='utf-8')
    # Only remove empty directories, after checking their resolved project boundary.
    for path in sorted((ROOT / 'docs').rglob('*'), key=lambda p: len(p.parts), reverse=True):
        if path.is_dir() and not any(path.iterdir()):
            safe_path(path.relative_to(ROOT)).rmdir()
    for new, expected in hashes.items():
        assert hashlib.sha256(safe_path(new).read_bytes()).hexdigest() == expected, new
    (REPORT / 'binary-hashes.json').write_text(json.dumps(hashes, ensure_ascii=False, indent=2), encoding='utf-8')
    print(f'Migration applied: {len(mapping)} moves; {len(hashes)} binary/source assets preserved byte-for-byte; backup: {backup}')


if __name__ == '__main__':
    main()
