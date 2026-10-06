const { test } = require('node:test');
const assert = require('node:assert/strict');
const fs = require('node:fs');
const path = require('node:path');
const vm = require('node:vm');

const root = path.join(__dirname, '..');
const html = fs.readFileSync(path.join(root, 'website/index.html'), 'utf8');
const source = fs.readFileSync(path.join(root, 'website/content.js'), 'utf8');
// The content module is data-only. Evaluate it without requiring a package install.
const { english, labels, models, steps } = vm.runInNewContext(
  source.replace(/export const /g, 'const ') + '\n({english, labels, models, steps})'
);

test('every homepage translation has nonempty English copy', () => {
  const keys = [...html.matchAll(/data-i18n="([^"]+)"/g)].map(match => match[1]);
  assert.ok(keys.length > 90, 'The test must cover the full homepage');
  for (const key of keys) assert.ok(typeof english[key] === 'string' && english[key].trim(), key);
});

test('accessible labels are present in both languages', () => {
  for (const [, key] of html.matchAll(/data-label="([^"]+)"/g)) {
    for (const language of ['zh', 'en']) assert.ok(labels[language][key], `${language}: ${key}`);
  }
});

test('all hardware models and recorded cases remain bilingual', () => {
  assert.deepEqual(Object.keys(models), ['v4', 'v3', 'v2']);
  assert.equal(steps.length, 4);
  for (const item of Object.values(models)) {
    for (const language of ['zh', 'en']) {
      assert.ok(item[language].title && item[language].description);
      assert.equal(item[language].features.length, 3);
    }
  }
  for (const item of steps) {
    assert.match(item.guide, /^\/docs\/.+\/$/);
    for (const language of ['zh', 'en']) {
      assert.ok(item[language].title && item[language].description && item[language].alt);
      assert.ok(item.prompt[language]);
    }
  }
});

test('new shared workflows retain visible paired-development version conditions', () => {
  assert.match(html, /<aside class="version-note"/);
  for (const copy of [html, english.versionNote]) {
    assert.match(copy, /V4/);
    assert.match(copy, /0\.3\.0/);
    assert.match(copy, /Skill/);
  }
  assert.match(english.versionNote, /development version/);
  assert.match(english.versionNote, /official release page/);
  assert.match(html, /公开下载以正式发布页为准/);
});

test('CDC diagram has a text caption and does not imply a hardware capture', () => {
  assert.match(html, /<figure class="shared-map" aria-labelledby="shared-map-caption">/);
  assert.match(html, /<figcaption id="shared-map-caption"/);
  assert.match(english.mapCaption, /diagram, not a hardware screenshot/);
  assert.match(english.mapCaption, /without adding WinUSB/);
  assert.match(english.mapCaption, /USB serial number/);
});

test('sampling and remote GUI limitations are translated', () => {
  assert.match(english.samplingNote, /does not promise hard real-time/);
  assert.match(english.samplingNote, /Zero transport drops does not mean zero missed samples/);
  assert.match(english.sharedRemote, /remote flashing and host management are unavailable/);
  assert.match(english.sharedRemote, /does not establish cross-machine acceptance/);
});

test('historical performance values and case assets are preserved', () => {
  assert.match(html, /20\.17<span>万次\/秒/);
  assert.match(english.sampleRate, /201\.7/);
  assert.match(html, /632\.49<span>KiB\/s/);
  assert.match(english.metric2, /HPM6E80/);
  assert.match(english.measurementNote, /not new benchmarks of the paired version/);
  assert.deepEqual(Array.from(steps, item => item.image), ['case-flash', 'case-watch', 'case-rtt', 'case-offline']);
});

test('task entry points, install guide and legacy documentation remain reachable', () => {
  for (const destination of [
    '/docs/mklink/development/v4-shared-cdc/#dap-restart',
    '/docs/mklink/development/v4-shared-cdc/#hpm-flashing',
    '/docs/mklink/getting-started/gui-install/',
    '/docs/mklink/getting-started/project-config/',
    '/docs/mklink/observation/rtt/',
    '/docs/mklink/observation/superwatch/',
    'https://microboot.readthedocs.io/zh-cn/latest/',
  ]) assert.ok(html.includes(destination), destination);
});
