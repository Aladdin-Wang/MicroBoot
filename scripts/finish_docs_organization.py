"""One-time navigation and draft labeling for the source organization."""
from pathlib import Path
import yaml

ROOT = Path(__file__).resolve().parents[1]

if __name__ == '__main__':
    config_path = ROOT / 'mkdocs.yml'
    text = config_path.read_text(encoding='utf-8')
    nav = [
        {'MKLink': [
            {'产品与功能总览':'mklink/overview.md'},
            {'开始使用':[{'安装上位机':'mklink/getting-started/gui-install.md'},{'连接硬件与配置工程':'mklink/getting-started/project-config.md'}]},
            {'烧录与升级':[{'在线编译与烧录':'mklink/flashing/online-flash.md'},{'脱机下载与量产':'mklink/flashing/offline-flash.md'},{'固件升级':'mklink/flashing/firmware-upgrade.md'},{'IAP / YModem':'mklink/flashing/ymodem.md'}]},
            {'运行观测':[{'RTT 日志与终端':'mklink/observation/rtt.md'},{'SuperWatch 实时变量':'mklink/observation/superwatch.md'},{'VOFA+ 第三方上位机':'mklink/observation/vofa.md'},{'RTOS Trace / SystemView':'mklink/observation/systemview.md'}]},
            {'故障定位':[{'内存与寄存器':'mklink/debugging/memory.md'},{'HardFault / RISC-V Trap':'mklink/debugging/hardfault.md'}]},
            {'串口与 Modbus':'mklink/communication/serial-modbus.md'},
            {'开发与定制':[{'下载器内部 Python API':'mklink/development/python-api.md'},{'用 AI 定制企业上位机':'mklink/development/custom-web-gui.md'}]},
            {'下载与支持':[{'资料下载':'mklink/support/downloads.md'},{'常见问题':'mklink/support/faq.md'},{'售后服务':'mklink/support/after-sales.md'}]},
            {'原理与扩展阅读':[{'选项字节':'mklink/articles/option-bytes.md'},{'单片机的灵魂伴侣':'mklink/articles/mcu-debugging-companion.md'},{'一个调试器，四套工具链':'mklink/articles/one-probe-four-toolchains.md'},{'DAPLink 与 PikaPython':'mklink/articles/daplink-pikapython.md'},{'USB MTP 拖拽下载':'mklink/articles/usb-mtp.md'}]},
            {'历史入口':[{'Memory 与 HardFault 旧入口':'mklink/legacy/memory-hardfault.md'},{'SuperWatch 旧版教程':'mklink/legacy/symbols-superwatch.md'}]},
        ]},
        {'嵌入式 AI':[
            {'使用概览':'embedded-ai/overview.md'},
            {'接入工程':[{'安装和更新 Skill':'embedded-ai/getting-started/skill.md'},{'Skill、MCP、CLI 与 GUI':'embedded-ai/getting-started/interfaces.md'},{'第一次接手工程':'embedded-ai/getting-started/first-project.md'},{'安全边界与排障':'embedded-ai/getting-started/safety.md'}]},
            {'工作流':[{title:'embedded-ai/workflows/' + name + '-workflow.md'} for title,name in [('调试工作流选择','debug'),('编译、烧录与验证','flash'),('脱机部署与触发','offline'),('RTT 采集与分析','rtt'),('变量、内存与寄存器','memory'),('PID / FOC 观测','pid'),('HardFault 定位','hardfault'),('RTOS Trace 分析','systemview')]]},
            {'实战案例':[{'STM32F103 + RT-Thread':'embedded-ai/cases/stm32f103-rt-thread.md'},{'HPM5301 全程调试实录':'embedded-ai/cases/hpm5301-debug-session.md'}]},
            {'为什么需要硬件闭环':'embedded-ai/articles/hardware-feedback-loop.md'},
        ]},
        {'先楫生态':[{'先楫 HPM 概览':'mklink/hpm/overview.md'},{'HPM5301 + FreeRTOS 实战':'mklink/hpm/hpm5301-freertos.md'}]},
        {'MicroBoot':[{'简介':'index.md'},{'CMSIS-Pack 移植':'microboot/getting-started/cmsis-pack.md'},{'源码移植':'microboot/getting-started/source-porting.md'},{'移植常见问题':'microboot/getting-started/troubleshooting.md'},{'配置':'microboot/configuration.md'}]},
        {'组件与知识库':[{'环形队列':'components/queue.md'},{'信号槽':'components/signals-slots.md'},{'多路延时':'components/multiple-delay.md'},{'变参函数与可变参数宏':'knowledge/c/variadic-macros.md'}]},
        {'待补充资料':'drafts/index.md'},
        {'淘宝':'https://shop330265532.taobao.com/category.htm?spm=pc_detail.30350276.shop_block.dshopinfo.1a0cf2c0rjEbBf'},
    ]
    start, end = text.index('nav:'), text.index('# 主题配置')
    text = text[:start] + yaml.safe_dump({'nav':nav}, allow_unicode=True, sort_keys=False) + '\n' + text[end:]
    text = text.replace('site_description: A sample documentation site  # 文档站点描述', 'site_description: MicroKeen MKLink、嵌入式 AI 与 MicroBoot 技术文档')
    text = text.replace('site_author: kk  # 文档作者', 'site_author: 洛阳智沐科技有限公司')
    config_path.write_text(text, encoding='utf-8')
    titles = {'project':'项目介绍','team':'团队介绍','bootloader':'BootLoader 组件','flash-blob':'Flash Blob','message-map':'Message Map','message-map-legacy':'Message Map 旧提纲','publish-subscribe':'发布订阅','shell':'Shell','ymodem':'YModem','code':'贡献代码','documentation':'贡献文档','flash-algorithms':'Flash 下载算法','thread-safety':'线程安全与函数可重入','bootloader-legacy':'BootLoader 旧空白页','rt-thread':'RT-Thread 移植','arm-2d':'Arm-2D 移植','fsm':'FSM','plooc':'PLOOC'}
    entries = []
    for path in sorted((ROOT / 'docs/drafts').rglob('*.md')):
        if path.name == 'index.md': continue
        original = path.read_text(encoding='utf-8')
        title = titles[path.stem]
        if not original.startswith('---\ntitle:'):
            header = yaml.safe_dump({'title':title + '（待补充）','search':{'exclude':True}}, allow_unicode=True, sort_keys=False)
            path.write_text('---\n' + header + '---\n\n!!! info "待补充资料"\n    本页保留原始空白页或提纲，尚未形成完整教程。\n\n' + original, encoding='utf-8')
        entries.append(f'- [{title}]({path.relative_to(ROOT / "docs/drafts").as_posix()})')
    (ROOT / 'docs/drafts/index.md').write_text('# 待补充资料\n\n以下内容尚未形成完整教程，保留在此，便于后续补充。现有可用教程请从 MKLink、嵌入式 AI、MicroBoot 或组件与知识库进入。\n\n' + '\n'.join(entries) + '\n', encoding='utf-8')
    config = ROOT / 'docs/microboot/configuration.md'
    config.write_text('# MicroBoot 配置\n\n' + config.read_text(encoding='utf-8'), encoding='utf-8')
    source = ROOT / 'docs/microboot/getting-started/source-porting.md'
    source.write_text('# 基于源码移植\n\n' + source.read_text(encoding='utf-8'), encoding='utf-8')
    print(f'Navigation organized; {len(entries)} drafts labeled; original draft text retained')
