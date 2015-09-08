/*
 * ail
 *
 * Copyright (c) 2000 - 2014 Samsung Electronics Co., Ltd. All rights reserved.
 *
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

#include <ail.h>
#include "ail_private.h"

struct _ail_map_t {
	int prop;
	const char *property;
};



static ail_cb_ret_e appinfo_func(const ail_appinfo_h appinfo, void *user_data)
{
	char *rs = NULL;
	ail_cb_ret_e ret = AIL_CB_RET_CONTINUE;

	ret = AIL_CB_RET_CONTINUE;

	ail_appinfo_get_str(appinfo, AIL_PROP_PACKAGE_STR, &rs);
	printf("%s|", rs);
	printf("\n");
	return ret;
}

int main(int argc, char** argv)
{
	ail_error_e ret = AIL_ERROR_OK;
	ail_appinfo_h handle;
	ail_filter_h filter;
	int i;
	int retval;

	int ivalue = -1;
	bool bvalue = false;
	char *svalue = NULL;
	char *appid=NULL;
	char line[256] = {0,};

	FILE *fp = NULL;

	if (argc != 3){
		fprintf(stdout, "Usage: ail_test <api_type> <file_name>\n");
		fprintf(stdout, "api_type : 1 for get_appinfo APIs, 2 for filter APIs\n");
		fprintf(stdout,"file_name: file containing list of APPs\n");
		return -1;
	}

	fp = fopen(argv[2], "r");
	if (!fp){
		fprintf(stderr, "Unable to file containing app-id list\n");
		return -1;
	}


	while (fgets(line, sizeof(line), fp)) {
		appid = line;
		line[strlen(appid)-1] = '\0';

		switch(atoi(argv[1])){
		case 1:
			/*****************ail_get_appinfo start******************/
			fprintf(stdout, "Test ail_get_appinfo Begin for appid=%s\n",appid);
			/*****************ail_get_appinfo_new end******************/

			ret = ail_get_appinfo(appid, &handle);
			if (ret != AIL_ERROR_OK) {
				fprintf(stderr, "Test ail_get_appinfo failed\n");
				return AIL_ERROR_FAIL;
			}else{
				fprintf(stdout, "Test ail_get_appinfo Pass\n");
			}
			fprintf(stdout, "Test ail_get_appinfo End\n");

			ret = ail_appinfo_get_int(handle, AIL_PROP_X_SLP_INSTALLEDTIME_INT, &ivalue);
			if (ret != AIL_ERROR_OK) {
				fprintf(stderr, "Test ail_appinfo_get_int failed\n");
				return AIL_ERROR_FAIL;
			}
			fprintf(stdout, "AIL_PROP_X_SLP_INSTALLEDTIME_INT [%s] : %d\n", appid, ivalue);

			ret = ail_appinfo_get_str(handle, AIL_PROP_NAME_STR, &svalue);
			if (ret != AIL_ERROR_OK) {
				fprintf(stderr, "Test ail_appinfo_get_str failed\n");
				return AIL_ERROR_FAIL;
			}
			fprintf(stdout, "AIL_PROP_NAME_STR[%s] : %s\n", appid, svalue);

			ret = ail_appinfo_get_str(handle, AIL_PROP_PACKAGE_STR, &svalue);
			if (ret != AIL_ERROR_OK) {
				fprintf(stderr, "Test ail_appinfo_get_str failed\n");
				return AIL_ERROR_FAIL;
			}
			fprintf(stdout, "AIL_PROP_PACKAGE_STR [%s] : %s\n", appid, svalue);

			ret = ail_appinfo_get_str(handle, AIL_PROP_EXEC_STR, &svalue);
			if (ret != AIL_ERROR_OK) {
				fprintf(stderr, "Test ail_appinfo_get_str failed\n");
				return AIL_ERROR_FAIL;
			}
			fprintf(stdout, "AIL_PROP_EXEC_STR [%s] : %s\n", appid, svalue);

			ret = ail_appinfo_get_str(handle, AIL_PROP_TYPE_STR, &svalue);
			if (ret != AIL_ERROR_OK) {
				fprintf(stderr, "Test ail_appinfo_get_str failed\n");
				return AIL_ERROR_FAIL;
			}
			fprintf(stdout, "AIL_PROP_TYPE_STR [%s] : %s\n", appid, svalue);

			ret = ail_appinfo_get_str(handle, AIL_PROP_ICON_STR, &svalue);
			if (ret != AIL_ERROR_OK) {
				fprintf(stderr, "Test ail_appinfo_get_str failed\n");
				return AIL_ERROR_FAIL;
			}
			fprintf(stdout, "AIL_PROP_ICON_STR [%s] : %s\n", appid, svalue);

			ret = ail_appinfo_get_str(handle, AIL_PROP_VERSION_STR, &svalue);
			if (ret != AIL_ERROR_OK) {
				fprintf(stderr, "Test ail_appinfo_get_str failed\n");
				return AIL_ERROR_FAIL;
			}
			fprintf(stdout, "AIL_PROP_VERSION_STR [%s] : %s\n", appid, svalue);

			ret = ail_appinfo_get_str(handle, AIL_PROP_MIMETYPE_STR, &svalue);
			if (ret != AIL_ERROR_OK) {
				fprintf(stderr, "Test ail_appinfo_get_str failed\n");
				return AIL_ERROR_FAIL;
			}
			fprintf(stdout, "AIL_PROP_MIMETYPE_STR Type [%s] : %s\n", appid, svalue);

			ret = ail_appinfo_get_str(handle, AIL_PROP_X_SLP_SERVICE_STR, &svalue);
			if (ret != AIL_ERROR_OK) {
				fprintf(stderr, "Test ail_appinfo_get_str failed\n");
				return AIL_ERROR_FAIL;
			}
			fprintf(stdout, "AIL_PROP_X_SLP_SERVICE_STR [%s] : %s\n", appid, svalue);

			ret = ail_appinfo_get_str(handle, AIL_PROP_X_SLP_PACKAGETYPE_STR, &svalue);
			if (ret != AIL_ERROR_OK) {
				fprintf(stderr, "Test ail_appinfo_get_str failed\n");
				return AIL_ERROR_FAIL;
			}
			fprintf(stdout, "AIL_PROP_X_SLP_PACKAGECATEGORIES_STR [%s] : %s\n", appid, svalue);

			ret = ail_appinfo_get_str(handle, AIL_PROP_X_SLP_PACKAGECATEGORIES_STR, &svalue);
			if (ret != AIL_ERROR_OK) {
				fprintf(stderr, "Test ail_appinfo_get_str failed\n");
				return AIL_ERROR_FAIL;
			}
			fprintf(stdout, "AIL_PROP_X_SLP_PACKAGECATEGORIES_STR [%s] : %s\n", appid, svalue);

			ret = ail_appinfo_get_str(handle, AIL_PROP_X_SLP_PACKAGEID_STR, &svalue);
			if (ret != AIL_ERROR_OK) {
				fprintf(stderr, "Test ail_appinfo_get_str failed\n");
				return AIL_ERROR_FAIL;
			}
			fprintf(stdout, "AIL_PROP_X_SLP_PACKAGEID_STR [%s] : %s\n", appid, svalue);

			ret = ail_appinfo_get_str(handle, AIL_PROP_X_SLP_SVC_STR, &svalue);
			if (ret != AIL_ERROR_OK) {
				fprintf(stderr, "Test ail_appinfo_get_str failed\n");
				return AIL_ERROR_FAIL;
			}
			fprintf(stdout, "AIL_PROP_X_SLP_SVC_STR [%s] : %s\n", appid, svalue);

			ret = ail_appinfo_get_str(handle, AIL_PROP_X_SLP_EXE_PATH, &svalue);
			if (ret != AIL_ERROR_OK) {
				fprintf(stderr, "Test ail_appinfo_get_str failed\n");
				return AIL_ERROR_FAIL;
			}
			fprintf(stdout, "AIL_PROP_X_SLP_EXE_PATH [%s] : %s\n", appid, svalue);

			ret = ail_appinfo_get_str(handle, AIL_PROP_X_SLP_APPID_STR, &svalue);
			if (ret != AIL_ERROR_OK) {
				fprintf(stderr, "Test ail_appinfo_get_str failed\n");
				return AIL_ERROR_FAIL;
			}
			fprintf(stdout, "AIL_PROP_X_SLP_APPID_STR [%s] : %s\n", appid, svalue);

			ret = ail_appinfo_get_str(handle, AIL_PROP_X_SLP_PKGID_STR, &svalue);
			if (ret != AIL_ERROR_OK) {
				fprintf(stderr, "Test ail_appinfo_get_str failed\n");
				return AIL_ERROR_FAIL;
			}
			fprintf(stdout, "AIL_PROP_X_SLP_PKGID_STR [%s] : %s\n", appid, svalue);

			ret = ail_appinfo_get_str(handle, AIL_PROP_X_SLP_DOMAIN_STR, &svalue);
			if (ret != AIL_ERROR_OK) {
				fprintf(stderr, "Test ail_appinfo_get_str failed\n");
				return AIL_ERROR_FAIL;
			}
			fprintf(stdout, "AIL_PROP_X_SLP_DOMAIN_STR [%s] : %s\n", appid, svalue);

			ret = ail_appinfo_get_str(handle, AIL_PROP_X_SLP_SUBMODEMAINID_STR, &svalue);
			if (ret != AIL_ERROR_OK) {
				fprintf(stderr, "Test ail_appinfo_get_str failed\n");
				return AIL_ERROR_FAIL;
			}
			fprintf(stdout, "AIL_PROP_X_SLP_SUBMODEMAINID_STR, [%s] : %s\n", appid, svalue);

			ret = ail_appinfo_get_str(handle, AIL_PROP_X_SLP_INSTALLEDSTORAGE_STR, &svalue);
			if (ret != AIL_ERROR_OK) {
				fprintf(stderr, "Test ail_appinfo_get_str failed\n");
				return AIL_ERROR_FAIL;
			}
			fprintf(stdout, "AIL_PROP_X_SLP_INSTALLEDSTORAGE_STR [%s] : %s\n", appid, svalue);

			ret = ail_appinfo_get_bool(handle, AIL_PROP_X_SLP_TASKMANAGE_BOOL, &bvalue);
			if (ret != AIL_ERROR_OK) {
				fprintf(stderr, "Test ail_appinfo_get_str failed\n");
				return AIL_ERROR_FAIL;
			}
			fprintf(stdout, "AIL_PROP_X_SLP_TASKMANAGE_BOOL [%s] : %d\n", appid, bvalue);

			ret = ail_appinfo_get_bool(handle, AIL_PROP_X_SLP_MULTIPLE_BOOL, &bvalue);
			if (ret != AIL_ERROR_OK) {
				fprintf(stderr, "Test ail_appinfo_get_str failed\n");
				return AIL_ERROR_FAIL;
			}
			fprintf(stdout, "AIL_PROP_X_SLP_MULTIPLE_BOOL [%s] : %d\n", appid, bvalue);

			ret = ail_appinfo_get_bool(handle, AIL_PROP_X_SLP_REMOVABLE_BOOL, &bvalue);
			if (ret != AIL_ERROR_OK) {
				fprintf(stderr, "Test ail_appinfo_get_str failed\n");
				return AIL_ERROR_FAIL;
			}
			fprintf(stdout, "AIL_PROP_X_SLP_REMOVABLE_BOOL [%s] : %d\n", appid, bvalue);

			ret = ail_appinfo_get_bool(handle, AIL_PROP_X_SLP_SUBMODE_BOOL, &bvalue);
			if (ret != AIL_ERROR_OK) {
				fprintf(stderr, "Test ail_appinfo_get_str failed\n");
				return AIL_ERROR_FAIL;
			}
			fprintf(stdout, "AIL_PROP_X_SLP_SUBMODE_BOOL [%s] : %d\n", appid, bvalue);

			ret = ail_appinfo_get_bool(handle, AIL_PROP_X_SLP_ISHORIZONTALSCALE_BOOL, &bvalue);
			if (ret != AIL_ERROR_OK) {
				fprintf(stderr, "Test ail_appinfo_get_str failed\n");
				return AIL_ERROR_FAIL;
			}
			fprintf(stdout, "AIL_PROP_X_SLP_ISHORIZONTALSCALE_BOOL [%s] : %d\n", appid, bvalue);

			ret = ail_appinfo_get_bool(handle, AIL_PROP_X_SLP_ENABLED_BOOL, &bvalue);
			if (ret != AIL_ERROR_OK) {
				fprintf(stderr, "Test ail_appinfo_get_bool failed\n");
				return AIL_ERROR_FAIL;
			}
			fprintf(stdout, "AIL_PROP_X_SLP_ENABLED_BOOL [%s] : %d\n", appid, bvalue);

			ret = ail_appinfo_get_int(handle, AIL_PROP_X_SLP_TEMP_INT, &ivalue);
			if (ret != AIL_ERROR_OK) {
				fprintf(stderr, "Test ail_appinfo_get_int failed\n");
				return AIL_ERROR_FAIL;
			}
			fprintf(stdout, "AIL_PROP_X_SLP_TEMP_INT [%s] : %d\n", appid, ivalue);
			ail_destroy_appinfo(handle);
			break;
		case 2:
				ret = ail_get_appinfo(appid, &handle);
				if (ret != AIL_ERROR_OK) {
				fprintf(stderr, "Test ail_get_appinfo failed\n");
				return AIL_ERROR_FAIL;
			}else{
				fprintf(stdout, "Test ail_get_appinfo Pass\n");
			}

			ret = ail_filter_new(&filter);
                        if (ret != AIL_ERROR_OK) {
                            return -1;
                        }


			ret = ail_appinfo_get_int(handle, AIL_PROP_X_SLP_INSTALLEDTIME_INT, &ivalue);
			if (ret != AIL_ERROR_OK) {
				fprintf(stderr, "Test ail_appinfo_get_int failed\n");
				return AIL_ERROR_FAIL;
			}
			fprintf(stdout, "AIL_PROP_X_SLP_INSTALLEDTIME_INT [%s] : %d\n", appid, ivalue);
			ret = ail_filter_add_int(filter, AIL_PROP_X_SLP_INSTALLEDTIME_INT, ivalue);
			if (ret != AIL_ERROR_OK) {
				return -1;
			}

			ret = ail_filter_list_appinfo_foreach(filter, appinfo_func, (void *)&i);
                        if (ret != AIL_ERROR_OK){
				return -1;
			}
			ail_filter_destroy(filter);
			ret = ail_filter_new(&filter);
                        if (ret != AIL_ERROR_OK) {
                            return -1;
                        }


			ret = ail_appinfo_get_str(handle, AIL_PROP_PACKAGE_STR, &svalue);
			if (ret != AIL_ERROR_OK) {
				fprintf(stderr, "Test ail_appinfo_get_str failed\n");
				return AIL_ERROR_FAIL;
			}
			fprintf(stdout, "AIL_PROP_PACKAGE_STR [%s] : %s\n", appid, svalue);
			ret = ail_filter_add_str(filter, AIL_PROP_PACKAGE_STR, svalue);
			if (ret != AIL_ERROR_OK) {
				return -1;
			}

			ret = ail_filter_list_appinfo_foreach(filter, appinfo_func, (void *)&i);
                        if (ret != AIL_ERROR_OK){
				return -1;
			}
			ail_filter_destroy(filter);
			ret = ail_filter_new(&filter);
                        if (ret != AIL_ERROR_OK) {
                            return -1;
                        }

