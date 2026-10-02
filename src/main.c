#include <zephyr/kernel.h>
#include <zephyr/drivers/adc.h>

#define SENSOR_INTERVAL_MS 10000
#define WET_THRESHOLD_MV 1200
#define DRY_THRESHOLD_MV 2200

static const struct adc_dt_spec sensor = ADC_DT_SPEC_GET(DT_PATH(zephyr_user));

int8_t sensorMVToPercent(int16_t reading_mv) {
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

int main(void)
{
        int ret;
        int16_t buf;
        int32_t sensor_reading_mv;
        struct adc_sequence sequence = {
                .buffer = &buf,
                .buffer_size = sizeof(buf)
        };

        if (!adc_is_ready_dt(&sensor)) {
                return 0;
        }

        ret = adc_channel_setup_dt(&sensor);
        if (ret < 0) {
                return 0;
        }

        ret = adc_sequence_init_dt(&sensor, &sequence);
        if (ret < 0) {
                return 0;
        }

        while(1){
                ret = adc_read(sensor.dev, &sequence);
                if (ret < 0) {
                        continue;
                }
                else {
                        sensor_reading_mv = buf;
                }

                adc_raw_to_millivolts_dt(&sensor, &sensor_reading_mv);
                printf("percent on AIN3: %d\n", sensorMVToPercent(sensor_reading_mv));
                k_msleep(SENSOR_INTERVAL_MS);
        }
}
