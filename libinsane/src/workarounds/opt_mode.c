#include <libinsane/workarounds.h>
#include <libinsane/util.h>


enum lis_error lis_api_workaround_opt_mode(struct lis_api *to_wrap, struct lis_api **impl)
{
	LIS_UNUSED(to_wrap);
	LIS_UNUSED(impl);
	return LIS_ERR_INTERNAL_NOT_IMPLEMENTED;
}
