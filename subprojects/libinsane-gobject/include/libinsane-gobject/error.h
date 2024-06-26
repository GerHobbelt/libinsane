#ifndef __LIBINSANE_GOBJECT_ERROR_H
#define __LIBINSANE_GOBJECT_ERROR_H

#include <glib-object.h>

G_BEGIN_DECLS

GQuark libinsane_error_quark(void);

#define LIBINSANE_ERROR libinsane_error_quark()

typedef enum {
	LIBINSANE_ERROR_OK = 0,
	LIBINSANE_ERROR_CANCELLED,
	LIBINSANE_ERROR_DEVICE_BUSY,
	LIBINSANE_ERROR_UNSUPPORTED,
	LIBINSANE_ERROR_INVALID_VALUE,
	LIBINSANE_ERROR_JAMMED,
	LIBINSANE_ERROR_COVER_OPEN,
	LIBINSANE_ERROR_IO_ERROR,
	LIBINSANE_ERROR_NO_MEM,
	LIBINSANE_ERROR_ACCESS_DENIED,
	LIBINSANE_ERROR_HW_IS_LOCKED,
	LIBINSANE_ERROR_INTERNAL_IMG_FORMAT_NOT_SUPPORTED,
	LIBINSANE_ERROR_INTERNAL_NOT_IMPLEMENTED,
	LIBINSANE_ERROR_INTERNAL_UNKNOWN_ERROR,
	LIBINSANE_ERROR_OFFLINE,
} LibinsaneError;

G_END_DECLS

#endif
