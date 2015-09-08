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



#ifndef __AIL_SQL_H__
#define __AIL_SQL_H__

#include "ail.h"

#define SQL_TBL_APP_INFO "app_info"
#define SQL_TBL_PKG_APP_INFO "package_app_info,package_app_localized_info,package_info"
#define SQL_TBL_PKG_APP_INFO_SVC "package_app_app_svc"
#define SQL_TBL_PKG_APP_INFO_ICON "package_localized_info"
#define SQL_TBL_APP_INFO_WITH_LOCALNAME "app_info LEFT OUTER JOIN localname " \
					"ON app_info.package=localname.package " \
					"and locale='%s'"

#define SQL_TBL_APP_INFO_FILTER "package_info a LEFT OUTER JOIN package_app_info b ON a.package = b.package LEFT OUTER JOIN package_app_localized_info c ON c.app_id = b.app_id and c.app_locale='%s' LEFT OUTER JOIN package_app_app_control d ON d.app_id = b.app_id"

#define SQL_TBL_APP_INFO_PKG "package_info a LEFT OUTER JOIN package_app_info b ON a.package = b.package LEFT OUTER JOIN package_app_localized_info c ON c.app_id = b.app_id and c.app_locale='No Locale' LEFT OUTER JOIN package_app_app_control d ON d.app_id = b.app_id"

#define SQL_TBL_PKG_APP_INFO_WITH_LOCALNAME "package_app_info LEFT OUTER JOIN package_app_localized_info" \
					"ON package_app_info.app_id=package_app_localized_info.app_id" \
					"and package_app_localized_info.locale='%s'" \
					"LEFT OUTER JOIN package_info" \
					"ON package_app_info.package=package_info.package"

#define SQL_FLD_APP_INFO "app_info.PACKAGE," \
			"app_info.EXEC," \
			"app_info.NAME," \
			"app_info.TYPE," \
			"app_info.ICON," \
			"app_info.CATEGORIES," \
			"app_info.VERSION," \
			"app_info.MIMETYPE," \
			"app_info.X_SLP_SERVICE," \
			"app_info.X_SLP_PACKAGETYPE," \
			"app_info.X_SLP_PACKAGECATEGORIES," \
			"app_info.X_SLP_PACKAGEID," \
			"app_info.X_SLP_URI," \
			"app_info.X_SLP_SVC," \
			"app_info.X_SLP_EXE_PATH," \
			"app_info.X_SLP_APPID," \
			"app_info.X_SLP_PKGID," \
			"app_info.X_SLP_DOMAIN," \
			"app_info.X_SLP_SUBMODEMAINID," \
			"app_info.X_SLP_INSTALLEDSTORAGE," \
			"app_info.X_SLP_BASELAYOUTWIDTH," \
			"app_info.X_SLP_INSTALLEDTIME," \
			"app_info.NODISPLAY," \
			"app_info.X_SLP_TASKMANAGE," \
			"app_info.X_SLP_MULTIPLE," \
			"app_info.X_SLP_REMOVABLE," \
			"app_info.X_SLP_ISHORIZONTALSCALE," \
			"app_info.X_SLP_ENABLED," \
			"app_info.X_SLP_SUBMODE" \

#define SQL_FLD_PKG_APP_INFO_FILTER "b.app_id as package," \
			"b.app_exec as exec," \
			"c.app_label as name," \
			"'Application' as type," \
			"c.app_icon as icon," \
			"'' as categories," \
			"a.package_version as version," \
			"'' as mimetype," \
			"'' as x_slp_service," \
			"a.package_type as x_slp_packagetype," \
			"'' as x_slp_package_categories," \
			"b.app_id as x_slp_packageid," \
			"'' as x_slp_uri," \
			"d.app_control as x_slp_svc," \
			"b.app_exec as x_slp_exe_path," \
			"b.app_id as x_slp_appid," \
			"b.package as x_slp_pkgid," \
			"'' as x_slp_domain," \
			"b.app_submode_mainid as x_slp_submodemainid," \
			"b.app_installed_storage as x_slp_installedstorage," \
			"0 as x_slp_baselayoutwidth," \
			"a.installed_time as x_slp_installedtime," \
			"b.app_nodisplay as nodisplay," \
			"b.app_taskmanage as x_slp_taskmanage," \
			"b.app_multiple as x_slp_muliple," \
			"a.package_removable as x_slp_removable," \
			"0 as x_slp_ishorizontalscale," \
			"b.app_enabled as x_slp_enabled," \
			"b.app_submode as x_slp_submode" \

#define SQL_FLD_PKG_APP_INFO "package_app_info.APP_ID," \
			"package_app_info.APP_EXEC," \
			"package_app_localized_info.APP_LABEL," \
			"package_app_info.APP_SUBMODE_MAINID," \
			"package_app_localized_info.APP_ICON," \
			"package_app_info.APP_SUBMODE_MAINID," \
			"package_info.PACKAGE_VERSION," \
			"package_app_info.APP_SUBMODE_MAINID," \
			"package_app_info.APP_SUBMODE_MAINID," \
			"package_info.PACKAGE_TYPE," \
			"package_app_info.APP_SUBMODE_MAINID," \
			"package_app_info.APP_ID," \
			"package_app_info.APP_SUBMODE_MAINID," \
			"package_app_info.APP_SUBMODE_MAINID," \
			"package_app_info.APP_EXEC," \
			"package_app_info.APP_ID," \
			"package_app_info.PACKAGE," \
			"package_app_info.APP_SUBMODE_MAINID," \
			"package_app_info.APP_SUBMODE_MAINID," \
			"package_app_info.APP_INSTALLED_STORAGE," \
			"package_app_info.APP_SUBMODE_MAINID," \
			"package_info.INSTALLED_TIME," \
			"package_app_info.APP_NODISPLAY," \
			"package_app_info.APP_TASKMANAGE," \
			"package_app_info.APP_MULTIPLE," \
			"package_info.PACKAGE_REMOVABLE," \
			"package_app_info.APP_SUBMODE_MAINID," \
			"package_app_info.APP_ENABLED," \
			"package_app_info.APP_SUBMODE" \

#define FILTER_QUERY_LIST_PACKAGE_APP_INFO	"select DISTINCT package_info.package " \
				"from package_info LEFT OUTER JOIN package_localized_info " \
				"ON package_info.package=package_localized_info.package " \
				"and package_localized_info.package_locale='%s'"

#define SQL_FLD_PKG_APP_INFO_MIME "package_app_app_svc.MIME_TYPE" \

#define SQL_FLD_PKG_APP_INFO_ICON "package_localized_info.PACKAGE_ICON" \

#define SQL_FLD_PKG_APP_INFO_SVC "package_app_app_svc.OPERATION," \
									 "package_app_app_svc.URI_SCHEME," \
									 "package_app_app_svc.MIME_TYPE," \
									 "package_app_app_svc.SUBAPP_NAME" \


#define SQL_FLD_APP_INFO_WITH_LOCALNAME SQL_FLD_APP_INFO",""localname.name"
#define SQL_LOCALNAME_IDX NUM_OF_PROP + 0

const char *sql_get_filter(int prop);
char *sql_get_locale();
int sql_get_app_info_idx(int prop);

#endif  /* __AIL_SQL_H__ */
