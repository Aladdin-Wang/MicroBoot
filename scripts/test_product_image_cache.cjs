const { test } = require('node:test');
const assert = require('node:assert/strict');
const fs = require('node:fs');
const path = require('node:path');
const read = name => fs.readFileSync(path.join(__dirname, '../website', name), 'utf8');
const version = '?v=com-cmd-20261114';

test('homepage refreshes corrected V4 image and its model-switch script', () => {
  const html = read('index.html');
  assert.equal(html.split(`mklink-v4.webp${version}`).length - 1, 3);
  assert.ok(html.includes(`/app.js${version}`));
  assert.ok(read('app.js').includes(`model === 'v4' ? '${version}' : ''`));
});

test('download center refreshes both corrected product images and its script', () => {
  const script = read('downloads/download-center.js');
  for (const image of ['mklink-v4.webp', 'hpmlink-v4-20261025.webp']) {
    assert.ok(script.includes(`https://microkeenai.com/assets/${image}${version}`));
  }
  assert.ok(read('downloads/index.html').includes(`/download-center.js${version}`));
});
