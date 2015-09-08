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
#include <ctype.h>
#include <db-util.h>
#include <vconf.h>
#include "ail.h"
#include "ail_private.h"
#include "ail_convert.h"
#include "ail_db.h"
#include "ail_sql.h"
#include "ail_package.h"
#include <assert.h>
#include <unistd.h>

#define LANGUAGE_LENGTH 2
#define DEFAULT_LOCALE		"No Locale"
#define MAX_QUERY_LEN	4096
#define PKG_SD_PATH "/opt/storage/sdcard/app2sd/"
#define QUERY_GET_LOCALNAME "select name from localname where package='%s' and locale='%s'"
#define MAX_SIZE 80000

struct ail_appinfo {
	char **values;
	sqlite3_stmt *stmt;
};

typedef struct _pkgmgr_locale_x {
	char *locale;
} pkgmgr_locale_x;

static char * __get_package(const char *appid);

char* _get_icon_from_package_localized_info(const char *appid)
{
	if (db_open_pkg_mgr(DB_OPEN_RO) < 0) {
		_E("pkg_mgr_db_open fail [%s]",appid);
		return NULL;
	}
	sqlite3_stmt *stmt = NULL;
	char *str = NULL;
	char *icon = NULL;
	char *package = NULL;
	package = __get_package(appid);

	char *query = sqlite3_mprintf("select package_localized_info.package_icon from package_localized_info where package_localized_info.package=%Q and package_localized_info.package_locale='No Locale'", package);
	if (NULL == query){
		_E("Memory allocation failed");
		free(package);
		return NULL;
	}
	free(package);

	retv_if (db_prepare(query, &stmt) < 0, NULL);

	do {
		if (db_step(stmt) < 0){
			goto end;
			}
		if (db_column_str(stmt, 0, &str) < 0)
			goto end;
		if (str)
			icon = strdup(str);
		else
			icon = NULL;


	} while(0);

end:
	db_finalize(stmt);
	sqlite3_free(query);
	return icon;

}

char* _get_icon_from_package_app_localized_info(const char *appid)
{
	if (db_open_pkg_mgr(DB_OPEN_RO) < 0) {
		_E("pkg_mgr_db_open fail [%s]",appid);
		return NULL;
	}
	sqlite3_stmt *stmt = NULL;
	char *str = NULL;
	char *icon = NULL;


	char *query = sqlite3_mprintf("select package_app_localized_info.app_icon from package_app_localized_info where package_app_localized_info.app_id=%Q and package_app_localized_info.app_locale='No Locale'", appid);
	if (NULL == query){
		_E("Memory allocation failed");
		return NULL;
	}

	retv_if (db_prepare(query, &stmt) < 0, NULL);

	do {
		if (db_step(stmt) < 0){
			goto end;
			}
		if (db_column_str(stmt, 0, &str) < 0)
			goto end;
		if (str)
			icon = strdup(str);
		else
			icon = NULL;


	} while(0);

end:
	db_finalize(stmt);
	sqlite3_free(query);
	return icon;

}

char* _get_mimetype_from_package_app_app_svc(const char *appid)
{

	if (db_open_pkg_mgr(DB_OPEN_RO) < 0) {
		_E("pkg_mgr_db_open fail [%s]",appid);
		return NULL;
	}
	sqlite3_stmt *stmt = NULL;
	int col = 0;
	char *val = NULL;
	char *ptr = NULL;
	char *del = ";";
	char tmp_mime[MAX_SIZE] = {0};
	int len = 0;
	char *mime = NULL;

	char *query = sqlite3_mprintf("select package_app_app_svc.mime_type from package_app_app_svc where package_app_app_svc.app_id=%Q", appid);
	if (NULL == query){
		_E("Memory allocation failed");
		return NULL;
	}
	retv_if (db_prepare(query, &stmt) < 0, NULL);

	while (db_step(stmt) == AIL_ERROR_OK){
		val = (char*)sqlite3_column_text(stmt, col);

		if (!strlen(val) )
			continue;

		strcat(tmp_mime, val);
		if(strlen(tmp_mime))
			strcat(tmp_mime, del);
	}

	len = strlen(tmp_mime);
	ptr = tmp_mime;
	ptr[len -1] = '\0';

	db_finalize(stmt);
	sqlite3_free(query);
	if(len >0){
		mime = strdup(tmp_mime);
		if(NULL == mime){
			_E("Memory Allocation Failed");
			return NULL;
		}else{
			return mime;
		}
	}else{
		return NULL;
	}
}


