# J-Scope 对比数据与条件

> 口径更正：约500组/秒仅为导出文件记录率，尚不能认定为HSS实际采样率或硬件上限。Raw与CSV一致不能排除保存环节抽样，界面读数差异仍未解释。相关性能对比已从正式文章撤下，用户已结束后续测试；本文件仅保留原始研究记录。


仅描述本次被SEGGER提示为clone、自报PLUS的实物，不代表正版J-Link PLUS。目标STM32F103RET6，AXF匹配16路纯正弦示例，周期4秒。HSS请求周期100us，SWD请求档位4/10/20/30MHz；配置值不等于独立实测SWCLK。未禁用检测，警告由用户人工处理。

CSV时间戳单位微秒，与4秒波形周期一致。完整更新率=(记录数-1)/(末时间戳-首时间戳)。Raw为小端uint32时间戳与每通道float；除早期4MHz单变量只存CSV外，各新增档位已全量核对Raw/CSV，浮点文本舍入容差1e-6。界面4–5kSamples/s读数与导出持续率有差异，本次结论采用实际导出Raw时间序列，不解释尚未查明的UI计算机制。初始带全零尾部文件不用于正文。

MKLink正文数据来自既有同板测试RAM性能矩阵，两种工具的软件采集路径及变量内容不同，不是裸硬件单因素实验。

50 MHz 单变量为请求档位验证，实际SWCLK未独立测量；17483条/34.96334秒，约500.00944次/秒。已停止采集，未复位或改写目标。

## 文件

- [export-summary.json](export-summary.json)
- [jscope-hss-10mhz-four.analysis.json](jscope-hss-10mhz-four.analysis.json)
- [jscope-hss-10mhz-four.csv](jscope-hss-10mhz-four.csv)
- [jscope-hss-10mhz-four.jraw](jscope-hss-10mhz-four.jraw)
- [jscope-hss-10mhz-single.analysis.json](jscope-hss-10mhz-single.analysis.json)
- [jscope-hss-10mhz-single.csv](jscope-hss-10mhz-single.csv)
- [jscope-hss-10mhz-single.jraw](jscope-hss-10mhz-single.jraw)
- [jscope-hss-20mhz-four.analysis.json](jscope-hss-20mhz-four.analysis.json)
- [jscope-hss-20mhz-four.csv](jscope-hss-20mhz-four.csv)
- [jscope-hss-20mhz-four.jraw](jscope-hss-20mhz-four.jraw)
- [jscope-hss-20mhz-single.analysis.json](jscope-hss-20mhz-single.analysis.json)
- [jscope-hss-20mhz-single.csv](jscope-hss-20mhz-single.csv)
- [jscope-hss-20mhz-single.jraw](jscope-hss-20mhz-single.jraw)
- [jscope-hss-30mhz-four.analysis.json](jscope-hss-30mhz-four.analysis.json)
- [jscope-hss-30mhz-four.csv](jscope-hss-30mhz-four.csv)
- [jscope-hss-30mhz-four.jraw](jscope-hss-30mhz-four.jraw)
- [jscope-hss-30mhz-single.analysis.json](jscope-hss-30mhz-single.analysis.json)
- [jscope-hss-30mhz-single.csv](jscope-hss-30mhz-single.csv)
- [jscope-hss-30mhz-single.jraw](jscope-hss-30mhz-single.jraw)
- [jscope-hss-4mhz-four.analysis.json](jscope-hss-4mhz-four.analysis.json)
- [jscope-hss-4mhz-four.csv](jscope-hss-4mhz-four.csv)
- [jscope-hss-4mhz-four.jraw](jscope-hss-4mhz-four.jraw)
- [jscope-hss-4mhz-single-clean.analysis.json](jscope-hss-4mhz-single-clean.analysis.json)
- [jscope-hss-4mhz-single-clean.csv](jscope-hss-4mhz-single-clean.csv)
- [jscope-hss-4mhz-single.analysis.json](jscope-hss-4mhz-single.analysis.json)
- [jscope-hss-4mhz-single.csv](jscope-hss-4mhz-single.csv)

- [jscope-hss-50mhz-single.analysis.json](jscope-hss-50mhz-single.analysis.json)
- [jscope-hss-50mhz-single.csv](jscope-hss-50mhz-single.csv)
- [jscope-hss-50mhz-single.jraw](jscope-hss-50mhz-single.jraw)
