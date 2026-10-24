import copy
import json
from pathlib import Path
import unittest
from build_download_center import validate
from sync_download_catalog import generate, METADATA

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
            self.assertEqual(self.data['sections'][key][1:], history['sections'][key])
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