/* get the first locale value*/
static int __fallback_locale_cb(void *data, int ncols, char **coltxt, char **colname)
{
	pkgmgr_locale_x *info = (pkgmgr_locale_x *)data;

	if (ncols >= 1)
		info->locale = strdup(coltxt[0]);
	else
		info->locale = NULL;

	return 0;
}

static int __check_validation_of_qurey_cb(void *data, int ncols, char **coltxt, char **colname)
{
	int *p = (int*)data;
	*p = atoi(coltxt[0]);
	return 0;
}

static int __check_app_locale_from_app_localized_info_by_exact(const char *appid, const char *locale)
{
	int result_query = -1;

	char *query = sqlite3_mprintf("select exists(select app_locale from package_app_localized_info where app_id=%Q and app_locale=%Q)", appid, locale);
	if (NULL == query){
		_E("Memory allocation failed");
		return AIL_ERROR_OUT_OF_MEMORY;
	}

	db_exec_sqlite_query(query, __check_validation_of_qurey_cb, (void *)&result_query);
	sqlite3_free(query);

	return result_query;
}

static int __check_app_locale_from_app_localized_info_by_fallback(const char *appid, const char *locale)
{
	int result_query = -1;
	char lang[3] = {'\0'};
	strncpy(lang, locale, LANGUAGE_LENGTH);

	char *query = sqlite3_mprintf("select exists(select app_locale from package_app_localized_info where app_id=%Q and app_locale like %Q%Q)", appid, lang, locale);
	if (NULL == query){
		_E("Memory allocation failed");
		return AIL_ERROR_OUT_OF_MEMORY;
	}

	db_exec_sqlite_query(query, __check_validation_of_qurey_cb, (void *)&result_query);
	sqlite3_free(query);

	return result_query;
}

static char* __get_app_locale_from_app_localized_info_by_fallback(const char *appid, const char *locale)
{
	char wildcard[2] = {'%','\0'};
	char lang[3] = {'\0'};
	char *locale_new = NULL;
	pkgmgr_locale_x *info = NULL;

	info = (pkgmgr_locale_x *)malloc(sizeof(pkgmgr_locale_x));
	if (info == NULL) {
		_E("Out of Memory!!!\n");
		return NULL;
	}
	memset(info, 0, sizeof(*info));

	strncpy(lang, locale, 2);
	char *query = sqlite3_mprintf("select app_locale from package_app_localized_info where app_id=%Q and app_locale like %Q%Q", appid, lang, wildcard);
	if (NULL == query){
		_E("Memory allocation failed");
		free(info);
		return NULL;
	}
	db_exec_sqlite_query(query, __fallback_locale_cb, (void *)info);
	locale_new = info->locale;
	free(info);
	sqlite3_free(query);

	return locale_new;
}

static char* __convert_syslocale_to_manifest_locale(char *syslocale)
{
	char *locale = malloc(6);
	if (!locale) {
		_E("Malloc Failed\n");
		return NULL;
	}

	sprintf(locale, "%c%c-%c%c", syslocale[0], syslocale[1], tolower(syslocale[3]), tolower(syslocale[4]));
	return locale;
}

