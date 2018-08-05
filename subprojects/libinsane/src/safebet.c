#include <stdlib.h>
#include <string.h>

#include <libinsane/capi.h>
#include <libinsane/dumb.h>
#include <libinsane/error.h>
#include <libinsane/multiplexer.h>
#include <libinsane/normalizers.h>
#include <libinsane/safebet.h>
#include <libinsane/util.h>
#include <libinsane/workarounds.h>

#ifdef OS_LINUX
#include <libinsane/sane.h>
#endif

#ifdef OS_WINDOWS
#include <libinsane/twain.h>
#include <libinsane/wia_automation.h>
#include <libinsane/wia_ll.h>
#endif


static int lis_getenv(const char *var, int default_val)
{
	const char *val_str;

	val_str = getenv(var);
	if (val_str == NULL) {
		return default_val;
	}
	return atoi(val_str);
}


enum lis_error lis_safebet(struct lis_api **out_impls)
{
	enum lis_error err = LIS_ERR_UNSUPPORTED;
	struct lis_api *impls[4] = { 0 };
	int nb_impls = 0;
	struct lis_api *next;

	*out_impls = NULL;

#ifdef OS_LINUX
	if (lis_getenv("LIBINSANE_SANE", 1)) {
		err = lis_api_sane(&impls[nb_impls]);
		if (LIS_IS_ERROR(err)) {
			goto err_impls;
		}
		nb_impls++;
	}
#endif
	if (lis_getenv("LIBINSANE_DUMB", nb_impls == 0)) {
		err = lis_api_dumb(&impls[nb_impls], "dumb");
		if (LIS_IS_ERROR(err)) {
			goto err_impls;
		}
		nb_impls++;
	}

	err = lis_api_multiplexer(impls, nb_impls, &next);
	if (LIS_IS_ERROR(err)) {
		goto err_impls;
	}
	*out_impls = next;

	if (lis_getenv("LIBINSANE_NORMALIZER_SOURCE_NODES", 1)) {
		err = lis_api_normalizer_source_nodes(*out_impls, &next);
		if (LIS_IS_ERROR(err)) {
			goto error;
		}
		*out_impls = next;
	}

	if (lis_getenv("LIBINSANE_WORKAROUND_OPT_VALUES", 1)) {
		err = lis_api_workaround_opt_values(*out_impls, &next);
		if (LIS_IS_ERROR(err)) {
			goto error;
		}
		*out_impls = next;
	}

	if (lis_getenv("LIBINSANE_WORKAROUND_OPT_NAMES", 1)) {
		err = lis_api_workaround_opt_names(*out_impls, &next);
		if (LIS_IS_ERROR(err)) {
			goto error;
		}
		*out_impls = next;
	}

	if (lis_getenv("LIBINSANE_NORMALIZER_SAFE_DEFAULTS", 1)) {
		err = lis_api_normalizer_safe_defaults(*out_impls, &next);
		if (LIS_IS_ERROR(err)) {
			goto error;
		}
		*out_impls = next;
	}

	if (lis_getenv("LIBINSANE_NORMALIZER_SOURCE_TYPES", 1)) {
		err = lis_api_normalizer_source_types(*out_impls, &next);
		if (LIS_IS_ERROR(err)) {
			goto error;
		}
		*out_impls = next;
	}

	return err;

error:
	(*out_impls)->cleanup(*out_impls);
	*out_impls = NULL;
	return err;

err_impls:
	for (nb_impls-- ; nb_impls >= 0 ; nb_impls--) {
		impls[nb_impls]->cleanup(impls[nb_impls]);
	}
	return err;
}
