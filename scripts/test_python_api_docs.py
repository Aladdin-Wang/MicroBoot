"""Offline contract checks for the compact device API and MUX reference."""
import ast
from html.parser import HTMLParser
from pathlib import Path
import re
import struct
import unittest

import markdown

ROOT = Path(__file__).resolve().parents[1]
API = ROOT / 'docs/mklink/development/python-api.md'
MUX = ROOT / 'docs/mklink/development/mux-protocol.md'
LEGACY = ROOT / 'docs/mklink/legacy/dump-memory-protocol.md'


class Ids(HTMLParser):
    def __init__(self):
        super().__init__()
        self.ids = []

    def handle_starttag(self, tag, attrs):
        attrs = dict(attrs)
        if 'id' in attrs:
            self.ids.append(attrs['id'])


class PythonApiDocsTests(unittest.TestCase):
    def test_published_api_anchors_survive_reorganization(self):
        original_ids = {
            'python-api', 'pcai', '1cmd-api', '11-ram', '12-ram', '13-flash',
            '14-flash', '15-flash', '16-cpu-reg', '17-dump', '18-flush', '19',
            '110', '111-vrefv4', '112-vccv3v4', '113-v3v4', '114-v4',
            '2-load-api', '21', '22-bin', '23-hex', '3-segger-rtt-api', '31-rtt',
            '32-rtt', '4-segger-systemview-api', '41-systemview', '4-vofa-api',
            '41-vofa', '5-apiv4', '51-uart', '52-rs485',
        }
        parser = Ids()
        parser.feed(markdown.markdown(API.read_text(), extensions=['toc', 'tables', 'fenced_code']))
        self.assertTrue(original_ids <= set(parser.ids), original_ids - set(parser.ids))
        self.assertEqual(len(parser.ids), len(set(parser.ids)))

    def test_python_examples_are_syntax_valid_without_running_hardware(self):
        snippets = re.findall(r'```python\n(.*?)\n```', API.read_text(), flags=re.S)
        self.assertGreaterEqual(len(snippets), 4)
        for snippet in snippets:
            ast.parse(snippet)

    def test_mux_header_offsets_and_widths_match_struct(self):
        source = MUX.read_text()
        self.assertIn("'<4sBBHII'", source)
        fields = [('4s', 'magic'), ('B', 'version'), ('B', 'opcode'),
                  ('H', 'payload_length'), ('I', 'epoch'), ('I', 'request_id')]
        prefix = '<'
        for fmt, name in fields:
            offset = struct.calcsize(prefix)
            width = struct.calcsize('<' + fmt)
            self.assertRegex(source, rf'\| {offset} \| {width} \| {name}')
            prefix += fmt
        self.assertEqual(struct.calcsize(prefix), 16)
        self.assertIn('| 16+N | 4 | CRC32（header + payload），小端 |', source)
        self.assertIn('只有能力 bit4 开启时，0x41 Watch 事件可使用 version=2', source)
        self.assertIn('其余命令、响应、RTT 和 Bulk 仍使用 version=1 + CRC', source)

    def test_current_sampling_contract_is_gated_and_not_legacy_fallback(self):
        source = MUX.read_text()
        self.assertEqual((1 << 3) | (1 << 4) | (1 << 9) | (1 << 10), 0x618)
        for text in ['0x618', '开发版', '0x33', '0x34', '0x35', '0x42', 'fastest=True, period=0']:
            self.assertIn(text, source)
        self.assertEqual(struct.calcsize('<IIQQIIHHHBB'), 40)
        self.assertEqual(16 + 40 + 452 + 4, 512)
        self.assertIn('`<IIQQIIHHHBB`', source)
        self.assertIn('最多 452B', source)
        self.assertIn('不是原子快照', source)
        self.assertIn('不再发送', API.read_text())
        self.assertIn('仅供维护旧固件和解析历史', LEGACY.read_text())
        self.assertIn('不发送旧 `cmd.dump_memory` 命令', LEGACY.read_text())

    def test_sources_and_firmware_verification_limit_are_visible(self):
        source = API.read_text()
        self.assertIn('v0.4.2', source)
        self.assertIn('0.4.3 main', source)
        self.assertIn('186b6a0ffa2408d48b8c558eb01782fae84e96bc', source)
        self.assertIn('未取得最新固件 `.pyi` / C 源码', source)
        self.assertIn('不是读取 R0/PC 等 CPU 核心寄存器', source)


if __name__ == '__main__':
    unittest.main()