static char* __get_app_locale_by_fallback(const char *appid, const char *syslocale)
{
	assert(appid);

	char *locale = NULL;
	char *locale_new = NULL;
	int check_result = 0;

	if(syslocale == NULL) {
		return	strdup(DEFAULT_LOCALE);
	}

	locale = __convert_syslocale_to_manifest_locale((char*)syslocale);

	/*check exact matching */
	check_result = __check_app_locale_from_app_localized_info_by_exact(appid, locale);

	/* Exact found */
	if (check_result == 1) {
		_D("%s find exact locale(%s)\n", appid, locale);
		return locale;
	}

	/* fallback matching */
	check_result = __check_app_locale_from_app_localized_info_by_fallback(appid, locale);
	if(check_result == 1) {
		   locale_new = __get_app_locale_from_app_localized_info_by_fallback(appid, locale);
		   _D("%s found (%s) language-locale in DB by fallback!\n", appid, locale_new);
		   free(locale);
		   if (locale_new == NULL)
			   locale_new =  strdup(DEFAULT_LOCALE);
		   return locale_new;
	}

	/* default locale */
	free(locale);
	_D("%s DEFAULT_LOCALE)\n", appid);
	return	strdup(DEFAULT_LOCALE);
}

static ail_error_e __retrieve_all_column(ail_appinfo_h ai)
{
	int i = 0;
	int j = 0;
	ail_error_e err = AIL_ERROR_OK;
	char *col = NULL;

	retv_if(!ai, AIL_ERROR_INVALID_PARAMETER);
	retv_if(!ai->stmt, AIL_ERROR_INVALID_PARAMETER);

	ai->values = calloc(NUM_OF_PROP, sizeof(char *));
	retv_if(!ai->values, AIL_ERROR_OUT_OF_MEMORY);

	for (i = 0; i < NUM_OF_PROP; i++) {
		err = db_column_str(ai->stmt, i, &col);
		if (AIL_ERROR_OK != err)
			break;

		if (!col) {
			ai->values[i] = AIL_EMPTY_STR;
		} else {
			ai->values[i] = strdup(col);
			if (!ai->values[i]) {
				err = AIL_ERROR_OUT_OF_MEMORY;
				break;
			}
		}
	}

	if (err < 0) {
		for (j = 0; j < i; ++j) {
			if (strcmp(ai->values[j], AIL_EMPTY_STR) != 0)
				free(ai->values[j]);
		}
		if (ai->values)
			free(ai->values);
		return err;
	} else
		return AIL_ERROR_OK;
}

int _appinfo_check_installed_storage(ail_appinfo_h ai)
{
	int index = 0;
	ail_prop_str_e prop = -1;
	char *pkgid = NULL;
	char *installed_storage = NULL;
	char buf[AIL_SQL_QUERY_MAX_LEN] = {'\0'};

	retv_if(!ai, AIL_ERROR_OK);

	if (ai->stmt) {
		prop = _ail_convert_to_prop_str(AIL_PROP_X_SLP_INSTALLEDSTORAGE_STR);
		index = sql_get_app_info_idx(prop);
		if (db_column_str(ai->stmt, index, &installed_storage) < 0){
			return AIL_ERROR_OK;
		}

		prop = _ail_convert_to_prop_str(AIL_PROP_X_SLP_PKGID_STR);
		index = sql_get_app_info_idx(prop);
		if (db_column_str(ai->stmt, index, &pkgid) < 0){
			return AIL_ERROR_OK;
		}
	} else {
		prop = _ail_convert_to_prop_str(AIL_PROP_X_SLP_INSTALLEDSTORAGE_STR);
		installed_storage = ai->values[prop];

		prop = _ail_convert_to_prop_str(AIL_PROP_X_SLP_PKGID_STR);
		pkgid = ai->values[prop];
	}

	if (strcmp(installed_storage, "installed_external") == 0) {
		snprintf(buf, AIL_SQL_QUERY_MAX_LEN - 1, "%s%s", PKG_SD_PATH, pkgid);
		if (access(buf, R_OK) != 0) {
			_E("can not access [%s]", buf);
			return AIL_ERROR_FAIL;
		}
	}

	return AIL_ERROR_OK;
}

