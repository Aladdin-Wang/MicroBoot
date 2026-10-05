const { test } = require('node:test');
const assert = require('node:assert/strict');
const fs = require('node:fs');
const path = require('node:path');
const vm = require('node:vm');
const source = fs.readFileSync(path.join(__dirname, '../docs/js/search-reset.js'), 'utf8');

function setup({ searchForm = true, hasInput = true, connected = true } = {}) {
    const frames = [], events = [];
    const input = { value: 'RTT', isConnected: connected, dispatchEvent: event => events.push([event.type, input.value]) };
    class HTMLFormElement {
        matches(selector) { assert.equal(selector, '.md-search__form'); return searchForm; }
        querySelector(selector) { assert.equal(selector, '[data-md-component="search-query"]'); return hasInput ? input : null; }
    }
    let listener;
    vm.runInNewContext(source, {
        document: { addEventListener(type, callback) { assert.equal(type, 'reset'); listener = callback; } },
        HTMLFormElement, Event, requestAnimationFrame: callback => frames.push(callback),
    });
    return { input, events, frames, reset: (defaultPrevented = false) => listener({ target: new HTMLFormElement(), defaultPrevented }) };
}

test('notifies Material after native reset changes the field value', () => {
    const state = setup();
    state.reset();
    assert.deepEqual(state.events, []);
    state.input.value = '';
    state.frames.shift()();
    assert.deepEqual(state.events, [['keyup', '']]);
});
test('supports repeated clears including already-empty queries', () => {
    const state = setup();
    state.input.value = '';
    for (let i = 0; i < 2; i++) { state.reset(); state.frames.shift()(); }
    assert.deepEqual(state.events, [['keyup', ''], ['keyup', '']]);
});
test('ignores other forms and missing search inputs', () => {
    for (const options of [{ searchForm: false }, { hasInput: false }]) {
        const state = setup(options); state.reset(); assert.equal(state.frames.length, 0);
    }
});
test('does not update canceled resets', () => {
    const state = setup(); state.reset(true); state.frames.shift()(); assert.deepEqual(state.events, []);
});
test('does not update inputs removed before the next frame', () => {
    const state = setup({ connected: false }); state.reset(); state.frames.shift()(); assert.deepEqual(state.events, []);
});
