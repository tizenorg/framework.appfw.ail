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



#include <stdlib.h>
#include <glib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include "ail.h"
#include "ail_private.h"
#include "ail_convert.h"
#include "ail_sql.h"
#include "ail_package.h"
#include "ail_db.h"

char *_get_where_clause(ail_filter_h filter);

struct ail_filter {
	GSList *list;
};

static inline void _add_cond_to_filter(ail_filter_h filter, struct element *cond)
{
	filter->list = g_slist_append(filter->list, cond);
}

EXPORT_API ail_error_e ail_filter_new(ail_filter_h *filter)
{
	struct ail_filter *f;

	retv_if (NULL == filter, AIL_ERROR_INVALID_PARAMETER);

	f = (struct ail_filter *)calloc(1, sizeof(struct ail_filter));
	retv_if (NULL == f, AIL_ERROR_OUT_OF_MEMORY);

	*filter = f;

	return AIL_ERROR_OK;
}

static inline void _destroy_cond(gpointer data, gpointer user_data)
{
	if (!data)
		return;

	struct element *cond;
	cond  = (struct element *)data;

	int t;
	ELEMENT_TYPE(cond, t);
	if (VAL_TYPE_STR == t) {
		if(ELEMENT_STR(cond)->value)
			free(ELEMENT_STR(cond)->value);
	}
	free(cond);
	return;
}


EXPORT_API ail_error_e ail_filter_destroy(ail_filter_h filter)
{
	retv_if (NULL == filter, AIL_ERROR_INVALID_PARAMETER);

	if (filter->list){
		g_slist_foreach(filter->list, _destroy_cond, NULL);
		g_slist_free(filter->list);
	}

	free(filter);
	db_close();

	return AIL_ERROR_OK;
}

EXPORT_API ail_error_e ail_filter_add_bool(ail_filter_h filter, const char *property, bool value)
{
	struct element *c;
	ail_prop_bool_e prop;

	retv_if (NULL == filter, AIL_ERROR_INVALID_PARAMETER);
	retv_if (NULL == property, AIL_ERROR_INVALID_PARAMETER);

	prop = _ail_convert_to_prop_bool(property);

	if (prop < E_AIL_PROP_BOOL_MIN || prop > E_AIL_PROP_BOOL_MAX)
		return AIL_ERROR_INVALID_PARAMETER;

	c = (struct element *)calloc(1, sizeof(struct element_bool));
	retv_if (NULL == c, AIL_ERROR_OUT_OF_MEMORY);

	ELEMENT_BOOL(c)->prop = (int)prop;
	ELEMENT_BOOL(c)->value = value;

	_add_cond_to_filter(filter, c);

	return AIL_ERROR_OK;
}

EXPORT_API ail_error_e ail_filter_add_int(ail_filter_h filter, const char *property, int value)
{
	struct element *c;
	ail_prop_int_e prop;

	retv_if (NULL == filter, AIL_ERROR_INVALID_PARAMETER);
	retv_if (NULL == property, AIL_ERROR_INVALID_PARAMETER);

	prop = _ail_convert_to_prop_int(property);

	if (prop < E_AIL_PROP_INT_MIN || prop > E_AIL_PROP_INT_MAX)
		return AIL_ERROR_INVALID_PARAMETER;

	c = (struct element *)calloc(1, sizeof(struct element_int));
	retv_if (NULL == c, AIL_ERROR_OUT_OF_MEMORY);

	ELEMENT_INT(c)->prop = (int)prop;
	ELEMENT_INT(c)->value = value;

	_add_cond_to_filter(filter, c);

	return AIL_ERROR_OK;
}

