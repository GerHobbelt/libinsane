#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <CUnit/Basic.h>

#define LIS_UNIT_TESTS

#include <libinsane/capi.h>
#include <libinsane/constants.h>
#include <libinsane/dumb.h>
#include <libinsane/log.h>
#include <libinsane/workarounds.h>
#include <libinsane/util.h>

#include "main.h"
#include "util.h"


static struct lis_api *g_dumb = NULL;
static struct lis_api *g_ips = NULL;


static int tests_ips_init(void)
{
	enum lis_error err;

	g_dumb = NULL;
	g_ips = NULL;

	err = lis_api_dumb(&g_dumb, "dummy0");
	if (LIS_IS_ERROR(err)) {
		return -1;
	}

	lis_dumb_set_nb_devices(g_dumb, 2);

	err = lis_api_workaround_invalid_page_size(g_dumb, &g_ips);
	if (LIS_IS_ERROR(err)) {
		return -1;
	}

	return 0;
}


static int tests_ips_clean(void)
{
	struct lis_api *api = (g_ips != NULL ? g_ips : g_dumb);
	api->cleanup(api);
	return 0;
}


static void tests_ips_on_off(void)
{
	struct lis_item *item;
	struct lis_scan_session *session;
	size_t bufsize;
	enum lis_error err;
	uint8_t buffer[64];
	static const uint8_t line_a[] = { 0x00, 0xAA, };
	static const uint8_t line_b[] = { 0x55, };
	static const uint8_t line_c[] = { 0xFF, };
	static const struct lis_dumb_read reads[] = {
		{ .content = line_a, .nb_bytes = LIS_COUNT_OF(line_a) },
		{ .content = line_b, .nb_bytes = LIS_COUNT_OF(line_b) },
		{ .content = line_c, .nb_bytes = LIS_COUNT_OF(line_c) },
	};
	struct lis_scan_parameters params = {
		.format = LIS_IMG_FORMAT_RAW_RGB_24,
		.width = 2500,
		.height = 2500,
		.image_size = 2500 * 2500 * 3,
	};

	LIS_ASSERT_EQUAL(tests_ips_init(), 0);

	item = NULL;
	err = g_ips->get_device(g_ips, LIS_DUMB_DEV_ID_FIRST, &item);
	LIS_ASSERT_EQUAL(err, LIS_OK);

	lis_dumb_set_scan_result(g_dumb, reads, LIS_COUNT_OF(reads));
	lis_dumb_set_scan_parameters(g_dumb, &params);

	err = item->scan_start(item, &session);
	LIS_ASSERT_EQUAL(err, LIS_OK);

	LIS_ASSERT_FALSE(session->end_of_feed(session));
	while(!session->end_of_page(session)) {
		bufsize = sizeof(buffer);
		err = session->scan_read(session, buffer, &bufsize);
		LIS_ASSERT_EQUAL(err, LIS_OK);
	}

	lis_dumb_set_scan_result(g_dumb, reads, LIS_COUNT_OF(reads));
	lis_dumb_set_scan_parameters(g_dumb, &params);
	LIS_ASSERT_FALSE(session->end_of_feed(session));
	while(!session->end_of_page(session)) {
		bufsize = sizeof(buffer);
		err = session->scan_read(session, buffer, &bufsize);
		LIS_ASSERT_EQUAL(err, LIS_OK);
	}

	lis_dumb_set_scan_result(g_dumb, reads, LIS_COUNT_OF(reads));
	params.height = 0; // <---
	lis_dumb_set_scan_parameters(g_dumb, &params);
	LIS_ASSERT_TRUE(session->end_of_feed(session));

	session->cancel(session);

	item->close(item);

	LIS_ASSERT_EQUAL(tests_ips_clean(), 0);
}


int register_tests(void)
{
	CU_pSuite suite = NULL;

	suite = CU_add_suite("Workaround_invalid_page_size", NULL, NULL);
	if (suite == NULL) {
		fprintf(stderr, "CU_add_suite() failed\n");
		return 0;
	}

	if (CU_add_test(suite, "tests_ips_on_off()", tests_ips_on_off) == NULL) {
		fprintf(stderr, "CU_add_test() has failed\n");
		return 0;
	}

	return 1;
}
