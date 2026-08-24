/*
 * SPDX-License-Identifier: Apache-2.0
 *
 * PAW3222 settings exposed through zmk-feature-custom-settings.
 */

#include <string.h>

#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/sys/util.h>

#include <cormoran/zmk/custom_settings.h>
#include <zmk/event_manager.h>

#include <paw3222.h>

#define DT_DRV_COMPAT pixart_paw3222

#if DT_HAS_COMPAT_STATUS_OKAY(DT_DRV_COMPAT)

#define PAW3222_SETTINGS_SUBSYSTEM_ID "cormoran__paw3222"
#define PAW3222_CPI_KEY "cpi@trackball"
#define PAW3222_RES_MIN 608
#define PAW3222_RES_MAX 4826
#define PAW3222_DEVICE DEVICE_DT_GET(DT_DRV_INST(0))

ZMK_CUSTOM_SETTING_DEFINE(
    paw3222_setting_cpi, PAW3222_SETTINGS_SUBSYSTEM_ID, PAW3222_CPI_KEY,
    ZMK_CUSTOM_SETTING_VALUE_TYPE_INT32,
    ZMK_CUSTOM_SETTING_VALUE_INT32(DT_INST_PROP_OR(0, res_cpi, PAW3222_RES_MIN)),
    ZMK_CUSTOM_SETTING_CONFIDENTIALITY_RPC_PUBLIC,
    ZMK_CUSTOM_SETTING_PERMISSION_UNSECURE,
    ZMK_CUSTOM_SETTING_PERMISSION_SECURE,
    ZMK_CUSTOM_SETTING_RANGE_INT32(PAW3222_RES_MIN, PAW3222_RES_MAX));

static void paw3222_apply_cpi(void) {
    struct zmk_custom_setting_value value;

    if (!device_is_ready(PAW3222_DEVICE)) {
        return;
    }

    if (zmk_custom_setting_read_by_key(PAW3222_SETTINGS_SUBSYSTEM_ID, PAW3222_CPI_KEY,
                                       &value) != 0 ||
        value.type != ZMK_CUSTOM_SETTING_VALUE_TYPE_INT32) {
        return;
    }

    /*
     * PAW3222 uses 38-CPI hardware steps. paw32xx_set_resolution() converts
     * the requested value to the closest supported step at or below it.
     */
    paw32xx_set_resolution(PAW3222_DEVICE, (uint16_t)value.int32_value);
}

static int paw3222_settings_event_listener(const zmk_event_t *eh) {
    if (as_zmk_custom_settings_initialized(eh) != NULL) {
        paw3222_apply_cpi();
        return 0;
    }

    const struct zmk_custom_setting_changed *ev = as_zmk_custom_setting_changed(eh);
    if (!ev || !ev->setting) {
        return 0;
    }

    if (strncmp(ev->setting->custom_subsystem_id, PAW3222_SETTINGS_SUBSYSTEM_ID,
                CONFIG_ZMK_CUSTOM_SETTINGS_CUSTOM_SUBSYSTEM_ID_MAX_LEN) == 0) {
        paw3222_apply_cpi();
    }

    return 0;
}

ZMK_LISTENER(paw3222_settings, paw3222_settings_event_listener);
ZMK_SUBSCRIPTION(paw3222_settings, zmk_custom_setting_changed);
ZMK_SUBSCRIPTION(paw3222_settings, zmk_custom_settings_initialized);

#endif
