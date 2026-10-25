// Dependency-free DOM-behavior tests. This deliberately does NOT claim CSS/layout
// or real-browser coverage. Run: node --test scripts/test_download_center.cjs
const { test } = require('node:test');
const assert = require('node:assert/strict');
const fs = require('node:fs');
const path = require('node:path');
const vm = require('node:vm');
const root = path.resolve(__dirname, '../website/downloads');
const script = fs.readFileSync(path.join(root, 'download-center.js'), 'utf8');
const html = fs.readFileSync(path.join(root, 'index.html'), 'utf8');
const catalog = JSON.parse(fs.readFileSync(path.join(root, 'download-catalog.json'), 'utf8'));

class Element {
  constructor(tag, owner) {
    this.tagName = tag.toLowerCase(); this.owner = owner; this.children = [];
    this.attributes = new Map(); this.listeners = new Map(); this.className = '';
    this._text = ''; this._value = undefined; this.parentElement = null;
    this.classList = {
      contains: name => this.className.split(/\s+/).includes(name),
      remove: name => { this.className = this.className.split(/\s+/).filter(x => x !== name).join(' '); },
      add: name => { if (!this.classList.contains(name)) this.className = `${this.className} ${name}`.trim(); },
      toggle: name => { const added = !this.classList.contains(name); this.classList[added ? 'add' : 'remove'](name); return added; },
    };
  }
  set textContent(value) { this._text = String(value); this.children = []; }
  get textContent() { return this._text + this.children.map(x => x.textContent).join(''); }
  set id(value) { this.setAttribute('id', value); }
  get id() { return this.getAttribute('id') || ''; }
  set value(value) { this._value = String(value); }
  get value() { return this._value ?? (this.tagName === 'select' ? this.children[0]?.value ?? '' : ''); }
  append(...children) { for (const child of children) { child.parentElement = this; this.children.push(child); } }
  appendChild(child) { this.append(child); return child; }
  replaceChildren(...children) { this.children = []; this._text = ''; this.append(...children); }
  setAttribute(name, value) { this.attributes.set(name, String(value)); if (name === 'id') this.owner.ids.set(String(value), this); }
  getAttribute(name) { return this.attributes.get(name) ?? null; }
  addEventListener(type, callback) { if (!this.listeners.has(type)) this.listeners.set(type, []); this.listeners.get(type).push(callback); }
  dispatch(type, data = {}) { for (const callback of this.listeners.get(type) || []) callback({target:this, ...data}); }
  closest(selector) { for (let el = this; el; el = el.parentElement) if (el.tagName === selector) return el; return null; }
  focus() { this.owner.activeElement = this; }
}
function descendants(root, predicate) {
  return root.children.flatMap(child => [...(predicate(child) ? [child] : []), ...descendants(child, predicate)]);
}
function tags(root, tag) { return descendants(root, child => child.tagName === tag); }
function byClass(root, className) { return descendants(root, child => child.classList.contains(className)); }
async function setup({data = catalog, responseOK = true, fetchError = false} = {}) {
  const document = {
    ids: new Map(), listeners: new Map(), activeElement: null,
    createElement(tag) { return new Element(tag, this); },
    querySelector(selector) { assert.match(selector, /^#[\w-]+$/); return this.ids.get(selector.slice(1)) || null; },
    addEventListener: Element.prototype.addEventListener,
    dispatch: Element.prototype.dispatch,
  };
  // Parse the id-bearing HTML nodes only; rendered descendants come from the app.
  for (const match of html.matchAll(/<([\w-]+)\b([^>]*\bid="([^"]+)"[^>]*)>/g)) {
    const el = document.createElement(match[1]);
    for (const attribute of match[2].matchAll(/([\w-]+)="([^"]*)"/g)) {
      el.setAttribute(attribute[1], attribute[2]);
      if (attribute[1] === 'class') el.className = attribute[2];
    }
  }
  const fetched = [];
  await vm.runInNewContext(`(async () => { ${script}\n })()`, {
    document, URL, Intl, Date, console,
    fetch: async url => { fetched.push(url); if (fetchError) throw Error('offline'); return {ok:responseOK, json:async () => structuredClone(data)}; },
  }, {timeout: 2000});
  return {document, fetched, get: id => document.ids.get(id)};
}
function expectedGroups() {
  const stable = releases => (releases || []).filter(r => r.channel === 'stable');
  const groups = [
    {id:'skill-releases', releases:stable(catalog.sections.skill)},
    {id:'software-releases', releases:stable(catalog.sections.software)},
    {id:'site-agent-releases', releases:stable(catalog.sections.siteAgent)},
  ];
  for (const hardware of ['V2','V3','V4']) {
    const releases = stable(catalog.sections.firmware[hardware]);
    const families = [...new Set(releases.map(r => r.family).filter(Boolean))];
    for (const family of families.length > 1 ? families : [null]) groups.push({id:`firmware-${hardware}`, family, releases:family ? releases.filter(r=>r.family===family) : releases});
  }
  return groups;
}
function groupsOf(target) { return byClass(target, 'release-group'); }
function filesOf(group) { return byClass(group, 'filename').filter(el => { for (let p = el.parentElement; p; p = p.parentElement) if (p.classList.contains('support-files')) return false; return true; }).map(el => el.textContent); }

// Compare every rendered release and every actual URL, not just labels. The same
// display version may occur in multiple hardware/family/build releases.
test('all selectors default to newest formal release and switch every historical option', async () => {
  const {get, fetched} = await setup();
  assert.equal(fetched.length, 1);
  assert.match(fetched[0], /^\/download-catalog\.json\?v=/);
  assert.match(get('load-status').textContent, /资源目录核对日期/);
  const used = new Map();
  let optionsTested = 0;
  for (const expected of expectedGroups()) {
    assert.ok(expected.releases.length, `missing formal releases for ${expected.id}`);
    const index = used.get(expected.id) || 0; used.set(expected.id, index + 1);
    const group = groupsOf(get(expected.id))[index];
    assert.ok(group, `missing release group ${expected.id}/${expected.family}`);
    const select = tags(group,'select')[0];
    assert.equal(select.children.length, expected.releases.length);
    assert.equal(select.value, '0');
    assert.ok(tags(group,'label')[0].htmlFor === select.id && select.id);
    for (let i = 0; i < expected.releases.length; i++) {
      const release = expected.releases[i];
      select.value = i; select.dispatch('change'); optionsTested++;
      assert.equal(byClass(group,'release-card').length,1,'switch replaces instead of appending');
      assert.match(tags(group,'h4')[0].textContent, new RegExp(release.version.replace(/[.*+?^${}()|[\]\\]/g,'\\$&')));
      for (const file of release.files) {
        assert.ok(filesOf(group).includes(file.name), `missing ${file.name}`);
        assert.ok(group.textContent.includes(file.sha256), `missing checksum ${file.name}`);
      }
      const primaryFiles = release.files.map(f => f.name).sort();
      assert.deepEqual(filesOf(group).sort(), primaryFiles, `wrong release files at ${expected.id} ${i}`);
      const actualURLs = tags(group,'a').map(a=>a.href).sort();
      const expectedURLs = [release.source, ...release.files.flatMap(f=>Object.values(f.urls || {})), ...(release.supportFiles || []).flatMap(f=>Object.values(f.urls || {}))].filter(Boolean).sort();
      assert.deepEqual(actualURLs, expectedURLs, `wrong link set at ${expected.id} ${i}`);
      assert.ok(actualURLs.every(url => /^https:\/\//.test(url) && !/\/preview\//i.test(url)));
      assert.match(byClass(group,'badge')[0].textContent, i===0 ? /最新正式版/ : /历史正式版/);
    }
    select.value = 0; select.dispatch('change');
    assert.ok(filesOf(group).includes(expected.releases[0].files[0].name));
  }
  for (const [id,count] of used) assert.equal(groupsOf(get(id)).length,count);
  assert.ok(optionsTested > 50, `expected historical coverage; saw ${optionsTested}`);
});

test('formal 0.3.6 Skill/app defaults and SiteAgent stay separate', async () => {
  const {get} = await setup();
  for (const id of ['skill-releases','software-releases']) {
    assert.match(tags(get(id),'h4')[0].textContent,/0\.3\.6/);
    assert.match(tags(get(id),'h4')[0].textContent,/最新正式版/);
    assert.ok(filesOf(get(id)).every(name => !/Site[ -]?Agent/i.test(name))); 
  }
  assert.match(get('site-agent-releases').textContent,/Site[ -]?Agent/i);
  assert.doesNotMatch(html, /id="preview(?:-releases)?"/);
  assert.doesNotMatch(JSON.stringify(catalog), /"channel"\s*:\s*"preview"|\/preview\//i);
});

test('firmware defaults remain hardware/family specific with distinct build ids', async () => {
  const {get} = await setup();
  for (const [hardware,version] of [['V2','V2.9.2'],['V3','V3.6.7']]) {
    assert.match(tags(get(`firmware-${hardware}`),'h4')[0].textContent,new RegExp(version.replaceAll('.','\\.')));
  }
  const v4 = catalog.sections.firmware.V4;
  const defaults = ['MicroLink','HPMLink'].map(family => v4.find(r=>r.family === family && r.channel === 'stable'));
  assert.deepEqual(defaults.map(r=>r.version),['V4.7.1','V4.7.0']);
  assert.ok(defaults.every(r=>r.build_id));
  assert.notEqual(defaults[0].build_id,defaults[1].build_id);
  const groups = groupsOf(get('firmware-V4'));
  assert.equal(groups.length,2);
  for (const release of defaults) {
    const group = groups.find(g=>tags(g,'label')[0].textContent.includes(release.family));
    assert.ok(group);
    assert.ok(group.textContent.includes(release.build_id),'build identity must be visible');
    assert.deepEqual(filesOf(group).sort(), release.files.map(f=>f.name).sort());
  }
});

test('menu handles repeated clicks, navigation close, Escape and focus return', async () => {
  const {document,get} = await setup(); const menu = get('menu'), nav = get('nav-links');
  for (let i=0;i<2;i++) {
    menu.dispatch('click'); assert.equal(menu.getAttribute('aria-expanded'),'true'); assert.ok(nav.classList.contains('open'));
    menu.dispatch('click'); assert.equal(menu.getAttribute('aria-expanded'),'false'); assert.ok(!nav.classList.contains('open'));
  }
  menu.dispatch('click'); document.dispatch('keydown',{key:'Escape'});
  assert.equal(menu.getAttribute('aria-expanded'),'false'); assert.equal(document.activeElement,menu);
  menu.dispatch('click'); const link = document.createElement('a'); nav.append(link); nav.dispatch('click',{target:link});
  assert.equal(menu.getAttribute('aria-expanded'),'false');
});

test('catalog network/HTTP/schema failures expose a useful fallback and no fabricated releases', async () => {
  for (const options of [{fetchError:true},{responseOK:false},{data:{schemaVersion:999, sections:{}}}]) {
    const {get} = await setup(options);
    assert.match(get('load-status').textContent,/版本目录暂时无法加载/);
    assert.equal(groupsOf(get('skill-releases')).length,0);
  }
});


test('formal UTC timestamps display in Beijing time and support files stay labeled as non-installers', async () => {
  const {get} = await setup();
  assert.match(get('skill-releases').textContent, /发布日期：2026-10-25/);
  assert.match(get('software-releases').textContent, /发布日期：2026-10-25/);
  for (const id of ['software-releases','site-agent-releases']) {
    const details = byClass(get(id), 'support-files');
    assert.equal(details.length, 1);
    assert.equal(details[0].tagName,'details');
    assert.equal(details[0].getAttribute('open'), null);
    assert.match(tags(details[0],'summary')[0].textContent,/非安装包/);
    for (const file of byClass(details[0],'filename')) assert.doesNotMatch(file.textContent,/\.exe$|\.zip$/i);
  }
});

test('missing resources render honest empty states without placeholder downloads', async () => {
  const data = structuredClone(catalog);
  data.sections = {skill:[], software:[], siteAgent:[], firmware:{V2:[],V3:[],V4:[]}};
  const {get} = await setup({data});
  for (const id of ['skill-releases','software-releases','site-agent-releases','firmware-V2','firmware-V3','firmware-V4']) {
    assert.match(get(id).textContent,/暂无可核实的正式版本文件/);
    assert.equal(tags(get(id),'a').length,0);
  }
});

test('renderer does not turn unsafe URLs or release-note markup into active content', async () => {
  const data = structuredClone(catalog), release = data.sections.skill[0];
  release.notes = ['<script>alert(1)</script>']; release.source = 'javascript:alert(1)';
  release.files[0].urls = {tencent:'javascript:alert(1)', github:'http://example.invalid/file.zip'};
  const {get} = await setup({data});
  assert.equal(tags(get('skill-releases'),'a').length,0);
  assert.equal(tags(get('skill-releases'),'script').length,0);
  assert.match(get('skill-releases').textContent,/<script>alert\(1\)<\/script>/);
  assert.match(get('skill-releases').textContent,/暂无已核实的下载地址/);
});

function ancestorWithClass(element, className) {
  for (let parent = element.parentElement; parent; parent = parent.parentElement) if (parent.classList.contains(className)) return parent;
  return null;
}

test('compact release cards keep primary downloads visible and secondary information closed', async () => {
  const {get} = await setup();
  const used = new Map();
  for (const expected of expectedGroups()) {
    const index = used.get(expected.id) || 0; used.set(expected.id,index+1);
    const group = groupsOf(get(expected.id))[index], select = tags(group,'select')[0];
    for (let i=0;i<expected.releases.length;i++) {
      select.value=i; select.dispatch('change');
      const card=byClass(group,'release-card')[0];
      for (const details of tags(card,'details')) {
        assert.equal(details.getAttribute('open'),null);
        assert.ok(!details.open,'details must start closed after version changes');
      }
      assert.equal(byClass(card,'release-notes').length,1);
      for (const name of byClass(card,'filename')) assert.ok(ancestorWithClass(name,'file-details'));
      for (const hash of byClass(card,'hash')) assert.ok(ancestorWithClass(hash,'file-details'));
      for (const build of byClass(card,'build-id')) assert.ok(ancestorWithClass(build,'release-notes'));
      for (const primary of byClass(card,'primary').filter(a=>!ancestorWithClass(a,'support-files'))) {
        for (let p=primary.parentElement;p && p!==card;p=p.parentElement) assert.notEqual(p.tagName,'details','primary download must not be hidden inside a disclosure');
      }
      assert.equal(byClass(card,'primary').filter(a=>!ancestorWithClass(a,'support-files')).length,expected.releases[i].files.length);
      for (const mirrors of byClass(card,'mirror-links')) {
        assert.ok(ancestorWithClass(mirrors,'mirrors'));
        assert.ok(tags(mirrors,'a').length);
        assert.equal(byClass(mirrors,'primary').length,0);
      }
    }
  }
});

test('compact page uses a simple heading and initially collapsed separate Site Agent', () => {
  assert.match(html, /<h1>下载中心<\/h1>/);
  assert.doesNotMatch(html, /class="hero|准备好工具|让想法开始运行/);
  const siteAgentOpening=html.match(/<details\b[^>]*class="site-agent"[^>]*>/)?.[0];
  assert.ok(siteAgentOpening,'Site Agent must have a separate disclosure');
  assert.doesNotMatch(siteAgentOpening,/\bopen\b/);
  assert.match(html,/<details class="site-agent">[\s\S]*?<div id="site-agent-releases"><\/div><\/details>/);
});

test('official imagery identifies V2, V3 and the application without changing download links', () => {
  for (const [model, source] of [['V2','mklink-v2.webp'],['V3','mklink-v3.webp']]) {
    const photo = html.match(new RegExp(`<img[^>]*src="https://microkeenai\\.com/assets/${source}"[^>]*>`))?.[0];
    assert.ok(photo, `${model} must use its own official product image`);
    assert.match(photo,new RegExp(`alt="[^"]*MicroLink ${model}[^\"]*"`));
    assert.match(photo,/loading="lazy"/);
    assert.match(photo,/width="\d+" height="\d+"/);
  }
  assert.match(html, /src="https:\/\/microkeenai.com\/docs\/images\/mklink\/gui\/online-flash.png"/);
  assert.match(html, /MKLink AI Probe · 在线烧录界面示例/);
  assert.match(html, /图片辅助识别，请以设备型号为准/);
});

test('V4 families retain distinct correctly labeled images across all firmware version switches', async () => {
  const {get} = await setup();
  const groups = groupsOf(get('firmware-V4'));
  assert.equal(groups.length,2);
  for (const group of groups) {
    const name = byClass(group,'hardware-name')[0].textContent;
    const select = tags(group,'select')[0];
    for (let i=0;i<select.children.length;i++) {
      select.value=i; select.dispatch('change');
      assert.equal(byClass(group,'hardware-name')[0].textContent,name);
      const photos = tags(group,'img');
      if (name === 'MicroLink V4') {
        assert.equal(photos.length,1);
        assert.equal(photos[0].src,'https://microkeenai.com/assets/mklink-v4.webp?v=v4-screen-20261007');
        assert.match(photos[0].alt,/MicroLink V4/);
        assert.equal(photos[0].loading,'lazy');
      } else {
        assert.equal(name,'HPMLink V4');
        assert.equal(photos.length,1);
        assert.equal(photos[0].src,'https://microkeenai.com/assets/hpmlink-v4-20261025.webp');
        assert.match(photos[0].alt,/HPMLink V4.*示意图.*HPMsLink/);
        assert.equal(photos[0].loading,'lazy');
        assert.match(tags(group,'figcaption')[0].textContent,/AI 生成/);
        assert.match(byClass(group,'hardware-description')[0].textContent,/请核对设备铭牌，不适用于 MicroLink V4/);
      }
    }
  }
});

test('website downloads use the requested neutral label across all versions', async () => {
  const {get} = await setup();
  for (const id of ['skill-releases','software-releases','site-agent-releases','firmware-V2','firmware-V3','firmware-V4']) {
    for (const group of groupsOf(get(id))) {
      const select = tags(group,'select')[0];
      for (let i=0;i<select.children.length;i++) {
        select.value=i; select.dispatch('change');
        for (const link of byClass(group,'primary')) {
          if (link.href.startsWith('https://download.microkeenai.com/')) {
            assert.equal(link.textContent,'网站下载');
            assert.match(link.getAttribute('aria-label'), /· 网站下载$/);
          }
        }
        for (const link of tags(group,'a')) assert.doesNotMatch(link.textContent,/腾讯云下载/);
      }
    }
  }
});

test('software preview is shared before the paired workflows, never a unilateral spacer', () => {
  assert.equal((html.match(/class="product-visual app-visual"/g)||[]).length,1);
  assert.ok(html.indexOf('class="product-visual app-visual"') < html.indexOf('class="tools-grid"'));
  for (const id of ['skill','software']) {
    const section = html.match(new RegExp(`<section id="${id}"[\\s\\S]*?</section>`))[0];
    assert.match(section, /class="product-heading"/);
    assert.doesNotMatch(section, /<figure|spacer/);
  }
  assert.match(html,/download-center.css\?v=20261025-aligned-ui/);
  assert.match(html,/download-center.js\?v=20261025-aligned-ui/);
});
