/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "spi.h"
#include "tim.h"
#include "gpio.h"
#include "fsmc.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "software_timer.h"
#include "led_7seg.h"
#include "button.h"
#include "lcd.h"
#include "picture.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void system_init();
void test_LedDebug();
void test_LedY0();
void test_LedY1();
void test_7seg();
void test_button();
void test_lcd();
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM2_Init();
  MX_SPI1_Init();
  MX_FSMC_Init();
  /* USER CODE BEGIN 2 */
  system_init();
  lcd_Clear(WHITE);
  test_lcd();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  while(!flag_timer2);
	  flag_timer2 = 0;
	  button_Scan();
	  trafic_lights();
//	  test_button();

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV4;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void system_init(){
	  HAL_GPIO_WritePin(OUTPUT_Y0_GPIO_Port, OUTPUT_Y0_Pin, 0);
	  HAL_GPIO_WritePin(OUTPUT_Y1_GPIO_Port, OUTPUT_Y1_Pin, 0);
	  HAL_GPIO_WritePin(DEBUG_LED_GPIO_Port, DEBUG_LED_Pin, 0);
	  timer_init();
	  led7_init();
	  button_init();
	  lcd_init();
	  setTimer2(50);
}

uint8_t count1 = 0;
uint8_t count2 = 0;
uint8_t count_second = 0;

uint8_t count_led_debug = 0;
uint8_t count_led_Y0 = 0;
uint8_t count_led_Y1 = 0;

uint8_t count_red = 5;
uint8_t count_green = 3;
uint8_t count_yellow = 2;
uint8_t temp_count[3];
uint8_t temp[3];

enum Mode{
	modeRED,
	modeGREEN,
	modeYELLOW,
	modeRunning,
	modeInit
};

enum Mode currentMode = modeInit;

void test_LedDebug(){
	count_led_debug = (count_led_debug + 1)%10;
	if(count_led_debug == 0){
		HAL_GPIO_TogglePin(DEBUG_LED_GPIO_Port, DEBUG_LED_Pin);
		HAL_GPIO_WritePin(OUTPUT_Y0_GPIO_Port, OUTPUT_Y0_Pin, 0);
		HAL_GPIO_WritePin(OUTPUT_Y1_GPIO_Port, OUTPUT_Y1_Pin, 0);
	}
}

void test_LedY0(){
	count_led_Y0 = (count_led_Y0 + 1)%10;
	if(count_led_Y0 == 0){
		HAL_GPIO_TogglePin(OUTPUT_Y0_GPIO_Port, OUTPUT_Y0_Pin);
		HAL_GPIO_WritePin(DEBUG_LED_GPIO_Port, DEBUG_LED_Pin, 0);
		HAL_GPIO_WritePin(OUTPUT_Y1_GPIO_Port, OUTPUT_Y1_Pin, 0);
	}
}

void test_LedY1(){
	count_led_Y1 = (count_led_Y1 + 1)%10;
	if(count_led_Y1 == 0){
		HAL_GPIO_TogglePin(OUTPUT_Y1_GPIO_Port, OUTPUT_Y1_Pin);
		HAL_GPIO_WritePin(DEBUG_LED_GPIO_Port, DEBUG_LED_Pin, 0);
		HAL_GPIO_WritePin(OUTPUT_Y0_GPIO_Port, OUTPUT_Y0_Pin, 0);
	}
}

void show_7seg(uint8_t num){
	if (num < 10) {
		led7_SetDigit(num, 3, 0);
	} else {
		uint8_t first = num / 10;
		uint8_t second = num % 10;
		led7_SetDigit(first, 2, 0);
		led7_SetDigit(second, 3, 0);
	}
}