void appinfo_set_stmt(ail_appinfo_h ai, sqlite3_stmt *stmt)
{
	ai->stmt = stmt;
}

ail_appinfo_h appinfo_create(void)
{
	ail_appinfo_h ai;
	ai = calloc(1, sizeof(struct ail_appinfo));
	retv_if (NULL == ai, NULL);
	ai->stmt = NULL;

	return ai;
}

void appinfo_destroy(ail_appinfo_h ai)
{
	if (ai)
		free(ai);
}

EXPORT_API ail_error_e ail_package_destroy_appinfo(ail_appinfo_h ai)
{
	return ail_destroy_appinfo(ai);
}

EXPORT_API ail_error_e ail_destroy_appinfo(ail_appinfo_h ai)
{
	int i = 0;

	retv_if(!ai, AIL_ERROR_INVALID_PARAMETER);
	retv_if(!ai->values, AIL_ERROR_INVALID_PARAMETER);

	for (i = 0; i < NUM_OF_PROP; i++) {
		if ( ai->values[i] && strcmp(ai->values[i], AIL_EMPTY_STR) != 0){
			free(ai->values[i]);
			ai->values[i] = NULL;
		}
	}
	if(ai->values[E_AIL_PROP_MIMETYPE_STR]){
		free((void*)ai->values[E_AIL_PROP_MIMETYPE_STR]);
		ai->values[E_AIL_PROP_MIMETYPE_STR] = NULL;
	}

	if(ai->values)
		free(ai->values);

	free(ai);
	db_close();

	return AIL_ERROR_OK;
}


EXPORT_API ail_error_e ail_package_get_appinfo(const char *package, ail_appinfo_h *ai)
{
	return ail_get_appinfo(package, ai);
}

static char * __get_package(const char *appid)
{
	retv_if(!appid, NULL);

	sqlite3_stmt *stmt = NULL;
	char *str = NULL;
	char *package = NULL;
	char *query = NULL;

	query = sqlite3_mprintf("SELECT package_app_info.PACKAGE FROM package_app_info WHERE package_app_info.APP_ID=%Q", appid);
	if (NULL == query){
		_E("Memory allocation failed");
		return NULL;
	}

	retv_if (db_prepare(query, &stmt) < 0, NULL);
	do {
		if (db_step(stmt) < 0)
			goto end;
		if (db_column_str(stmt, 0, &str) < 0)
			goto end;
		if (str)
		{
			package = strdup(str);
			if (!package){
				_E("Memory allocation failed");
				return NULL;
			}
		}
		else
			package = NULL;

	} while(0);

end:
	sqlite3_free(query);
	db_finalize(stmt);
	return package;

}

static ail_error_e __ail_get_appinfo_new_icon(const char *appid, ail_appinfo_h ai)
{

	char query[AIL_SQL_QUERY_MAX_LEN] = {0};
	sqlite3_stmt *stmt = NULL;
	char *val = NULL;
	int ret = AIL_ERROR_OK;
	char *package = NULL;
	char *w = NULL;

	retv_if(!appid, AIL_ERROR_INVALID_PARAMETER);
	retv_if(!ai, AIL_ERROR_INVALID_PARAMETER);
	package = __get_package(appid);


	w = sqlite3_mprintf("package_localized_info.PACKAGE=%Q and package_localized_info.package_locale='No Locale'", package);
	if (NULL == w){
		_E("Memory allocation failed");
		free(package);
		return AIL_ERROR_OUT_OF_MEMORY;
	}

	free(package);

	snprintf(query, AIL_SQL_QUERY_MAX_LEN, "SELECT %s FROM %s WHERE %s",SQL_FLD_PKG_APP_INFO_ICON, SQL_TBL_PKG_APP_INFO_ICON, w);

	if (db_prepare(query, &stmt) != AIL_ERROR_OK) {
		_E("db_prepare fail for query = %s",query);
		ret =  AIL_ERROR_DB_FAILED;
		goto end;
	}
	appinfo_set_stmt(ai, stmt);

	if(ai->values[E_AIL_PROP_ICON_STR]){
		free((void*)ai->values[E_AIL_PROP_ICON_STR]);
		ai->values[E_AIL_PROP_ICON_STR] = NULL;
	}

	do{
		if (db_step(stmt) < 0)
			break;
		if (db_column_str(stmt, 0, &val) < 0)
			break;
		if (strcmp(val, "(NULL)")){
			ai->values[E_AIL_PROP_ICON_STR] = strdup(val);
			if (!ai->values[E_AIL_PROP_ICON_STR]){
				_E("Memory allocation failed");
				return AIL_ERROR_OUT_OF_MEMORY;
			}
		}
		else{
			ai->values[E_AIL_PROP_ICON_STR] = strdup(AIL_EMPTY_STR);
		}
	}while(0);

	if (db_finalize(ai->stmt) != AIL_ERROR_OK){
		ret =  AIL_ERROR_DB_FAILED;
		goto end;
	}
end:
	(ai)->stmt = NULL;
	sqlite3_free(w);
	return ret;
}

