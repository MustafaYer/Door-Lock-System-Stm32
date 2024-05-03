/** Put this in the src folder **/

#include "i2c-lcd.h"
extern I2C_HandleTypeDef hi2c1;  // change your handler here accordingly

#define SLAVE_ADDRESS_LCD 0x4E // change this according to ur setup

void lcd_send_cmd(char cmd) {
	char data_u, data_l;
	uint8_t data_t[4];
	data_u = (cmd & 0xf0);
	data_l = ((cmd << 4) & 0xf0);
	data_t[0] = data_u | 0x0C;  //en=1, rs=0
	data_t[1] = data_u | 0x08;  //en=0, rs=0
	data_t[2] = data_l | 0x0C;  //en=1, rs=0
	data_t[3] = data_l | 0x08;  //en=0, rs=0
	HAL_I2C_Master_Transmit(&hi2c1, SLAVE_ADDRESS_LCD, (uint8_t*) data_t, 4, 100);
//	HAL_I2C_Master_Transmit_IT(&hi2c1, SLAVE_ADDRESS_LCD, (uint8_t*) data_t, 4);
}

void lcd_send_data(char data) {
	char data_u, data_l;
	uint8_t data_t[4];
	data_u = (data & 0xf0);
	data_l = ((data << 4) & 0xf0);
	data_t[0] = data_u | 0x0D;  //en=1, rs=0
	data_t[1] = data_u | 0x09;  //en=0, rs=0
	data_t[2] = data_l | 0x0D;  //en=1, rs=0
	data_t[3] = data_l | 0x09;  //en=0, rs=0
	HAL_I2C_Master_Transmit(&hi2c1, SLAVE_ADDRESS_LCD, (uint8_t*) data_t, 4, 100);
//	HAL_I2C_Master_Transmit_IT(&hi2c1, SLAVE_ADDRESS_LCD, (uint8_t*) data_t, 4);
}

void lcd_clear(void) {
	lcd_send_cmd(0x80);
	for (int i = 0; i < 70; i++) {
		lcd_send_data(' ');
	}
}

void lcd_put_cur(int row, int col) {
	switch (row) {
	case 0:
		col |= 0x80;
		break;
	case 1:
		col |= 0xC0;
		break;
	}

	lcd_send_cmd(col);
}

void lcd_init(void) {
	// 4 bit initialisation

//	if(next == 0){
//		timeout = milis + 50;
//		next = 1;
//	}else if(next == 1){
//		if(milis > timeout){
//			next = 2;
//		}
//	}else if(next == 2){
//		lcd_send_cmd(0x30);
//		next = 3;
//	}else if(next == 3){
//		timeout = milis + 5;
//		next = 4;
//	}else if(next == 4){
//		if(milis > timeout){
//			next = 5;
//		}
//	}else if(next == 5){
//		lcd_send_cmd(0x30);
//		next = 6;
//	}else if(next == 6){
//		timeout = milis + 1;
//		next = 7;
//	}else if(next == 7){
//		if(milis > timeout){
//			next = 8;
//		}
//	}else if(next == 8){
//		lcd_send_cmd(0x30);
//		next = 9;
//	}else if(next == 9){
//		timeout = milis + 10;
//		next = 10;
//	}else if(next == 10){
//		if(milis > timeout){
//			next = 11;
//		}
//	}else if(next == 11){
//		lcd_send_cmd(0x20);
//		next = 13;
//	}else if(next == 13){
//		timeout = milis + 10;
//		next = 14;
//	}else if(next == 14){
//		if(milis > timeout){
//			next = 15;
//		}
//	}else if(next == 15){
//		lcd_send_cmd(0x28);
//		next = 16;
//	}else if(next == 16){
//		timeout = milis + 1;
//		next = 17;
//	}else if(next == 17){
//		if(milis > timeout){
//			next = 18;
//		}
//	}else if(next == 18){
//		lcd_send_cmd(0x08);
//		next = 19;
//	}else if(next == 19){
//		timeout = milis + 1;
//		next = 20;
//	}else if(next == 20){
//		if(milis > timeout){
//			next = 21;
//		}
//	}else if(next == 21){
//		lcd_send_cmd(0x01);
//		next = 22;
//	}else if(next == 22){
//		timeout = milis + 2;
//		next = 23;
//	}else if(next == 23){
//		if(milis > timeout){
//			next = 24;
//		}
//	}else if(next == 24){
//		lcd_send_cmd(0x06);
//		next = 25;
//	}else if(next == 25){
//		timeout = milis + 1;
//		next = 26;
//	}else if(next == 26){
//		if(milis > timeout){
//			next = 27;
//		}
//	}else if(next == 27){
//		lcd_send_cmd(0x0C);
//		next = 1000;
//	}
	HAL_Delay(60);  // wait for >40ms
	lcd_send_cmd(0x30);
	HAL_Delay(5);  // wait for >4.1ms
	lcd_send_cmd(0x30);

	HAL_Delay(1);  // wait for >100us
	lcd_send_cmd(0x30);
	HAL_Delay(10);
	lcd_send_cmd(0x20);  // 4bit mode

	HAL_Delay(50);

	// dislay initialisation
	lcd_send_cmd(0x28); // Function set --> DL=0 (4 bit mode), N = 1 (2 line display) F = 0 (5x8 characters)
	HAL_Delay(1);
	lcd_send_cmd(0x08); //Display on/off control --> D=0,C=0, B=0  ---> display off
	HAL_Delay(1);
	lcd_send_cmd(0x01);  // clear display
	HAL_Delay(1);
	HAL_Delay(1);
	lcd_send_cmd(0x06); //Entry mode set --> I/D = 1 (increment cursor) & S = 0 (no shift)
	HAL_Delay(1);
	lcd_send_cmd(0x0C); //Display on/off control --> D = 1, C and B = 0. (Cursor and blink, last two bits)
}

void lcd_send_string(char *str) {
	while (*str)
		lcd_send_data(*str++);
}



//void lcd_send_int(int number) {
//	char buffer[50];
//	sprintf(buffer, "%d", number);
//
//	lcd_send_string(buffer);
//}
