# Write Document

我们常常会遇到写文档，写profile报告的情况，怎么写？
参考 https://mdc-web-tomcat17.ubisoft.org/confluence/display/APEX/NX_Graphics

1.需要确定这个profile是怎么产生的？ 在哪个平台，使用什么API，什么场景等。
2.对当前的profile截一张场景图，以直观的知道这个profile是在什么情况下生成的。
格式
Paltform: NX 
Graphics API : vulkan
Map: xxxxx
Screenshot: 

3.先从整体到局部然后是的优化方案。
1.OverView
统计当前帧数，GPU时间，CPU时间
统计当前的Drawcall，场景三角面数等情况。
然后给出简单的结论，是GPU的问题还是CPU的问题。

2.统计每个Pass的情况，他们的帧数，为什么会消耗这么多时间，顶点数，shader，texture等情况，得出为什么是这么多时间。
如果跟target不一致，需要给出优化方案。
3.再次总结优化方案和总结。
4.排版，排版比较重要，这个要学会，
