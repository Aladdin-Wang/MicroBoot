"""Homepage and dynamic case destinations must resolve, including deep links."""
from pathlib import Path
from tempfile import TemporaryDirectory
import unittest

from check_website import Links, check_urls


class WebsiteLinksTests(unittest.TestCase):
    def setUp(self):
        self.directory = TemporaryDirectory()
        self.addCleanup(self.directory.cleanup)
        self.root = Path(self.directory.name)
        self.page = Links()
        self.page.feed('<main id="main"></main><a href="#main">Start</a>')
        target = self.root / 'docs/guide/index.html'
        target.parent.mkdir(parents=True)
        target.write_text('<h2 id="capture">Capture</h2>', encoding='utf-8')

    def test_document_anchors_and_case_links(self):
        check_urls(self.root, self.page, ['/docs/guide/#capture'])

    def test_missing_document_anchor_is_rejected(self):
        with self.assertRaisesRegex(AssertionError, 'Missing destination anchor'):
            check_urls(self.root, self.page, ['/docs/guide/#missing'])

    def test_missing_dynamic_case_page_is_rejected(self):
        with self.assertRaisesRegex(AssertionError, 'Missing destination'):
            check_urls(self.root, self.page, ['/docs/missing/'])

    def test_website_only_skips_unbuilt_docs_but_checks_homepage(self):
        check_urls(self.root, self.page, ['/docs/missing/#not-built'], website_only=True)
        with self.assertRaisesRegex(AssertionError, 'Missing anchor'):
            check_urls(self.root, self.page, ['#missing'], website_only=True)

    def test_external_links_are_outside_local_check(self):
        check_urls(self.root, self.page, ['https://example.com/guide/#capture'])


if __name__ == '__main__':
    unittest.main()
