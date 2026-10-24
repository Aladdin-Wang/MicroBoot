const node = (tag, text, className) => {
  const element = document.createElement(tag);
  if (text !== undefined) element.textContent = text;
  if (className) element.className = className;
  return element;
};
const safeURL = value => {
  try { const url = new URL(value); return url.protocol === 'https:' ? url.href : null; }
  catch { return null; }
};
const date = value => value ? value.slice(0, 10) : '官方未注明';
const size = bytes => Number.isSafeInteger(bytes) && bytes >= 0
  ? `${(bytes / 1024 / 1024).toLocaleString('zh-CN', { maximumFractionDigits: 2 })} MiB · ${bytes.toLocaleString('zh-CN')} 字节` : '文件大小待核实';
function renderRelease(release, latest) {
  const card = node('div', undefined, 'release-card');
  const title = node('h4', release.version);
  title.append(node('span', release.channel === 'preview' ? '测试版 · 非正式' : latest ? '最新正式版' : '历史正式版', 'badge'));
  card.append(title, node('p', `发布日期：${date(release.date)}${release.hardware ? ` · 适用：${release.hardware}` : ''}`, 'meta'));
  if (!release.date && release.assetPublishedAt) card.append(node('p', `官方附件上传：${date(release.assetPublishedAt)}（不代表原始发布日期）`, 'meta'));
  if (release.dateNote) card.append(node('p', release.dateNote, 'meta'));
  const notes = node('ul', undefined, 'notes');
  for (const note of release.notes?.length ? release.notes : ['官方未提供独立更新说明，请查看对应发布记录。']) notes.append(node('li', note.replace(/^#{1,6}\s+/, '').replace(/\*\*(.*?)\*\*/g, '$1')));
  card.append(notes);
  if (safeURL(release.source)) { const source = node('a', '官方发布记录 ↗'); source.href = safeURL(release.source); card.append(source); }
  const files = [...(release.files || [])].sort((a,b) => Number(/windows/i.test(b.platform || b.name)) - Number(/windows/i.test(a.platform || a.name)));
  for (const file of files) {
    const box = node('div', undefined, `file${/windows/i.test(file.platform || file.name) ? ' windows' : ''}`);
    box.append(node('p', file.name, 'filename'), node('p', `${file.platform || '完整发布文件'}${file.hardware ? ` · 仅适用 ${file.hardware}` : ''} · ${size(file.bytes)}`, 'meta'));
    const hash = node('p', `SHA-256：${file.sha256 || '尚未提供，下载前请向官方核实'}`, 'hash'); box.append(hash);
    const links = node('div', undefined, 'file-links');
    let primary = true;
    for (const [key,label] of [['tencent','腾讯云下载'],['github','GitHub 备用'],['gitee','Gitee 备用']]) {
      const href = safeURL(file.urls?.[key]); if (!href) continue;
      const a = node('a', primary && key !== 'tencent' ? label.replace('备用','下载') : label, primary ? 'primary' : '');
      a.href = href; a.setAttribute('aria-label', `${file.name} · ${a.textContent}`); links.append(a); primary = false;
    }
    if (!links.children.length) box.append(node('p', '暂无已核实的下载地址', 'meta')); else box.append(links);
    card.append(box);
  }
  return card;
}
function mount(target, releases, label) {
  if (!releases.length) { target.append(node('p', '暂无可核实的正式版本文件。资源补齐并核对后展示，不用测试版替代正式版。', 'empty')); return; }
  const group = node('div', undefined, 'release-group');
  const select = node('select'); const id = `${target.id}-${target.children.length}-version`; select.id = id;
  const selectLabel = node('label', `${label} · 选择版本`); selectLabel.htmlFor = id;
  releases.forEach((release,index) => { const option = node('option', `${release.version} · ${release.channel === 'preview' ? '测试版' : index === 0 ? '最新正式版' : '历史版本'}${release.date ? ` · ${date(release.date)}` : ''}`); option.value = String(index); select.append(option); });
  const content = node('div'); content.setAttribute('aria-live','polite');
  const update = () => { const index = Number(select.value); content.replaceChildren(renderRelease(releases[index], index === 0)); };
  select.addEventListener('change', update); group.append(selectLabel,select,content); target.append(group); update();
}
const menu = document.querySelector('#menu'); const nav = document.querySelector('#nav-links');
function closeMenu() { nav.classList.remove('open'); menu.setAttribute('aria-expanded','false'); }
menu.addEventListener('click', () => { const open = nav.classList.toggle('open'); menu.setAttribute('aria-expanded',String(open)); });
nav.addEventListener('click',event => { if (event.target.closest('a')) closeMenu(); });
document.addEventListener('keydown',event => { if(event.key === 'Escape' && menu.getAttribute('aria-expanded') === 'true') { closeMenu(); menu.focus(); } });
try {
  const response = await fetch('/download-catalog.json?v=20261024-final');
  if (!response.ok) throw new Error('catalog unavailable');
  const data = await response.json();
  if (data.schemaVersion !== 1 || !data.sections) throw new Error('unsupported catalog');
  const preview = document.querySelector('#preview-releases');
  for (const [key,label] of [['skill','Skill'],['software','上位机']]) {
    const releases = data.sections[key] || [];
    mount(document.querySelector(`#${key}-releases`), releases.filter(r => r.channel === 'stable'),label);
    const tests = releases.filter(r => r.channel === 'preview');
    if (tests.length) mount(preview,tests,`${label}测试版`);
  }
  for (const hardware of ['V2','V3','V4']) {
    const releases = data.sections.firmware?.[hardware] || [];
    const target = document.querySelector(`#firmware-${hardware}`);
    const stable = releases.filter(r => r.channel === 'stable');
    const families = [...new Set(stable.map(r => r.family).filter(Boolean))];
    if (families.length > 1) {
      target.append(node('p', '此型号有不同固件系列，请按设备现有固件与官方说明选择，不可跨系列混刷。', 'warning'));
      for (const family of families) mount(target, stable.filter(r => r.family === family), `${family} ${hardware} 固件`);
    } else mount(target,stable, `${hardware} 固件`);
    const tests = releases.filter(r => r.channel === 'preview'); if (tests.length) mount(preview,tests,`${hardware} 固件测试版`);
  }
  if (!preview.children.length) preview.append(node('p','暂无已核实的测试版下载文件。','empty'));
  const missing = document.querySelector('#missing-resources');
  for (const item of data.missingResources || []) missing.append(node('li',item));
  document.querySelector('#load-status').textContent = `资源目录核对日期：${data.generatedAt ? date(data.generatedAt) : '待记录'}。下载与安装前请核对发布说明。`;
} catch {
  document.querySelector('#load-status').textContent = '版本目录暂时无法加载，请稍后刷新，或通过官网文档中的官方发布入口获取文件。';
}
