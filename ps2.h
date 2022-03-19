#pragma once

#include <stdio.h>

#include "pico/stdlib.h"

#define PS2_PIN_DATA 11
#define PS2_PIN_COMMAND 10
#define PS2_PIN_ATTENTION 5
#define PS2_PIN_CLOCK 4

#define PS2_LEFT_BIT 0x01
#define PS2_DOWN_BIT 0x02
#define PS2_RIGHT_BIT 0x04
#define PS2_UP_BIT 0x08
#define PS2_START_BIT 0x10
#define PS2_R3_BIT 0x20
#define PS2_L3_BIT 0x40
#define PS2_SELECT_BIT 0x80
#define PS2_SQUARE_BIT 0x01
#define PS2_CROSS_BIT 0x02
#define PS2_CIRCLE_BIT 0x04
#define PS2_TRIANGLE_BIT 0x08
#define PS2_R1_BIT 0x10
#define PS2_L1_BIT 0x20
#define PS2_R2_BIT 0x40
#define PS2_L2_BIT 0x80
#define PS2_DENGO_A_BIT PS2_SQUARE_BIT
#define PS2_DENGO_B_BIT PS2_CROSS_BIT
#define PS2_DENGO_C_BIT PS2_CIRCLE_BIT

void ps2_init(void);
void ps2_task(void);
uint8_t ps2_read(uint8_t cmd);
int ps2_is_button_pressed(int button_type, uint8_t button);
int ps2_is_LEFT_pressed(void);
int ps2_is_DOWN_pressed(void);
int ps2_is_RIGHT_pressed(void);
int ps2_is_UP_pressed(void);
int ps2_is_START_pressed(void);
int ps2_is_R3_pressed(void);
int ps2_is_L3_pressed(void);
int ps2_is_SELECT_pressed(void);
int ps2_is_SQUARE_pressed(void);
int ps2_is_CROSS_pressed(void);
int ps2_is_CIRCLE_pressed(void);
int ps2_is_TRIANGLE_pressed(void);
int ps2_is_R1_pressed(void);
int ps2_is_L1_pressed(void);
int ps2_is_R2_pressed(void);
int ps2_is_L2_pressed(void);
int ps2_is_dengo_A_pressed(void);
int ps2_is_dengo_B_pressed(void);
int ps2_is_dengo_C_pressed(void);
int ps2_dengo_get_power(void);
int ps2_dengo_get_break(void);
