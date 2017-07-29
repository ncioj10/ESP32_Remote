deps_config := \
	/c/Users/Nicolas/AppData/Roaming/SPB_Data/esp-idf/components/app_trace/Kconfig \
	/c/Users/Nicolas/AppData/Roaming/SPB_Data/esp-idf/components/aws_iot/Kconfig \
	/c/Users/Nicolas/AppData/Roaming/SPB_Data/esp-idf/components/bt/Kconfig \
	/c/Users/Nicolas/AppData/Roaming/SPB_Data/esp-idf/components/esp32/Kconfig \
	/c/Users/Nicolas/AppData/Roaming/SPB_Data/esp-idf/components/ethernet/Kconfig \
	/c/Users/Nicolas/AppData/Roaming/SPB_Data/esp-idf/components/fatfs/Kconfig \
	/c/Users/Nicolas/AppData/Roaming/SPB_Data/esp-idf/components/freertos/Kconfig \
	/c/Users/Nicolas/AppData/Roaming/SPB_Data/esp-idf/components/log/Kconfig \
	/c/Users/Nicolas/AppData/Roaming/SPB_Data/esp-idf/components/lwip/Kconfig \
	/c/Users/Nicolas/AppData/Roaming/SPB_Data/esp-idf/components/mbedtls/Kconfig \
	/c/Users/Nicolas/AppData/Roaming/SPB_Data/esp-idf/components/openssl/Kconfig \
	/c/Users/Nicolas/AppData/Roaming/SPB_Data/esp-idf/components/spi_flash/Kconfig \
	/c/Users/Nicolas/AppData/Roaming/SPB_Data/esp-idf/components/bootloader/Kconfig.projbuild \
	/c/Users/Nicolas/AppData/Roaming/SPB_Data/esp-idf/components/esptool_py/Kconfig.projbuild \
	/c/Users/Nicolas/AppData/Roaming/SPB_Data/esp-idf/components/partition_table/Kconfig.projbuild \
	/c/Users/Nicolas/AppData/Roaming/SPB_Data/esp-idf/Kconfig

include/config/auto.conf: \
	$(deps_config)


$(deps_config): ;
