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

#ifndef _LIBNVME_NS_H_
#define _LIBNVME_NS_H_

#include <stdbool.h>

#include "libnvme_common.h"
#include "libnvme_ns_spec.h"

/**
 * nvme_ns_dev_path_get() - Retrieve the device path of specified NVMe namespace
 *
 * Retrieve the device path of specified NVMe controller.
 *
 * @ns:
 *	Pointer of 'struct nvme_ns'.
 *	If this pointer is NULL, your program will be terminated by assert().
 *
 * Return:
 *	String of device path in the form of "/dev/nvme[0-9]+n[0-9]+".
 *	Please don't free this memory, it will be released by
 *	nvme_nss_free() or nvme_ns_free().
 *	If certain namespace is inactive, NULL will be returned.
 *	If certain namespace is active but kernel block file does not exists,
 *	empty string will be returned.
 */
_DLL_PUBLIC const char *nvme_ns_dev_path_get(struct nvme_ns *ns);

_DLL_PUBLIC uint32_t nvme_ns_ns_id_get(struct nvme_ns *ns);

#endif /* End of _LIBNVME_NS_H_ */
