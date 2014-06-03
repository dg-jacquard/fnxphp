// Copyright (C) 2011 Dg Nechtan <dnechtan@gmail.com>, MIT

/* $Id: fnx_dispatcher.h 315725 2011-08-29 16:59:33Z nechtan $ */

#ifndef PHP_FNX_DISPATCHER_H
#define PHP_FNX_DISPATCHER_H

#define FNX_DISPATCHER_PROPERTY_NAME_REQUEST	"_request"
#define FNX_DISPATCHER_PROPERTY_NAME_VIEW		"_view"
#define FNX_DISPATCHER_PROPERTY_NAME_ROUTER		"_router"
#define FNX_DISPATCHER_PROPERTY_NAME_INSTANCE	"_instance"
#define	FNX_DISPATCHER_PROPERTY_NAME_RENDER		"_auto_render"
#define	FNX_DISPATCHER_PROPERTY_NAME_RETURN		"_return_response"
#define	FNX_DISPATCHER_PROPERTY_NAME_FLUSH		"_instantly_flush"
#define	FNX_DISPATCHER_PROPERTY_NAME_ARGS		"_invoke_args"

#define FNX_DISPATCHER_PROPERTY_NAME_MODULE		"_default_module"
#define FNX_DISPATCHER_PROPERTY_NAME_CONTROLLER	"_default_controller"
#define FNX_DISPATCHER_PROPERTY_NAME_ACTION		"_default_action"

#define FNX_ERROR_CONTROLLER					"Error"
#define FNX_ERROR_ACTION						"error"

#define FNX_DISPATCHER_PROPERTY_NAME_PLUGINS		"_plugins"

#define FNX_PLUGIN_HOOK_ROUTESTARTUP				"routerstartup"
#define FNX_PLUGIN_HOOK_ROUTESHUTDOWN 			"routershutdown"
#define FNX_PLUGIN_HOOK_LOOPSTARTUP				"dispatchloopstartup"
#define FNX_PLUGIN_HOOK_PREDISPATCH 				"predispatch"
#define FNX_PLUGIN_HOOK_POSTDISPATCH				"postdispatch"
#define FNX_PLUGIN_HOOK_LOOPSHUTDOWN				"dispatchloopshutdown"
#define FNX_PLUGIN_HOOK_PRERESPONSE				"preresponse"

#define FNX_PLUGIN_HANDLE(p, n, request, response) \
	do { \
		if (!ZVAL_IS_NULL(p)) { \
			zval **_t_plugin;\
			for(zend_hash_internal_pointer_reset(Z_ARRVAL_P(p));\
					zend_hash_has_more_elements(Z_ARRVAL_P(p)) == SUCCESS;\
					zend_hash_move_forward(Z_ARRVAL_P(p))) {\
				if (zend_hash_get_current_data(Z_ARRVAL_P(p), (void**)&_t_plugin) == SUCCESS) {\
					if (zend_hash_exists(&(Z_OBJCE_PP(_t_plugin)->function_table), n, sizeof(n))) {\
						zend_call_method_with_2_params(_t_plugin, Z_OBJCE_PP(_t_plugin), NULL, n, NULL, request, response);\
					}\
				}\
			}\
		}\
	} while(0)

extern zend_class_entry *fnx_dispatcher_ce;

fnx_dispatcher_t * fnx_dispatcher_instance(fnx_dispatcher_t *this_ptr TSRMLS_DC);
fnx_response_t * fnx_dispatcher_dispatch(fnx_dispatcher_t *dispatcher TSRMLS_DC);
int fnx_dispatcher_set_request(fnx_dispatcher_t *dispatcher, fnx_request_t *request TSRMLS_DC);

PHP_METHOD(fnx_application, app);
PHP_FUNCTION(set_error_handler);
FNX_STARTUP_FUNCTION(dispatcher);
#endif