static ail_error_e __ail_get_appinfo_new_mimetype(const char *appid, ail_appinfo_h ai)
{
	char query[AIL_SQL_QUERY_MAX_LEN] = {0};
	sqlite3_stmt *stmt = NULL;
	int col = 0;
	char *val = NULL;
	char *del = ";" ;
	int ret = AIL_ERROR_OK;

	retv_if(!appid, AIL_ERROR_INVALID_PARAMETER);
	retv_if(!ai, AIL_ERROR_INVALID_PARAMETER);

	char *w = sqlite3_mprintf("package_app_app_svc.APP_ID=%Q", appid);
	if (NULL == w){
		_E("Memory allocation failed");
		return AIL_ERROR_OUT_OF_MEMORY;
	}

	snprintf(query, AIL_SQL_QUERY_MAX_LEN, "SELECT %s FROM %s WHERE %s",SQL_FLD_PKG_APP_INFO_MIME, SQL_TBL_PKG_APP_INFO_SVC, w);
	if (db_prepare(query, &stmt) != AIL_ERROR_OK) {
		_E("db_prepare fail for query = %s",query);
		ret =  AIL_ERROR_DB_FAILED;
		goto end;
	}

	appinfo_set_stmt(ai, stmt);

	if(ai->values[E_AIL_PROP_MIMETYPE_STR]){
		free((void*)ai->values[E_AIL_PROP_MIMETYPE_STR]);
		ai->values[E_AIL_PROP_MIMETYPE_STR] = NULL;
	}
	ai->values[E_AIL_PROP_MIMETYPE_STR] = (char*) calloc(MAX_SIZE,1);
	if (NULL == ai->values[E_AIL_PROP_MIMETYPE_STR]){
		_E("Memory allocation failed");
		ret = AIL_ERROR_OUT_OF_MEMORY;
		goto end;
	}

	while (db_step(stmt) == AIL_ERROR_OK){
		val = (char*)sqlite3_column_text(stmt, col);

		if (!strlen(val) )
			continue;

		strcat(ai->values[E_AIL_PROP_MIMETYPE_STR], val);
		if(strlen(ai->values[E_AIL_PROP_MIMETYPE_STR]))
		strcat(ai->values[E_AIL_PROP_MIMETYPE_STR], del);
	}

	if (db_finalize(ai->stmt) != AIL_ERROR_OK){
		ret =  AIL_ERROR_DB_FAILED;
		goto end;
	}

end:
	(ai)->stmt = NULL;
	sqlite3_free(w);
	return ret;
}

