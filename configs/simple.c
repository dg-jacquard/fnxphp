// Copyright (C) 2011 Dg Nechtan <dnechtan@gmail.com>, MIT

/* $Id: simple.c 324504 2012-03-24 02:48:04Z nechtan $ */

zend_class_entry *fnx_config_simple_ce;

/** {{{ ARG_INFO
 */
ZEND_BEGIN_ARG_INFO_EX(fnx_config_simple_construct_arginfo, 0, 0, 1)
	ZEND_ARG_INFO(0, config_file)
	ZEND_ARG_INFO(0, section)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(fnx_config_simple_get_arginfo, 0, 0, 0)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(fnx_config_simple_rget_arginfo, 0, 0, 1)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(fnx_config_simple_set_arginfo, 0, 0, 2)
	ZEND_ARG_INFO(0, name)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(fnx_config_simple_isset_arginfo, 0, 0, 1)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(fnx_config_simple_unset_arginfo, 0, 0, 1)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()
/* }}} */

/** {{{ fnx_config_t * fnx_config_simple_instance(fnx_config_t *this_ptr, zval *values, zval *readonly TSRMLS_DC)
*/
fnx_config_t * fnx_config_simple_instance(fnx_config_t *this_ptr, zval *values, zval *readonly TSRMLS_DC) {
	fnx_config_t *instance;

	switch (Z_TYPE_P(values)) {
		case IS_ARRAY:
			if (this_ptr) {
				instance = this_ptr;
			} else {
				MAKE_STD_ZVAL(instance);
				object_init_ex(instance, fnx_config_simple_ce);
			}
			zend_update_property(fnx_config_simple_ce, instance, ZEND_STRL(FNX_CONFIG_PROPERT_NAME), values TSRMLS_CC);
			if (readonly) {
				convert_to_boolean(readonly);
				zend_update_property_bool(fnx_config_simple_ce, instance, ZEND_STRL(FNX_CONFIG_PROPERT_NAME_READONLY), Z_BVAL_P(readonly) TSRMLS_CC);
			}
			return instance;
		break;
		default:
			fnx_trigger_error(FNX_ERR_TYPE_ERROR TSRMLS_CC, "Invalid parameters provided, must be an array");
			return NULL;
	}
}
/* }}} */

/** {{{ zval * fnx_config_simple_format(fnx_config_t *instance, zval **ppzval TSRMLS_DC)
 */
zval * fnx_config_simple_format(fnx_config_t *instance, zval **ppzval TSRMLS_DC) {
	zval *readonly, *ret;
	readonly = zend_read_property(fnx_config_simple_ce, instance, ZEND_STRL(FNX_CONFIG_PROPERT_NAME_READONLY), 1 TSRMLS_CC);
	ret = fnx_config_simple_instance(NULL, *ppzval, readonly TSRMLS_CC);
	return ret;
}
/* }}} */

/** {{{ proto public Fnx_Config_Simple::__construct(mixed $array, string $readonly)
*/
PHP_METHOD(fnx_config_simple, __construct) {
	zval *values, *readonly = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|z", &values, &readonly) == FAILURE) {
		zval *prop;

		MAKE_STD_ZVAL(prop);
		array_init(prop);
		zend_update_property(fnx_config_simple_ce, getThis(), ZEND_STRL(FNX_CONFIG_PROPERT_NAME), prop TSRMLS_CC);
		zval_ptr_dtor(&prop);

		return;
	}

	(void)fnx_config_simple_instance(getThis(), values, readonly TSRMLS_CC);
}
/** }}} */

/** {{{ proto public Fnx_Config_Simple::get(string $name = NULL)
*/
PHP_METHOD(fnx_config_simple, get) {
	zval *ret, **ppzval;
	char *name;
	uint len = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|s", &name, &len) == FAILURE) {
		return;
	}

	if (!len) {
		RETURN_ZVAL(getThis(), 1, 0);
	} else {
		zval *properties;
		HashTable *hash;

		properties = zend_read_property(fnx_config_simple_ce, getThis(), ZEND_STRL(FNX_CONFIG_PROPERT_NAME), 1 TSRMLS_CC);
		hash  = Z_ARRVAL_P(properties);

		if (zend_hash_find(hash, name, len + 1, (void **) &ppzval) == FAILURE) {
			RETURN_FALSE;
		}

		if (Z_TYPE_PP(ppzval) == IS_ARRAY) {
			if ((ret = fnx_config_simple_format(getThis(), ppzval TSRMLS_CC))) {
				RETURN_ZVAL(ret, 1, 1);
			} else {
				RETURN_NULL();
			}
		} else {
			RETURN_ZVAL(*ppzval, 1, 0);
		}
	}

	RETURN_FALSE;
}
/* }}} */

/** {{{ proto public Fnx_Config_Simple::toArray(void)
*/
PHP_METHOD(fnx_config_simple, toArray) {
	zval *properties = zend_read_property(fnx_config_simple_ce, getThis(), ZEND_STRL(FNX_CONFIG_PROPERT_NAME), 1 TSRMLS_CC);
	RETURN_ZVAL(properties, 1, 0);
}
/* }}} */