void test_lcd(){
	lcd_Fill(0, 0, 240, 20, BLUE);
	lcd_StrCenter(0, 2, "Traffic lights !!!", RED, BLUE, 16, 1);
//	lcd_ShowStr(20, 30, "Test lcd screen", WHITE, RED, 24, 0);
//	lcd_DrawCircle(60, 120, GREEN, 40, 1);
//	lcd_DrawCircle(160, 120, BRED, 40, 0);
//	lcd_ShowPicture(80, 200, 90, 90, gImage_logo);

	lcd_DrawLine(80, 50, 80, 150, BLUE);
	lcd_DrawLine(160, 50, 160, 150, BLUE);
	lcd_DrawLine(80, 230, 80, 330, BLUE);
	lcd_DrawLine(160, 230, 160, 330, BLUE);
	lcd_DrawLine(0, 150, 80, 150, BLUE);
	lcd_DrawLine(160, 150, 240, 150, BLUE);
	lcd_DrawLine(0, 230, 80, 230, BLUE);
	lcd_DrawLine(160, 230, 240, 230, BLUE);

	lcd_DrawCircle(95, 140, GRAY, 10, 1);
	lcd_DrawCircle(120, 140, GRAY, 10, 1);
	lcd_DrawCircle(145, 140, GRAY, 10, 1);
	lcd_DrawCircle(170, 165, GRAY, 10, 1);
	lcd_DrawCircle(170, 190, GRAY, 10, 1);
	lcd_DrawCircle(170, 215, GRAY, 10, 1);

	lcd_ShowStr(10, 30, "MODE:", RED, WHITE, 16, 1);
	lcd_ShowIntNum(50, 30, 1, 1, RED, WHITE, 16);
	lcd_DrawCircle(30, 80, BRED, 20, 0);
}

