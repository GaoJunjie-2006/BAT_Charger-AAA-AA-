#include "stdint.h"


void led_init(void);
void tim_init_it(void);
typedef struct{
	uint32_t raw_value[8];
	float adc[5];
	float adc_cu_1;
	float adc_cu_2;
	float adc_cu_3;
	float adc_cu_4;
	float current[5];
	float ris_vol[5];
}	adc_data;


uint16_t ccr_caculate(uint8_t channel);





/*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*/
//以下是回调函数
/*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*/