EXPORT_API ail_error_e ail_get_appinfo(const char *appid, ail_appinfo_h *ai)
{
	ail_error_e ret = AIL_ERROR_OK;
	char query[AIL_SQL_QUERY_MAX_LEN] = {0};
	sqlite3_stmt *stmt = NULL;
	char *w = NULL;

	retv_if(!appid, AIL_ERROR_INVALID_PARAMETER);
	retv_if(!ai, AIL_ERROR_INVALID_PARAMETER);

	*ai = appinfo_create();
	retv_if(!*ai, AIL_ERROR_OUT_OF_MEMORY);

	w = sqlite3_mprintf("PACKAGE=%Q", appid);
	if (NULL == w){
		_E("Memory allocation failed");
		if (*ai)
			appinfo_destroy(*ai);
		return AIL_ERROR_OUT_OF_MEMORY;
	}

	snprintf(query, AIL_SQL_QUERY_MAX_LEN, "SELECT * FROM (SELECT %s FROM %s) WHERE %s", SQL_FLD_PKG_APP_INFO_FILTER, SQL_TBL_APP_INFO_PKG, w);

	do {
		ret = db_open_pkg_mgr(DB_OPEN_RO);
		if (ret < 0) break;

		ret = db_prepare(query, &stmt);
		if (ret < 0) break;

		ret = db_step(stmt);
		if (ret < 0) {
			db_finalize(stmt);
			break;
		}

		(*ai)->stmt = stmt;

		ret = _appinfo_check_installed_storage(*ai);
		if (ret < 0) {
			db_finalize((*ai)->stmt);
			break;
		}

		ret = __retrieve_all_column(*ai);
		if (ret < 0) {
			db_finalize((*ai)->stmt);
			break;
		}

		ret = db_finalize((*ai)->stmt);
		if (ret < 0) break;
		(*ai)->stmt = NULL;

		ret = AIL_ERROR_OK;
		break;
	} while(0);

	sqlite3_free(w);
	if(ret != AIL_ERROR_OK){
		appinfo_destroy(*ai);
	}else{
		if (!(strcmp((*ai)->values[E_AIL_PROP_ICON_STR], "(NULL)"))){
			ret = __ail_get_appinfo_new_icon(appid, *ai);
			if (ret != AIL_ERROR_OK) {
				_E("icon not retrieved");
				appinfo_destroy(*ai);
				return ret;
			}
		}

		ret = __ail_get_appinfo_new_mimetype(appid, *ai);
		if (ret != AIL_ERROR_OK) {
			_E("Mimetype not retrieved");
			appinfo_destroy(*ai);
		}
	}

	return ret;
}

EXPORT_API ail_error_e ail_appinfo_get_bool(const ail_appinfo_h ai, const char *property, bool *value)
{
	ail_prop_bool_e prop = 0;
	int index = 0;

	retv_if(!ai, AIL_ERROR_INVALID_PARAMETER);
	retv_if(!property, AIL_ERROR_INVALID_PARAMETER);
	retv_if(!value, AIL_ERROR_INVALID_PARAMETER);

	prop = _ail_convert_to_prop_bool(property);

	if (prop < E_AIL_PROP_BOOL_MIN || prop > E_AIL_PROP_BOOL_MAX)
		return AIL_ERROR_INVALID_PARAMETER;

	if (ai->stmt) {
		index = sql_get_app_info_idx(prop);
		if (db_column_bool(ai->stmt, index, value) < 0)
			return AIL_ERROR_DB_FAILED;
	} else {
	if (strncmp(ai->values[prop], "true", strlen("true")) == 0 || strncmp(ai->values[prop], "True", strlen("True")) == 0 || strncmp(ai->values[prop], "TRUE", strlen("TRUE")) == 0)
		*value = true;
	else
		*value = false;
	}
	return AIL_ERROR_OK;
}



EXPORT_API ail_error_e ail_appinfo_get_int(const ail_appinfo_h ai, const char *property, int *value)
{
	ail_prop_int_e prop = 0;
	int index = 0;

	retv_if(!ai, AIL_ERROR_INVALID_PARAMETER);
	retv_if(!property, AIL_ERROR_INVALID_PARAMETER);
	retv_if(!value, AIL_ERROR_INVALID_PARAMETER);

	prop = _ail_convert_to_prop_int(property);

	if (prop < E_AIL_PROP_INT_MIN || prop > E_AIL_PROP_INT_MAX)
		return AIL_ERROR_INVALID_PARAMETER;

	if (ai->stmt) {
		index = sql_get_app_info_idx(prop);
		if (db_column_int(ai->stmt, index, value) < 0){
			return AIL_ERROR_DB_FAILED;
			}
	} else{
		*value = atoi(ai->values[prop]);
	}

	return AIL_ERROR_OK;
}

