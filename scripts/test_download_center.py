import copy
import json
from pathlib import Path
import unittest
from build_download_center import validate

class DownloadCatalogTests(unittest.TestCase):
    def setUp(self):
        self.data = json.loads((Path(__file__).resolve().parents[1] / 'website/downloads/download-catalog.json').read_text())
    def test_catalog(self):
        validate(self.data)
    def test_preview_cannot_be_stable(self):
        release = copy.deepcopy(self.data['sections']['software'][0])
        release.update(version='0.3.6', channel='stable')
        self.data['sections']['software'] = [release]
        with self.assertRaises(AssertionError): validate(self.data)
    def test_non_https_rejected(self):
        file = self.data['sections']['software'][0]['files'][0]
        file['urls']['github'] = 'javascript:alert(1)'
        with self.assertRaises(AssertionError): validate(self.data)
    def test_missing_hash_rejected(self):
        self.data['sections']['software'][0]['files'][0]['sha256'] = ''
        with self.assertRaises(AssertionError): validate(self.data)
