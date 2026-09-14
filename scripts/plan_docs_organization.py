"""Prepare the one-time 2026-09-10 source-path migration; does not move files."""
import json
from pathlib import Path
import re

ROOT = Path(__file__).resolve().parents[1]
DOCS = ROOT / 'docs'
REPORT = ROOT / 'maintenance/docs-organization'


def slug(name):
    return re.sub(r'-+', '-', re.sub(r'[^a-z0-9.]+', '-', name.lower())).strip('-')


def make_plan():
    mapping = {}
    def add(old, new):
        assert (ROOT / old).is_file(), old
        assert new not in mapping.values(), new
        if old != new:
            mapping[old] = new

    sections = {
        'overview.md': ['microlink.md'],
        'getting-started': ['gui-install.md', 'project-config.md'],
        'flashing': ['online-flash.md', 'offline_download.md', 'firmware-upgrade.md', 'ymodem.md'],
        'observation': ['RTT_printf.md', 'superwatch.md', 'vofa.md', 'SystemView.md'],
        'debugging': ['memory.md', 'hardfault.md'],
        'communication': ['serial-modbus.md'],
        'development': ['python_api.md', 'custom-web-gui.md'],
        'support': ['downloads.md', 'questions.md', 'after-sales.md'],
        'hpm': ['HPM.md', 'hpm5301-freertos-case.md'],
        'articles': ['USB_MTP.md', 'Mlink_PikaPython.md', '一个调试器，干掉四套工具链.md', '单片机的灵魂伴侣.md', '如何利用opt下载算法文件自由更改单片机选项字节.md'],
        'legacy': ['memory-hardfault.md', 'symbols-superwatch.md'],
    }
    names = {'offline_download.md':'offline-flash.md', 'RTT_printf.md':'rtt.md', 'SystemView.md':'systemview.md', 'python_api.md':'python-api.md', 'questions.md':'faq.md', 'HPM.md':'overview.md', 'hpm5301-freertos-case.md':'hpm5301-freertos.md', 'USB_MTP.md':'usb-mtp.md', 'Mlink_PikaPython.md':'daplink-pikapython.md', '一个调试器，干掉四套工具链.md':'one-probe-four-toolchains.md', '单片机的灵魂伴侣.md':'mcu-debugging-companion.md', '如何利用opt下载算法文件自由更改单片机选项字节.md':'option-bytes.md'}
    for folder, files in sections.items():
        for name in files:
            add('docs/tools/microlink/' + name, 'docs/mklink/' + (folder if folder.endswith('.md') else folder + '/' + names.get(name, name)))
    for path in (DOCS / 'AI').glob('*.md'):
        name = path.name
        if name == 'AI.md': target = 'overview.md'
        elif name == 'AI_MKLink.md': target = 'articles/hardware-feedback-loop.md'
        elif name == 'hpm5301-ai-debug-article.md': target = 'cases/hpm5301-debug-session.md'
        elif name == 'stm32f103-case.md': target = 'cases/stm32f103-rt-thread.md'
        elif name.endswith('-workflow.md'): target = 'workflows/' + name
        else: target = 'getting-started/' + name
        add(path.relative_to(ROOT).as_posix(), 'docs/embedded-ai/' + target)
    # The long-standing documentation homepage stays at index.md (MicroBoot introduction).
    for path in (DOCS / 'quick-start').glob('*.md'):
        target = 'source-porting.md' if path.name == 'quick-start.md' else path.name
        if path.name == 'RT-Thread.md': target = '../../drafts/microboot/rt-thread.md'
        add(path.relative_to(ROOT).as_posix(), str(Path('docs/microboot/getting-started', target)).replace('\\', '/') if not target.startswith('../') else 'docs/drafts/microboot/rt-thread.md')
    add('docs/usage-guide/config.md', 'docs/microboot/configuration.md')
    for path in (DOCS / 'components').rglob('*.md'):
        name = path.parent.name
        target = {'signals_slots':'signals-slots', 'multiple_delay':'multiple-delay', 'subscribe_publish':'publish-subscribe', 'flash_blob':'flash-blob', 'msg_map':'message-map', 'msgmap':'message-map-legacy'}.get(name, name)
        folder = 'components' if name in ['queue', 'signals_slots', 'multiple_delay'] else 'drafts/components'
        add(path.relative_to(ROOT).as_posix(), f'docs/{folder}/{target}.md')
    add('docs/utilities/ParameterMacro.md','docs/knowledge/c/variadic-macros.md')
    add('docs/utilities/ThreadSafety.md','docs/drafts/knowledge/thread-safety.md')
    add('docs/utilities/FLM.md','docs/drafts/knowledge/flash-algorithms.md')
    add('docs/bootloader/BootLoader.md','docs/drafts/microboot/bootloader-legacy.md')
    for old, new in [('3rd-party/Arm-2D/arm_2d.md','third-party/arm-2d.md'),('3rd-party/fsm/fsm.md','third-party/fsm.md'),('3rd-party/plooc/plooc.md','third-party/plooc.md'),('about/about.md','about/project.md'),('about/team.md','about/team.md'),('contribute/code.md','contributing/code.md'),('contribute/document.md','contributing/documentation.md')]:
        add('docs/' + old,'docs/drafts/' + new)

    groups = {
        'products': {'MKLink_V2.png':'mklink-v2-front.png','MKLink V3.png':'mklink-v3-front.png','MKLinkV3 LCD.jpg':'mklink-lcd-angle.jpg','MKLinkV3 LCD.png':'mklink-lcd-front.png'},
        'flashing': {'key.png':'offline-trigger-button.png','V4_KEY.png':'v4-offline-screen.png','V4_cmd.jpg':'v4-command-screen.jpg','MTP.jpg':'usb-mtp-device.jpg','FLM.jpg':'flm-algorithm.jpg','FLMTool.png':'flm-converter.png','STM32F4_FLM.jpg':'stm32f4-flm-selection.jpg','STM32FLMO.jpg':'stm32-flm-object.jpg','flmo.jpg':'flm-object.jpg','loadbin.png':'load-bin.png','load_offline.jpg':'load-offline.jpg','opt.png':'option-bytes.png','Upan.png':'usb-drive.png'},
        'rtt': {'RTT_Draw.jpg':'rtt-data-flow.jpg','RTT.drawio':'rtt-data-flow.drawio','RTT_printf.drawio':'rtt-printf.drawio','RTT_printf.jpg':'rtt-printf.jpg','RTT.jpg':'rtt.jpg','RTT.png':'rtt.png','RTT_MAP.jpg':'rtt-map.jpg','RTT_MAP.png':'rtt-map.png','RTT_ADDR.png':'rtt-address.png','rtt_code.jpg':'rtt-code.jpg','AUTO_RTT.png':'rtt-auto-detection.png','RTTView.jpg':'rtt-view.jpg','RTTView_CMD.jpg':'rtt-view-command.jpg','RTTView_baiduyun.jpg':'rtt-view-download.jpg','rtthread_rtt.png':'rt-thread-rtt.png','agile_rtt.png':'agile-rtt.png','printf.png':'printf.png','cmsis_compiler.png':'cmsis-compiler.png','compiler.png':'compiler.png'},
        'communication': {'10M_Baud.jpg':'uart-10m-baud.jpg','10M_TTL.jpg':'uart-10m-waveform.jpg','10M.png':'uart-10m.png','USB CDC.png':'usb-cdc.png','shebei.png':'usb-devices.png'},
        'debugging': {'clk.jpg':'swd-clock-waveform.jpg','SWD.jpg':'swd.jpg','JTAG.jpg':'jtag.jpg','MAX_Clock.jpg':'max-clock.jpg','sct.jpg':'scatter-file.jpg','sct1.jpg':'scatter-file-02.jpg','keil版本.png':'keil-version.png'},
        'systemview': {'mLinksysemView.jpg':'mklink-systemview.jpg','systemView.jpg':'systemview.jpg','SystemViewInfo.jpg':'systemview-info.jpg','SystemViewdemo.jpg':'systemview-demo.jpg','SystemViewMap.jpg':'systemview-map.jpg','SystemViewPag.jpg':'systemview-package.jpg','SystemViewPag_last.jpg':'systemview-package-final.jpg','SystemViewUart.jpg':'systemview-uart.jpg',**{f'SystemViewLink{i}.jpg':f'systemview-connection-{i:02}.jpg' for i in range(1,5)}},
        'vofa': {'vofa.png':'vofa.png','vofa_map.png':'vofa-map.png'},
        'development': {'python_API.png':'python-api-overview.png','API.png':'python-api.png','python.jpg':'python.jpg'},
        'hpm': {'SES1.jpg':'ses-gdb-server.jpg','SES2.jpg':'ses-cmsis-dap-config.jpg','SES3.jpg':'ses-target-connected.jpg','HPM_MAP.png':'hpm-map.png','HPM_RTT.png':'hpm-rtt.png','hpm_rtt_menu.png':'hpm-rtt-menu.png'},
        'support': {'weixin.jpg':'wechat-contact.jpg','wx.png':'wechat-article-contact.png','WX.jpg':'wechat-legacy.jpg','视频号.jpg':'wechat-video-channel.jpg','no_link.png':'device-not-connected.png','dap驱动.png':'dap-driver.png','TB.png':'taobao.png','readdocs.png':'read-the-docs.png'},
        'legacy-comparisons': {'daoban.png':'jlink-clone-message.png','defective.png':'jlink-defective-message.png','JLINK_Download.jpg':'jlink-download.jpg','JLINK_V12_Download.jpg':'jlink-v12-download.jpg','MicroLink_Download.jpg':'microlink-download.jpg','MicroLink_Download.png':'microlink-download.png','SEGGER_JLINK.png':'segger-jlink.png','MicroLink_Jlink.jpg':'microlink-jlink.jpg'},
    }
    exact = {name:(folder, target) for folder, files in groups.items() for name, target in files.items()}
    ai = {'1.png':'dma-adc-case-01.png','2.png':'dma-adc-case-02.png','AI-Agent-Skills-下载器-MCU-闭环.drawio.png':'agent-hardware-loop.drawio.png','AI.png':'ai-overview.png','claude.png':'claude.png','codex.png':'codex.png', **{f'HardFault_{i}.jpg':f'hardfault-case-{i:02}.jpg' for i in range(1,4)}}
    microboot = {'updata':'reset-startup-flow','flash_user':'flash-user-data','frame':'framework','NewUpada':'update-flow-variant-01','NewUpada1':'update-flow-variant-02','架构':'architecture','芯片安全保护启动':'secure-startup'}
    pending = []
    for path in sorted((DOCS / 'images').rglob('*')):
        if not path.is_file(): continue
        old = path.relative_to(ROOT).as_posix()
        rel = path.relative_to(DOCS / 'images').as_posix()
        name = path.name
        if name.endswith('.bkp'):
            # Editor backups are preserved outside the published documentation tree.
            target = 'maintenance/docs-organization/editor-backups/' + rel
        elif rel.startswith('microlink/hpm5301/'):
            target = 'docs/images/mklink/cases/hpm5301/' + name
        elif rel.startswith('microlink/custom-gui/'):
            target = 'docs/images/mklink/custom-gui/' + name
        elif rel.startswith('microlink/gui/'):
            target = 'docs/images/mklink/gui/' + name
        elif rel.startswith('microlink/') and name in exact:
            folder, target_name = exact[name]
            if name.endswith('.drawio'):
                target = 'docs/images/source/mklink/' + target_name
            else:
                target = 'docs/images/mklink/' + folder + '/' + target_name
        elif rel.startswith('microlink/') and name in ai:
            target = 'docs/images/embedded-ai/' + ai[name]
        elif rel.startswith('quick-start/'):
            index = re.search(r'_(\d+)\.', name)
            number = int(index.group(1)) if index else 1
            target = f'docs/images/microboot/cmsis-pack/step-{number:02}{path.suffix.lower()}'
        elif rel.startswith('components/'):
            target = 'docs/images/' + rel.replace('signals_slots','signals-slots').replace('/for.jpg','/queue-loop.jpg').replace('/signals_slots.png','/signals-slots.png')
        elif name == 'Python API.emmx':
            target = 'docs/images/source/mklink/python-api.emmx'
        elif '/' not in rel and (name.split('.')[0] in microboot):
            stem = name.split('.')[0]
            suffix = name[len(stem):]
            category = 'source/microboot' if suffix == '.drawio' else 'microboot/diagrams'
            target = 'docs/images/' + category + '/' + microboot[stem] + suffix
        elif name in ['flash.png','flash.drawio']:
            target = 'docs/images/' + ('source/mklink/' if name.endswith('.drawio') else 'mklink/flashing/') + 'flash-layout' + path.suffix
        elif name == 'favicon.ico':
            target = 'docs/images/brand/favicon.ico'
        else:
            target = 'docs/images/pending-review/' + slug(name)
            pending.append({'old': old, 'new': target, 'reason':'缺少清晰的引用或用途证据；保留原图，等待补充说明。'})
        add(old, target)
    REPORT.mkdir(parents=True, exist_ok=True)
    (REPORT / 'path-map.json').write_text(json.dumps(mapping, ensure_ascii=False, indent=2), encoding='utf-8')
    (REPORT / 'pending-review.json').write_text(json.dumps(pending, ensure_ascii=False, indent=2), encoding='utf-8')
    print(f'Plan: {len(mapping)} file moves, {len(pending)} assets require clarification')


if __name__ == '__main__':
    make_plan()
