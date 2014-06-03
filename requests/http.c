// Copyright (C) 2011 Dg Nechtan <dnechtan@gmail.com>, MIT

/* $Id: http.c 325431 2012-04-24 07:41:42Z nechtan $ */

#include "ext/standard/url.h"

static zend_class_entry * fnx_request_http_ce;

/** {{{ fnx_request_t * fnx_request_http_instance(fnx_request_t *this_ptr, char *request_uri, char *base_uri TSRMLS_DC)
*/
fnx_request_t * fnx_request_http_instance(fnx_request_t *this_ptr, char *request_uri, char *base_uri TSRMLS_DC) {
	fnx_request_t *instance;
	zval *method, *params, *settled_uri = NULL;

	if (this_ptr) {
		instance = this_ptr;
	} else {
		MAKE_STD_ZVAL(instance);
		object_init_ex(instance, fnx_request_http_ce);
	}

    MAKE_STD_ZVAL(method);
    if (SG(request_info).request_method) {
        ZVAL_STRING(method, (char *)SG(request_info).request_method, 1);
    } else if (strncasecmp(sapi_module.name, "cli", 3)) {
        ZVAL_STRING(method, "Unknow", 1);
    } else {
        ZVAL_STRING(method, "Cli", 1);
    }
	zend_update_property(fnx_request_http_ce, instance, ZEND_STRL(FNX_REQUEST_PROPERTY_NAME_METHOD), method TSRMLS_CC);
	zval_ptr_dtor(&method);

	if (request_uri) {
		MAKE_STD_ZVAL(settled_uri);
		ZVAL_STRING(settled_uri, request_uri, 1);
	} else {
		zval *uri;
		do {
#ifdef PHP_WIN32
			/* check this first so IIS will catch */
			uri = fnx_request_query(FNX_GLOBAL_VARS_SERVER, ZEND_STRL("HTTP_X_REWRITE_URL") TSRMLS_CC);
			if (Z_TYPE_P(uri) != IS_NULL) {
				settled_uri = uri;
				break;
			}
			zval_ptr_dtor(&uri);

			/* IIS7 with URL Rewrite: make sure we get the unencoded url (double slash problem) */
			uri = fnx_request_query(FNX_GLOBAL_VARS_SERVER, ZEND_STRL("IIS_WasUrlRewritten") TSRMLS_CC);
			if (Z_TYPE_P(uri) != IS_NULL) {
				zval *rewrited = fnx_request_query(FNX_GLOBAL_VARS_SERVER, ZEND_STRL("IIS_WasUrlRewritten") TSRMLS_CC);
				zval *unencode = fnx_request_query(FNX_GLOBAL_VARS_SERVER, ZEND_STRL("UNENCODED_URL") TSRMLS_CC);
				if (Z_TYPE_P(rewrited) = IS_LONG
						&& Z_LVAL_P(rewrited) == 1
						&& Z_TYPE_P(unencode) == IS_STRING
						&& Z_STRLEN_P(unencode) > 0) {
					settled_uri = uri;
				}
				break;
			}
			zval_ptr_dtor(&uri);
#endif
			uri = fnx_request_query(FNX_GLOBAL_VARS_SERVER, ZEND_STRL("PATH_INFO") TSRMLS_CC);
			if (Z_TYPE_P(uri) != IS_NULL) {
				settled_uri = uri;
				break;
			}
			zval_ptr_dtor(&uri);

			uri = fnx_request_query(FNX_GLOBAL_VARS_SERVER, ZEND_STRL("REQUEST_URI") TSRMLS_CC);
			if (Z_TYPE_P(uri) != IS_NULL) {
				/* Http proxy reqs setup request uri with scheme and host [and port] + the url path, only use url path */
				if (strstr(Z_STRVAL_P(uri), "http") == Z_STRVAL_P(uri)) {
					php_url *url_info = php_url_parse(Z_STRVAL_P(uri));
					zval_ptr_dtor(&uri);

					if (url_info && url_info->path) {
						MAKE_STD_ZVAL(settled_uri);
						ZVAL_STRING(settled_uri, url_info->path, 1);
					}

					php_url_free(url_info);
				} else {
					char *pos  = NULL;
					if ((pos = strstr(Z_STRVAL_P(uri), "?"))) {
						MAKE_STD_ZVAL(settled_uri);
						ZVAL_STRINGL(settled_uri, Z_STRVAL_P(uri), pos - Z_STRVAL_P(uri), 1);
						zval_ptr_dtor(&uri);
					} else {
						settled_uri = uri;
					}
				}
				break;
			}
			zval_ptr_dtor(&uri);

			uri = fnx_request_query(FNX_GLOBAL_VARS_SERVER, ZEND_STRL("ORIG_PATH_INFO") TSRMLS_CC);
			if (Z_TYPE_P(uri) != IS_NULL) {
				/* intended do nothing */
				/* zval *query = fnx_request_query(FNX_GLOBAL_VARS_SERVER, ZEND_STRL("QUERY_STRING") TSRMLS_CC);
				if (Z_TYPE_P(query) != IS_NULL) {
				}
				*/
				settled_uri = uri;
				break;
			}
			zval_ptr_dtor(&uri);

		} while (0);
	}

	if (settled_uri) {
		char *p = Z_STRVAL_P(settled_uri);

		while (*p == '/' && *(p + 1) == '/') {
			p++;
		}

		if (p != Z_STRVAL_P(settled_uri)) {
			char *garbage = Z_STRVAL_P(settled_uri);
			ZVAL_STRING(settled_uri, p, 1);
			efree(garbage);
		}

		zend_update_property(fnx_request_http_ce, instance, ZEND_STRL(FNX_REQUEST_PROPERTY_NAME_URI), settled_uri TSRMLS_CC);
		fnx_request_set_base_uri(instance, base_uri, Z_STRVAL_P(settled_uri) TSRMLS_CC);
		zval_ptr_dtor(&settled_uri);
	}

	MAKE_STD_ZVAL(params);
	array_init(params);
	zend_update_property(fnx_request_http_ce, instance, ZEND_STRL(FNX_REQUEST_PROPERTY_NAME_PARAMS), params TSRMLS_CC);
	zval_ptr_dtor(&params);

	return instance;
}
/* }}} */

