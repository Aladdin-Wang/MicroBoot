import {english, labels, models, steps} from './content.js';

const $ = selector => document.querySelector(selector);
const translations = [...document.querySelectorAll('[data-i18n]')];
const chinese = Object.fromEntries(translations.map(el => [el.dataset.i18n, el.innerHTML]));
let language = 'zh';
let model = 'v4';
let step = 0;
try { if (localStorage.getItem('microkeen-language') === 'en') language = 'en'; } catch { /* Preference storage is optional. */ }

function selectTabs(selector, attribute, value) {
  document.querySelectorAll(selector).forEach(button => {
    const selected = button.dataset[attribute] === String(value);
    button.setAttribute('aria-selected', String(selected));
    button.tabIndex = selected ? 0 : -1;
  });
}
function renderModel() {
  const item = models[model], copy = item[language];
  $('#model-name').textContent = `MKLink ${model.toUpperCase()}`;
  $('#model-title').innerHTML = copy.title;
  $('#model-description').textContent = copy.description;
  const img = $('#model-image');
  img.src = `/assets/${item.image}.webp`;
  img.alt = `MKLink ${model.toUpperCase()} ${language === 'zh' ? '实物图' : 'product photograph'}`;
  $('#model-features').replaceChildren(...copy.features.map((feature, index) => {
    const li = document.createElement('li'), number = document.createElement('span');
    number.textContent = `0${index + 1}`;
    li.append(number, feature);
    return li;
  }));
  selectTabs('[data-model]', 'model', model);
  $('#model-panel').setAttribute('aria-labelledby', `tab-${model}`);
}
function renderStep() {
  const item = steps[step], copy = item[language];
  $('#case-title').textContent = copy.title;
  $('#case-description').textContent = copy.description;
  $('#case-log').textContent = item.prompt[language];
  $('#case-guide').href = item.guide;
  $('#case-guide').textContent = language === 'zh' ? '查看使用方法 ↗' : 'Read the guide ↗';
  $('#case-log').setAttribute('aria-label', language === 'zh' ? '可以对 AI 这样说' : 'Ask AI');
  $('#case-image').src = `/assets/${item.image}.webp`;
  $('#case-image').alt = copy.alt;
  $('#case-counter').textContent = `0${step + 1} / 04`;
  selectTabs('[data-step]', 'step', step);
  $('#case-panel').setAttribute('aria-labelledby', `step-${step}`);
}
function renderLanguage() {
  document.documentElement.lang = language === 'zh' ? 'zh-CN' : 'en';
  const dictionary = language === 'zh' ? chinese : english;
  translations.forEach(el => { el.innerHTML = dictionary[el.dataset.i18n]; });
  document.querySelectorAll('[data-label]').forEach(el => el.setAttribute('aria-label', labels[language][el.dataset.label]));
  $('#language').innerHTML = language === 'zh' ? 'EN <span aria-hidden="true">↗</span>' : '中文 <span aria-hidden="true">↗</span>';
  $('#language').setAttribute('aria-label', language === 'zh' ? 'Switch to English' : '切换为中文');
  $('.hero-product img').alt = language === 'zh' ? 'MKLink V4 实物斜侧视图，带显示屏与侧面接口' : 'MKLink V4 product photograph with display and side connector';
  $('.nav .brand').setAttribute('aria-label', language === 'zh' ? 'MicroKeen 首页' : 'MicroKeen home');
  document.title = language === 'zh' ? 'MicroKeen · 让想法在硬件上运行' : 'MicroKeen · Connect ideas to hardware';
  $('meta[name="description"]').content = language === 'zh' ? 'MicroKeen MKLink，让 AI 与工程师一起完成嵌入式烧录、观测和调试。探索硬件、软件与真实调试案例。' : 'MicroKeen MKLink connects engineers, AI and embedded hardware. Explore products, software and a recorded debugging session.';
  renderModel(); renderStep();
}
$('#language').addEventListener('click', () => {
  language = language === 'zh' ? 'en' : 'zh';
  try { localStorage.setItem('microkeen-language', language); } catch { /* Continue without persistence. */ }
  renderLanguage();
});
document.querySelectorAll('[data-model]').forEach(button => button.addEventListener('click', () => { model = button.dataset.model; renderModel(); }));
document.querySelectorAll('[data-step]').forEach(button => button.addEventListener('click', () => { step = Number(button.dataset.step); renderStep(); }));
$('#next-step').addEventListener('click', () => { step = (step + 1) % steps.length; renderStep(); });
// Arrow keys, Home and End provide standard keyboard navigation for both tab lists.
document.querySelectorAll('[role=tablist]').forEach(list => list.addEventListener('keydown', event => {
  const buttons = [...list.querySelectorAll('[role=tab]')];
  const current = buttons.indexOf(document.activeElement);
  if (current < 0 || !['ArrowRight', 'ArrowLeft', 'Home', 'End'].includes(event.key)) return;
  event.preventDefault();
  const next = event.key === 'Home' ? 0 : event.key === 'End' ? buttons.length - 1 : (current + (event.key === 'ArrowRight' ? 1 : -1) + buttons.length) % buttons.length;
  buttons[next].focus(); buttons[next].click();
}));
function closeMenu() { $('#nav-links').classList.remove('open'); $('#menu').setAttribute('aria-expanded', 'false'); }
$('#menu').addEventListener('click', () => { const open = $('#nav-links').classList.toggle('open'); $('#menu').setAttribute('aria-expanded', String(open)); });
$('#nav-links').addEventListener('click', event => { if (event.target.closest('a')) closeMenu(); });
document.addEventListener('keydown', event => { if (event.key === 'Escape' && $('#menu').getAttribute('aria-expanded') === 'true') { closeMenu(); $('#menu').focus(); } });
window.matchMedia('(min-width: 701px)').addEventListener('change', event => { if (event.matches) closeMenu(); });
const dialog = $('#image-dialog');
$('#enlarge').addEventListener('click', () => { $('#dialog-image').src = $('#case-image').src; $('#dialog-image').alt = $('#case-image').alt; dialog.showModal(); });
$('#close-dialog').addEventListener('click', () => dialog.close());
dialog.addEventListener('click', event => { const rect = dialog.getBoundingClientRect(); if (event.target === dialog && (event.clientX < rect.left || event.clientX > rect.right || event.clientY < rect.top || event.clientY > rect.bottom)) dialog.close(); });
dialog.addEventListener('close', () => $('#enlarge').focus());
renderLanguage();
