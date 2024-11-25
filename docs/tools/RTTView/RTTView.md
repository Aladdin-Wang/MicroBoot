# 超越JLINK？这是DAPLink的最后一块拼图

## 一、引言

你听说过**J-Link**的**RTT**么？官方的宣传是这样的：

![RTTView](..\..\.\images\microlink\RTTView.jpg)

简单来说，只要拥有了**J-Link**，你就可以享受以下的便利：

- 无需占用**USART**或者**USB**转串口工具，将**printf**重定位到一个由J-LINK提供的虚拟串口上；
- 支持任何J-LINK声称支持的芯片
- 高速通信，不影响芯片的实时响应



它的缺点也是明显的：

- **你必须拥有一个J-Link**，如果你使用的是 **CMSIS-DAP**或者**ST-Link**之类的第三方调试工具，就无法享受这一福利；
- 你必须在工程中手动插入一段代码。



在我发布的文章[正版Jlink速度很牛么？中国工程师用开源轻松拿捏](https://mp.weixin.qq.com/s/ocaBIDV0DxBJwXpgW1X7OQ)中，使用开源的DAPLink替代Jlink，

![jscope](..\..\.\images\microlink\jscope.jpg)

## 一、引言



![RTTView_CMD](E:\software\MicroBoot\docs\images\microlink\RTTView_CMD.jpg)





![sin](E:\software\MicroBoot\docs\images\microlink\sin.jpg)