/** {{{ proto public Fnx_Request_Http::getQuery(mixed $name, mixed $default = NULL)
*/
FNX_REQUEST_METHOD(fnx_request_http, Query, 	FNX_GLOBAL_VARS_GET);
/* }}} */

/** {{{ proto public Fnx_Request_Http::getPost(mixed $name, mixed $default = NULL)
*/
FNX_REQUEST_METHOD(fnx_request_http, Post,  	FNX_GLOBAL_VARS_POST);
/* }}} */

/** {{{ proto public Fnx_Request_Http::getRequet(mixed $name, mixed $default = NULL)
*/
FNX_REQUEST_METHOD(fnx_request_http, Request, FNX_GLOBAL_VARS_REQUEST);
/* }}} */

/** {{{ proto public Fnx_Request_Http::getFiles(mixed $name, mixed $default = NULL)
*/
FNX_REQUEST_METHOD(fnx_request_http, Files, 	FNX_GLOBAL_VARS_FILES);
/* }}} */

/** {{{ proto public Fnx_Request_Http::getCookie(mixed $name, mixed $default = NULL)
*/
FNX_REQUEST_METHOD(fnx_request_http, Cookie, 	FNX_GLOBAL_VARS_COOKIE);
/* }}} */

/** {{{ proto public Fnx_Request_Http::isXmlHttpRequest()
*/
PHP_METHOD(fnx_request_http, isAjax) {
	zval * header = fnx_request_query(FNX_GLOBAL_VARS_SERVER, ZEND_STRL("HTTP_X_REQUESTED_WITH") TSRMLS_CC);
	if (Z_TYPE_P(header) == IS_STRING
			&& strncasecmp("XMLHttpRequest", Z_STRVAL_P(header), Z_STRLEN_P(header)) == 0) {
		RETURN_TRUE;
	}
	RETURN_FALSE;
}
/* }}} */

