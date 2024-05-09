/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
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
#include "i2c-lcd.h"
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
I2C_HandleTypeDef hi2c1;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
#define ROW_COUNT 4
#define COL_COUNT 4



char password[] = "1234"; // Doğru şifre
int entered_password[4] = {}; // 4 karakterlik bir dizi
int entered_new_password[4] = {}; // 4 karakterlik bir dizi

char key = '\0';
char last_key = '\0';

uint16_t main_next = 0;
uint8_t next = 0;
uint8_t change_password_next = 0;

uint32_t milis = 0;

uint32_t timeout = 0;
uint32_t main_timeout = 0;
uint32_t change_password_timeout = 0;

int j = 0; // j değişkenini static olarak tanımlayın
int k = 0;

uint16_t row_pins[ROW_COUNT] = {R1_Pin, R2_Pin, R3_Pin, R4_Pin};
uint16_t col_pins[COL_COUNT] = {C1_Pin, C2_Pin, C3_Pin, C4_Pin};


char keypad_scan(void) {

    char key_map[ROW_COUNT][COL_COUNT] = { //Keypad Karakterler
        {'1', '2', '3', 'A'},
        {'4', '5', '6', 'B'},
        {'7', '8', '9', 'C'},
        {'*', '0', '#', 'D'}
    };

    static int key_state[ROW_COUNT][COL_COUNT] = { 0 }; // Tuş durumlarını sakla
	char pressed_key = '\0'; // Basılan tuşun değeri

	for (int row = 0; row < ROW_COUNT; row++) {
		HAL_GPIO_WritePin(GPIOA, row_pins[row], GPIO_PIN_RESET); // Row pinini düşük seviyeye ayarla

		for (int col = 0; col < COL_COUNT; col++) {
			if (HAL_GPIO_ReadPin(GPIOB, col_pins[col]) == GPIO_PIN_RESET) {
				if (key_state[row][col] == 0) { // Tuş basılmış mı kontrol et
					key_state[row][col] = 1; // Tuşun basıldığını kaydet
					pressed_key = key_map[row][col]; // Basılan tuşun değerini al
				}
			} else {
				key_state[row][col] = 0; // Tuş bırakıldığında durumu sıfırla
			}
		}

		HAL_GPIO_WritePin(GPIOA, row_pins[row], GPIO_PIN_SET); // Row pinini tekrar yüksek seviyeye ayarla
	}

	return pressed_key; // Basılan tuşun değerini döndür
}

void main_menu() {

	if (main_next == 0) { // Ana menuye giris
		lcd_clear();
		lcd_put_cur(0, 0);
		lcd_send_string("Butona Basiniz ");
		last_key = 0;
		change_password_next = 0;
		main_next = 1;

	} else if (main_next == 1) {

		if (HAL_GPIO_ReadPin(BUTTON_GPIO_Port, BUTTON_Pin) == 1) {
			main_timeout = milis + 1250;

		}
		if (milis > main_timeout) {
			main_next = 2;
		}

	} else if (main_next == 2) {

		lcd_clear();
		lcd_put_cur(0, 0);
		lcd_send_string("Sifre Giriniz: ");
		main_next = 3;

	} else if (main_next == 3) { // sifre girme

		if (milis > main_timeout) {
			if (key != '\0') { // Herhangi bir tuşa basıldıysa
				lcd_put_cur(1, j);
				lcd_send_string("*");
				entered_password[j++] = key; // Basılan tuslari diziye ata
				main_timeout = milis + 250;

				if (j == 4) { // Sifre 4 karakter alır

					main_timeout = milis + 250;
					j = 0;
					main_next = 6;
				}
			}
		}

	} else if (main_next == 6) {

		if (milis > main_timeout) {
			int i = 0;
			for ( i = 0; i < 4; i++) {
				if (entered_password[i] != password[i]) { // Sifre karsilastirma
					break; // Karakterler eşit değilse döngüden çık
				}
			}
			if (i == 4) { // Döngü normal şekilde tamamlanmışsa şifreler eşittir
				lcd_clear();
				lcd_put_cur(0, 0);
				lcd_send_string("Kapi Aciliyor");
				main_timeout = milis + 1000;
				main_next = 101;
			} else {
				lcd_clear();
				lcd_put_cur(0, 0);
				lcd_send_string("Sifre Yanlis");
				main_timeout = milis + 1000;
				main_next = 102;
			}
		}

	} else if (main_next == 101 || main_next == 102) { // Sifre girme islemi bittiyse
		if (milis > main_timeout) {
			main_next = 0;
		}
	}
}

void change_password() { // Sifre degistirme menusu

	if (change_password_next == 0) {
		if (key == '*') { // Yıldız tuşuna basılma durumu

			change_password_next = 1;
			change_password_timeout = milis + 500;
		}

	} else if (change_password_next == 1) {

		main_next = 1000;
		change_password_next = 2;

	} else if (change_password_next == 2) {

		if (milis > change_password_timeout) {
			if (key == '*') { // 2. defa yıldız tusuna basıldıysa
				change_password_next = 3;
			}
		}

	} else if (change_password_next == 3) {
		lcd_clear();
		lcd_put_cur(0, 0);
		lcd_send_string("Yeni Sifre:");
		change_password_next = 4;

	} else if (change_password_next == 4) {
		if (key != '\0') { // Herhangi bir tusa basildiysa
			lcd_put_cur(1, k);
			lcd_send_string("*");
			entered_new_password[k++] = key;

			change_password_timeout = milis + 300;

			if (k == 4) {

				change_password_timeout = milis + 300;
				k = 0;
				change_password_next = 5;
			}
		}
		if(key == 'D'){
			change_password_next = 0;
			main_next = 0;
			k = 0;
		}

	} else if (change_password_next == 5) {
		if (milis > change_password_timeout) {
			for (int i = 0; i < 4; i++) {
				password[i] = entered_new_password[i]; // yeni sifreyi atama
			}
			if (HAL_GPIO_ReadPin(BUTTON_GPIO_Port, BUTTON_Pin) == 0) {

				lcd_clear();
				lcd_put_cur(0, 0);
				lcd_send_string("Sifre Degisti");

				change_password_timeout = milis + 1000;
				change_password_next = 6;
			}
		}
	}else if(change_password_next == 6){
		if(milis > change_password_timeout){
			change_password_next = 100;
			main_next = 0;
		}
	}
}

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
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */

	lcd_init();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1) {
		milis = HAL_GetTick();
		key = keypad_scan();
		main_menu();
		change_password();


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
  RCC_OscInitStruct.PLL.PLLN = 180;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Activate the Over-Drive mode
  */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
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
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, R1_Pin|R2_Pin|R4_Pin|R3_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : BUTTON_Pin */
  GPIO_InitStruct.Pin = BUTTON_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(BUTTON_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : R1_Pin R2_Pin R4_Pin */
  GPIO_InitStruct.Pin = R1_Pin|R2_Pin|R4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : R3_Pin */
  GPIO_InitStruct.Pin = R3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(R3_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : C4_Pin C2_Pin C1_Pin C3_Pin */
  GPIO_InitStruct.Pin = C4_Pin|C2_Pin|C1_Pin|C3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

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