/*			ret = ail_appinfo_get_str(handle, AIL_PROP_EXEC_STR, &svalue);
			if (ret != AIL_ERROR_OK) {
				fprintf(stderr, "Test ail_appinfo_get_str failed\n");
				return AIL_ERROR_FAIL;
			}
			fprintf(stdout, "AIL_PROP_EXEC_STR [%s] : %s\n", appid, svalue);
			ret = ail_filter_add_str(filter, AIL_PROP_EXEC_STR, svalue);
			if (ret != AIL_ERROR_OK) {
				return -1;
			}

			ret = ail_filter_list_appinfo_foreach(filter, appinfo_func, (void *)&i);
                        if (ret != AIL_ERROR_OK){
				return -1;
			}
			ail_filter_destroy(filter);
			ret = ail_filter_new(&filter);
                        if (ret != AIL_ERROR_OK) {
                            return -1;
                        }

			ret = ail_appinfo_get_str(handle, AIL_PROP_TYPE_STR, &svalue);
			if (ret != AIL_ERROR_OK) {
				fprintf(stderr, "Test ail_appinfo_get_str failed\n");
				return AIL_ERROR_FAIL;
			}
			fprintf(stdout, "AIL_PROP_TYPE_STR [%s] : %s\n", appid, svalue);
			ret = ail_filter_add_str(filter, AIL_PROP_TYPE_STR, svalue);
			if (ret != AIL_ERROR_OK) {
				return -1;
			}

			ret = ail_filter_list_appinfo_foreach(filter, appinfo_func, (void *)&i);
			if (ret != AIL_ERROR_OK){
				return -1;
			}
			ail_filter_destroy(filter);
			ret = ail_filter_new(&filter);
                        if (ret != AIL_ERROR_OK) {
                            return -1;
                       }*/
	/*		ret = ail_appinfo_get_str(handle, AIL_PROP_ICON_STR, &svalue);
			if (ret != AIL_ERROR_OK) {
				fprintf(stderr, "Test ail_appinfo_get_str failed\n");
				return AIL_ERROR_FAIL;
			}
			fprintf(stdout, "AIL_PROP_ICON_STR [%s] : %s\n", appid, svalue);
			ret = ail_filter_add_str(filter, AIL_PROP_ICON_STR, svalue);
			if (ret != AIL_ERROR_OK) {
				return -1;
			}

			ret = ail_filter_list_appinfo_foreach(filter, appinfo_func, (void *)&i);
			if (ret != AIL_ERROR_OK){
				return -1;
			}
			ail_filter_destroy(filter); */
	/*		ret = ail_filter_new(&filter);
                        if (ret != AIL_ERROR_OK) {
                            return -1;
                        }
			ret = ail_appinfo_get_str(handle, AIL_PROP_VERSION_STR, &svalue);
			if (ret != AIL_ERROR_OK) {
				fprintf(stderr, "Test ail_appinfo_get_str failed\n");
				return AIL_ERROR_FAIL;
			}
			fprintf(stdout, "AIL_PROP_VERSION_STR [%s] : %s\n", appid, svalue);
			ret = ail_filter_add_str(filter, AIL_PROP_VERSION_STR, svalue);
			if (ret != AIL_ERROR_OK) {
				return -1;
			}

			ret = ail_filter_list_appinfo_foreach(filter, appinfo_func, (void *)&i);
			if (ret != AIL_ERROR_OK){
				return -1;
			}
			ail_filter_destroy(filter);
			ret = ail_filter_new(&filter);
                        if (ret != AIL_ERROR_OK) {
                            return -1;
                        }

			ret = ail_appinfo_get_str(handle, AIL_PROP_X_SLP_PACKAGETYPE_STR, &svalue);
			if (ret != AIL_ERROR_OK) {
				fprintf(stderr, "Test ail_appinfo_get_str failed\n");
				return AIL_ERROR_FAIL;
			}
			fprintf(stdout, "AIL_PROP_X_SLP_PACKAGETYPE_STR [%s] : %s\n", appid, svalue);

			ret = ail_filter_add_str(filter, AIL_PROP_X_SLP_PACKAGETYPE_STR, svalue);
			if (ret != AIL_ERROR_OK) {
				return -1;
			}

			ret = ail_filter_list_appinfo_foreach(filter, appinfo_func, (void *)&i);
			if (ret != AIL_ERROR_OK){
				return -1;
			}

			ail_filter_destroy(filter);

			ret = ail_filter_new(&filter);
			if (ret != AIL_ERROR_OK) {
				return -1;
			}

			ret = ail_appinfo_get_str(handle, AIL_PROP_X_SLP_PACKAGEID_STR, &svalue);
			if (ret != AIL_ERROR_OK) {
				fprintf(stderr, "Test ail_appinfo_get_str failed\n");
				return AIL_ERROR_FAIL;
			}
			fprintf(stdout, "AIL_PROP_X_SLP_PACKAGEID_STR [%s] : %s\n", appid, svalue);

			ret = ail_filter_add_str(filter, AIL_PROP_X_SLP_PACKAGEID_STR, svalue);
			if (ret != AIL_ERROR_OK) {
				return -1;
			}

			ret = ail_filter_list_appinfo_foreach(filter, appinfo_func, (void *)&i);
			if (ret != AIL_ERROR_OK){
				return -1;
			}

			ail_filter_destroy(filter);

			ret = ail_filter_new(&filter);
			if (ret != AIL_ERROR_OK) {
				return -1;
			}

			ret = ail_appinfo_get_str(handle, AIL_PROP_X_SLP_EXE_PATH, &svalue);
			if (ret != AIL_ERROR_OK) {
				fprintf(stderr, "Test ail_appinfo_get_str failed\n");
				return AIL_ERROR_FAIL;
			}
			fprintf(stdout, "AIL_PROP_X_SLP_EXE_PATH [%s] : %s\n", appid, svalue);

			ret = ail_filter_add_str(filter, AIL_PROP_X_SLP_EXE_PATH, svalue);
			if (ret != AIL_ERROR_OK) {
				return -1;
			}

			ret = ail_filter_list_appinfo_foreach(filter, appinfo_func, (void *)&i);
			if (ret != AIL_ERROR_OK){
				return -1;
			}

			ail_filter_destroy(filter);

			ret = ail_filter_new(&filter);
			if (ret != AIL_ERROR_OK) {
				return -1;
			}
			ret = ail_appinfo_get_str(handle, AIL_PROP_X_SLP_APPID_STR, &svalue);
			if (ret != AIL_ERROR_OK) {
				fprintf(stderr, "Test ail_appinfo_get_str failed\n");
				return AIL_ERROR_FAIL;
			}
			fprintf(stdout, "AIL_PROP_X_SLP_APPID_STR [%s] : %s\n", appid, svalue);

			ret = ail_filter_add_str(filter, AIL_PROP_X_SLP_APPID_STR, svalue);
			if (ret != AIL_ERROR_OK) {
				return -1;
			}

			ret = ail_filter_list_appinfo_foreach(filter, appinfo_func, (void *)&i);
			if (ret != AIL_ERROR_OK){
				return -1;
			}

			ail_filter_destroy(filter);

			ret = ail_filter_new(&filter);
			if (ret != AIL_ERROR_OK) {
				return -1;
			}
			ret = ail_appinfo_get_str(handle, AIL_PROP_X_SLP_PKGID_STR, &svalue);
			if (ret != AIL_ERROR_OK) {
				fprintf(stderr, "Test ail_appinfo_get_str failed\n");
				return AIL_ERROR_FAIL;
			}
			fprintf(stdout, "AIL_PROP_X_SLP_PKGID_STR [%s] : %s\n", appid, svalue);

			ret = ail_filter_add_str(filter, AIL_PROP_X_SLP_PKGID_STR, svalue);
			if (ret != AIL_ERROR_OK) {
				return -1;
			}

			ret = ail_filter_list_appinfo_foreach(filter, appinfo_func, (void *)&i);
			if (ret != AIL_ERROR_OK){
				return -1;
			}

			ail_filter_destroy(filter);

			ret = ail_filter_new(&filter);
			if (ret != AIL_ERROR_OK) {
				return -1;
			}*/
