# Openwrt-tty2tcp

说明：Openwrt下，串口与TCP之间进行数据转换。

使用：
1、下载它并放到package或者你喜欢的任何目录并解压它。
2、在openwrt主目录执行:
  ./scripts/feeds update -i
  ./scripts/feeds install tty2tcp
  make package/tty2tcp/compile
或：
  make menuconfig
  注：在Examples下将tty2tcp选中
  make V=99
