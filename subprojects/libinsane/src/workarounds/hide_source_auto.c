#include <stdlib.h>
#include <string.h>
#include <regex.h>

#include <libinsane/capi.h>
#include <libinsane/log.h>
#include <libinsane/workarounds.h>
#include <libinsane/util.h>

#include "../basewrapper.h"


#define NAME "hide_source_auto"

#define SRC_NAME_REGEX "^(auto|automatic)$"


static enum lis_error root_get_children(struct lis_item *self, struct lis_item ***children)
{
	enum lis_error err;
	regex_t preg;
	int i, r, offset;
	struct lis_item *original;
	struct lis_item **new_children;

	// free any previously allocated item list
	free(lis_bw_item_get_user_ptr(self));

	original = lis_bw_get_original_item(self);
	err = original->get_children(original, children);
	if (LIS_IS_ERROR(err)) {
		return err;
	}

	r = regcomp(&preg, SRC_NAME_REGEX, REG_ICASE | REG_EXTENDED);
	if (r != 0) {
		char buf[256];
		regerror(r, &preg, buf, sizeof(buf));
		buf[sizeof(buf) - 1] = '\0';
		lis_log_error(
			"Failed to compile regex: [%s]: %d, %s",
			SRC_NAME_REGEX, r, buf
		);
		return (r == REG_ESPACE ? LIS_ERR_NO_MEM : LIS_ERR_INTERNAL_UNKNOWN_ERROR);
	}

	// copy the children list we got from the wrapped item
	for (i = 0 ; (*children)[i] != NULL ; i++) {
	}
	new_children = calloc(i + 1, sizeof(struct lis_item *));
	memcpy(new_children, *children, (i + 1) * sizeof(struct lis_item *));
	*children = new_children;
	lis_bw_item_set_user_ptr(self, new_children);

	// remove children with matching names from the new list
	offset = 0;
	for (i = 0 ; new_children[i] != NULL ; i++) {
		r = regexec(
			&preg, (new_children[i])->name,
			0 /* nmatches */, NULL /* pmatch */, 0 /* eflags */
		);
		if (r != REG_NOMATCH && r != 0) {
			lis_log_error("Regex %d has failed, code=%d !", i, r);
			return LIS_ERR_INTERNAL_UNKNOWN_ERROR;
		}
		if (r == REG_NOMATCH) {
			if (offset > 0) {
				new_children[i - offset] = new_children[i];
			}
		} else {
			lis_log_info("Hiding source '%s'", (new_children[i])->name);
			offset++;
		}
	}
	new_children[i - offset] = NULL;

	regfree(&preg);
	return LIS_OK;
}


static enum lis_error item_filter(struct lis_item *item, int root, void *user_data)
{
	LIS_UNUSED(user_data);
	if (root) {
		item->get_children = root_get_children;
	}
	return LIS_OK;
}


static void on_close_item(struct lis_item *item, int root, void *user_data)
{
	LIS_UNUSED(root);
	LIS_UNUSED(user_data);
	// free any previously allocated item list
	free(lis_bw_item_get_user_ptr(item));
}


enum lis_error lis_api_workaround_hide_source_auto(struct lis_api *to_wrap, struct lis_api **impl)
{
	enum lis_error err;
	err = lis_api_base_wrapper(to_wrap, impl, NAME);
	if (LIS_IS_OK(err)) {
		lis_bw_set_item_filter(*impl, item_filter, NULL);
		lis_bw_set_on_close_item(*impl, on_close_item, NULL);
	}
	return err;
}
