/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2023 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "string.h"
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
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
uint8_t RxBuffer[1];
uint8_t TxBuffer[40];

uint8_t FirstMenu[500] =
				"===================================================== \r\n"
				"           Welcome to First selection Menu            \r\n"
				"   Please selected only choise which the menu have    \r\n"
				"             Press the number of choise               \r\n"
				"===================================================== \r\n"
				"                 0 : LED Control                      \r\n"
				"                1 : Button Status                     \r\n"
				"===================================================== \r\n";

uint8_t SecondMenu[700] =
				"===================================================== \r\n"
				"               Welcome to Led Control                 \r\n"
				"   Please selected only choise which the menu have    \r\n"
				"             Press the Character of choise            \r\n"
				"===================================================== \r\n"
				"                a : Speed Up +1 Hz                    \r\n"
				"                s : Speed Down -1 Hz                  \r\n"
				"                d : On / Off LED                      \r\n"
		        "                x : Prevoius menus                    \r\n"
				"===================================================== \r\n";

uint8_t ThirdMenu[300] =
				"===================================================== \r\n"
				"              Welcome to Button Status                \r\n"
				"       This Menu will show button status B1           \r\n"
				"         Press 'x' to get to prevoius menu            \r\n"
				"===================================================== \r\n";

int8_t Hz = 0;
int8_t PreHz = 0;
uint16_t Millis = 0;

uint8_t TextState = 0;

uint8_t LedStatus = 0;
uint8_t LedOn[10] = "LedOn\r\n";
uint8_t LedOff[15] = "LedOff\r\n";

uint8_t wrong[20] = "Worng Option\r\n";

uint8_t Button = 0;
uint8_t PreButton = 0;
uint8_t Press[20] = "ButtonPressed\r\n";
uint8_t unPress[20] = "ButtonUnpressed\r\n";

uint8_t OneTimePress = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */

