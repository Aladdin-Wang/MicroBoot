"""Generate the website catalog offline from reviewed official metadata snapshots.

This command only writes website/downloads/download-catalog.json. It never writes
an upstream update index, downloads binaries, or changes a release/tag.
"""
import argparse
import copy
import hashlib
import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
METADATA = ROOT / 'website/downloads/metadata'
CATALOG = ROOT / 'website/downloads/download-catalog.json'
RELEASE_ROOT = 'https://download.microkeenai.com/mklink/releases/v0.3.6/'
STABLE_URL = 'https://download.microkeenai.com/mklink/stable/latest.json'
FIRMWARE_ROOT = 'https://download.microkeenai.com/mklink/firmware/'
SOURCE_COMMIT = 'd000370b901a72e4f8ec8fe09cf8b837f2579f0c'
EXPECTED_FIRMWARE = {
    ('microlink', 'V2'): 'V2.9.2', ('microlink', 'V3'): 'V3.6.7',
    ('microlink', 'V4'): 'V4.7.1', ('hpmlink', 'V4'): 'V4.7.0',
}


def load(name):
    return json.loads((METADATA / name).read_text(encoding='utf-8'))


def generate_previous():
    data = copy.deepcopy(load('history.json'))
    stable = load('stable-latest.json')
    manifest = load('v0.3.6-release-manifest.json')
    verified = load('verified-sources.json')
    assert stable['version'] == manifest['release_version'] == '0.3.6'
    assert stable['channel'] == 'stable'
    assert stable['skill']['source_commit'] == manifest['source_commit'] == SOURCE_COMMIT
    checksums = dict(line.split(None, 1)[::-1] for line in
                     (METADATA / 'v0.3.6-SHA256SUMS.txt').read_text().splitlines() if line.strip())
    checksums = {name.strip().lstrip('*'): digest for name, digest in checksums.items()}
    assets = {asset['name']: asset for asset in manifest['assets']}
    for name, asset in assets.items():
        assert checksums[name] == asset['sha256'], f'Checksum disagreement: {name}'
    # Include integrity sidecars with locally calculated digests of the exact fetched bytes.
    for name, snapshot in [('release-manifest.json', 'v0.3.6-release-manifest.json'),
                           ('SHA256SUMS.txt', 'v0.3.6-SHA256SUMS.txt')]:
        raw = (METADATA / snapshot).read_bytes()
        assets[name] = {'name': name, 'size': len(raw), 'sha256': hashlib.sha256(raw).hexdigest()}
    for key, item in [('skill', stable['skill']), ('windows', stable['platforms']['windows-x86_64'])]:
        name = item['url'].rsplit('/', 1)[1]
        assert item['sha256'] == assets[name]['sha256'] and item['size'] == assets[name]['size'], key
        assert item['url'] == RELEASE_ROOT + name
    def file(name, platform):
        asset = assets[name]
        proof = verified['files'][name]
        assert proof['size'] == asset['size'] and proof['sha256'] == asset['sha256'], name
        assert proof['urls']['tencent'] == RELEASE_ROOT + name
        return {'name': name, 'platform': platform, 'bytes': asset['size'],
                'sha256': asset['sha256'], 'urls': proof['urls'],
                'verification': proof['verification']}
    release = {
        'version': stable['version'], 'date': stable['pub_date'], 'channel': 'stable',
        'notes': [line.lstrip('- ').strip() for line in stable['notes'].splitlines() if line.strip()],
        'source': 'https://github.com/MicroKeen/Mklink-AI-Probe/releases/tag/v0.3.6',
        'sourceCommit': SOURCE_COMMIT,
        'dateNote': '发布日期来自正式更新索引，页面按北京时间（UTC+8）显示。',
    }
    skill = dict(release, files=[file('Mklink-AI-Probe-v0.3.6-Skill.zip', '完整 AI Skill')])
    software = dict(release, files=[file('Mklink-AI-Probe-v0.3.6-x64-Setup.exe', 'Windows x64 安装包')])
    site_agent = dict(release, notes=['Windows Site Agent 是独立便携组件，不代替上位机或完整 Skill。解压后按包内说明使用。'], files=[file('MKLink-Site-Agent-v0.3.6-windows-x86_64-portable.zip', 'Windows x64 Site Agent · 便携包')])
    software['supportFiles'] = [file('Mklink-AI-Probe-v0.3.6-x64-Setup.exe.sig', '自动更新签名 · 非安装包'), file('release-manifest.json', '正式资源清单 · 非安装包'), file('SHA256SUMS.txt', 'SHA-256 校验清单 · 非安装包')]
    site_agent['supportFiles'] = [file('MKLink-Site-Agent-v0.3.6-windows-x86_64-portable.manifest.json', 'Site Agent 完整性清单 · 非安装包')]
    data['sections']['skill'].insert(0, skill)
    data['sections']['software'].insert(0, software)
    data['sections']['siteAgent'] = [site_agent]
    seen = set()
    for snapshot in ['firmware-latest-v2.json', 'firmware-latest.json']:
        index = load(snapshot)
        assert index['schema'] == 'mklink-firmware-v1'
        for entry in index['firmwares']:
            key = entry['family'], entry['model']
            assert key not in seen and EXPECTED_FIRMWARE[key] == entry['version']
            seen.add(key)
            proof = verified['files'][entry['name']]
            assert proof['size'] == entry['size'] and proof['sha256'] == entry['sha256']
            assert all(entry['urls'][source] == url for source, url in proof['urls'].items())
            family = {'microlink': 'MicroLink', 'hpmlink': 'HPMLink'}[entry['family']]
            hardware = family + ' ' + entry['model']
            firmware = {
                'version': entry['version'], 'channel': 'stable',
                'date': index['published_at'], 'dateKind': 'official-index',
                'dateNote': '日期为官方固件索引发布时间（北京时间）；build_id 保留官方原值，不推测其时区。',
                'hardware': hardware, 'family': family, 'build_id': entry['build_id'],
                'notes': [f'仅适用于 {hardware}，不可跨硬件型号或 MicroLink / HPMLink 系列混刷。',
                          '官方索引未提供此包独立功能更新说明；同版本修订请同时核对 build_id 和 SHA-256。'],
                'source': FIRMWARE_ROOT + ('latest-v2.json' if entry['model'] == 'V2' else 'latest.json'),
                'files': [{'name': entry['name'], 'platform': entry['name'].rsplit('.', 1)[1].upper(),
                           'hardware': hardware, 'bytes': entry['size'], 'sha256': entry['sha256'],
                           'urls': proof['urls'], 'verification': proof['verification']}],
            }
            if entry['family'] == 'microlink':
                firmware['notes'][1:] = [
                    '修复 Keil 调试时 RTT / SuperWatch 共存问题。',
                    '本轮三份升版二进制已做软件回归与编译校验，未重新刷机；不代表三个型号均完成本轮真机验证。',
                ]
            data['sections']['firmware'][entry['model']].insert(0, firmware)
    assert seen == set(EXPECTED_FIRMWARE)
    # Stable insertion order within V4 keeps ordinary MicroLink first, then HPMLink.
    data['sections']['firmware']['V4'].sort(key=lambda r: r['family'] != 'MicroLink')
    data['generatedAt'] = verified['checkedAt']
    data['sources'] += [STABLE_URL, RELEASE_ROOT + 'release-manifest.json', FIRMWARE_ROOT + 'latest.json', FIRMWARE_ROOT + 'latest-v2.json']
    data['missingResources'] = [
        '0.3.6 正式提供 Windows 上位机、完整 Skill 和 Windows Site Agent。本次没有 Linux / macOS 新包；选择历史正式版本可下载已有包。',
        'Gitee 已核实完整 Skill 及本次四份固件。Windows 安装包超过 Gitee 100 MB 限制，使用腾讯云或 GitHub；其他未核实备用源不生成链接。',
        '历史文件保留原有已核实入口；未提供腾讯云或 Gitee 按钮的历史包尚无已核实镜像。',
        *data['missingResources'],
    ]
    return data


