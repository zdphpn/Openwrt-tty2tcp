# Openwrt-tty2tcp

## 说明：
    Openwrt下，串口与TCP之间进行数据转换。  

## 使用：

1. 下载它并放到openwrt主目录/package下并解压它。  

2. 在openwrt主目录执行:  

  * ./scripts/feeds update -i  
  * ./scripts/feeds install tty2tcp  
  * make package/tty2tcp/compile  
  
2. 或：

  * make menuconfig  
  _注：在Examples下将tty2tcp选中_   
  * make V=99  
