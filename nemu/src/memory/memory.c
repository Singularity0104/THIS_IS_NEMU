#include "common.h"
#include "cache.h"

uint32_t dram_read(hwaddr_t, size_t);
void dram_write(hwaddr_t, size_t, uint32_t);

/* Memory accessing interfaces */

uint32_t hwaddr_read(hwaddr_t addr, size_t len) {
	if(cache_1_find(&Cache_1, addr) == NULL) {
		return dram_read(addr, len) & (~0u >> ((4 - len) << 3));
	}
	uint32_t offset = addr & ((uint32_t)0xffffffff >> (32 - Cache_1_B_bit));
	char *ptr = cache_1_find(&Cache_1, addr);
	char tmp = 0;
	uint32_t data = 0;
	int i;
	for(i = 0; i < len; i++) {
		if(offset)
		tmp = *ptr;
		ptr++;
		data = data << 8;
		data += ((uint32_t)tmp & 0xff);
	}
	return data;
}

void hwaddr_write(hwaddr_t addr, size_t len, uint32_t data) {
	dram_write(addr, len, data);
}

uint32_t lnaddr_read(lnaddr_t addr, size_t len) {
	return hwaddr_read(addr, len);
}

void lnaddr_write(lnaddr_t addr, size_t len, uint32_t data) {
	hwaddr_write(addr, len, data);
}

uint32_t swaddr_read(swaddr_t addr, size_t len) {
#ifdef DEBUG
	assert(len == 1 || len == 2 || len == 4);
#endif
	return lnaddr_read(addr, len);
}

void swaddr_write(swaddr_t addr, size_t len, uint32_t data) {
#ifdef DEBUG
	assert(len == 1 || len == 2 || len == 4);
#endif
	lnaddr_write(addr, len, data);
}

