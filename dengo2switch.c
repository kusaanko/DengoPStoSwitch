/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Ha Thach (tinyusb.org), 2022 Kusaanko
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#include <stdio.h>
#include <string.h>

#include "pico/stdlib.h"

#include "bsp/board.h"
#include "tusb.h"

#include "usb_descriptors.h"

#include "ps2.h"

typedef struct TU_ATTR_PACKED
{
    uint16_t buttons; ///< Buttons mask for currently pressed buttons
    uint8_t hat;      ///< Buttons mask for currently pressed buttons in the DPad/hat
    uint8_t x;        ///< Delta x  movement of left analog-stick
    uint8_t y;        ///< Delta y  movement of left analog-stick
    uint8_t rx;       ///< Delta Rx movement of analog left trigger
    uint8_t ry;       ///< Delta Ry movement of analog right trigger
    uint8_t vendor_spec;
} hori_gamepad_report_t;

static hori_gamepad_report_t hori_gamepad_report = {.x = 128, .y = 128, .rx = 128, .ry = 128, .hat = 0, .buttons = 0};
static int pre_power = -1;
static int pre_break = -1;
static uint32_t pre_buttons = 0;

#define HORIPAD_Y 0x0001
#define HORIPAD_B 0x0002
#define HORIPAD_A 0x0004
#define HORIPAD_X 0x0008
#define HORIPAD_L 0x0010
#define HORIPAD_R 0x0020
#define HORIPAD_ZL 0x0040
#define HORIPAD_ZR 0x0080
#define HORIPAD_MINUS 0x0100
#define HORIPAD_PLUS 0x0200
#define HORIPAD_LCLICK 0x0400
#define HORIPAD_RCLICK 0x0800
#define HORIPAD_HOME 0x1000
#define HORIPAD_CAPTURE 0x2000
#define HORIPAD_UP 0x00
#define HORIPAD_UP_RIGHT 0x01
#define HORIPAD_RIGHT 0x02
#define HORIPAD_DOWN_RIGHT 0x03
#define HORIPAD_DOWN 0x04
#define HORIPAD_DOWN_LEFT 0x05
#define HORIPAD_LEFT 0x06
#define HORIPAD_UP_LEFT 0x07
#define HORIPAD_CENTER 0x08

static uint8_t dengo_break_notch[] = {128, 96, 82, 68, 52, 38, 24, 18, 10, 0};
static uint32_t dengo_select_press_time = 0;
static uint32_t dengo_plus_press_time = 0;

//--------------------------------------------------------------------+
// MACRO CONSTANT TYPEDEF PROTYPES
//--------------------------------------------------------------------+

/* Blink pattern
 * - 250 ms  : device not mounted
 * - 1000 ms : device mounted
 * - 2500 ms : device is suspended
 */
enum
{
    BLINK_NOT_MOUNTED = 250,
    BLINK_MOUNTED = 1000,
    BLINK_SUSPENDED = 2500,
};

static uint32_t blink_interval_ms = BLINK_NOT_MOUNTED;

void led_blinking_task(void);
void hid_update(void);

