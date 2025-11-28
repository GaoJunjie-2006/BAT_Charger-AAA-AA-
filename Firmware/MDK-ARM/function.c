#include "function.h"
#include "stm32f1xx_hal.h"
#include "adc.h"
#include "tim.h"
#include "gpio.h"

#define current_ris_1 0.1 //电流采样电阻的阻值，建议用精密的万用表测量。
#define current_ris_2 0.1 //电流采样电阻的阻值，建议用精密的万用表测量。
#define current_ris_3 0.1 //电流采样电阻的阻值，建议用精密的万用表测量。
#define current_ris_4 0.1 //电流采样电阻的阻值，建议用精密的万用表测量。


/**/
/*结构体和变量*/
/**/
adc_data adc_data1;
int err[5];//这是计算占空比的误差值
int ccr_npn[5] = {0};
uint64_t discharge_time[5] = {0};

/**/
/*led初始化函数，上电后led闪烁五次*/
/**/
void led_init(void){
	for( int i = 0 ; i < 4 ; i++){
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4,GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_6,GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_7,GPIO_PIN_SET);
		HAL_Delay(250);
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4,GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_6,GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_7,GPIO_PIN_RESET);
		HAL_Delay(250);
	}
}



/**/
/*定时器中断初始化函数，开启定时器中断*/
/**/
void tim_init_it(void){
	HAL_TIM_Base_Start_IT(&htim1);
	HAL_TIM_Base_Start_IT(&htim2);
	HAL_TIM_Base_Start_IT(&htim3);
	HAL_TIM_Base_Start_IT(&htim4);
}



/**/
/*adc初始化*/
/**/
void adc_init(void){
	HAL_ADC_Start_DMA(&hadc1,adc_data1.raw_value,8);
}


/**/
/*ccr数值计算*/
/**/
uint16_t ccr_caculate(uint8_t channel){
	/*电池1占空比计算*/
	if(adc_data1.ris_vol[channel] >= 0.35f){
		ccr_npn[channel] --;
	}
	else if(adc_data1.adc[channel] >= 1.45){
		ccr_npn[channel] --;
	}
	else if(adc_data1.adc[channel] <= 0.9){
		ccr_npn[channel] ++;
	}
	
	if(ccr_npn[channel] < 0 ){
		ccr_npn[channel] = 0;
	}
	else if(ccr_npn[channel] > 128){
		ccr_npn[channel] = 128;
	}
	return ccr_npn[channel];
}







/*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*/
//以下是回调函数
/*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*/
void PeriodElapsedCallback(TIM_HandleTypeDef *htim){
	if(htim -> Instance == TIM2){
		
	};
	if(htim -> Instance == 	TIM4){
		/*电池1放电*/
		if(adc_data1.adc[1] >= 0.9 && discharge_time[1] < 3000000){
			HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,GPIO_PIN_SET);
			discharge_time[1] ++;
		}
		else{
			HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,GPIO_PIN_RESET);
			discharge_time[1] = 0;
		}
		/*电池2放电*/
		if(adc_data1.adc[2] >= 0.9 && discharge_time[2] < 3000000){
			HAL_GPIO_WritePin(GPIOB,GPIO_PIN_2,GPIO_PIN_SET);
			discharge_time[2] ++;
		}
		else{
			HAL_GPIO_WritePin(GPIOB,GPIO_PIN_2,GPIO_PIN_RESET);
			discharge_time[2] = 0;
		}
		/*电池3放电*/
		if(adc_data1.adc[3] >= 0.9 && discharge_time[3] < 3000000){
			HAL_GPIO_WritePin(GPIOB,GPIO_PIN_10,GPIO_PIN_SET);
			discharge_time[3] ++;
		}
		else{
			HAL_GPIO_WritePin(GPIOB,GPIO_PIN_10,GPIO_PIN_RESET);
			discharge_time[3] = 0;
		}
		/*电池4放电*/
		if(adc_data1.adc[4] >= 0.9 && discharge_time[4] < 3000000){
			HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,GPIO_PIN_SET);
			discharge_time[4] ++;
		}
		else{
			HAL_GPIO_WritePin(GPIOB,GPIO_PIN_11,GPIO_PIN_RESET);
			discharge_time[4] = 0;
		}
		__HAL_TIM_SetCompare(&htim1 , TIM_CHANNEL_1, ccr_caculate(1));
		__HAL_TIM_SetCompare(&htim1 , TIM_CHANNEL_2, ccr_caculate(2));
		__HAL_TIM_SetCompare(&htim1 , TIM_CHANNEL_3, ccr_caculate(3));
		__HAL_TIM_SetCompare(&htim1 , TIM_CHANNEL_4, ccr_caculate(4));
	}
}


void ConvCpltCallback(ADC_HandleTypeDef *hadc){
	if(hadc -> Instance == ADC1){
		
		adc_data1.adc[1] = adc_data1.raw_value[0] / 4095.0f * 3.3f;
		adc_data1.adc[2] = adc_data1.raw_value[1] / 4095.0f * 3.3f;
		adc_data1.adc[3] = adc_data1.raw_value[2] / 4095.0f * 3.3f;
		adc_data1.adc[4] = adc_data1.raw_value[3] / 4095.0f * 3.3f;//读取电池实际电压
		
		adc_data1.adc_cu_1 = adc_data1.raw_value[4] / 4095.0f * 3.3f;
		adc_data1.adc_cu_2 = adc_data1.raw_value[5] / 4095.0f * 3.3f;
		adc_data1.adc_cu_3 = adc_data1.raw_value[6] / 4095.0f * 3.3f;
		adc_data1.adc_cu_4 = adc_data1.raw_value[7] / 4095.0f * 3.3f;	//读取采样电阻对地电压
		
		adc_data1.ris_vol[1] = adc_data1.adc_cu_1 - adc_data1.adc[1];
		adc_data1.ris_vol[2] = adc_data1.adc_cu_2 - adc_data1.adc[2];
		adc_data1.ris_vol[3] = adc_data1.adc_cu_3 - adc_data1.adc[3];
		adc_data1.ris_vol[4] = adc_data1.adc_cu_4 - adc_data1.adc[4];//电流采样电阻的电压
		
		adc_data1.current[1] = adc_data1.ris_vol[1] / current_ris_1 ; 
		adc_data1.current[2] = adc_data1.ris_vol[2] / current_ris_2 ; 
		adc_data1.current[3] = adc_data1.ris_vol[3] / current_ris_3 ; 
		adc_data1.current[4] = adc_data1.ris_vol[4] / current_ris_4 ;//计算各通道电流值 
	}
}










