#include <libinsane/workarounds.h>
#include <libinsane/util.h>


enum lis_error lis_api_workaround_cache(
		struct lis_api *to_wrap, struct lis_api **out_impl
	)
{
	LIS_UNUSED(to_wrap);
	LIS_UNUSED(out_impl);
	// TODO
	return LIS_ERR_INTERNAL_NOT_IMPLEMENTED;
}
