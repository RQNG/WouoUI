# 作者
[Legend](https://github.com/liux-pro)

# 说明
基于SDL2,移植u8g2,添加Arduino兼容层,模拟必要的Arduino API,模拟编码、按键长短按时序,使WouoUI在能在PC平台编译运。

键盘QWER分别代表编码器左、右旋转,短按,长按。(注意把电脑输入法关了)

EEPROM,ADC,HID无实际功能。
# Windows编译
1. 装好 [msys2](https://www.msys2.org/) 环境
2. （可选）使用 msys2-mingw64 执行`sed -i "s#mirror.msys2.org/#mirrors.tuna.tsinghua.edu.cn/msys2/#g" /etc/pacman.d/mirrorlist*`切换国内源
3. 使用 msys2-mingw64 执行`pacman -S mingw-w64-x86_64-SDL2`，安装SDL2
4. 使用 msys2-mingw64 执行`pacman -S mingw-w64-x86_64-toolchain cmake make ninja git`，安装编译器等
5. `git clone --recurse-submodules https://github.com/RQNG/WouoUI.git`
6. `cd Third-party-design/Emulator && cmake -B build"`
7. `cd build && ninja`
8. `./WouoUI.exe`