/** {{{ proto public Fnx_Request_Http::get(mixed $name, mixed $default)
 * params -> post -> get -> cookie -> server
 */
PHP_METHOD(fnx_request_http, get) {
	char	*name 	= NULL;
	int 	len	 	= 0;
	zval 	*def 	= NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|z", &name, &len, &def) == FAILURE) {
		WRONG_PARAM_COUNT;
	} else {
		zval *value = fnx_request_get_param(getThis(), name, len TSRMLS_CC);
		if (value) {
			RETURN_ZVAL(value, 1, 0);
		} else {
			zval *params	= NULL;
			zval **ppzval	= NULL;

			FNX_GLOBAL_VARS_TYPE methods[4] = {
				FNX_GLOBAL_VARS_POST,
				FNX_GLOBAL_VARS_GET,
				FNX_GLOBAL_VARS_COOKIE,
				FNX_GLOBAL_VARS_SERVER
			};

			{
				int i = 0;
				for (;i<4; i++) {
					params = PG(http_globals)[methods[i]];
					if (params && Z_TYPE_P(params) == IS_ARRAY) {
						if (zend_hash_find(Z_ARRVAL_P(params), name, len + 1, (void **)&ppzval) == SUCCESS ){
							RETURN_ZVAL(*ppzval, 1, 0);
						}
					}
				}

			}
			if (def) {
				RETURN_ZVAL(def, 1, 0);
			}
		}
	}
	RETURN_NULL();
}
/* }}} */

/** {{{ proto public Fnx_Request_Http::__construct(string $request_uri, string $base_uri)
*/
PHP_METHOD(fnx_request_http, __construct) {
	char *request_uri = NULL;
	char *base_uri	  = NULL;
	int  rlen		  = 0;
	int  blen 		  = 0;

	fnx_request_t *self = getThis();
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|ss", &request_uri, &rlen, &base_uri, &blen) == FAILURE) {
		return;
	}

	(void)fnx_request_http_instance(self, request_uri, base_uri TSRMLS_CC);
}
/* }}} */

/** {{{ proto private Fnx_Request_Http::__clone
 */
PHP_METHOD(fnx_request_http, __clone) {
}
/* }}} */

/** {{{ fnx_request_http_methods
 */
zend_function_entry fnx_request_http_methods[] = {
	PHP_ME(fnx_request_http, getQuery, 		NULL, ZEND_ACC_PUBLIC)
	PHP_ME(fnx_request_http, getRequest, 	NULL, ZEND_ACC_PUBLIC)
	PHP_ME(fnx_request_http, getPost, 		NULL, ZEND_ACC_PUBLIC)
	PHP_ME(fnx_request_http, getCookie,		NULL, ZEND_ACC_PUBLIC)
	PHP_ME(fnx_request_http, getFiles,		NULL, ZEND_ACC_PUBLIC)
	PHP_ME(fnx_request_http, get,			NULL, ZEND_ACC_PUBLIC)
	PHP_ME(fnx_request_http, isAjax,         NULL, ZEND_ACC_PUBLIC)
	PHP_ME(fnx_request_http, __construct,	NULL, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
	PHP_ME(fnx_request_http, __clone,		NULL, ZEND_ACC_PRIVATE|ZEND_ACC_CLONE)
	{NULL, NULL, NULL}
};
/* }}} */

/** {{{ FNX_STARTUP_FUNCTION
 */
FNX_STARTUP_FUNCTION(request_http){
	zend_class_entry ce;
	FNX_INIT_CLASS_ENTRY(ce, "Fnx_Request_Http", "Fnx\\Request\\Http", fnx_request_http_methods);
	fnx_request_http_ce = zend_register_internal_class_ex(&ce, fnx_request_ce, NULL TSRMLS_CC);

	zend_declare_class_constant_string(fnx_request_ce, ZEND_STRL("SCHEME_HTTP"), "http" TSRMLS_CC);
	zend_declare_class_constant_string(fnx_request_ce, ZEND_STRL("SCHEME_HTTPS"), "https" TSRMLS_CC);

	return SUCCESS;
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
