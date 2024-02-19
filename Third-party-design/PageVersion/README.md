# WouoUI Page 版

## 作者

Sheep118

## 说明
这是一个改动自WouoUI的纯C语言，无依赖库，只适用于128*64OLED的代码框架，将WouoUI抽象出一部分统一的接口，以方便快速构建一个具有类似WouoUI风格的OLEDUI。只需要使用简单的初始化函数便可以创建一个具有WouoUI风格的丝滑OLED动画的UI。

具体的接口函数说明请参考下面仓库里的README文件

[https://github.com/Sheep118/WouoUI-PageVersion](https://github.com/Sheep118/WouoUI-PageVersion)

## 本文件夹结构

```c
|---Csource
	|---src  (这个文件夹是WouoUIPage最主要的源代码文件，移植时主要用这里面的几个文件)
	|---example  (这个是使用WouoUIPage所提供的接口构建的一些应用的例子,只有对应.c.h文件)
|---ProjectExamples  (这个文件夹内放着使用WouoUIPage的工程的参考，按主控的类型划分，为移植时提供一些参考，所有工程文件均以zip压缩包上传到对应文件夹中)
    |---Air001(Air001的工程压缩包和底层的接口文件示例)
    |---Stm32(Stm32的工程压缩包和底层的接口文件示例)
```

## 开源协议

本项目(WouoUI Page版)遵循 MPL2.0开源协议，使用者可以在遵守MPL2.0开源协议的情况下，自由使用、修改本项目的开源代码。
