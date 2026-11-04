#include <zephyr/kernel.h>
#include <zephyr/drivers/adc.h>

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/gatt.h>
// Sensor parameters
#define SENSOR_INTERVAL_MS 10000
#define WET_THRESHOLD_MV 1200
#define DRY_THRESHOLD_MV 2200

static int8_t percent_moisture;
static const struct adc_dt_spec sensor = ADC_DT_SPEC_GET(DT_PATH(zephyr_user));

#define DEVICE_NAME CONFIG_BT_DEVICE_NAME
#define DEVICE_NAME_LEN (sizeof(DEVICE_NAME) - 1)
#define ADV_LEN 12

/* Advertising data */
static uint8_t manuf_data[ADV_LEN] = {
	0x01 /*SKD version */,
	0x83 /* STM32WB - P2P Server 1 */,
	0x00 /* GROUP A Feature  */,
	0x00 /* GROUP A Feature */,
	0x00 /* GROUP B Feature */,
	0x00 /* GROUP B Feature */,
	0x00, /* BLE MAC start -MSB */
	0x00,
	0x00,
	0x00,
	0x00,
	0x00, /* BLE MAC stop */
};

static const struct bt_data ad[] = {
	BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
	BT_DATA(BT_DATA_NAME_COMPLETE, DEVICE_NAME, DEVICE_NAME_LEN),
	BT_DATA(BT_DATA_MANUFACTURER_DATA, manuf_data, ADV_LEN)
};

/* BLE connection */
struct bt_conn *ble_conn;

static const struct bt_uuid_128 st_service_uuid = BT_UUID_INIT_128(
	BT_UUID_128_ENCODE(0x0000fe40, 0xcc7a, 0x482a, 0x984a, 0x7f2ed5b3e58f));

static const struct bt_uuid_128 sensor_notif_uuid = BT_UUID_INIT_128(
	BT_UUID_128_ENCODE(0x0000fe41, 0x8e22, 0x4541, 0x9d4c, 0x21edae82ed19));

volatile bool notify_enable;

static void mpu_ccc_cfg_changed(const struct bt_gatt_attr *attr, uint16_t value)
{
	ARG_UNUSED(attr);
	notify_enable = (value == BT_GATT_CCC_NOTIFY);
}

BT_GATT_SERVICE_DEFINE(stsensor_svc,
BT_GATT_PRIMARY_SERVICE(&st_service_uuid),
BT_GATT_CHARACTERISTIC(&sensor_notif_uuid.uuid, BT_GATT_CHRC_NOTIFY,
		       BT_GATT_PERM_READ, NULL, NULL, &percent_moisture),
BT_GATT_CCC(mpu_ccc_cfg_changed, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),
);

K_SEM_DEFINE(ble_conn_sem, 0, 1);
atomic_t ble_conn_flag;

int8_t sensor_mv_to_percent(int16_t reading_mv) {
        int8_t percent;
        if (reading_mv > DRY_THRESHOLD_MV) {
                percent = 0;
        }
        else if (reading_mv < WET_THRESHOLD_MV) {
                percent = 100;
        }
        else {
                percent = ((reading_mv-WET_THRESHOLD_MV) / ((DRY_THRESHOLD_MV-WET_THRESHOLD_MV)/100));
        }
        return percent;
}

static void bt_ready(int err)
{
	if (!err) {
		err = bt_le_adv_start(BT_LE_ADV_CONN_FAST_1, ad, ARRAY_SIZE(ad), NULL, 0);
	}
	/* Start advertising */
}

static void connected(struct bt_conn *connected, uint8_t err)
{
	if (!err && !ble_conn) {
                ble_conn = bt_conn_ref(connected);
                atomic_set(&ble_conn_flag, 1);
                k_sem_give(&ble_conn_sem);
	}
}

static void disconnected(struct bt_conn *disconn, uint8_t reason)
{
	if (ble_conn) {
                atomic_clear(&ble_conn_flag);
		bt_conn_unref(ble_conn);
		ble_conn = NULL;
	}

}

static void recycled(void)
{
        bt_le_adv_start(BT_LE_ADV_CONN_FAST_1, ad, ARRAY_SIZE(ad), NULL, 0);
}

BT_CONN_CB_DEFINE(conn_callbacks) = {
	.connected = connected,
	.disconnected = disconnected,
        .recycled = recycled,
};

int main(void)
{
        bt_enable(bt_ready);
        int16_t buf;
        int32_t sensor_reading_mv;
        struct adc_sequence sequence = {
                .buffer = &buf,
                .buffer_size = sizeof(buf)
        };

        if (!adc_is_ready_dt(&sensor)) {return 0;}
        if (adc_channel_setup_dt(&sensor) < 0) {return 0;}
        if (adc_sequence_init_dt(&sensor, &sequence) < 0) {return 0;}

        while(1){
                k_sem_take(&ble_conn_sem, K_FOREVER);
                while(atomic_get(&ble_conn_flag)) {
                        if (!notify_enable) {
                                k_msleep(SENSOR_INTERVAL_MS);
                                continue;
                        }

                        if (adc_read(sensor.dev, &sequence) < 0) {
                                k_msleep(SENSOR_INTERVAL_MS);
                                continue;
                        }
                        else {
                                sensor_reading_mv = buf;
                        }

                        if (adc_raw_to_millivolts_dt(&sensor, &sensor_reading_mv) < 0) {
                                k_msleep(SENSOR_INTERVAL_MS);
                                continue;
                        }

                        percent_moisture = sensor_mv_to_percent(sensor_reading_mv);

                        bt_gatt_notify(NULL, &stsensor_svc.attrs[2],
                                &percent_moisture, sizeof(percent_moisture));

                        k_msleep(SENSOR_INTERVAL_MS);
                }
        }
}
