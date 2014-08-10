#include "sump.h"
#include "pca.h"


void sump_provide_metadata(uint32_t probes,
		uint32_t ssamples,
		uint32_t dsamples,
		uint32_t srate) {

	struct {
		uint8_t key;
		const char *value;
	} str_info[] = {
		{ SUMP_META_STR_DEVNAME, DEVICE_NAME FIRMWARE_VERSION },
		{ SUMP_META_STR_FPGA_VER, FIRMWARE_VERSION },
		{ SUMP_META_STR_ANC_VER, FIRMWARE_VERSION},
		{ 0x00, 0x00 }
	};

	struct {
		uint8_t key;
		uint32_t value;
	} int32_info[] = {
		{ SUMP_META_INT_PROBES, common_swap(probes) },
		{ SUMP_META_INT_SSAMPLES, common_swap(ssamples) },
		{ SUMP_META_INT_DSAMPLES, common_swap(dsamples) },
		{ SUMP_META_INT_SRATE, common_swap(srate) },
		{ SUMP_META_INT_PROTO_VER, common_swap(0x02) },
		{ 0x00, 0x00 }
	};

	uint8_t i = 0;

	while (str_info[i].key != 0 && str_info[i].value) {
		serial_poll_sendc(str_info[i].key);
		printf(str_info[i].value);
		serial_poll_sendc(SUMP_META_STR_NULL);
		i++;
	}

	i = 0;
	while (int32_info[i].key != 0) {
		serial_poll_sendc(int32_info[i].key);
		serial_poll_send(&int32_info[i].value, 4);
		i++;
	}

	serial_poll_sendc(SUMP_META_STR_NULL);
}
