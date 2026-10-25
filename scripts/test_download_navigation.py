import unittest
from patch_download_navigation import patch, TARGET

class NavigationPatchTests(unittest.TestCase):
    def test_docs_nav_only(self):
        old = '<a href="../support/downloads/" class="md-nav__link">资料下载</a><a href="../support/downloads/">正文</a>'
        new,count = patch(old,'https://microkeenai.com/docs/mklink/overview/index.html')
        self.assertEqual(count,1)
        self.assertIn(TARGET,new)
        self.assertIn('<a href="../support/downloads/">正文</a>',new)
    def test_home_targets_and_idempotence(self):
        old = '<a href="/docs/mklink/support/downloads/" data-i18n="downloads">下载</a>'
        new,count = patch(old,'https://microkeenai.com/',True)
        self.assertEqual(count,1)
        self.assertEqual(patch(new,'https://microkeenai.com/',True),(new,0))
    def test_external_same_path_untouched(self):
        old = '<a href="https://example.com/docs/mklink/support/downloads/" class="md-nav__link">下载</a>'
        self.assertEqual(patch(old,'https://microkeenai.com/docs/'),(old,0))

    def test_active_legacy_mobile_label_only(self):
        label = '<label class="md-nav__link md-nav__link--active" for="__toc"><span>资料下载</span><span class="md-icon"></span></label>'
        link = '<a href="'+TARGET+'" class="md-nav__link md-nav__link--active">资料下载</a>'
        toc = '<nav class="md-nav--secondary"><label for="__toc">目录</label></nav>'
        old = label + link + toc
        url = 'https://microkeenai.com/docs/mklink/support/downloads/index.html'
        new,count = patch(old,url)
        self.assertEqual((new,count),(link+toc,1))
        self.assertEqual(patch(new,url),(new,0))
        self.assertEqual(patch(old,'https://microkeenai.com/docs/other/index.html'),(old,0))