void trafic_lights(){
	switch (currentMode) {
		case modeInit:
			currentMode = modeRunning;
			break;
		case modeRunning:
			HAL_GPIO_WritePin(DEBUG_LED_GPIO_Port, DEBUG_LED_Pin, 0);
			HAL_GPIO_WritePin(OUTPUT_Y0_GPIO_Port, OUTPUT_Y0_Pin, 0);
			HAL_GPIO_WritePin(OUTPUT_Y1_GPIO_Port, OUTPUT_Y1_Pin, 0);
			lcd_ShowIntNum(50, 30, 1, 1, RED, WHITE, 16);
			lcd_ShowIntNum(26, 72, 0, 2, RED, WHITE, 16);
			count_second = (count_second + 1) % 20;
			count1 = (count1 + 1) % (20 * (count_red + count_green + count_yellow));
			count2 = (count2 + 1) % (20 * (count_red + count_green + count_yellow));
			if (count1 <= (20 * count_red) && count2 <= (20 * (count_green + count_yellow))){
				if (temp[0] == 0) {
					temp[0] = count_red;
				} else {
					if (count_second == 0) {
						temp[0]--;
					}
				}
				show_7seg(temp[0]);
				lcd_DrawCircle(120, 140, GRAY, 10, 1);
				lcd_DrawCircle(170, 215, GRAY, 10, 1);
				lcd_DrawCircle(145, 140, RED, 10, 1);
				if (count2 <= (20 * count_green)) {
					lcd_DrawCircle(170, 165, GREEN, 10, 1);
				} else {
					lcd_DrawCircle(170, 165, GRAY, 10, 1);
					lcd_DrawCircle(170, 190, YELLOW, 10, 1);
				}
			} else if (count1 > (20 * count_red) && count1 <= (20 * (count_red + count_green)) && count2 <= (20 * (count_red + count_green + count_yellow))) {
				if (temp[1] == 0) {
					temp[1] = count_green;
				} else {
					if (count_second == 0) {
						temp[1]--;
					}
				}
				show_7seg(temp[1]);
				lcd_DrawCircle(145, 140, GRAY, 10, 1);
				lcd_DrawCircle(95, 140, GREEN, 10, 1);
				lcd_DrawCircle(170, 190, GRAY, 10, 1);
				lcd_DrawCircle(170, 215, RED, 10, 1);
			} else if (count1 > (20 * (count_red + count_green)) && count1 <= (20 * (count_red + count_green + count_yellow)) && count2 <= (20 * (count_red + count_green + count_yellow))) {
				if (temp[2] == 0) {
					temp[2] = count_yellow;
				} else {
					if (count_second == 0) {
						temp[2]--;
					}
				}
				show_7seg(temp[2]);
				lcd_DrawCircle(95, 140, GRAY, 10, 1);
				lcd_DrawCircle(120, 140, YELLOW, 10, 1);
			}

			if (button_count[0] == 1) {
				currentMode = modeRED;
			}
			break;

		case modeRED:
			temp[0] = 0;
			temp[1] = 0;
			temp[2] = 0;
			test_LedDebug();

			lcd_DrawCircle(95, 140, GRAY, 10, 1);
			lcd_DrawCircle(120, 140, GRAY, 10, 1);
			lcd_DrawCircle(145, 140, RED, 10, 1);
			lcd_DrawCircle(170, 165, GRAY, 10, 1);
			lcd_DrawCircle(170, 190, GRAY, 10, 1);
			lcd_DrawCircle(170, 215, RED, 10, 1);

			lcd_ShowIntNum(50, 30, 2, 1, RED, WHITE, 16);

			temp_count[1] = 0;
			temp_count[2] = 0;
			if (temp_count[0] == 0) {
				temp_count[0] = count_red;
			}
			lcd_ShowIntNum(26, 72, temp_count[0], 2, RED, WHITE, 16);
			show_7seg(temp_count[0]);

			if (button_count[0] == 1) {
				currentMode = modeYELLOW;
			}

			if (button_count[1] == 1) {
				temp_count[0] = temp_count[0] + 1;
				if (temp_count[0] > 99) {
					temp_count[0] = 1;
				}
			}

			if (button_count[2] == 1) {
				count_red = temp_count[0];
			}
			break;

		case modeYELLOW:
			temp[0] = 0;
			temp[1] = 0;
			temp[2] = 0;
			test_LedY0();

			lcd_DrawCircle(95, 140, GRAY, 10, 1);
			lcd_DrawCircle(120, 140, YELLOW, 10, 1);
			lcd_DrawCircle(145, 140, GRAY, 10, 1);
			lcd_DrawCircle(170, 165, GRAY, 10, 1);
			lcd_DrawCircle(170, 190, YELLOW, 10, 1);
			lcd_DrawCircle(170, 215, GRAY, 10, 1);

			lcd_ShowIntNum(50, 30, 3, 1, RED, WHITE, 16);

			temp_count[0] = 0;
			temp_count[2] = 0;
			if (temp_count[1] == 0) {
				temp_count[1] = count_yellow;
			}
			lcd_ShowIntNum(26, 72, temp_count[1], 2, RED, WHITE, 16);
			show_7seg(temp_count[1]);

			if (button_count[0] == 1) {
				currentMode = modeGREEN;
			}
			if (button_count[1] == 1) {
				temp_count[1] = temp_count[1] + 1;
				if (temp_count[1] > 99) {
					temp_count[1] = 1;
				}
			}

			if (button_count[2] == 1) {
				count_yellow = temp_count[1];
			}
			break;

		case modeGREEN:
			temp[0] = 0;
			temp[1] = 0;
			temp[2] = 0;
			test_LedY1();

			lcd_DrawCircle(95, 140, GREEN, 10, 1);
			lcd_DrawCircle(120, 140, GRAY, 10, 1);
			lcd_DrawCircle(145, 140, GRAY, 10, 1);
			lcd_DrawCircle(170, 165, GREEN, 10, 1);
			lcd_DrawCircle(170, 190, GRAY, 10, 1);
			lcd_DrawCircle(170, 215, GRAY, 10, 1);

			lcd_ShowIntNum(50, 30, 4, 1, RED, WHITE, 16);

			temp_count[0] = 0;
			temp_count[1] = 0;
			if (temp_count[2] == 0) {
				temp_count[2] = count_green;
			}
			lcd_ShowIntNum(26, 72, temp_count[2], 2, RED, WHITE, 16);
			show_7seg(temp_count[2]);

			if (button_count[0] == 1) {
				lcd_DrawCircle(95, 140, GRAY, 10, 1);
				lcd_DrawCircle(170, 165, GRAY, 10, 1);
				currentMode = modeRunning;
			}
			if (button_count[1] == 1) {
				temp_count[2] = temp_count[2] + 1;
				if (temp_count[2] > 99) {
					temp_count[2] = 1;
				}
			}

			if (button_count[2] == 1) {
				count_green = temp_count[2];
			}
			break;
	}

}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
