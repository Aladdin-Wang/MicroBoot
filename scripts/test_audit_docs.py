"""Regression tests for root-hosted and /docs/-hosted link audits."""
import unittest

from audit_docs import local_target


class LocalTargetTests(unittest.TestCase):
    def test_root_hosted_links(self):
        self.assertEqual(local_target('404.html', '/.'), 'index.html')
        self.assertEqual(local_target('404.html', '/mklink/overview/'), 'mklink/overview/index.html')
        self.assertEqual(local_target('404.html', '/assets/main.css'), 'assets/main.css')

    def test_docs_mount_absolute_links(self):
        self.assertEqual(local_target('404.html', '/docs/.', '/docs/'), 'index.html')
        self.assertEqual(local_target('404.html', '/docs/', '/docs/'), 'index.html')
        self.assertEqual(local_target('404.html', '/docs', '/docs/'), 'index.html')
        self.assertEqual(local_target('404.html', '/docs/mklink/overview/', '/docs/'), 'mklink/overview/index.html')
        self.assertEqual(local_target('404.html', '/docs/assets/main.css', '/docs/'), 'assets/main.css')

    def test_relative_links_are_independent_of_mount(self):
        for prefix in ('/', '/docs/'):
            self.assertEqual(local_target('mklink/overview/index.html', '../../images/a.png', prefix), 'images/a.png')
            self.assertEqual(local_target('mklink/overview/index.html', '', prefix), 'mklink/overview/index.html')
            self.assertEqual(local_target('mklink/overview/index.html', '../support/', prefix), 'mklink/support/index.html')

    def test_encoded_paths(self):
        self.assertEqual(local_target('404.html', '/docs/images/a%20b.png', '/docs/'), 'images/a b.png')

    def test_outside_mount_is_not_silently_accepted(self):
        for path in ('/docs-other/index.html', '/assets/main.css', '/docs/../assets/main.css'):
            self.assertIsNone(local_target('404.html', path, '/docs/'))
        self.assertIsNone(local_target('index.html', '../private.txt'))


if __name__ == '__main__':
    unittest.main()
