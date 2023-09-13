#include <stdio.h>
#include <string.h>

#include <CUnit/Basic.h>

#include <libinsane/capi.h>
#include <libinsane/constants.h>
#include <libinsane/dumb.h>
#include <libinsane/log.h>
#include <libinsane/util.h>
#include <libinsane/normalizers.h>
#include <libinsane/workarounds.h>

#include "main.h"
#include "util.h"


static struct lis_api *g_dumb = NULL;
static struct lis_api *g_api_nodes = NULL;
static struct lis_api *g_api_source_auto = NULL;


static int tests_hide_source_auto_init(void)
{
	static const union lis_value opt_source_constraint[] = {
		{ .string = OPT_VALUE_SOURCE_FLATBED, },
		{ .string = OPT_VALUE_SOURCE_ADF " COW", },
		{ .string = "Auto", },
		{ .string = "Automatic", },
		{ .string = "Automatic document Feeder TULIPE", },
		{ .string = "0000\\Root\\Flatbed MEH", },
	};
	static const struct lis_option_descriptor opt_source_template = {
		.name = OPT_NAME_SOURCE,
		.title = "source title",
		.desc = "source desc",
		.capabilities = LIS_CAP_SW_SELECT,
		.value = {
			.type = LIS_TYPE_STRING,
			.unit = LIS_UNIT_NONE,
		},
		.constraint = {
			.type = LIS_CONSTRAINT_LIST,
			.possible.list = {
				.nb_values = LIS_COUNT_OF(opt_source_constraint),
				.values = (union lis_value*)&opt_source_constraint,
			},
		},
	};
	static const union lis_value opt_source_default = {
		.string = OPT_VALUE_SOURCE_ADF
	};
	enum lis_error err;

	g_dumb = NULL;
	g_api_nodes = NULL;
	g_api_source_auto = NULL;
	err = lis_api_dumb(&g_dumb, "dummy0");
	if (LIS_IS_ERROR(err)) {
		return -1;
	}

	lis_dumb_set_nb_devices(g_dumb, 2);
	lis_dumb_add_option(
		g_dumb, &opt_source_template, &opt_source_default,
		LIS_SET_FLAG_MUST_RELOAD_PARAMS
	);

	err = lis_api_normalizer_source_nodes(g_dumb, &g_api_nodes);
	if (LIS_IS_ERROR(err)) {
		return -1;
	}

	return 0;
}


static int tests_hide_source_auto_clean(void)
{
	struct lis_api *api = (
		g_api_source_auto != NULL ? g_api_source_auto : (
			g_api_nodes != NULL ? g_api_nodes :
				g_dumb
		)
	);
	api->cleanup(api);
	return 0;
}


static void tests_hide_source_auto(void)
{
	enum lis_error err;
	struct lis_item *root;
	struct lis_item **sources;

	LIS_ASSERT_EQUAL(tests_hide_source_auto_init(), 0);

	err = lis_api_workaround_hide_source_auto(g_api_nodes, &g_api_source_auto);
	LIS_ASSERT_EQUAL(err, LIS_OK);

	err = g_api_source_auto->get_device(g_api_source_auto, LIS_DUMB_DEV_ID_FIRST, &root);
	LIS_ASSERT_EQUAL(err, LIS_OK);

	err = root->get_children(root, &sources);
	LIS_ASSERT_EQUAL(err, LIS_OK);

	LIS_ASSERT_EQUAL(strcmp(sources[0]->name, OPT_VALUE_SOURCE_FLATBED), 0);
	LIS_ASSERT_EQUAL(strcmp(sources[1]->name, OPT_VALUE_SOURCE_ADF " COW"), 0);
	LIS_ASSERT_EQUAL(strcmp(sources[2]->name, "Automatic document Feeder TULIPE"), 0);
	LIS_ASSERT_EQUAL(strcmp(sources[3]->name, "0000\\Root\\Flatbed MEH"), 0);
	LIS_ASSERT_EQUAL(sources[4], NULL);

	root->close(root);

	LIS_ASSERT_EQUAL(tests_hide_source_auto_clean(), 0);
}


int register_tests(void)
{
	CU_pSuite suite = NULL;

	suite = CU_add_suite("workaround_hide_source_auto", NULL, NULL);
	if (suite == NULL) {
		fprintf(stderr, "CU_add_suite() failed\n");
		return 0;
	}

	if (CU_add_test(suite, "tests_hide_source_auto()", tests_hide_source_auto) == NULL) {
		fprintf(stderr, "CU_add_test() has failed\n");
		return 0;
	}

	return 1;
}