char *appinfo_get_localname(const char *package, char *locale)
{
	if (db_open_pkg_mgr(DB_OPEN_RO) < 0) {
		_E("db_open fail %s",package);
		return NULL;
	}
	sqlite3_stmt *stmt = NULL;
	char *str = NULL;
	char *localname = NULL;
	char  del;

	if(locale == NULL)
	{
		locale = strdup("No Locale");
		if(locale == NULL)
		{
			_E("Malloc failed!!");
			return NULL;
		}
	}

	/*
	Change locale format if it is not correct.
	Example change 'ko_KR' to 'ko-kr'.
	*/
	if(strncmp(locale,"No Locale",strlen("No Locale")))
	{
		del = locale[2];
		if(del == '_')
		{
			locale[2] = '-';
			locale[3] = tolower(locale[3]);
			locale[4] = tolower(locale[4]);
		}
	}

	char *query = sqlite3_mprintf("select app_label from package_app_localized_info where app_id=%Q and app_locale=%Q", package, locale);
	if (NULL == query){
		_E("Memory allocation failed");
		return NULL;
	}

	retv_if (db_prepare(query, &stmt) < 0, NULL);

	do {
		if (db_step(stmt) < 0){
			goto end;
			}
		if (db_column_str(stmt, 0, &str) < 0)
			goto end;
		if (str)
			localname = strdup(str);
		else
			localname = NULL;


	} while(0);

end:
	db_finalize(stmt);
	sqlite3_free(query);
	return localname;
}

