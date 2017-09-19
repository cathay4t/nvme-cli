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

#include "nvme_test_utils.h"
#include "nvme_test_ns.h"

static void _test_nvme_ctrl_pro_query(struct nvme_ctrl *cnt);

static void _test_nvme_psd_pro_query(struct nvme_psd *psd);

static void _test_nvme_ctrl_raw_id_data(struct nvme_ctrl *cnt);

static void _test_nvme_psd_pro_query(struct nvme_psd *psd)
{
	_print_prop_u16(nvme_psd, psd, mp);
	_print_prop_u8(nvme_psd, psd, mxps);
	_print_prop_u8(nvme_psd, psd, nops);
	_print_prop_u32(nvme_psd, psd, enlat);
	_print_prop_u32(nvme_psd, psd, exlat);
	_print_prop_u8(nvme_psd, psd, rrt);
	_print_prop_u8(nvme_psd, psd, rrl);
	_print_prop_u8(nvme_psd, psd, rwt);
	_print_prop_u8(nvme_psd, psd, rwl);
	_print_prop_u16(nvme_psd, psd, idlp);
	_print_prop_u8(nvme_psd, psd, ips);
	_print_prop_u16(nvme_psd, psd, actp);
	_print_prop_u8(nvme_psd, psd, apw);
	_print_prop_u8(nvme_psd, psd, aps);
}

static void _test_nvme_ctrl_pro_query(struct nvme_ctrl *cnt)
{
	struct nvme_psd **psds = NULL;
	uint8_t i = 0;
	uint8_t psd_count = 0;

	_print_prop_char(nvme_ctrl, cnt, dev_path);
	_print_prop_u16(nvme_ctrl, cnt, vid);
	_print_prop_u16(nvme_ctrl, cnt, ssvid);
	_print_prop_char(nvme_ctrl, cnt, sn);
	_print_prop_char(nvme_ctrl, cnt, mn);
	_print_prop_char(nvme_ctrl, cnt, fr);
	_print_prop_u8(nvme_ctrl, cnt, rab);
	_print_prop_u32(nvme_ctrl, cnt, ieee);
	_print_prop_u8(nvme_ctrl, cnt, cmic);
	_print_prop_u8(nvme_ctrl, cnt, mdts);
	_print_prop_u16(nvme_ctrl, cnt, cntlid);
	_print_prop_u32(nvme_ctrl, cnt, ver);
	_print_prop_char(nvme_ctrl, cnt, ver_str);
	_print_prop_u32(nvme_ctrl, cnt, rtd3r);
	_print_prop_u32(nvme_ctrl, cnt, rtd3e);
	_print_prop_u16(nvme_ctrl, cnt, oaes);
	_print_prop_u32(nvme_ctrl, cnt, ctratt);
	_print_prop_char(nvme_ctrl, cnt, fguid);
	_print_prop_u16(nvme_ctrl, cnt, oacs);
	_print_prop_u8(nvme_ctrl, cnt, acl);
	_print_prop_u8(nvme_ctrl, cnt, aerl);
	_print_prop_u8(nvme_ctrl, cnt, frmw);
	_print_prop_u8(nvme_ctrl, cnt, lpa);
	_print_prop_u8(nvme_ctrl, cnt, elpe);
	_print_prop_u8(nvme_ctrl, cnt, npss);
	_print_prop_u8(nvme_ctrl, cnt, avscc);
	_print_prop_u8(nvme_ctrl, cnt, apsta);
	_print_prop_u16(nvme_ctrl, cnt, wctemp);
	_print_prop_u16(nvme_ctrl, cnt, cctemp);
	_print_prop_u16(nvme_ctrl, cnt, mtfa);
	_print_prop_u32(nvme_ctrl, cnt, hmpre);
	_print_prop_u32(nvme_ctrl, cnt, hmmin);
	printf("tnvmcap: %" PRIu64 "\n",
	       le64toh(*((uint64_t *) nvme_ctrl_tnvmcap_get(cnt))));
	printf("unvmcap: %" PRIu64 "\n",
	       le64toh(*((uint64_t *) nvme_ctrl_unvmcap_get(cnt))));
	_print_prop_u32(nvme_ctrl, cnt, rpmbs);
	_print_prop_u16(nvme_ctrl, cnt, edstt);
	_print_prop_u8(nvme_ctrl, cnt, esto);
	_print_prop_u8(nvme_ctrl, cnt, fwug);
	_print_prop_u16(nvme_ctrl, cnt, kas);
	_print_prop_u16(nvme_ctrl, cnt, hctma);
	_print_prop_u16(nvme_ctrl, cnt, mntmt);
	_print_prop_u16(nvme_ctrl, cnt, mxtmt);
	_print_prop_u32(nvme_ctrl, cnt, sanicap);
	_print_prop_u8(nvme_ctrl, cnt, sqes);
	_print_prop_u8(nvme_ctrl, cnt, cqes);
	_print_prop_u16(nvme_ctrl, cnt, maxcmd);
	_print_prop_u32(nvme_ctrl, cnt, nn);
	_print_prop_u16(nvme_ctrl, cnt, oncs);
	_print_prop_u16(nvme_ctrl, cnt, fuses);
	_print_prop_u8(nvme_ctrl, cnt, fna);
	_print_prop_u8(nvme_ctrl, cnt, vwc);
	_print_prop_u16(nvme_ctrl, cnt, awun);
	_print_prop_u16(nvme_ctrl, cnt, awupf);
	_print_prop_u8(nvme_ctrl, cnt, nvscc);
	_print_prop_u16(nvme_ctrl, cnt, acwu);
	_print_prop_u16(nvme_ctrl, cnt, sgls);
	_print_prop_char(nvme_ctrl, cnt, subnqn);
	_print_prop_u32(nvme_ctrl, cnt, ioccsz);
	_print_prop_u32(nvme_ctrl, cnt, iorcsz);
	_print_prop_u16(nvme_ctrl, cnt, icdoff);
	_print_prop_u8(nvme_ctrl, cnt, ctrattr);
	_print_prop_u8(nvme_ctrl, cnt, msdbd);

	psds = nvme_ctrl_psds_get(cnt);
	psd_count = nvme_ctrl_npss_get(cnt) + 1;
	printf("Got %" PRIu8 " PSD\n", psd_count);

	for (i = 0; i < psd_count; ++i) {
		_test_nvme_psd_pro_query(psds[i]);
	}
}

