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
    assert 'preview' not in json.dumps(data).lower(), 'Retired preview data must not be published'
    sections = data['sections']
    groups = [sections['skill'], sections['software'], sections['siteAgent'],
              *[sections['firmware'][v] for v in ('V2', 'V3', 'V4')]]
    for releases in groups:
        seen = set()
        for release in releases:
            assert release['channel'] == 'stable', 'Only formal releases belong in this catalog'
            assert release['version'] and release['files']
            identity = release.get('family'), release['version'], release.get('build_id')
            assert identity not in seen, f'Duplicate release: {identity}'
            seen.add(identity)
            for file in release['files'] + release.get('supportFiles', []):
                assert file['name'] and isinstance(file['bytes'], int) and file['bytes'] > 0
                assert re.fullmatch('[a-fA-F0-9]{64}', file['sha256']), file['name']
                assert file['urls'] and set(file['urls']).issubset({'tencent', 'github', 'gitee'})
                for url in file['urls'].values():
                    parsed = urlsplit(url)
                    assert parsed.scheme == 'https' and parsed.netloc
                    assert not parsed.username and not parsed.password
    for key in ('skill', 'software', 'siteAgent'):
        assert sections[key][0]['version'] == '0.3.6'
    for model, families in {'V2': {'MicroLink': 'V2.9.2'}, 'V3': {'MicroLink': 'V3.6.7'},
                            'V4': {'MicroLink': 'V4.7.1', 'HPMLink': 'V4.7.0'}}.items():
        for family, version in families.items():
            latest = next(r for r in sections['firmware'][model] if r['family'] == family)
            assert latest['version'] == version and latest['build_id']
            assert latest['hardware'] == family + ' ' + model
            for file in latest['files']:
                assert file['hardware'] == latest['hardware']
                assert file['name'].startswith(family + '_' + version + '.')
                assert file['name'].endswith('.rbl' if model == 'V2' else '.uf2')
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