EXPORT_API ail_error_e ail_appinfo_get_str(const ail_appinfo_h ai, const char *property, char **str)
{
	int index = 0;
	char *value = NULL;
	char *pkg = NULL;
	char *pkg_type = NULL;
	char *locale = NULL;
	char *localname = NULL;
	ail_prop_str_e prop = 0;
	char *locale_new = NULL;

	retv_if(!ai, AIL_ERROR_INVALID_PARAMETER);
	retv_if(!property, AIL_ERROR_INVALID_PARAMETER);
	retv_if(!str, AIL_ERROR_INVALID_PARAMETER);

	prop = _ail_convert_to_prop_str(property);

	if (prop < E_AIL_PROP_STR_MIN || prop > E_AIL_PROP_STR_MAX)
		return AIL_ERROR_INVALID_PARAMETER;


	switch(prop){
		case E_AIL_PROP_NAME_STR:
			if(ai->stmt){
				if (db_column_str(ai->stmt, E_AIL_PROP_X_SLP_PACKAGETYPE_STR, &pkg_type) < 0)
					return AIL_ERROR_DB_FAILED;
				if(pkg_type && (strcasecmp(pkg_type, "tpk") ==0))
				{
					locale = sql_get_locale();
					retv_if (NULL == locale, AIL_ERROR_FAIL);

					if (db_column_str(ai->stmt, E_AIL_PROP_PACKAGE_STR, &pkg) < 0){
						free(locale);
						return AIL_ERROR_DB_FAILED;
					}
					if (pkg == NULL){
						free(locale);
						return AIL_ERROR_DB_FAILED;
					}
					locale_new = __get_app_locale_by_fallback(pkg, locale);
					localname = (char *)appinfo_get_localname(pkg,locale_new);
					free(locale);
					free(locale_new);
				} else {
					if (db_column_str(ai->stmt, E_AIL_PROP_NAME_STR, &localname) < 0)
						return AIL_ERROR_DB_FAILED;
					if(!localname){
						if (db_column_str(ai->stmt, E_AIL_PROP_PACKAGE_STR, &pkg) < 0){
							return AIL_ERROR_DB_FAILED;
						}
						if (pkg == NULL){
							return AIL_ERROR_DB_FAILED;
						}
						locale_new = strdup(DEFAULT_LOCALE);
						if(locale_new == NULL){
							_E("STRDUP: OUT OF MEMORY");
							return AIL_ERROR_OUT_OF_MEMORY;
						}
						localname = (char*)appinfo_get_localname(pkg,locale_new);
						free(locale_new);
					}
				}
			}else{
				pkg_type = ai->values[E_AIL_PROP_X_SLP_PACKAGETYPE_STR];
				pkg = ai->values[E_AIL_PROP_PACKAGE_STR];
				retv_if (NULL == pkg, AIL_ERROR_FAIL);

				locale = sql_get_locale();
				retv_if (NULL == locale, AIL_ERROR_FAIL);

				if(pkg_type && (strcasecmp(pkg_type, "tpk") ==0))
				{
					locale_new = __get_app_locale_by_fallback(pkg, locale);
					localname = (char *)appinfo_get_localname(pkg,locale_new);
					free(locale);
					free(locale_new);
				} else {
					localname = (char *)appinfo_get_localname(pkg,locale);
					free(locale);
					if(!localname){
						localname = (char*) appinfo_get_localname(pkg,DEFAULT_LOCALE);
					}
				}

			}

			if (localname) {
				if (!ai->stmt) {
					if (ai->values) {
						if (strcmp(ai->values[prop], AIL_EMPTY_STR) != 0)
							free(ai->values[prop]);
						ai->values[prop] = localname;
					}
				}
				*str = localname;
				return AIL_ERROR_OK;
			}

			break;

		case E_AIL_PROP_ICON_STR:
			if(ai->stmt){
				index = sql_get_app_info_idx(prop);
				if (db_column_str(ai->stmt, index, &value) < 0){
					return AIL_ERROR_DB_FAILED;
				}
				if((value == NULL) || (strcmp(value,AIL_EMPTY_STR) == 0)){
					if (db_column_str(ai->stmt, E_AIL_PROP_PACKAGE_STR, &pkg) < 0){
						free(locale);
						return AIL_ERROR_DB_FAILED;
					}
					if (pkg == NULL){
						free(locale);
						return AIL_ERROR_DB_FAILED;
				   	}
					value =  _get_icon_from_package_app_localized_info(pkg);
					if((value == NULL) || (strcmp(value,AIL_EMPTY_STR) == 0)){
						if(value != NULL){
							free(value);
							value = NULL;
							}
						value = _get_icon_from_package_localized_info(pkg);
					}
				}
				*str = value;

			}else{
				*str = ai->values[prop];
			}
			break;

		case E_AIL_PROP_MIMETYPE_STR:
			if(ai->stmt){
				index = sql_get_app_info_idx(prop);
				if (db_column_str(ai->stmt, index, &value) < 0){
					return AIL_ERROR_DB_FAILED;
				}
				if ((value == NULL) || (strcmp(value,AIL_EMPTY_STR) == 0)){
					if (db_column_str(ai->stmt, E_AIL_PROP_PACKAGE_STR, &pkg) < 0){
						free(locale);
						return AIL_ERROR_DB_FAILED;
					}
					if (pkg == NULL){
						free(locale);
						return AIL_ERROR_DB_FAILED;
				   	}
					value =  _get_mimetype_from_package_app_app_svc(pkg);
				}
				*str = value;

			}else{
				*str = ai->values[prop];
			}
			break;

		default:
			if(ai->stmt){
				index = sql_get_app_info_idx(prop);
				if (db_column_str(ai->stmt, index, &value) < 0){
					return AIL_ERROR_DB_FAILED;
				}
				*str = value;
			}else{
				*str = ai->values[prop];

			}
		}

	return AIL_ERROR_OK;
}

EXPORT_API ail_error_e ail_close_appinfo_db(void)
{
	return db_close();
}

// End of file
