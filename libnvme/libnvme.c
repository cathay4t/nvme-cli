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

#include <libnvme/libnvme.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <linux/limits.h>

#include "utils.h"
#include "sysfs.h"
#include "ctrl.h"
#include "ioctl.h"
#include "ns.h"

struct _num_str_conv {
	const int num;
	const char *str;
};

static const struct _num_str_conv _NVME_RC_MSG_CONV[] = {
	{NVME_OK, "OK"},
	{NVME_ERR_NO_MEMORY, "Out of memory"},
	{NVME_ERR_BUG, "BUG of libnvme library"},
	{NVME_ERR_PERMISSION_DENY, "Permission deny"},
	{NVME_ERR_INVALID_ARGUMENT, "Invalid argument"},
};

int nvme_ctrls_get(struct nvme_ctrl ***cnts, uint32_t *cnt_count,
		   const char **err_msg)
{
	int rc = NVME_OK;
	uint32_t i = 0;
	const char **dev_paths = NULL;
	uint32_t count = 0;
	char err_msg_buff[_NVME_ERR_MSG_BUFF_LEN];
	int fd = -1;

	assert(cnts != NULL);
	assert(cnt_count != NULL);

	_nvme_err_msg_clear(err_msg_buff);

	*cnts = NULL;
	*cnt_count = 0;
	if (err_msg != NULL)
		*err_msg = NULL;

	_good(_nvme_ctrl_dev_paths_get(&dev_paths, &count, err_msg_buff),
	      rc, out);

	if (count == 0)
		goto out;

	*cnts = (struct nvme_ctrl **) calloc(count, sizeof(struct nvme_ctrl *));
	_alloc_null_check(err_msg_buff, *cnts, rc, out);
	*cnt_count = count;

	for (i = 0; i < count; ++i) {
		_good(_nvme_ioctl_fd_open(dev_paths[i], &fd, err_msg_buff),
		      rc, out);
		_good(_nvme_ctrl_get_by_fd(fd, &((*cnts)[i]), dev_paths[i],
					   err_msg_buff),
		      rc, out);
		close(fd);
		fd = -1;
	}

out:
	if (fd >= 0)
		close(fd);
	_nvme_ctrl_dev_paths_free(dev_paths, count);
	if (rc != NVME_OK) {
		nvme_ctrls_free(*cnts, *cnt_count);
		*cnts = NULL;
		*cnt_count = 0;
		if (err_msg != NULL)
			*err_msg = strdup(err_msg_buff);
			/* Ignore the error of no memory */
	}
	return rc;
}

void nvme_ctrls_free(struct nvme_ctrl **cnts, uint32_t cnt_count)
{
	uint32_t i = 0;

	if ((cnts == NULL) || (cnt_count == 0))
		return;

	for (i = 0; i < cnt_count; ++i)
		nvme_ctrl_free(cnts[i]);
	free(cnts);
}

const char *nvme_strerror(int rc)
{
	size_t i = 0;
	for (; i < sizeof(_NVME_RC_MSG_CONV)/sizeof(_NVME_RC_MSG_CONV[0]);
	     ++i) {
		if (_NVME_RC_MSG_CONV[i].num == rc)
			return _NVME_RC_MSG_CONV[i].str;
	}
	return "Invalid argument";
}

int nvme_ctrl_get_by_fd(int fd, struct nvme_ctrl **cnt, const char **err_msg)
{
	int rc = NVME_OK;
	char err_msg_buff[_NVME_ERR_MSG_BUFF_LEN];

	assert(cnt != NULL);
	assert(fd >= 0);

	*cnt = NULL;
	if (err_msg != NULL)
		*err_msg = NULL;

	rc = _nvme_ctrl_get_by_fd(fd, cnt, NULL /* unknown dev path */,
				  err_msg_buff);
	if ((rc != NVME_OK) && (err_msg != NULL))
		*err_msg = strdup(err_msg_buff);
		/* Ignore the error of no memory */

	return rc;
}

int nvme_nss_get(const char *dev_path, struct nvme_ns ***nss,
		 uint32_t *ns_count, const char **err_msg)
{
	int rc = NVME_OK;
	char err_msg_buff[_NVME_ERR_MSG_BUFF_LEN];
	int fd = -1;
	char ctrl_dev_path[PATH_MAX];
	uint32_t *ns_ids = NULL;
	uint32_t ns_id_count = 0;
	uint32_t i = 0;
	struct nvme_ctrl *cnt = NULL;

	assert(dev_path != NULL);
	assert(nss != NULL);
	assert(ns_count != NULL);
	_nvme_err_msg_clear(err_msg_buff);

	*nss = NULL;
	*ns_count = 0;
	_good(_validate_nvme_dev_path(dev_path, err_msg_buff), rc, out);
	_good(_extract_ctrl_dev_path(dev_path, ctrl_dev_path, err_msg_buff),
	      rc, out);
	_good(_nvme_ioctl_fd_open(ctrl_dev_path, &fd, err_msg_buff), rc, out);
	_good(nvme_ctrl_get_by_fd(fd, &cnt, err_msg), rc, out);
	_good(_nvme_ctrl_active_ns_id_list_get(fd, cnt, &ns_ids, &ns_id_count,
					       err_msg_buff),
	      rc, out);
	*nss = (struct nvme_ns **)
		calloc(ns_id_count, sizeof(struct nvme_ns *));
	_alloc_null_check(err_msg_buff, *nss, rc, out);
	*ns_count = ns_id_count;

	for (i = 0; i < ns_id_count; ++i)
		_good(_nvme_ns_new(fd, cnt, ctrl_dev_path, ns_ids[i],
				   &((*nss)[i]), err_msg_buff), rc, out);

out:
	free(ns_ids);
	nvme_ctrl_free(cnt);
	if (fd >= 0)
		close(fd);
	if (rc != NVME_OK) {
		nvme_nss_free(*nss, *ns_count);
		*nss = NULL;
		*ns_count = 0;
		if (err_msg != NULL)
			*err_msg = strdup(err_msg_buff);
			/* Ignore the error of no memory */
	}
	return rc;
}

void nvme_nss_free(struct nvme_ns **nss, uint32_t ns_count)
{
	uint32_t i = 0;

	if ((nss == NULL) || (ns_count == 0))
		return;

	for (i = 0; i < ns_count; ++i)
		nvme_ns_free(nss[i]);
	free(nss);
}