EXPORT_API ail_error_e ail_filter_add_str(ail_filter_h filter, const char *property, const char *value)
{
	struct element *c; //condition
	ail_prop_str_e prop;

	retv_if (NULL == filter, AIL_ERROR_INVALID_PARAMETER);
	retv_if (NULL == property, AIL_ERROR_INVALID_PARAMETER);

	prop = _ail_convert_to_prop_str(property);

	if (prop < E_AIL_PROP_STR_MIN || prop > E_AIL_PROP_STR_MAX)
		return AIL_ERROR_INVALID_PARAMETER;

	retv_if (NULL == value, AIL_ERROR_INVALID_PARAMETER);
	retv_if (strlen(value) == 0, AIL_ERROR_INVALID_PARAMETER);

	c = (struct element *)calloc(1, sizeof(struct element_str));

	retv_if (NULL == c, AIL_ERROR_OUT_OF_MEMORY);

	ELEMENT_STR(c)->prop = (int)prop;
	ELEMENT_STR(c)->value = strdup(value);
	if (!ELEMENT_STR(c)->value) {
		free(c);
		return AIL_ERROR_OUT_OF_MEMORY;
	}

	_add_cond_to_filter(filter, c);

	return AIL_ERROR_OK;
}

static void _get_condition(gpointer data, char **condition)
{
	struct element *e = (struct element *)data;
	const char *f = NULL;
	char buf[AIL_SQL_QUERY_MAX_LEN] = {0};
	char *b_value = NULL;
	int t = 0;

	f =  sql_get_filter(e->prop);
	if (!f)
		return;

	ELEMENT_TYPE(e, t);

	switch (t) {
		case VAL_TYPE_BOOL:
			if(ELEMENT_BOOL(e)->value)
				b_value = strdup("true");
			else
				b_value = strdup("false");

			snprintf(buf, sizeof(buf), f, b_value);
			break;
		case VAL_TYPE_INT:
			snprintf(buf, sizeof(buf), f, ELEMENT_INT(e)->value);
			break;
		case VAL_TYPE_STR:
			if (E_AIL_PROP_NAME_STR == e->prop) {
				snprintf(buf, sizeof(buf), f, ELEMENT_STR(e)->value);
			} else {
				snprintf(buf, sizeof(buf), f, ELEMENT_STR(e)->value);
			}
			break;
		default:
			_E("Invalid property type");
			*condition = NULL;
			return;
	}
	if(b_value)
		free(b_value);

	*condition = strdup(buf);

	return;
}

char *_get_where_clause(ail_filter_h filter)
{
	char *c = NULL;
	char w[AIL_SQL_QUERY_MAX_LEN] = {0,};
	c = NULL;

	GSList *l;

	snprintf(w, AIL_SQL_QUERY_MAX_LEN, " WHERE ");

	for (l = filter->list; l; l = g_slist_next(l)) {
		_get_condition(l->data, &c);
		if (!c) return NULL;
		if (*c == 0) {
			free(c);
			return NULL;
		}

		strncat(w, c, sizeof(w)-strlen(w)-1);
		w[sizeof(w)-1] = '\0';
		if (c) {
			free(c);
			c = NULL;
		}

		if (g_slist_next(l)) {
			strncat(w, " and ", sizeof(w)-strlen(w)-1);
			w[sizeof(w)-1] = '\0';
		}
	}

	return strdup(w);
}

