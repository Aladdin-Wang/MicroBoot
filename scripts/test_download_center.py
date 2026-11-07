import copy
import json
from pathlib import Path
import unittest
from build_download_center import validate
from sync_download_catalog import generate, generate_previous, METADATA

ROOT = Path(__file__).resolve().parents[1]

class DownloadCatalogTests(unittest.TestCase):
    def setUp(self):
        self.data = json.loads((ROOT / 'website/downloads/download-catalog.json').read_text())
    def test_catalog(self):
        validate(self.data)
    def test_generated_catalog_matches_snapshots(self):
        self.assertEqual(self.data, generate())
    def test_preview_rejected(self):
        self.data['sections']['software'][0]['channel'] = 'preview'
        with self.assertRaises(AssertionError): validate(self.data)
    def test_preview_url_rejected(self):
        self.data['sections']['software'][0]['files'][0]['urls']['tencent'] = 'https://download.microkeenai.com/mklink/preview/latest.json'
        with self.assertRaises(AssertionError): validate(self.data)
    def test_non_https_rejected(self):
        self.data['sections']['software'][0]['files'][0]['urls']['github'] = 'javascript:alert(1)'
        with self.assertRaises(AssertionError): validate(self.data)
    def test_missing_hash_rejected(self):
        self.data['sections']['software'][0]['files'][0]['sha256'] = ''
        with self.assertRaises(AssertionError): validate(self.data)
    def test_firmware_hardware_mismatch_rejected(self):
        self.data['sections']['firmware']['V4'][0]['files'][0]['hardware'] = 'HPMLink V4'
        with self.assertRaises(AssertionError): validate(self.data)
    def test_build_id_required_for_latest(self):
        del self.data['sections']['firmware']['V2'][0]['build_id']
        with self.assertRaises((AssertionError, KeyError)): validate(self.data)
    def test_historical_releases_preserved_unchanged(self):
        history = json.loads((METADATA / 'history.json').read_text())
        for key in ('skill', 'software'):
            self.assertEqual(self.data['sections'][key][2:], history['sections'][key])
        for model, original in history['sections']['firmware'].items():
            for release in original:
                self.assertIn(release, self.data['sections']['firmware'][model])
    def test_new_software_windows_only_and_gitee_skill_only(self):
        for key in ('software', 'siteAgent'):
            for file in self.data['sections'][key][0]['files']:
                self.assertIn('Windows', file['platform'])
                self.assertNotIn('gitee', file['urls'])
        self.assertIn('gitee', self.data['sections']['skill'][0]['files'][0]['urls'])
    def test_seven_formal_resources(self):
        files = []
        for key in ('skill', 'software', 'siteAgent'):
            release = self.data['sections'][key][0]
            files += release['files'] + release.get('supportFiles', [])
        self.assertEqual(len(files), 7)
        self.assertEqual(len({f['name'] for f in files}), 7)
    def test_no_preview_ui(self):
        for name in ('index.html', 'download-center.js'):
            content = (ROOT / 'website/downloads' / name).read_text()
            self.assertNotIn('preview', content)
            self.assertNotIn('尚未正式发布', content)

    def test_previous_firmware_releases_preserved(self):
        for model, version in [('V2', 'V2.9.1'), ('V3', 'V3.6.6'), ('V4', 'V4.7.0')]:
            releases = [r for r in self.data['sections']['firmware'][model] if r['family'] == 'MicroLink']
            self.assertEqual(releases[1]['version'], version)
            self.assertEqual(releases[1]['files'][0]['urls']['tencent'].split('/')[-2], '20261024')
            self.assertIn('未重新刷机', ' '.join(releases[0]['notes']))
            self.assertEqual(releases[0]['files'][0]['urls']['tencent'].split('/')[-2], '20261025')

    def test_entire_previous_catalog_sections_preserved(self):
        previous = generate_previous()
        for key in ('skill', 'software', 'siteAgent'):
            self.assertEqual(self.data['sections'][key][1:], previous['sections'][key])
        self.assertEqual(self.data['sections']['firmware'], previous['sections']['firmware'])
        self.assertEqual(self.data['sections']['software'][0]['sourceCommit'],
                         '0c9c2d97d8f69e70f17c67659f5a644fd6379182')
