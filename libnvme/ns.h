/*
 * Copyright (C) 2017 Red Hat, Inc.
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; If not, see <http://www.gnu.org/licenses/>.
 *
 * Author: Gris Ge <fge@redhat.com>
 */

#ifndef _NVME_NS_H_
#define _NVME_NS_H_

#include <stdint.h>

#include "libnvme/libnvme.h"

#pragma pack(push, 1)
struct _nvme_spec_lbaf {
	uint8_t ms[2];
	uint8_t lbads;
	uint8_t _bit_field_0;
};

struct _nvme_spec_id_ns_data {
	uint8_t nsze[8];
	uint8_t ncap[8];
	uint8_t nuse[8];
	uint8_t nsfeat;
	uint8_t nlbaf;
	uint8_t flbas;
	uint8_t mc;
	uint8_t dpc;
	uint8_t dps;
	uint8_t nmic;
	uint8_t rescap;
	uint8_t fpi;
	uint8_t elfeat;
	uint8_t nawun[2];
	uint8_t nawupf[2];
	uint8_t nacwu[2];
	uint8_t nabsn[2];
	uint8_t nabo[2];
	uint8_t nabspf[2];
	uint8_t noiob[2];
	uint8_t nvmcap[16];
	uint8_t reserved_0[40];
	uint8_t nguid[16];
	uint8_t eui64[8];
	struct _nvme_spec_lbaf lbafs[16];
	uint8_t reserved_1[192];
	uint8_t vendor_specific[3712];
};
#pragma pack(pop)

int _nvme_ns_new(int fd, struct nvme_ctrl *cnt, const char *ctrl_dev_path,
		 uint32_t ns_id, struct nvme_ns **ns, char *err_msg);

#endif	/* End of _NVME_NS_H_ */