void UARTInterruptConfig();
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {
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
	MX_USART2_UART_Init();
	/* USER CODE BEGIN 2 */

	UARTInterruptConfig();
	RxBuffer[0] = ' ';
	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1) {
		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
		static uint32_t timestamp = 0;
		if (HAL_GetTick() > timestamp) {

			if (Hz > 0) {
				Millis = 500 / Hz;
			} else {
				Millis = 0;
			}
			timestamp = HAL_GetTick() + Millis;

			if(LedStatus == 0 || Hz == 0){
				HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, RESET);
			}
			else{
				HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
			}
		}

		switch (TextState) {

		case 0:
			HAL_UART_Transmit_IT(&huart2, FirstMenu, strlen((char*) FirstMenu));
			TextState = 1;
			break;
		case 1:
			switch (RxBuffer[0]){

			case ' ':
				break;
			case '0' :
					RxBuffer[0] = ' ';
					TextState = 2;
				break;
			case '1' :
					RxBuffer[0] = ' ';
					TextState = 4;
				break;
			default:
					HAL_UART_Transmit_IT(&huart2, wrong, strlen((char*) wrong));
					RxBuffer[0] = ' ';
					TextState = 1;
				break;
			}
			break;


		case 2:
			HAL_UART_Transmit_IT(&huart2, SecondMenu, strlen((char*) SecondMenu));
			TextState = 3;
			break;
		case 3:
			switch(RxBuffer[0]){

			case ' ':
				break;
			case 'a':
				Hz += 1;
				PreHz += 1;

				sprintf((char*) TxBuffer, "Now LED Blink %d Hz\r\n", Hz);
				HAL_UART_Transmit_IT(&huart2, TxBuffer, strlen((char*) TxBuffer));
				TextState = 3;
				RxBuffer[0] = ' ';
				break;
			case 's' :
				Hz -= 1;
				if(Hz <= 0){
					Hz = 0;
				}
				PreHz -= 1;
				if(PreHz <= 0){
					PreHz = 0;
				}

				sprintf((char*) TxBuffer, "Now LED Blink %d Hz\r\n", Hz);
				HAL_UART_Transmit_IT(&huart2, TxBuffer, strlen((char*) TxBuffer));
				TextState = 3;
				RxBuffer[0] = ' ';
				break;
			case 'd':
				if(LedStatus){
					LedStatus = 0;
					RxBuffer[0] = ' ';

					Hz = 0;
					HAL_UART_Transmit_IT(&huart2, LedOff, strlen((char*) LedOff));
					HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, SET);
				}
				else{
					LedStatus = 1;
					RxBuffer[0] =' ';

					Hz = PreHz;
					HAL_UART_Transmit_IT(&huart2, LedOn, strlen((char*) LedOn));
				}
				break;
			case 'x':
				RxBuffer[0] = ' ';
				TextState = 0;
				break;
			default :
				RxBuffer[0] = ' ';
				HAL_UART_Transmit_IT(&huart2, wrong, strlen((char*) wrong));
				TextState = 3;
				break;
			}
			break;


		case 4:
			HAL_UART_Transmit_IT(&huart2, ThirdMenu, strlen((char*) ThirdMenu));
			TextState = 5;
			break;
		case 5:
			Button = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13);
			if(Button == 1 && PreButton == 0){
				HAL_UART_Transmit_IT(&huart2, unPress, strlen((char*) unPress));
				OneTimePress = 0;
			}
			else if(Button == 0 && PreButton == 0){
				if(!OneTimePress){
					HAL_UART_Transmit_IT(&huart2, Press, strlen((char*) Press));
					OneTimePress = 1;
				}
			}
			switch(RxBuffer[0]){

			case ' ':
				break;
			case 'x':
				RxBuffer[0] = ' ';
				TextState = 0;
				break;
			default:
				HAL_UART_Transmit_IT(&huart2, wrong, strlen((char*) wrong));
				RxBuffer[0] = ' ';
				TextState = 5;
				break;
			}
			PreButton = Button;
			break;
		}

	}
	/* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void) {
	RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };

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
	RCC_OscInitStruct.PLL.PLLM = 16;
	RCC_OscInitStruct.PLL.PLLN = 336;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
	RCC_OscInitStruct.PLL.PLLQ = 4;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
			| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) {
		Error_Handler();
	}
}

/**
 * @brief USART2 Initialization Function
 * @param None
 * @retval None
 */
static void MX_USART2_UART_Init(void) {

	/* USER CODE BEGIN USART2_Init 0 */

	/* USER CODE END USART2_Init 0 */

	/* USER CODE BEGIN USART2_Init 1 */

	/* USER CODE END USART2_Init 1 */
	huart2.Instance = USART2;
	huart2.Init.BaudRate = 57600;
	huart2.Init.WordLength = UART_WORDLENGTH_8B;
	huart2.Init.StopBits = UART_STOPBITS_1;
	huart2.Init.Parity = UART_PARITY_NONE;
	huart2.Init.Mode = UART_MODE_TX_RX;
	huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart2.Init.OverSampling = UART_OVERSAMPLING_16;
	if (HAL_UART_Init(&huart2) != HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN USART2_Init 2 */

	/* USER CODE END USART2_Init 2 */

}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void) {
	GPIO_InitTypeDef GPIO_InitStruct = { 0 };

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOH_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pin : B1_Pin */
	GPIO_InitStruct.Pin = B1_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pin : LD2_Pin */
	GPIO_InitStruct.Pin = LD2_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
void UARTInterruptConfig() {
	HAL_UART_Receive_IT(&huart2, RxBuffer, 1);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	if (huart == &huart2) {
		RxBuffer[2] = '\0';

		sprintf((char*) TxBuffer, "Enter : %s\r\n", RxBuffer);
		HAL_UART_Transmit(&huart2, TxBuffer, strlen((char*) TxBuffer), 100);

		HAL_UART_Receive_IT(&huart2, RxBuffer, 1);
	}
}
/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1) {
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