def generate():
    # Keep the complete preceding catalog reproducible; prepend only this release.
    data = generate_previous()
    stable = load('v0.4.0-stable-latest.json')
    manifest = load('v0.4.0-release-manifest.json')
    verified = load('v0.4.0-verified-sources.json')
    version = '0.4.0'
    source_commit = '0c9c2d97d8f69e70f17c67659f5a644fd6379182'
    root = 'https://download.microkeenai.com/mklink/releases/v0.4.0/'
    assert stable['version'] == manifest['release_version'] == version
    assert stable['channel'] == 'stable'
    assert stable['skill']['source_commit'] == manifest['source_commit'] == source_commit
    assert set(stable['platforms']) == {'windows-x86_64'}
    assets = {a['name']: a for a in manifest['assets']}
    sums = dict(line.split(None, 1)[::-1] for line in
                (METADATA / 'v0.4.0-SHA256SUMS.txt').read_text().splitlines() if line.strip())
    for name, asset in assets.items():
        assert sums[name] == asset['sha256']
    for name in ('release-manifest.json', 'SHA256SUMS.txt'):
        raw = (METADATA / ('v0.4.0-' + name)).read_bytes()
        assets[name] = dict(name=name, size=len(raw), sha256=hashlib.sha256(raw).hexdigest())
    for item in (stable['skill'], stable['platforms']['windows-x86_64']):
        name = item['url'].rsplit('/', 1)[1]
        assert item['url'] == root + name
        assert (item['size'], item['sha256']) == (assets[name]['size'], assets[name]['sha256'])
    def file(name, platform):
        asset, proof = assets[name], verified['files'][name]
        assert (proof['size'], proof['sha256']) == (asset['size'], asset['sha256'])
        assert proof['urls']['tencent'] == root + name
        for source, url in proof['urls'].items():
            if source == 'github':
                assert url == 'https://github.com/MicroKeen/Mklink-AI-Probe/releases/download/v0.4.0/' + name
        return dict(name=name, platform=platform, bytes=asset['size'], sha256=asset['sha256'],
                    urls=proof['urls'], verification=proof['verification'])
    release = dict(version=version, date=stable['pub_date'], channel='stable',
                   notes=[line.lstrip('- ').strip() for line in stable['notes'].splitlines() if line.strip()],
                   source='https://github.com/MicroKeen/Mklink-AI-Probe/releases/tag/v0.4.0',
                   sourceCommit=source_commit,
                   dateNote='发布日期来自正式更新索引，页面按北京时间（UTC+8）显示。')
    skill = dict(release, files=[file('Mklink-AI-Probe-v0.4.0-Skill.zip', '完整 AI Skill')])
    software = dict(release, files=[file('Mklink-AI-Probe-v0.4.0-x64-Setup.exe', 'Windows x64 安装包')])
    agent = dict(release, notes=['Windows Site Agent 是独立便携组件，不代替上位机或完整 Skill。解压后按包内说明使用。'],
                 files=[file('MKLink-Site-Agent-v0.4.0-windows-x86_64-portable.zip', 'Windows x64 Site Agent · 便携包')])
    software['supportFiles'] = [file('Mklink-AI-Probe-v0.4.0-x64-Setup.exe.sig', '自动更新签名 · 非安装包'),
                                file('release-manifest.json', '正式资源清单 · 非安装包'),
                                file('SHA256SUMS.txt', 'SHA-256 校验清单 · 非安装包')]
    agent['supportFiles'] = [file('MKLink-Site-Agent-v0.4.0-windows-x86_64-portable.manifest.json', 'Site Agent 完整性清单 · 非安装包')]
    for key, value in [('skill', skill), ('software', software), ('siteAgent', agent)]:
        data['sections'][key].insert(0, value)
    data['generatedAt'] = verified['checkedAt']
    data['sources'].append(root + 'release-manifest.json')
    data['missingResources'][:2] = [
        '0.4.0 正式提供 Windows 上位机、完整 Skill 和 Windows Site Agent。本次没有 Linux / macOS 新包；0.3.6 及更早历史正式文件完整保留。',
        '新版本 GitHub 仅使用 MicroKeen/Mklink-AI-Probe；旧 Aladdin-Wang GitHub 仓库不再维护新版本。Gitee Aladdin-Wang 镜像仍有效，仅显示实际发布且已核实的文件；Windows 安装包超过其 100 MB 限制。',
    ]
    return data


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--check', action='store_true', help='Fail if the checked-in catalog differs; write nothing')
    args = parser.parse_args()
    rendered = json.dumps(generate(), ensure_ascii=False, indent=2) + '\n'
    if args.check:
        assert CATALOG.read_text(encoding='utf-8') == rendered, 'Catalog differs: run scripts/sync_download_catalog.py'
        print('Download catalog matches reviewed metadata snapshots')
    else:
        CATALOG.write_text(rendered, encoding='utf-8')
        print(f'Generated website-only catalog: {CATALOG}')


if __name__ == '__main__':
    main()
