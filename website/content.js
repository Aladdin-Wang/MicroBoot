export const english = {
  sampleRate: '201.7<span>k samples/s</span>', toolsEyebrow: 'FROM DEVELOPMENT TO PRODUCTION', startEyebrow: 'GET STARTED WITH MKLINK',
  liveTitle:'Watch your program change.', liveIntro:'Select variables and plot them. Tune a PID loop or inspect a state transition. Ask AI to analyze the same stream while you review it in the Web GUI.',
  waveCaption:'STM32F103 · Nine sine-wave channels · Read the case study ↗',
  metric1:'STM32F103 single-variable sampling',condition1:'30 MHz SWD · 20-second recording',metric2:'HPM6E80 firmware programming',condition2:'30 MHz JTAG · 8 MiB image · verification timed separately',
  measurementNote:'Tasks were measured separately. The multi-channel demonstration is not the single-variable benchmark. Open each result for full measurement conditions.',
  toolsTitle:'Tools for the problem at hand.',tool1:'Build, flash, verify.',tool1Text:'Compile and program your project, then check that the firmware is running.',
  tool2:'Flash without a computer.',tool2Text:'Prepare the firmware and task, then trigger standalone flashing at the workbench.',
  tool3:'A console without extra UART wires.',tool3Text:'Read logs and send commands to your firmware through RTT View.',tool4:'Inspect the fault.',tool4Text:'Read memory and registers. Use project symbols to locate the fault in your source.',
  startTitle:'Connect your next development board.',startIntro:'Install the software, connect your board, and tell AI where the project is. Or open the Web GUI and debug directly.',install:'Install software & AI Skill',buy:'Shop MKLink V4 ↗',getDownloads:'Software, firmware & resources ↗',getSupport:'Contact & technical support ↗',getMicroboot:'MicroBoot open-source framework ↗',

  skip: 'Skip to content', products: 'Products', software: 'Software', docs: 'Docs', downloads: 'Downloads', support: 'Support',
  hero1: 'Give AI a connection.', hero2: 'To real hardware.',
  heroDescription: 'From the first line of code to flashing, observation and debugging.<br>MKLink connects engineers, AI and your hardware.',
  explore: 'Explore MKLink', seeCase: 'See AI in action', heroCaption: 'One device, from development to production.', scroll: 'Explore more', heroFoot: 'Built for hands-on development',
  hardwareEyebrow: '01 / MEET THE HARDWARE', hardware1: 'Small device.', hardware2: 'More possibilities.', realPhoto: 'MKLink product photograph',
  compare: 'Explore models and features', hardwareBottom: 'From your desk to standalone flashing.<br>Your project. A familiar set of tools.', connection: 'Connection and project setup',
  workflowEyebrow: '02 / AI AT THE WORKBENCH', workflow1: 'From a question.', workflow2: 'To the evidence.',
  workflowIntro: 'Flashing is just the beginning.<br>Ask AI to flash, read and analyze.<br>Inspect the same data in the Web GUI.',
  flash: 'Build & flash', watch: 'Observe', trap: 'RTT console', recover: 'Offline flash', recorded: 'Recorded session', next: 'Next step', viewScreenshot: 'View hardware session',
  caseNote: 'Screenshots from published hardware case studies. Enlarge a screenshot or read the documentation for conditions.', fullCase: 'Read the HPM case study',
  company: '洛阳智沐科技有限公司', rtd: 'Docs on Read the Docs ↗', screenshotTitle: 'MKLink · Hardware session', screenshotNote: 'The original interface is in Chinese. Scroll horizontally to inspect details.'
};
export const labels = {
  zh: {navigation:'主导航',menu:'切换导航',models:'选择 MKLink 型号',caseSteps:'调试实录步骤',enlarge:'放大实机截图',close:'关闭截图'},
  en: {navigation:'Main navigation',menu:'Toggle navigation',models:'Choose an MKLink model',caseSteps:'Recorded debugging steps',enlarge:'Enlarge hardware screenshot',close:'Close screenshot'}
};
export const models = {
  v4: {image:'mklink-v4', zh:{title:'看得见状态。<br>掌握每一次烧录。',description:'显示屏、RS485、功率监测与可选择的 Python 脱机脚本，把调试台上的能力带到生产工位。',features:['显示与交互','独立脱机烧录','RS485 与功率监测']},en:{title:'See the status.<br>Stay in control.',description:'A display, RS485, power monitoring and selectable Python offline scripts take your workflow from the bench to production.',features:['On-device display','Standalone flashing','RS485 and power monitoring']}},
  v3: {image:'mklink-v3',zh:{title:'从开发桌面，<br>走向独立烧录。',description:'板载存储、目标电压跟随与按键触发，让研发和小批量生产共用一台下载器。',features:['板载存储','目标电压跟随','按键触发脱机烧录']},en:{title:'From your desk.<br>To standalone work.',description:'Onboard storage, target-voltage following and button-triggered flashing support development and small-batch production.',features:['Onboard storage','Target-voltage following','Button-triggered offline flashing']}},
  v2: {image:'mklink-v2',zh:{title:'连接开发，<br>从这里开始。',description:'在线下载、调试与 USB 转串口。通过虚拟磁盘拖入固件，用于日常研发与机台触发场景。',features:['CMSIS-DAP 在线调试','USB 转 UART','虚拟磁盘拖入固件']},en:{title:'Your connection.<br>Starts here.',description:'Online flashing, debugging and USB to UART. Transfer firmware through the virtual drive for daily development and fixture-triggered workflows.',features:['CMSIS-DAP debugging','USB to UART','Firmware transfer via virtual drive']}}
};
export const steps = [
  {image:'case-flash',guide:'/docs/embedded-ai/workflows/flash-workflow/',prompt:{zh:'把程序编译并下载，看看系统节拍和任务计数有没有在走。',en:'Build and flash the project. Check whether system ticks and task counters are advancing.'},zh:{title:'下载后，检查程序在运行。',description:'编译、校验之后连续读取系统节拍和任务计数，确认任务在执行。',alt:'Codex 通过 MKLink 编译烧录并检查系统节拍和任务计数的对话'},en:{title:'Check that the firmware is running.',description:'After building and verifying the flash, read ticks and task counters over time to check execution.',alt:'Codex conversation verifying system ticks and task counters using MKLink'}},
  {image:'case-watch',guide:'/docs/mklink/hpm/overview/',prompt:{zh:'观察目标值、反馈和误差，帮我分析响应慢在哪里。',en:'Observe the target, response and error. Help me find why the response is slow.'},zh:{title:'调参时，看同一份数据。',description:'Web GUI 展示曲线，AI 订阅共享数据流。调整参数后，直接比较响应变化。',alt:'HPM6E80 调参后参考值、响应和误差的真实 SuperWatch 界面'},en:{title:'Review the same data while tuning.',description:'Plot curves in the Web GUI while AI subscribes to the shared stream. Compare the response after parameter changes.',alt:'Real SuperWatch interface showing HPM6E80 reference, response and error after tuning'}},
  {image:'case-rtt',guide:'/docs/mklink/observation/rtt/',prompt:{zh:'打开 RTT 终端，看看程序有哪些调试命令。',en:'Open the RTT console and check which debug commands the firmware provides.'},zh:{title:'直接与板上程序对话。',description:'程序接入 RTT 命令行后，通过调试连接查看日志、输入命令，无需另外接串口。',alt:'STM32F103 RTT 命令行的真实 Web GUI 截图'},en:{title:'Talk to the firmware.',description:'With an RTT console integrated in the firmware, read logs and send commands through the debug connection.',alt:'Real Web GUI screenshot of the STM32F103 RTT console'}},
  {image:'case-offline',guide:'/docs/mklink/flashing/offline-flash/',prompt:{zh:'把这版固件做成脱机任务，检查配置并做一次烧录验证。',en:'Prepare an offline task for this firmware, check the configuration and verify a flash run.'},zh:{title:'准备好，带到生产工位。',description:'把固件和参数保存在下载器中，完成验证后即可脱离电脑执行。',alt:'HPM5301 脱机烧录成功的真实界面'},en:{title:'Prepare it for the workbench.',description:'Store firmware and settings on the probe, verify the task, then flash without a computer.',alt:'Real interface showing successful HPM5301 standalone flashing'}}
];