/*------------- MAIN -------------*/
int main(void)
{
    board_init();
    tusb_init();
    ps2_init();

    while (1)
    {
        tud_task(); // tinyusb device task
        led_blinking_task();

        if (tud_hid_ready())
        {
            ps2_task();

            uint32_t buttons = 0;
            if (ps2_is_dengo_A_pressed())
            {
                buttons |= 0x01;
            }
            if (ps2_is_dengo_B_pressed())
            {
                buttons |= 0x02;
            }
            if (ps2_is_dengo_C_pressed())
            {
                buttons |= 0x04;
            }
            if (ps2_is_START_pressed())
            {
                buttons |= 0x08;
            }
            if (ps2_is_SELECT_pressed())
            {
                if (dengo_select_press_time == 0)
                {
                    dengo_select_press_time = board_millis();
                }
                buttons |= 0x10;
            }
            else
            {
                if (dengo_select_press_time > 0 && board_millis() - dengo_select_press_time <= 100)
                {
                    dengo_plus_press_time = board_millis();
                }
                dengo_select_press_time = 0;
            }
            if (board_button_read())
            {
                buttons |= 0x20;
            }
            if (dengo_plus_press_time > 0)
            {
                buttons |= 0x40;
            }
            if (board_millis() - dengo_plus_press_time > 50)
            {
                dengo_plus_press_time = 0;
            }

            int power = ps2_dengo_get_power();
            int break_ = ps2_dengo_get_break();
            if (power == -1)
                power = pre_power;
            if (break_ == -1)
                break_ = pre_break;

            if (pre_buttons != buttons || pre_break != break_ || pre_power != power)
            {
                hori_gamepad_report.buttons = 0;
                hori_gamepad_report.hat = HORIPAD_CENTER;
                if (ps2_is_SELECT_pressed() && board_millis() - dengo_select_press_time > 100)
                {
                    if (ps2_is_dengo_A_pressed())
                        hori_gamepad_report.hat = HORIPAD_LEFT;
                    if (ps2_is_dengo_B_pressed())
                        hori_gamepad_report.hat = HORIPAD_DOWN;
                    if (ps2_is_dengo_C_pressed())
                        hori_gamepad_report.hat = HORIPAD_RIGHT;
                    if (ps2_is_START_pressed())
                        hori_gamepad_report.hat = HORIPAD_UP;
                }
                else
                {
                    if (ps2_is_dengo_A_pressed())
                        hori_gamepad_report.buttons |= HORIPAD_Y;
                    if (ps2_is_dengo_B_pressed())
                        hori_gamepad_report.buttons |= HORIPAD_B;
                    if (ps2_is_dengo_C_pressed())
                        hori_gamepad_report.buttons |= HORIPAD_A;
                    if (ps2_is_START_pressed())
                        hori_gamepad_report.buttons |= HORIPAD_X;
                }
                if (board_button_read())
                {
                    hori_gamepad_report.buttons |= HORIPAD_HOME;
                }
                if (dengo_plus_press_time > 0)
                {
                    hori_gamepad_report.buttons |= HORIPAD_PLUS;
                }
                if (break_ >= 1)
                {
                    hori_gamepad_report.y = dengo_break_notch[break_];
                }
                else
                {
                    hori_gamepad_report.y = 138 + 117 * power / 5;
                }
                if (break_ == 0 && power == 0)
                {
                    hori_gamepad_report.y = 128;
                }
                hid_update();
            }

            pre_buttons = buttons;
            pre_power = power;
            pre_break = break_;
        }
    }

    return 0;
}

void hid_update(void)
{
    tud_hid_report(0, &hori_gamepad_report, sizeof(hori_gamepad_report));
}

//--------------------------------------------------------------------+
// Device callbacks
//--------------------------------------------------------------------+

// Invoked when device is mounted
void tud_mount_cb(void)
{
    blink_interval_ms = BLINK_MOUNTED;
}

// Invoked when device is unmounted
void tud_umount_cb(void)
{
    blink_interval_ms = BLINK_NOT_MOUNTED;
}

// Invoked when usb bus is suspended
// remote_wakeup_en : if host allow us  to perform remote wakeup
// Within 7ms, device must draw an average of current less than 2.5 mA from bus
void tud_suspend_cb(bool remote_wakeup_en)
{
    (void)remote_wakeup_en;
    blink_interval_ms = BLINK_SUSPENDED;
}

// Invoked when usb bus is resumed
void tud_resume_cb(void)
{
    blink_interval_ms = BLINK_MOUNTED;
}

// Invoked when sent REPORT successfully to host
// Application can use this to send the next report
// Note: For composite reports, report[0] is report ID
void tud_hid_report_complete_cb(uint8_t instance, uint8_t const *report, uint8_t len)
{
    (void)instance;
    (void)len;
}

// Invoked when received GET_REPORT control request
// Application must fill buffer report's content and return its length.
// Return zero will cause the stack to STALL request
uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t *buffer, uint16_t reqlen)
{
    // TODO not Implemented
    (void)instance;
    (void)report_id;
    (void)report_type;
    (void)buffer;
    (void)reqlen;

    return 0;
}

// Invoked when received SET_REPORT control request or
// received data on OUT endpoint ( Report ID = 0, Type = 0 )
void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t const *buffer, uint16_t bufsize)
{
    (void)instance;

    if (report_type == HID_REPORT_TYPE_OUTPUT)
    {
        // Set keyboard LED e.g Capslock, Numlock etc...
        if (report_id == REPORT_ID_KEYBOARD)
        {
            // bufsize should be (at least) 1
            if (bufsize < 1)
                return;

            uint8_t const kbd_leds = buffer[0];

            if (kbd_leds & KEYBOARD_LED_CAPSLOCK)
            {
                // Capslock On: disable blink, turn led on
                blink_interval_ms = 0;
                board_led_write(true);
            }
            else
            {
                // Caplocks Off: back to normal blink
                board_led_write(false);
                blink_interval_ms = BLINK_MOUNTED;
            }
        }
    }
}

//--------------------------------------------------------------------+
// BLINKING TASK
//--------------------------------------------------------------------+
void led_blinking_task(void)
{
    static uint32_t start_ms = 0;
    static bool led_state = false;

    // blink is disabled
    if (!blink_interval_ms)
        return;

    // Blink every interval ms
    if (board_millis() - start_ms < blink_interval_ms)
        return; // not enough time
    start_ms += blink_interval_ms;

    board_led_write(led_state);
    led_state = 1 - led_state; // toggle
}