/** {{{ proto public Fnx_Config_Simple::set($name, $value)
*/
PHP_METHOD(fnx_config_simple, set) {
	zval *readonly = zend_read_property(fnx_config_simple_ce, getThis(), ZEND_STRL(FNX_CONFIG_PROPERT_NAME_READONLY), 1 TSRMLS_CC);

	if (!Z_BVAL_P(readonly)) {
		zval *name, *value, *props;
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zz", &name, &value) == FAILURE) {
			return;
		}

		if (Z_TYPE_P(name) != IS_STRING || Z_TYPE_P(name) != IS_STRING) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Expect a string key name");
			RETURN_FALSE;
		}

		Z_ADDREF_P(value);
		props = zend_read_property(fnx_config_simple_ce, getThis(), ZEND_STRL(FNX_CONFIG_PROPERT_NAME), 1 TSRMLS_CC);
		if (zend_hash_update(Z_ARRVAL_P(props), Z_STRVAL_P(name), Z_STRLEN_P(name) + 1, (void **)&value, sizeof(zval*), NULL) == SUCCESS) {
			RETURN_TRUE;
		} else {
			Z_DELREF_P(value);
		}
	}

	RETURN_FALSE;
}
/* }}} */

/** {{{ proto public Fnx_Config_Simple::__isset($name)
*/
PHP_METHOD(fnx_config_simple, __isset) {
	char *name;
	uint len;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &name, &len) == FAILURE) {
		return;
	} else {
		zval *prop = zend_read_property(fnx_config_simple_ce, getThis(), ZEND_STRL(FNX_CONFIG_PROPERT_NAME), 1 TSRMLS_CC);
		RETURN_BOOL(zend_hash_exists(Z_ARRVAL_P(prop), name, len + 1));
	}
}
/* }}} */

/** {{{ proto public Fnx_Config_Simple::offsetUnset($index)
*/
PHP_METHOD(fnx_config_simple, offsetUnset) {
	zval *readonly = zend_read_property(fnx_config_simple_ce, getThis(), ZEND_STRL(FNX_CONFIG_PROPERT_NAME_READONLY), 1 TSRMLS_CC);

	if (!Z_BVAL_P(readonly)) {
		zval *name, *props;
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &name) == FAILURE) {
			return;
		}

		if (Z_TYPE_P(name) != IS_STRING || Z_TYPE_P(name) != IS_STRING) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Expect a string key name");
			RETURN_FALSE;
		}

		props = zend_read_property(fnx_config_simple_ce, getThis(), ZEND_STRL(FNX_CONFIG_PROPERT_NAME), 1 TSRMLS_CC);
		if (zend_hash_del(Z_ARRVAL_P(props), Z_STRVAL_P(name), Z_STRLEN_P(name) + 1) == SUCCESS) {
			RETURN_TRUE;
		}
	}

	RETURN_FALSE;
}
/* }}} */

/** {{{ proto public Fnx_Config_Simple::count($name)
*/
PHP_METHOD(fnx_config_simple, count) {
	zval *prop = zend_read_property(fnx_config_simple_ce, getThis(), ZEND_STRL(FNX_CONFIG_PROPERT_NAME), 1 TSRMLS_CC);
	RETURN_LONG(zend_hash_num_elements(Z_ARRVAL_P(prop)));
}
/* }}} */

/** {{{ proto public Fnx_Config_Simple::rewind(void)
*/
PHP_METHOD(fnx_config_simple, rewind) {
	zval *prop = zend_read_property(fnx_config_simple_ce, getThis(), ZEND_STRL(FNX_CONFIG_PROPERT_NAME), 1 TSRMLS_CC);
	zend_hash_internal_pointer_reset(Z_ARRVAL_P(prop));
}
/* }}} */

/** {{{ proto public Fnx_Config_Simple::current(void)
*/
PHP_METHOD(fnx_config_simple, current) {
	zval *prop, **ppzval, *ret;

	prop = zend_read_property(fnx_config_simple_ce, getThis(), ZEND_STRL(FNX_CONFIG_PROPERT_NAME), 1 TSRMLS_CC);
	if (zend_hash_get_current_data(Z_ARRVAL_P(prop), (void **)&ppzval) == FAILURE) {
		RETURN_FALSE;
	}

	if (Z_TYPE_PP(ppzval) == IS_ARRAY) {
		if ((ret = fnx_config_simple_format(getThis(), ppzval TSRMLS_CC))) {
			RETURN_ZVAL(ret, 1, 1);
		} else {
			RETURN_NULL();
		}
	} else {
		RETURN_ZVAL(*ppzval, 1, 0);
	}
}
/* }}} */

