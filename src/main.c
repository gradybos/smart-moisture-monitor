#include <zephyr/kernel.h>
#include <zephyr/drivers/adc.h>

#define SENSOR_INTERVAL_MS 10000

static const struct adc_dt_spec sensor = ADC_DT_SPEC_GET(DT_PATH(zephyr_user));

int main(void)
{
        int ret;
        int16_t buf;
        int32_t val_mv;
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
                        val_mv = buf;
                }

                ret = adc_raw_to_millivolts_dt(&sensor, &val_mv);
                printf("mV on AIN3: %d\n", val_mv);
                k_msleep(SENSOR_INTERVAL_MS);
        }
}