static void _test_nvme_ctrl_raw_id_data(struct nvme_ctrl *cnt)
{
	const uint8_t *raw_id_data = NULL;
	size_t i = 0;
	int j = 0;

	raw_id_data = nvme_ctrl_raw_id_data_get(cnt);

	printf("control identify raw data:\n");

	for(; i < NVME_CTRL_RAW_IDENTIFY_DATA_LEN; ++i) {
		++j;
		if (j >= 50) {
			printf("%02x\n", raw_id_data[i]);
			j = 0;
		} else {
			printf("%02x", raw_id_data[i]);
		}
	}
	printf("\n");
}

int main(void) {
	int rc = NVME_OK;
	uint32_t cnt_count = 0;
	struct nvme_ctrl **cnts = NULL;
	const char *err_msg = NULL;
	uint32_t i = 0;

	rc = nvme_ctrls_get(&cnts, &cnt_count, &err_msg);
	if (rc != NVME_OK) {
		printf("FAIL: Cannot get NVMe controller list %d: %s\n",
		       rc, err_msg);
		free((char *) err_msg);
		goto out;
	} else {
		printf("PASS: Got %" PRIu32 " NVMe controller(s)\n", cnt_count);
	}

	for (i = 0; i < cnt_count; ++i) {
		_test_nvme_ctrl_raw_id_data(cnts[i]);
		_test_nvme_ctrl_pro_query(cnts[i]);
		if (nvme_ctrl_ver_get(cnts[i]) >= NVME_SPEC_VERSION(1, 2, 0))
			printf("INFO: We are facing 1.2.0+ NVMe "
			       "implementation.\n");
		if (_nvme_ns_test(nvme_ctrl_dev_path_get(cnts[i])) != 0)
			goto out;
	}

out:
	nvme_ctrls_free(cnts, cnt_count);
	if (rc == NVME_OK)
		exit(EXIT_SUCCESS);
	else
		exit(EXIT_FAILURE);
}
