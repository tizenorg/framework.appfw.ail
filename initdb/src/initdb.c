/*
 * ail
 *
 * Copyright (c) 2000 - 2011 Samsung Electronics Co., Ltd. All rights reserved.
 *
 * Contact: Jayoun Lee <airjany@samsung.com>, Sewook Park <sewook7.park@samsung.com>, Jaeho Lee <jaeho81.lee@samsung.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */



#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>

#include "ail.h"
#include "ail_private.h"

#define OWNER_ROOT 0
#define GROUP_MENU 6010
#define BUFSZE 1024
#define OPT_DESKTOP_DIRECTORY "/opt/share/applications"
#define USR_DESKTOP_DIRECTORY "/usr/share/applications"
#define APP_INFO_DB_FILE "/opt/dbspace/.app_info.db"

#ifdef _E
#undef _E
#endif
#define _E(fmt, arg...) fprintf(stderr, "[AIL_INITDB][E][%s,%d] "fmt"\n", __FUNCTION__, __LINE__, ##arg);

#ifdef _D
#undef _D
#endif
#define _D(fmt, arg...) fprintf(stderr, "[AIL_INITDB][D][%s,%d] "fmt"\n", __FUNCTION__, __LINE__, ##arg);

static int initdb_count_app(void)
{
	ail_filter_h filter;
	ail_error_e ret;
	int total = 0;

	ret = ail_filter_new(&filter);
	if (ret != AIL_ERROR_OK) {
		return -1;
	}

	ret = ail_filter_add_bool(filter, AIL_PROP_NODISPLAY_BOOL, false);
	if (ret != AIL_ERROR_OK) {
		ail_filter_destroy(filter);
		return -1;
	}

	ret = ail_filter_count_appinfo(filter, &total);
	if (ret != AIL_ERROR_OK) {
		ail_filter_destroy(filter);
		return -1;
	}

	ail_filter_destroy(filter);

	return total;
}



char* _desktop_to_package(const char* desktop)
{
	char *package, *tmp;

	retv_if(!desktop, NULL);

	package = strdup(desktop);
	retv_if(!package, NULL);

	tmp = strrchr(package, '.');
	if(tmp == NULL) {
		_E("(tmp == NULL) return\n");
		free(package);
		return NULL;
	}

	if (strcmp(tmp, ".desktop")) {
		_E("%s is not a desktop file", desktop);
		free(package);
		return NULL;
	}

	*tmp = '\0';

	return package;
}



int initdb_load_directory(const char *directory)
{
	DIR *dir;
	struct dirent entry, *result;
	int len, ret;
	char buf[BUFSZE];

	// desktop file
	dir = opendir(directory);
	if (!dir) {
		if (strerror_r(errno, buf, sizeof(buf)) == 0)
			_E("Failed to access the [%s] because %s\n", directory, buf);
		return AIL_ERROR_FAIL;
	}

	len = strlen(directory) + 1;
	_D("Loading desktop files from %s\n", directory);

	for (ret = readdir_r(dir, &entry, &result);
			ret == 0 && result != NULL;
			ret = readdir_r(dir, &entry, &result)) {
		char *package;

		if (entry.d_name[0] == '.') continue;

		package = _desktop_to_package(entry.d_name);
		if (!package) {
			_E("Failed to convert file to package[%s]\n", entry.d_name);
			continue;
		}

		if (ail_desktop_add(package) != AIL_ERROR_OK) {
			_E("Failed to add a package[%s]\n", package);
		}

		free(package);
	}

	closedir(dir);

	return AIL_ERROR_OK;
}



static int initdb_change_perm(const char *db_file)
{
	char buf[BUFSZE];
	char journal_file[BUFSZE];
	char *files[3];
	int ret, i;

	files[0] = (char *)db_file;
	files[1] = journal_file;
	files[2] = NULL;

	retv_if(!db_file, AIL_ERROR_FAIL);

	snprintf(journal_file, sizeof(journal_file), "%s%s", db_file, "-journal");

	for (i = 0; files[i]; i++) {
		ret = chown(files[i], OWNER_ROOT, GROUP_MENU);
		if (ret == -1) {
			strerror_r(errno, buf, sizeof(buf));
			_E("FAIL : chown %s %d.%d, because %s", db_file, OWNER_ROOT, GROUP_MENU, buf);
			return AIL_ERROR_FAIL;
		}

		ret = chmod(files[i], S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
		if (ret == -1) {
			strerror_r(errno, buf, sizeof(buf));
			_E("FAIL : chmod %s 0664, because %s", db_file, buf);
			return AIL_ERROR_FAIL;
		}
	}

	return AIL_ERROR_OK;
}


static int __is_authorized()
{
	/* ail_init db should be called by as root privilege. */

	uid_t uid = getuid();
	if ((uid_t) 0 == uid)
		return 1;
	else
		return 0;
}


int main(int argc, char *argv[])
{
	int ret;

	if (!__is_authorized()) {
		fprintf(stderr, "You are not an authorized user!\n");
		_D("You are not an authorized user!\n");
		return AIL_ERROR_FAIL;
	}

	ret = setenv("AIL_INITDB", "1", 1);
	_D("AIL_INITDB : %d", ret);

	ret = initdb_count_app();
	if (ret > 0) {
		_D("Some Apps in the App Info DB.");
		return AIL_ERROR_OK;
	}

	ret = initdb_load_directory(OPT_DESKTOP_DIRECTORY);
	if (ret == AIL_ERROR_FAIL) {
		_E("cannot load opt desktop directory.");
		return AIL_ERROR_FAIL;
	}

	ret = initdb_load_directory(USR_DESKTOP_DIRECTORY);
	if (ret == AIL_ERROR_FAIL) {
		_E("cannot load usr desktop directory.");
		return AIL_ERROR_FAIL;
	}

	ret = initdb_change_perm(APP_INFO_DB_FILE);
	if (ret == AIL_ERROR_FAIL) {
		_E("cannot chown.");
		return AIL_ERROR_FAIL;
	}

	return AIL_ERROR_OK;
}



// END