/*			ret = ail_appinfo_get_str(handle, AIL_PROP_X_SLP_DOMAIN_STR, &svalue);
			if (ret != AIL_ERROR_OK) {
				fprintf(stderr, "Test ail_appinfo_get_str failed\n");
				return AIL_ERROR_FAIL;
			}
			fprintf(stdout, "AIL_PROP_X_SLP_DOMAIN_STR [%s] : %s\n", appid, svalue);

			ret = ail_filter_add_str(filter, AIL_PROP_X_SLP_DOMAIN_STR, svalue);
			if (ret != AIL_ERROR_OK) {
				return -1;
			}

			ret = ail_filter_list_appinfo_foreach(filter, appinfo_func, (void *)&i);
			if (ret != AIL_ERROR_OK){
				return -1;
			}

			ail_filter_destroy(filter);

			ret = ail_filter_new(&filter);
			if (ret != AIL_ERROR_OK) {
				return -1;
			}
			ret = ail_appinfo_get_str(handle, AIL_PROP_X_SLP_SUBMODEMAINID_STR, &svalue);
			if (ret != AIL_ERROR_OK) {
				fprintf(stderr, "Test ail_appinfo_get_str failed\n");
				return AIL_ERROR_FAIL;
			}
			fprintf(stdout, "AIL_PROP_X_SLP_SUBMODEMAINID_STR, [%s] : %s\n", appid, svalue);

			ret = ail_filter_add_str(filter, AIL_PROP_X_SLP_SUBMODEMAINID_STR, svalue);
			if (ret != AIL_ERROR_OK) {
				return -1;
			}

			ret = ail_filter_list_appinfo_foreach(filter, appinfo_func, (void *)&i);
			if (ret != AIL_ERROR_OK){
				return -1;
			}

			ail_filter_destroy(filter);

			ret = ail_filter_new(&filter);
			if (ret != AIL_ERROR_OK) {
				return -1;
			}
*/	/*		ret = ail_appinfo_get_str(handle, AIL_PROP_X_SLP_INSTALLEDSTORAGE_STR, &svalue);
			if (ret != AIL_ERROR_OK) {
				fprintf(stderr, "Test ail_appinfo_get_str failed\n");
				return AIL_ERROR_FAIL;
			}
			fprintf(stdout, "AIL_PROP_X_SLP_INSTALLEDSTORAGE_STR [%s] : %s\n", appid, svalue);

			ret = ail_filter_add_str(filter, AIL_PROP_X_SLP_INSTALLEDSTORAGE_STR, svalue);
			if (ret != AIL_ERROR_OK) {
				return -1;
			}

			ret = ail_filter_list_appinfo_foreach(filter, appinfo_func, (void *)&i);
			if (ret != AIL_ERROR_OK){
				return -1;
			}

			ail_filter_destroy(filter);

			ret = ail_filter_new(&filter);
			if (ret != AIL_ERROR_OK) {
				return -1;
			}*/
