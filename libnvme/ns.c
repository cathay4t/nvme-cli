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
#define _GNU_SOURCE
/* ^ For strerror_r() */
#include <string.h>
#include <assert.h>
#include <stdint.h>
#include <inttypes.h>

#include "libnvme/libnvme.h"

#include "ns.h"
#include "utils.h"
#include "ioctl.h"

#define _NVME_ADMIN_CMD_IDENTIFY_CNS_IDENTIFY_NS		0x00

struct nvme_lbaf{
	struct _nvme_spec_lbaf raw_data;
};

struct _nvme_ns_strs {
	const char *nguid;
	const char *eui64;
};

struct nvme_ns {
	const char *dev_path;
	struct _nvme_ns_strs strs;
	struct _nvme_spec_id_ns_data raw_data;
	struct nvme_lbaf **lbafs;
	uint32_t ns_id;
};

static int _nvme_lbafs_new(struct nvme_ns *ns, char *err_msg);

_getter_func_gen_uint64_t(nvme_ns, nsze);
_getter_func_gen_uint64_t(nvme_ns, ncap);
_getter_func_gen_uint64_t(nvme_ns, nuse);
_getter_func_gen_uint8_t(nvme_ns, nsfeat);
_getter_func_gen_uint8_t(nvme_ns, nlbaf);
_getter_func_gen_uint8_t(nvme_ns, flbas);
_getter_func_gen_uint8_t(nvme_ns, mc);
_getter_func_gen_uint8_t(nvme_ns, dpc);
_getter_func_gen_uint8_t(nvme_ns, dps);
_getter_func_gen_uint8_t(nvme_ns, nmic);
_getter_func_gen_uint8_t(nvme_ns, rescap);
_getter_func_gen_uint8_t(nvme_ns, fpi);
_getter_func_gen_uint8_t(nvme_ns, elfeat);
_getter_func_gen_uint16_t(nvme_ns, nawun);
_getter_func_gen_uint16_t(nvme_ns, nawupf);
_getter_func_gen_uint16_t(nvme_ns, nacwu);
_getter_func_gen_uint16_t(nvme_ns, nabsn);
_getter_func_gen_uint16_t(nvme_ns, nabo);
_getter_func_gen_uint16_t(nvme_ns, nabspf);
_getter_func_gen_uint16_t(nvme_ns, noiob);
_getter_func_gen_str(nvme_ns, nguid);
_getter_func_gen_str(nvme_ns, eui64);

int _nvme_ns_new(int fd, struct nvme_ctrl *cnt, const char *ctrl_dev_path,
		 uint32_t ns_id, struct nvme_ns **ns, char *err_msg)
{
	int rc = NVME_OK;

	assert(cnt != NULL);
	assert(fd >= 0);
	assert(ctrl_dev_path != NULL);
	assert(ns != NULL);

	*ns = (struct nvme_ns *) calloc(1, sizeof(struct nvme_ns));
	_alloc_null_check(err_msg, *ns, rc, out);

	_good(_nvme_ioctl_identify(fd, (uint8_t *) &((*ns)->raw_data),
				   _NVME_ADMIN_CMD_IDENTIFY_CNS_IDENTIFY_NS,
				   ns_id, err_msg),
	      rc, out);
	_good(_nvme_lbafs_new(*ns, err_msg), rc, out);

	/*
	 * TODO(Gris Ge): need to verify sysfs `nsid` file to get dev_path of
	 *		  namespace
	 */
	_hex_str_prop_init(nvme_ns, *ns, nguid, err_msg, rc, out);
	_hex_str_prop_init(nvme_ns, *ns, eui64, err_msg, rc, out);

	(*ns)->ns_id = ns_id;

out:
	if (rc != NVME_OK) {
		nvme_ns_free(*ns);
		*ns = NULL;
	}
	return rc;
}

void nvme_ns_free(struct nvme_ns *ns)
{
	uint8_t i = 0;
	if (ns == NULL)
		return;

	if (ns->lbafs != NULL)
		for (i = 0; i <= nvme_ns_nlbaf_get(ns); ++i)
			free(ns->lbafs[i]);
	free(ns->lbafs);

	free((void *) ns->dev_path);
	free((void *) ns->strs.nguid);
	free((void *) ns->strs.eui64);
	memset(ns, 0, sizeof(struct nvme_ns));
	/* ^ Just ensure there will be no dangling pointer */
	free(ns);
}

const char *nvme_ns_dev_path_get(struct nvme_ns *ns)
{
	assert(ns != NULL);
	errno = 0;
	return ns->dev_path;
}

const uint8_t *nvme_ns_raw_id_data_get(struct nvme_ns *ns)
{
	assert(ns != NULL);
	errno = 0;
	return (uint8_t *) &(ns->raw_data);
}

const uint8_t *nvme_ns_vendor_specfic_get(struct nvme_ns *ns)
{
	assert(ns != NULL);
	errno = 0;
	return ns->raw_data.vendor_specific;
}

static int _nvme_lbafs_new(struct nvme_ns *ns, char *err_msg)
{
	uint8_t lbaf_count = 0;
	uint8_t i = 0;
	struct nvme_lbaf **lbafs = NULL;
	struct nvme_lbaf *lbaf = NULL;
	size_t max_lbaf_count = 0;

	errno = 0;

	assert(ns != NULL);
	assert(ns->lbafs == NULL);

	lbaf_count = nvme_ns_nlbaf_get(ns) + 1;

	max_lbaf_count = sizeof(ns->raw_data.lbafs)/
		sizeof(struct _nvme_spec_lbaf);

	if (lbaf_count > max_lbaf_count) {
		_nvme_err_msg_set(err_msg, "Invalid NVMe data, got %" PRIu8
				  "(should be less than %zu) LBA format",
				  lbaf_count, max_lbaf_count);
		errno = NVME_ERR_BUG;
		return NVME_ERR_BUG;
	}

	lbafs = (struct nvme_lbaf **) calloc(lbaf_count,
					     sizeof(struct nvme_lbaf *));

	if (lbafs == NULL)
		goto nomem;

	for (i = 0; i < lbaf_count; ++i) {
		lbaf = (struct nvme_lbaf *) calloc(1, sizeof(struct nvme_lbaf));
		if (lbaf == NULL)
			goto nomem;
		lbafs[i] = lbaf;
		memcpy(&(lbaf->raw_data),
		       &(ns->raw_data.lbafs[i]),
		       sizeof(struct _nvme_spec_lbaf));
	}
	ns->lbafs = lbafs;
	return NVME_OK;

nomem:
	if (lbafs != NULL)
		for (i = 0; i < lbaf_count; ++i)
			free(lbafs[i]);
	free(lbafs);
	ns->lbafs = NULL;
	errno = ENOMEM;
	return NVME_ERR_NO_MEMORY;
}

const uint8_t *nvme_ns_nvmcap_get(struct nvme_ns *ns)
{
	assert(ns != NULL);
	errno = 0;
	return ns->raw_data.nvmcap;
}

uint32_t nvme_ns_ns_id_get(struct nvme_ns *ns)
{
	assert(ns != NULL);
	errno = 0;
	return ns->ns_id;
}
