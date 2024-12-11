#include <zephyr/types.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/drivers/gpio.h>
#include <dk_buttons_and_leds.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/mgmt/mcumgr/mgmt/mgmt.h>

#define SLEEP_TIME_MS   500
#define LED_OTA_STATUS  DK_LED1
#define LED_CON_STATUS  DK_LED2
#define LED_RUNNING     DK_LED3

static struct bt_conn *current_conn;

static const struct bt_data ad[] = {
    BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
    BT_DATA_BYTES(BT_DATA_UUID128_ALL,
              0x84, 0xaa, 0x60, 0x74, 0x52, 0x8a, 0x8b, 0x86,
              0xd3, 0x4c, 0xb7, 0x1d, 0x1d, 0xdc, 0x53, 0x8d),
};

/* 系統狀態 */
static bool dfu_mode = false;

static void connected(struct bt_conn *conn, uint8_t err)
{
    if (err) {
        printk("Connection failed (err %u)\n", err);
        return;
    }

    printk("Connected\n");
    current_conn = bt_conn_ref(conn);
    dk_set_led_on(LED_CON_STATUS);
}

static void disconnected(struct bt_conn *conn, uint8_t reason)
{
    printk("Disconnected (reason %u)\n", reason);

    if (current_conn) {
        bt_conn_unref(current_conn);
        current_conn = NULL;
    }

    dk_set_led_off(LED_CON_STATUS);

    // 如果在 DFU 模式下斷開連接，關閉 DFU 指示燈
    if (dfu_mode) {
        dfu_mode = false;
        dk_set_led_off(LED_OTA_STATUS);
    }
}

static struct bt_conn_cb conn_callbacks = {
    .connected = connected,
    .disconnected = disconnected,
};

static void bt_ready(int err)
{
    if (err) {
        printk("Bluetooth init failed (err %d)\n", err);
        return;
    }

    printk("Bluetooth initialized\n");

    // 使用設備名稱進行廣播
    err = bt_le_adv_start(BT_LE_ADV_CONN_NAME, ad, ARRAY_SIZE(ad), NULL, 0);
    if (err) {
        printk("Advertising failed to start (err %d)\n", err);
        return;
    }

    printk("Advertising successfully started\n");
}

/* 按鈕處理函數 */
static void button_handler(uint32_t button_state, uint32_t has_changed)
{
    if (has_changed & DK_BTN1_MSK) {
        if (button_state & DK_BTN1_MSK) {
            // 切換 DFU 模式
            dfu_mode = !dfu_mode;
            printk("%s DFU mode\n", dfu_mode ? "Entering" : "Exiting");

            if (dfu_mode) {
                dk_set_led_on(LED_OTA_STATUS);
            } else {
                dk_set_led_off(LED_OTA_STATUS);
            }
        }
    }
}

int main(void)
{
    int err;
    bool led_running = false;

    printk("Starting MCUboot BLE OTA example\n");

    // Initialize LEDs and Buttons
    err = dk_leds_init();
    if (err) {
        printk("LEDs init failed (err %d)\n", err);
        return 0;
    }

    err = dk_buttons_init(button_handler);
    if (err) {
        printk("Buttons init failed (err %d)\n", err);
        return 0;
    }

    // Initialize Bluetooth
    err = bt_enable(bt_ready);
    if (err) {
        printk("Bluetooth init failed (err %d)\n", err);
        return 0;
    }

    bt_conn_cb_register(&conn_callbacks);

    // // Initialize MCUmgr
    // err = os_mgmt_register_group();
    // if (err) {
    //     printk("Failed to register OS management group (err: %d)\n", err);
    //     return 0;
    // }

    // err = img_mgmt_register_group();
    // if (err) {
    //     printk("Failed to register IMG management group (err: %d)\n", err);
    //     return 0;
    // }

    // // Initialize SMP over Bluetooth transport
    // err = mcumgr_smp_bt_register();
    // if (err) {
    //     printk("Failed to register SMP BT transport (err: %d)\n", err);
    //     return 0;
    // }

    printk("MCUboot BLE OTA example started\n");
    dk_set_led_on(LED_RUNNING);
    led_running = true;

    while (1) {
        k_msleep(SLEEP_TIME_MS);
        // 只在非 DFU 模式下閃爍運行指示燈
        if (!dfu_mode) {
            led_running = !led_running;
            (led_running) ? dk_set_led_on(LED_RUNNING) : dk_set_led_off(LED_RUNNING);
        }
    }

    return 0;
}