/*			ret = ail_appinfo_get_bool(handle, AIL_PROP_X_SLP_TASKMANAGE_BOOL, &bvalue);
			if (ret != AIL_ERROR_OK) {
				fprintf(stderr, "Test ail_appinfo_get_str failed\n");
				return AIL_ERROR_FAIL;
			}
			fprintf(stdout, "AIL_PROP_X_SLP_TASKMANAGE_BOOL [%s] : %d\n", appid, bvalue);

			ail_filter_add_bool(filter, AIL_PROP_X_SLP_TASKMANAGE_BOOL, bvalue);
			if (ret != AIL_ERROR_OK) {
				return -1;
			}

			ret = ail_filter_list_appinfo_foreach(filter, appinfo_func, (void *)&i);
			if (ret != AIL_ERROR_OK){
				return -1;
			}

			ail_filter_destroy(filter);

			ret = ail_filter_new(&filter);
			if (ret != AIL_ERROR_OK) {
				return -1;
			}
			ret = ail_appinfo_get_bool(handle, AIL_PROP_X_SLP_MULTIPLE_BOOL, &bvalue);
			if (ret != AIL_ERROR_OK) {
				fprintf(stderr, "Test ail_appinfo_get_str failed\n");
				return AIL_ERROR_FAIL;
			}
			fprintf(stdout, "AIL_PROP_X_SLP_MULTIPLE_BOOL [%s] : %d\n", appid, bvalue);

			ret = ail_filter_add_bool(filter, AIL_PROP_X_SLP_MULTIPLE_BOOL, bvalue);
			if (ret != AIL_ERROR_OK) {
				return -1;
			}

			ret = ail_filter_list_appinfo_foreach(filter, appinfo_func, (void *)&i);
			if (ret != AIL_ERROR_OK){
				return -1;
			}

			ail_filter_destroy(filter);

			ret = ail_filter_new(&filter);
			if (ret != AIL_ERROR_OK) {
				return -1;
			}
*/			ret = ail_appinfo_get_bool(handle, AIL_PROP_X_SLP_REMOVABLE_BOOL, &bvalue);
			if (ret != AIL_ERROR_OK) {
				fprintf(stderr, "Test ail_appinfo_get_str failed\n");
				return AIL_ERROR_FAIL;
			}
			fprintf(stdout, "AIL_PROP_X_SLP_REMOVABLE_BOOL [%s] : %d\n", appid, bvalue);

			ret = ail_filter_add_bool(filter, AIL_PROP_X_SLP_REMOVABLE_BOOL, bvalue);
			if (ret != AIL_ERROR_OK) {
				return -1;
			}

			ret = ail_filter_list_appinfo_foreach(filter, appinfo_func, (void *)&i);
			if (ret != AIL_ERROR_OK){
				return -1;
			}

			ail_filter_destroy(filter);

	/*		ret = ail_filter_new(&filter);
			if (ret != AIL_ERROR_OK) {
				return -1;
			}
			ret = ail_appinfo_get_bool(handle, AIL_PROP_X_SLP_SUBMODE_BOOL, &bvalue);
			if (ret != AIL_ERROR_OK) {
				fprintf(stderr, "Test ail_appinfo_get_str failed\n");
				return AIL_ERROR_FAIL;
			}
			fprintf(stdout, "AIL_PROP_X_SLP_SUBMODE_BOOL [%s] : %d\n", appid, bvalue);

			ret = ail_filter_add_bool(filter, AIL_PROP_X_SLP_SUBMODE_BOOL, bvalue);
			if (ret != AIL_ERROR_OK) {
				return -1;
			}

			ret = ail_filter_list_appinfo_foreach(filter, appinfo_func, (void *)&i);
			if (ret != AIL_ERROR_OK){
				return -1;
			}

			ail_filter_destroy(filter);

			ret = ail_filter_new(&filter);
			if (ret != AIL_ERROR_OK) {
				return -1;
			}
			ret = ail_appinfo_get_bool(handle, AIL_PROP_X_SLP_ISHORIZONTALSCALE_BOOL, &bvalue);
			if (ret != AIL_ERROR_OK) {
				fprintf(stderr, "Test ail_appinfo_get_str failed\n");
				return AIL_ERROR_FAIL;
			}
			fprintf(stdout, "AIL_PROP_X_SLP_ISHORIZONTALSCALE_BOOL [%s] : %d\n", appid, bvalue);

			ret = ail_filter_add_bool(filter, AIL_PROP_X_SLP_ISHORIZONTALSCALE_BOOL, bvalue);
			if (ret != AIL_ERROR_OK) {
				return -1;
			}

			ret = ail_filter_list_appinfo_foreach(filter, appinfo_func, (void *)&i);
			if (ret != AIL_ERROR_OK){
				return -1;
			}

			ail_filter_destroy(filter);

			ret = ail_filter_new(&filter);
			if (ret != AIL_ERROR_OK) {
				return -1;
			}
			ret = ail_appinfo_get_bool(handle, AIL_PROP_X_SLP_ENABLED_BOOL, &bvalue);
			if (ret != AIL_ERROR_OK) {
				fprintf(stderr, "Test ail_appinfo_get_bool failed\n");
				return AIL_ERROR_FAIL;
			}
			fprintf(stdout, "AIL_PROP_X_SLP_ENABLED_BOOL [%s] : %d\n", appid, bvalue);

			ret = ail_filter_add_bool(filter, AIL_PROP_X_SLP_ENABLED_BOOL, bvalue);
			if (ret != AIL_ERROR_OK) {
				return -1;
			}

			ret = ail_filter_list_appinfo_foreach(filter, appinfo_func, (void *)&i);
			if (ret != AIL_ERROR_OK){
				return -1;
			}

			ail_filter_destroy(filter);

			ret = ail_filter_new(&filter);
			if (ret != AIL_ERROR_OK) {
				return -1;
			}
			ret = ail_appinfo_get_int(handle, AIL_PROP_X_SLP_TEMP_INT, &ivalue);
			if (ret != AIL_ERROR_OK) {
				fprintf(stderr, "Test ail_appinfo_get_int failed\n");
				return AIL_ERROR_FAIL;
			}
			fprintf(stdout, "AIL_PROP_X_SLP_TEMP_INT [%s] : %d\n", appid, ivalue);

			ret = ail_filter_add_int(filter, AIL_PROP_X_SLP_TEMP_INT, ivalue);
			if (ret != AIL_ERROR_OK) {
				return -1;
			}

			ret = ail_filter_list_appinfo_foreach(filter, appinfo_func, (void *)&i);
			if (ret != AIL_ERROR_OK){
				return -1;
			}

			ail_filter_destroy(filter);
*/
			ail_destroy_appinfo(handle);






			break;
		default:
			return -1;
		}
		memset(line, '\0', 256);
	}
	retval = fclose(fp);
	if (retval)
		printf("Closing of file failed");
	return EXIT_SUCCESS;
}
