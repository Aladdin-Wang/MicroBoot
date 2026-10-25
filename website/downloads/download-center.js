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
const date = value => {
  if (!value) return '官方未注明';
  if (/^\d{4}-\d{2}-\d{2}$/.test(value)) return value;
  const parsed = new Date(value);
  if (Number.isNaN(parsed.getTime())) return '官方未注明';
  return new Intl.DateTimeFormat('sv-SE', { timeZone: 'Asia/Shanghai', year: 'numeric', month: '2-digit', day: '2-digit' }).format(parsed);
};
const size = bytes => Number.isSafeInteger(bytes) && bytes >= 0
  ? `${(bytes / 1024 / 1024).toLocaleString('zh-CN', { maximumFractionDigits: 2 })} MiB · ${bytes.toLocaleString('zh-CN')} 字节` : '文件大小待核实';
function renderRelease(release, latest) {
  const card = node('div', undefined, 'release-card');
  const title = node('h4', release.version);
  title.append(node('span', latest ? '最新正式版' : '历史正式版', 'badge'));
  card.append(title, node('p', `发布日期：${date(release.date)}${release.hardware ? ` · 适用：${release.hardware}` : ''}`, 'meta'));
  const details = node('details', undefined, 'release-notes');
  details.append(node('summary', '更新说明与版本详情'));
  if (release.build_id) details.append(node('p', `构建标识 build_id：${release.build_id}`, 'meta build-id'));
  if (!release.date && release.assetPublishedAt) details.append(node('p', `官方附件上传：${date(release.assetPublishedAt)}（不代表原始发布日期）`, 'meta'));
  if (release.dateNote) details.append(node('p', release.dateNote, 'meta'));
  const notes = node('ul', undefined, 'notes');
  for (const note of release.notes?.length ? release.notes : ['官方未提供独立更新说明，请查看对应发布记录。']) notes.append(node('li', note.replace(/^#{1,6}\s+/, '').replace(/\*\*(.*?)\*\*/g, '$1')));
  details.append(notes);
  if (safeURL(release.source)) { const source = node('a', '官方发布记录 ↗'); source.href = safeURL(release.source); details.append(source); }
  const files = [...(release.files || [])].sort((a,b) => Number(/windows/i.test(b.platform || b.name)) - Number(/windows/i.test(a.platform || a.name)));
  function renderFile(file) {
    const box = node('div', undefined, `file${/windows/i.test(file.platform || file.name) ? ' windows' : ''}`);
    box.append(node('p', file.platform || '完整发布文件', 'file-title'), node('p', `${file.hardware ? `仅适用 ${file.hardware} · ` : ''}${size(file.bytes)}`, 'meta'));
    const links = node('div', undefined, 'file-links');
    const mirrors = node('details', undefined, 'mirrors');
    mirrors.append(node('summary', '备用下载'));
    const mirrorLinks = node('div', undefined, 'mirror-links');
    let primary = true;
    for (const [key,label] of [['tencent','腾讯云下载'],['github','GitHub 备用'],['gitee','Gitee 备用']]) {
      const href = safeURL(file.urls?.[key]); if (!href) continue;
      const a = node('a', primary && key !== 'tencent' ? label.replace('备用','下载') : label, primary ? 'primary' : '');
      a.href = href; a.setAttribute('aria-label', `${file.name} · ${a.textContent}`);
      if (primary) links.append(a); else mirrorLinks.append(a);
      primary = false;
    }
    if (mirrorLinks.children.length) { mirrors.append(mirrorLinks); links.append(mirrors); }
    if (!links.children.length) box.append(node('p', '暂无已核实的下载地址', 'meta')); else box.append(links);
    const integrity = node('details', undefined, 'file-details');
    integrity.append(node('summary', '文件名与 SHA-256'), node('p', file.name, 'filename'), node('p', `SHA-256：${file.sha256 || '尚未提供，下载前请向官方核实'}`, 'hash'));
    box.append(integrity);
    return box;
  }
  for (const file of files) card.append(renderFile(file));
  if (release.supportFiles?.length) {
    const details = node('details', undefined, 'support-files');
    details.append(node('summary', '校验清单与签名（非安装包）'));
    for (const file of release.supportFiles) details.append(renderFile(file));
    card.append(details);
  }
  card.append(details);
  return card;
}
function mount(target, releases, label, hardwareFamily) {
  if (!releases.length) { target.append(node('p', '暂无可核实的正式版本文件。资源补齐并核对后展示。', 'empty')); return; }
  const group = node('div', undefined, 'release-group');
  if (hardwareFamily) {
    if (hardwareFamily === 'MicroLink') {
      const figure = node('figure', undefined, 'product-visual hardware-visual');
      const photo = node('img');
      photo.src = 'https://microkeenai.com/assets/mklink-v4.webp?v=v4-screen-20261007';
      photo.alt = 'MicroLink V4 彩屏下载器实物，带显示屏与侧面接口';
      photo.width = 398; photo.height = 420; photo.loading = 'lazy'; photo.decoding = 'async';
      figure.append(photo, node('figcaption', 'MicroLink V4 · 实物图'));
      group.append(figure);
    } else if (hardwareFamily === 'HPMLink') {
      const figure = node('figure', undefined, 'product-visual hardware-visual');
      const photo = node('img');
      photo.src = 'https://microkeenai.com/assets/hpmlink-v4-20261025.webp';
      photo.alt = 'HPMLink V4 下载器外观示意图，基于 MKLink 图片生成，机身标识 HPMsLink';
      photo.width = 1221; photo.height = 1289; photo.loading = 'lazy'; photo.decoding = 'async';
      figure.append(photo, node('figcaption', 'HPMLink V4 · 外观示意图（AI 生成）'));
      group.append(figure);
    }
    group.append(node('h3', `${hardwareFamily} V4`, 'hardware-name'));
    group.append(node('p', hardwareFamily === 'HPMLink' ? '仅适用于 HPMLink V4 · UF2。请核对设备铭牌，不适用于 MicroLink V4。' : '仅适用于 MicroLink V4 · UF2', 'hardware-description'));
  }
  const select = node('select'); const id = `${target.id}-${target.children.length}-version`; select.id = id;
  const selectLabel = node('label', `${label} · 版本`); selectLabel.htmlFor = id;
  releases.forEach((release,index) => { const option = node('option', `${release.version} · ${index === 0 ? '最新正式版' : '历史版本'}${release.date ? ` · ${date(release.date)}` : ''}`); option.value = String(index); select.append(option); });
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
  const response = await fetch('/download-catalog.json?v=20261025-stable036');
  if (!response.ok) throw new Error('catalog unavailable');
  const data = await response.json();
  if (data.schemaVersion !== 1 || !data.sections) throw new Error('unsupported catalog');
  for (const [key,label] of [['skill','完整 Skill'],['software','上位机'],['siteAgent','Windows Site Agent']]) {
    const releases = data.sections[key] || [];
    mount(document.querySelector(`#${key === 'siteAgent' ? 'site-agent' : key}-releases`), releases.filter(r => r.channel === 'stable'),label);
  }
  for (const hardware of ['V2','V3','V4']) {
    const releases = data.sections.firmware?.[hardware] || [];
    const target = document.querySelector(`#firmware-${hardware}`);
    const stable = releases.filter(r => r.channel === 'stable');
    const families = [...new Set(stable.map(r => r.family).filter(Boolean))];
    if (families.length > 1) {
      for (const family of families) mount(target, stable.filter(r => r.family === family), `${family} ${hardware} 固件`, family);
    } else mount(target,stable, `${hardware} 固件`);
  }
  const missing = document.querySelector('#missing-resources');
  for (const item of data.missingResources || []) missing.append(node('li',item));
  document.querySelector('#load-status').textContent = `资源目录核对日期：${data.generatedAt ? date(data.generatedAt) : '待记录'}。下载与安装前请核对发布说明。`;
} catch {
  document.querySelector('#load-status').textContent = '版本目录暂时无法加载，请稍后刷新，或通过官网文档中的官方发布入口获取文件。';
}

