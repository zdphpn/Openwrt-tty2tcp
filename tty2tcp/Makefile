include $(TOPDIR)/rules.mk

PKG_NAME:=tty2tcp
PKG_VERSION:=1.0

include $(INCLUDE_DIR)/package.mk

define Package/tty2tcp
  CATEGORY:=Examples
  TITLE:=tty2tcp
  DEPENDS:=+libstdcpp
endef

define Package/tty2tcp/description
  tty2tcp,exchange data between tty(ttyS0) and tcp(9000)
endef

define Package/tty2tcp/install
	$(INSTALL_DIR) $(1)/usr/bin $(1)/etc/init.d
	
	#已经存在tty2tcp(bin)
	$(INSTALL_BIN) ./src/tty2tcp $(1)/usr/bin
	#只有tty2tcp.cpp,需要编译生成tty2tcp(bin)
	#$(INSTALL_BIN) $(PKG_BUILD_DIR)/tty2tcp $(1)/usr/bin
	
	$(INSTALL_BIN) ./files/tty2tcp.init $(1)/etc/init.d/tty2tcp
endef

$(eval $(call BuildPackage,tty2tcp))