/** {{{ proto public Fnx_Config_Simple::key(void)
*/
PHP_METHOD(fnx_config_simple, key) {
	zval *prop;
	char *string;
	long index;

	prop = zend_read_property(fnx_config_simple_ce, getThis(), ZEND_STRL(FNX_CONFIG_PROPERT_NAME), 1 TSRMLS_CC);
	zend_hash_get_current_key(Z_ARRVAL_P(prop), &string, &index, 0);
	if (zend_hash_get_current_key_type(Z_ARRVAL_P(prop)) == HASH_KEY_IS_LONG) {
		RETURN_LONG(index);
	} else {
		RETURN_STRING(string, 1);
	}
}
/* }}} */

/** {{{ proto public Fnx_Config_Simple::next(void)
*/
PHP_METHOD(fnx_config_simple, next) {
	zval *prop = zend_read_property(fnx_config_simple_ce, getThis(), ZEND_STRL(FNX_CONFIG_PROPERT_NAME), 1 TSRMLS_CC);
	zend_hash_move_forward(Z_ARRVAL_P(prop));
	RETURN_TRUE;
}
/* }}} */

/** {{{ proto public Fnx_Config_Simple::valid(void)
*/
PHP_METHOD(fnx_config_simple, valid) {
	zval *prop = zend_read_property(fnx_config_simple_ce, getThis(), ZEND_STRL(FNX_CONFIG_PROPERT_NAME), 1 TSRMLS_CC);
	RETURN_LONG(zend_hash_has_more_elements(Z_ARRVAL_P(prop)) == SUCCESS);
}
/* }}} */

/** {{{ proto public Fnx_Config_Simple::readonly(void)
*/
PHP_METHOD(fnx_config_simple, readonly) {
	zval *readonly = zend_read_property(fnx_config_simple_ce, getThis(), ZEND_STRL(FNX_CONFIG_PROPERT_NAME_READONLY), 1 TSRMLS_CC);
	RETURN_BOOL(Z_LVAL_P(readonly));
}
/* }}} */

/** {{{ proto public Fnx_Config_Simple::__destruct
*/
PHP_METHOD(fnx_config_simple, __destruct) {
}
/* }}} */

/** {{{ proto private Fnx_Config_Simple::__clone
*/
PHP_METHOD(fnx_config_simple, __clone) {
}
/* }}} */

/** {{{ fnx_config_simple_methods
*/
zend_function_entry fnx_config_simple_methods[] = {
	PHP_ME(fnx_config_simple, __construct, fnx_config_simple_construct_arginfo, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
	/* PHP_ME(fnx_config_simple, __destruct,	NULL, ZEND_ACC_PUBLIC|ZEND_ACC_DTOR) */
	PHP_ME(fnx_config_simple, __isset, fnx_config_simple_isset_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(fnx_config_simple, get, fnx_config_simple_get_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(fnx_config_simple, set, fnx_config_simple_set_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(fnx_config_simple, count, fnx_config_void_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(fnx_config_simple, offsetUnset,	fnx_config_simple_unset_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(fnx_config_simple, rewind, fnx_config_void_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(fnx_config_simple, current, fnx_config_void_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(fnx_config_simple, next,	fnx_config_void_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(fnx_config_simple, valid, fnx_config_void_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(fnx_config_simple, key, fnx_config_void_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(fnx_config_simple, readonly,	fnx_config_void_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(fnx_config_simple, toArray, fnx_config_void_arginfo, ZEND_ACC_PUBLIC)
	PHP_MALIAS(fnx_config_simple, __set, set, fnx_config_simple_set_arginfo, ZEND_ACC_PUBLIC)
	PHP_MALIAS(fnx_config_simple, __get, get, fnx_config_simple_get_arginfo, ZEND_ACC_PUBLIC)
	PHP_MALIAS(fnx_config_simple, offsetGet, get, fnx_config_simple_rget_arginfo, ZEND_ACC_PUBLIC)
	PHP_MALIAS(fnx_config_simple, offsetExists, __isset, fnx_config_simple_isset_arginfo, ZEND_ACC_PUBLIC)
	PHP_MALIAS(fnx_config_simple, offsetSet, set, fnx_config_simple_set_arginfo, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};
/* }}} */

/** {{{ FNX_STARTUP_FUNCTION
*/
FNX_STARTUP_FUNCTION(config_simple) {
	zend_class_entry ce;

	FNX_INIT_CLASS_ENTRY(ce, "Fnx_Config_Classic", "Fnx\\Config\\Classic", fnx_config_simple_methods);
	fnx_config_simple_ce = zend_register_internal_class_ex(&ce, fnx_config_ce, NULL TSRMLS_CC);

#ifdef HAVE_SPL
	zend_class_implements(fnx_config_simple_ce TSRMLS_CC, 3, zend_ce_iterator, zend_ce_arrayaccess, spl_ce_Countable);
#else
	zend_class_implements(fnx_config_simple_ce TSRMLS_CC, 2, zend_ce_iterator, zend_ce_arrayaccess);
#endif
	zend_declare_property_bool(fnx_config_simple_ce, ZEND_STRL(FNX_CONFIG_PROPERT_NAME_READONLY), 0, ZEND_ACC_PROTECTED TSRMLS_CC);

	fnx_config_simple_ce->ce_flags |= ZEND_ACC_FINAL_CLASS;

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
