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
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <linux/limits.h>
#include <inttypes.h>
#include <regex.h>
#include <unistd.h>
#include <stdbool.h>

#include "utils.h"

#define _NVME_DEV_PATH_REGEX		\
	"^/dev/nvme[0-9]\\{1,\\}\\(n[0-9]\\{1,\\}\\)\\{0,1\\}$"

const char *_u8_data_to_ascii(uint8_t *data, size_t size)
{
	char *rc_str = NULL;

	assert(data != NULL);
	assert(size != 0);

	rc_str = (char *) calloc(size + 1, sizeof(char));
	if (rc_str == NULL) {
		errno = ENOMEM;
		return NULL;
	}

	/* Remove trailing white space */
	while(size > 0) {
		if ((data[size - 1] == ' ') || (data[size - 1] == '\0'))
			--size;
		else
			break;
	}

	if (size > 0)
		memcpy(rc_str, data, size);

	errno = 0;
	return rc_str;
}

const char *_u8_data_to_hex_str(uint8_t *data, size_t size)
{
	char *rc_str = NULL;
	size_t str_len = 0;
	size_t i = 0;
	bool all_zero = true;

	str_len = size * 2 + 1;

	rc_str = calloc(1, sizeof(char) * str_len);

	if (rc_str == NULL) {
		errno = ENOMEM;
		return NULL;
	}

	for (i = 0; i < size; ++i) {
		if (data[i] != 0)
			all_zero = false;
		sprintf(&rc_str[i * 2], "%02x", data[i]);
	}
	if (all_zero == true)
		rc_str[0] = '\0';

	return rc_str;
}

int _validate_nvme_dev_path(const char *dev_path, char *err_msg)
{
	int rc = NVME_OK;
	int errno_save = 0;
	char strerr_buff[_NVME_ERR_MSG_BUFF_LEN];
	regex_t regex;
	int regex_rc = 0;

	/* Check exits of this file */
	if (access(dev_path, F_OK|R_OK) != 0) {
		errno_save = errno;
		rc = NVME_ERR_INVALID_ARGUMENT;
		_nvme_err_msg_set(err_msg, "Invalid NVMe device path: %s",
				  strerror_r(errno_save, strerr_buff,
					     _NVME_ERR_MSG_BUFF_LEN));
		goto out;
	}

	/* Check file name regex */
	regex_rc = regcomp(&regex, _NVME_DEV_PATH_REGEX, 0 /* no flag */);
	if (regex_rc != 0) {
		rc = NVME_ERR_BUG;
		_nvme_err_msg_set(err_msg, "BUG: _validate_nvme_dev_path(): "
				  "regcomp() failed with %d", regex_rc);
		goto out;
	}

	regex_rc = regexec(&regex, dev_path, 0 /* no match required */,
			   NULL /* no output match pointer */,
			   0 /* no flag */);

	if (regex_rc != 0) {
		rc = NVME_ERR_INVALID_ARGUMENT;
		_nvme_err_msg_set(err_msg, "Invalid NVMe device path: '%s', "
				  "should be in form of '/dev/nvme[0-9]+' "
				  "or '/dev/nvme[0-9]+n[0-9]+'", dev_path);
	}

	regfree(&regex);

out:
	return rc;
}

int _extract_ctrl_dev_path(const char *dev_path, char *nvme_ctrl_dev_path,
			   char *err_msg)
{
	int rc = NVME_OK;
	uint32_t ctrl_no = 0;
	int errno_save = 0;

	assert(dev_path != NULL);

	if (sscanf(dev_path, "/dev/nvme%" SCNu32, &ctrl_no) == EOF) {
		errno_save = errno;
		rc = NVME_ERR_BUG;
		_nvme_err_msg_set(err_msg, "BUG: _extract_ctrl_dev_path(): "
				  "sscanf() failed with %d on '%s'",
				  errno_save, dev_path);
		goto out;
	}
	snprintf(nvme_ctrl_dev_path, PATH_MAX, "/dev/nvme%" PRIu32, ctrl_no);

out:
	if (rc != NVME_OK)
		memset(nvme_ctrl_dev_path, 0, PATH_MAX);

	return rc;
}
