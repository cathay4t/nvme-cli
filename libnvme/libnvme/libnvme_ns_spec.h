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

#ifndef _LIBNVME_NS_SPEC_H_
#define _LIBNVME_NS_SPEC_H_

#include <stdint.h>

#include "libnvme_common.h"

struct _DLL_PUBLIC nvme_ns;

_DLL_PUBLIC uint64_t nvme_ns_nsze_get(struct nvme_ns *ns);
_DLL_PUBLIC uint64_t nvme_ns_ncap_get(struct nvme_ns *ns);
_DLL_PUBLIC uint64_t nvme_ns_nuse_get(struct nvme_ns *ns);
_DLL_PUBLIC uint8_t nvme_ns_nsfeat_get(struct nvme_ns *ns);
_DLL_PUBLIC uint8_t nvme_ns_nlbaf_get(struct nvme_ns *ns);
_DLL_PUBLIC uint8_t nvme_ns_flbas_get(struct nvme_ns *ns);
_DLL_PUBLIC uint8_t nvme_ns_mc_get(struct nvme_ns *ns);
_DLL_PUBLIC uint8_t nvme_ns_dpc_get(struct nvme_ns *ns);
_DLL_PUBLIC uint8_t nvme_ns_dps_get(struct nvme_ns *ns);
_DLL_PUBLIC uint8_t nvme_ns_nmic_get(struct nvme_ns *ns);
_DLL_PUBLIC uint8_t nvme_ns_rescap_get(struct nvme_ns *ns);
_DLL_PUBLIC uint8_t nvme_ns_fpi_get(struct nvme_ns *ns);
_DLL_PUBLIC uint8_t nvme_ns_elfeat_get(struct nvme_ns *ns);
_DLL_PUBLIC uint16_t nvme_ns_nawun_get(struct nvme_ns *ns);
_DLL_PUBLIC uint16_t nvme_ns_nawupf_get(struct nvme_ns *ns);
_DLL_PUBLIC uint16_t nvme_ns_nacwu_get(struct nvme_ns *ns);
_DLL_PUBLIC uint16_t nvme_ns_nabsn_get(struct nvme_ns *ns);
_DLL_PUBLIC uint16_t nvme_ns_nabo_get(struct nvme_ns *ns);
_DLL_PUBLIC uint16_t nvme_ns_nabspf_get(struct nvme_ns *ns);
_DLL_PUBLIC uint16_t nvme_ns_noiob_get(struct nvme_ns *ns);
_DLL_PUBLIC const uint8_t *nvme_ns_nvmcap_get(struct nvme_ns *ns);
_DLL_PUBLIC const char *nvme_ns_nguid_get(struct nvme_ns *ns);
_DLL_PUBLIC const char *nvme_ns_eui64_get(struct nvme_ns *ns);

#endif /* End of _LIBNVME_NS_SPEC_H_ */