EXPORT_API ail_error_e ail_filter_count_appinfo(ail_filter_h filter, int *cnt)
{
	char q[AIL_SQL_QUERY_MAX_LEN] = {0};
	char *w = NULL;
	char *tmp_q = NULL;
	char *l = NULL;
	sqlite3_stmt *stmt = NULL;
	ail_appinfo_h ai = NULL;
	int filter_count = 0;
	char *locale = NULL;

	retv_if(!cnt, AIL_ERROR_INVALID_PARAMETER);

	if (db_open_pkg_mgr(DB_OPEN_RO) != AIL_ERROR_OK)
		return AIL_ERROR_DB_FAILED;

	snprintf(q, sizeof(q), "SELECT * FROM (SELECT %s FROM %s)", SQL_FLD_PKG_APP_INFO_FILTER, SQL_TBL_APP_INFO_FILTER);

	tmp_q = strdup(q);
	retv_if (NULL == tmp_q, AIL_ERROR_OUT_OF_MEMORY);
	l = sql_get_locale();
	if (NULL == l) {
		_E("Failed to get locale string");
		free(tmp_q);
		return AIL_ERROR_FAIL;
	}
	locale = (char*) calloc(strlen(l)+1,1);
	if(locale == NULL) {
		free(tmp_q);
		free(l);
		return AIL_ERROR_OUT_OF_MEMORY;
	}
	strncpy(locale ,l, 2);
	strncat(locale,"-",1);
	locale[3] = tolower(l[3]);
	locale[4] = tolower(l[4]);
	free(l);

	snprintf(q, sizeof(q), tmp_q, locale);
	free(locale);
	free(tmp_q);

	if (filter && filter->list) {
		w = _get_where_clause(filter);
		retv_if (NULL == w, AIL_ERROR_FAIL);

		strncat(q, w, sizeof(q)-strlen(q)-1);
		q[sizeof(q)-1] = '\0';
		free(w);
	}
	else
		_D("No filter exists. All records are retreived");

	if (db_prepare(q, &stmt) != AIL_ERROR_OK) {
		_E("db_prepare fail for query = %s",q);
		return AIL_ERROR_DB_FAILED;
	}

	ai = appinfo_create();
	if (ai == NULL) {
		_E("failed to create appinfo");
		db_finalize(stmt);
		return AIL_ERROR_OUT_OF_MEMORY;
	}

	appinfo_set_stmt(ai, stmt);

	while (db_step(stmt) == AIL_ERROR_OK) {
		if(_appinfo_check_installed_storage(ai) != AIL_ERROR_OK)
			continue;

		filter_count++;
	}

	db_finalize(stmt);

	appinfo_destroy(ai);
	*cnt = filter_count;

	return AIL_ERROR_OK;
}

EXPORT_API ail_error_e ail_filter_list_appinfo_foreach(ail_filter_h filter, ail_list_appinfo_cb cb, void *user_data)
{
	char q[AIL_SQL_QUERY_MAX_LEN] = {0};
	char *tmp_q = NULL;
	char *w = NULL;
	char *l = NULL;
	ail_cb_ret_e r;
	sqlite3_stmt *stmt = NULL;
	ail_appinfo_h ai = NULL;
	char *locale = NULL;

	retv_if (NULL == cb, AIL_ERROR_INVALID_PARAMETER);

	if (db_open_pkg_mgr(DB_OPEN_RO) != AIL_ERROR_OK)
		return AIL_ERROR_DB_FAILED;

	snprintf(q, sizeof(q), "SELECT * FROM (SELECT %s FROM %s)", SQL_FLD_PKG_APP_INFO_FILTER, SQL_TBL_APP_INFO_FILTER);

	tmp_q = strdup(q);
	retv_if (NULL == tmp_q, AIL_ERROR_OUT_OF_MEMORY);
	l = sql_get_locale();
	if (NULL == l) {
		_E("Failed to get locale string");
		free(tmp_q);
		return AIL_ERROR_FAIL;
	}

	locale = (char*) calloc(strlen(l)+1,1);
	if(locale == NULL) {
		free(tmp_q);
		free(l);
		return AIL_ERROR_OUT_OF_MEMORY;
	}
	strncpy(locale ,l, 2);
	strncat(locale,"-",1);
	locale[3] = tolower(l[3]);
	locale[4] = tolower(l[4]);
	free(l);

	snprintf(q, sizeof(q), tmp_q, locale);
	free(locale);
	free(tmp_q);

	if (filter && filter->list) {
		w = _get_where_clause(filter);
		retv_if (NULL == w, AIL_ERROR_FAIL);
		strncat(q, w, sizeof(q)-strlen(q)-1);
		q[sizeof(q)-1] = '\0';
		free(w);
	}
	else
		_D("No filter exists. All records are retreived");

	if (db_prepare(q, &stmt) != AIL_ERROR_OK) {
		_E("db_prepare fail for query = %s",q);
		return AIL_ERROR_DB_FAILED;
	}

	ai = appinfo_create();
	if (ai == NULL)
		return AIL_ERROR_OUT_OF_MEMORY;

	appinfo_set_stmt(ai, stmt);
	while (db_step(stmt) == AIL_ERROR_OK) {
		if(_appinfo_check_installed_storage(ai) != AIL_ERROR_OK)
			continue;

		r = cb(ai, user_data);
		if (AIL_CB_RET_CANCEL == r)
			break;
	}
	db_finalize(stmt);
	appinfo_destroy(ai);
	return AIL_ERROR_OK;
}
