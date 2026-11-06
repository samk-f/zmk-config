#define DT_DRV_COMPAT zmk_behavior_keepalive

#include <zephyr/device.h>
#include <zephyr/kernel.h>
#include <drivers/behavior.h>
#include <zmk/behavior.h>
#include <zmk/events/keycode_state_changed.h>

#if DT_HAS_COMPAT_STATUS_OKAY(DT_DRV_COMPAT)

struct keepalive_config {
    uint32_t keycode;
    uint32_t interval_ms;
};

struct keepalive_data {
    const struct device *dev;
    struct k_work_delayable work;
    bool active;
};

static void keepalive_tick(struct k_work *work) {
    struct k_work_delayable *dwork = k_work_delayable_from_work(work);
    struct keepalive_data *data = CONTAINER_OF(dwork, struct keepalive_data, work);
    const struct keepalive_config *cfg = data->dev->config;

    if (!data->active) {
        return;
    }

    int64_t now = k_uptime_get();
    raise_zmk_keycode_state_changed_from_encoded(cfg->keycode, true, now);
    raise_zmk_keycode_state_changed_from_encoded(cfg->keycode, false, now);

    k_work_reschedule(&data->work, K_MSEC(cfg->interval_ms));
}

static int on_pressed(struct zmk_behavior_binding *binding,
                      struct zmk_behavior_binding_event event) {
    const struct device *dev = zmk_behavior_get_binding(binding->behavior_dev);
    struct keepalive_data *data = dev->data;

    bool enable = binding->param1 != 0;

    if (enable && !data->active) {
        data->active = true;
        k_work_reschedule(&data->work, K_NO_WAIT);
    } else if (!enable && data->active) {
        data->active = false;
        k_work_cancel_delayable(&data->work);
    }
    return ZMK_BEHAVIOR_OPAQUE;
}

static int on_released(struct zmk_behavior_binding *binding,
                       struct zmk_behavior_binding_event event) {
    return ZMK_BEHAVIOR_OPAQUE;
}

static const struct behavior_driver_api keepalive_api = {
    .binding_pressed = on_pressed,
    .binding_released = on_released,
};

static int keepalive_init(const struct device *dev) {
    struct keepalive_data *data = dev->data;
    data->dev = dev;
    data->active = false;
    k_work_init_delayable(&data->work, keepalive_tick);
    return 0;
}

#define KA_INST(n)                                                                  \
    static struct keepalive_data keepalive_data_##n;                                \
    static const struct keepalive_config keepalive_cfg_##n = {                      \
        .keycode = DT_INST_PROP(n, keycode),                                        \
        .interval_ms = DT_INST_PROP(n, interval_ms),                                \
    };                                                                              \
    BEHAVIOR_DT_INST_DEFINE(n, keepalive_init, NULL, &keepalive_data_##n,           \
                            &keepalive_cfg_##n, POST_KERNEL,                        \
                            CONFIG_KERNEL_INIT_PRIORITY_DEFAULT, &keepalive_api);

DT_INST_FOREACH_STATUS_OKAY(KA_INST)

#endif
