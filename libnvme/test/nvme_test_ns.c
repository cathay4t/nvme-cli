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
#include <stdint.h>
#include <inttypes.h>
#include <stdio.h>
#include <assert.h>
#include <endian.h>

#include <libnvme/libnvme.h>

#include "nvme_test_ns.h"
#include "nvme_test_utils.h"

static int _test_ns(struct nvme_ns *ns);

static int _test_ns(struct nvme_ns *ns)
{
	int rc = NVME_OK;
	assert(ns != NULL);

	printf("\nName space ID: %" PRIu32 "\n\n", nvme_ns_ns_id_get(ns));

	_print_prop_char(nvme_ns, ns, dev_path);
	_print_prop_u64(nvme_ns, ns, nsze);
	_print_prop_u64(nvme_ns, ns, ncap);
	_print_prop_u64(nvme_ns, ns, nuse);
	_print_prop_u8(nvme_ns, ns, nsfeat);
	_print_prop_u8(nvme_ns, ns, nlbaf);
	_print_prop_u8(nvme_ns, ns, flbas);
	_print_prop_u8(nvme_ns, ns, mc);
	_print_prop_u8(nvme_ns, ns, dpc);
	_print_prop_u8(nvme_ns, ns, dps);
	_print_prop_u8(nvme_ns, ns, nmic);
	_print_prop_u8(nvme_ns, ns, rescap);
	_print_prop_u8(nvme_ns, ns, fpi);
	_print_prop_u8(nvme_ns, ns, elfeat);
	_print_prop_u16(nvme_ns, ns, nawun);
	_print_prop_u16(nvme_ns, ns, nacwu);
	_print_prop_u16(nvme_ns, ns, nabsn);
	_print_prop_u16(nvme_ns, ns, nabo);
	_print_prop_u16(nvme_ns, ns, nabspf);
	_print_prop_u16(nvme_ns, ns, noiob);
	printf("nvmcap: %" PRIu64 "\n",
	       le64toh(*((uint64_t *) nvme_ns_nvmcap_get(ns))));
	_print_prop_char(nvme_ns, ns, nguid);
	_print_prop_char(nvme_ns, ns, eui64);

	return rc;
}

int _nvme_ns_test(const char *blk_path)
{
	struct nvme_ns **nss = NULL;
	uint32_t ns_count = 0;
	uint32_t i = 0;
	int rc = NVME_OK;
	const char *err_msg = NULL;

	rc = nvme_nss_get(blk_path, &nss, &ns_count, &err_msg);
	if (rc != NVME_OK)
		goto out;

	for (i = 0; i < ns_count; ++i) {
		if (_test_ns(nss[i]) != NVME_OK) {
			rc = NVME_ERR_BUG;
			goto out;
		}
	}

out:
	if (rc != NVME_OK)
		printf("%s", err_msg);
	if (err_msg != NULL)
		free((void *) err_msg);

	nvme_nss_free(nss, ns_count);

	return rc == NVME_OK ? 0 : 1;
}
