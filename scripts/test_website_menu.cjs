const { test } = require('node:test');
const assert = require('node:assert/strict');
const fs = require('node:fs');
const path = require('node:path');
const vm = require('node:vm');

const app = fs.readFileSync(path.join(__dirname, '../website/app.js'), 'utf8');
const css = fs.readFileSync(path.join(__dirname, '../website/styles.css'), 'utf8');
const menuCode = app.slice(app.indexOf('function closeMenu()'), app.indexOf('const dialog ='));

function setup() {
  let open = false, focused = false, expanded = 'false', keydown, resize;
  const listeners = {};
  const menu = {
    setAttribute(name, value) { assert.equal(name, 'aria-expanded'); expanded = value; },
    getAttribute(name) { assert.equal(name, 'aria-expanded'); return expanded; },
    focus() { focused = true; },
    addEventListener(type, callback) { listeners[`menu:${type}`] = callback; },
  };
  const links = {
    classList: {
      remove(name) { assert.equal(name, 'open'); open = false; },
      toggle(name) { assert.equal(name, 'open'); return open = !open; },
    },
    addEventListener(type, callback) { listeners[`links:${type}`] = callback; },
  };
  vm.runInNewContext(menuCode, {
    $: selector => selector === '#menu' ? menu : links,
    document: { addEventListener(type, callback) { assert.equal(type, 'keydown'); keydown = callback; } },
    window: { matchMedia(query) {
      assert.equal(query, '(min-width: 821px)');
      return { addEventListener(type, callback) { assert.equal(type, 'change'); resize = callback; } };
    } },
  });
  return {
    state: () => ({ open, expanded, focused }),
    click: () => listeners['menu:click'](),
    followLink: isLink => listeners['links:click']({ target: { closest: () => isLink } }),
    key: key => keydown({ key }),
    resize: matches => resize({ matches }),
  };
}

test('menu can be repeatedly opened and closed', () => {
  const menu = setup();
  for (let count = 0; count < 2; count++) {
    menu.click(); assert.deepEqual(menu.state(), { open: true, expanded: 'true', focused: false });
    menu.click(); assert.deepEqual(menu.state(), { open: false, expanded: 'false', focused: false });
  }
});

test('Escape closes the open menu and restores button focus', () => {
  const menu = setup(); menu.click(); menu.key('Escape');
  assert.deepEqual(menu.state(), { open: false, expanded: 'false', focused: true });
});

test('only following a navigation link closes the menu', () => {
  const menu = setup(); menu.click(); menu.followLink(false);
  assert.equal(menu.state().open, true);
  menu.followLink(true);
  assert.equal(menu.state().open, false);
  assert.equal(menu.state().expanded, 'false');
});

test('returning to desktop resets the mobile menu state', () => {
  const menu = setup(); menu.click(); menu.resize(false);
  assert.equal(menu.state().open, true);
  menu.resize(true);
  assert.equal(menu.state().open, false);
  assert.equal(menu.state().expanded, 'false');
});

test('tablet CSS and menu reset use adjacent breakpoints', () => {
  assert.match(css, /@media\(max-width:820px\)/);
  assert.match(app, /min-width: 821px/);
});
