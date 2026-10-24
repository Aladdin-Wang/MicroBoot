"""Validate and build the download subdomain; never modify update manifests."""
import json
from pathlib import Path
import re
import shutil
from urllib.parse import urlsplit

ROOT = Path(__file__).resolve().parents[1]
SOURCE = ROOT / 'website/downloads'
OUTPUT = ROOT / 'dist-download'
FILES = ('index.html', 'download-center.css', 'download-center.js', 'download-catalog.json')


def validate(data):
    assert data['schemaVersion'] == 1
    sections = data['sections']
    groups = [sections['skill'], sections['software'], *[sections['firmware'][v] for v in ('V2', 'V3', 'V4')]]
    for releases in groups:
        for release in releases:
            assert release['channel'] in ('stable', 'preview')
            assert release['version'] and release['files']
            if release['version'].lstrip('v') == '0.3.6':
                assert release['channel'] == 'preview', '0.3.6 must not become stable'
            for file in release['files']:
                assert file['name'] and isinstance(file['bytes'], int) and file['bytes'] > 0
                assert re.fullmatch('[a-fA-F0-9]{64}', file['sha256']), file['name']
                assert file['urls'] and set(file['urls']).issubset({'tencent', 'github', 'gitee'})
                for url in file['urls'].values():
                    parsed = urlsplit(url)
                    assert parsed.scheme == 'https' and parsed.netloc
    return data


def main():
    validate(json.loads((SOURCE / 'download-catalog.json').read_text()))
    assert OUTPUT.resolve() == OUTPUT, 'Refusing linked output directory'
    OUTPUT.mkdir(exist_ok=True)
    for name in FILES:
        assert (OUTPUT / name).resolve() == OUTPUT / name
        shutil.copy2(SOURCE / name, OUTPUT / name)
    print(f'Download center built: {OUTPUT}; only {", ".join(FILES)} may be deployed')


if __name__ == '__main__':
    main()
