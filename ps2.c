
#include "ps2.h"

static uint8_t ps2_buttons1 = 255;
static uint8_t ps2_buttons2 = 255;

void ps2_init(void)
{
    gpio_init(PS2_PIN_DATA);
    gpio_init(PS2_PIN_COMMAND);
    gpio_init(PS2_PIN_ATTENTION);
    gpio_init(PS2_PIN_CLOCK);
    gpio_set_dir(PS2_PIN_DATA, GPIO_IN);
    gpio_set_dir(PS2_PIN_COMMAND, GPIO_OUT);
    gpio_set_dir(PS2_PIN_ATTENTION, GPIO_OUT);
    gpio_set_dir(PS2_PIN_CLOCK, GPIO_OUT);
    gpio_pull_up(PS2_PIN_DATA);
    gpio_put(PS2_PIN_ATTENTION, 1);
    gpio_put(PS2_PIN_CLOCK, 1);
}

void ps2_task(void)
{
    gpio_put(PS2_PIN_ATTENTION, 0);
    sleep_us(18);
    uint8_t b1 = ps2_read(0x01);
    uint8_t b2 = ps2_read(0x42);
    uint8_t b3 = ps2_read(0x00);
    ps2_buttons1 = ps2_read(0x00);
    ps2_buttons2 = ps2_read(0x00);
    gpio_put(PS2_PIN_ATTENTION, 1);
    if(b1 != 255 && b3 != 90) {
        ps2_buttons1 = 255;
        ps2_buttons2 = 255;
    }
}

uint8_t ps2_read(uint8_t cmd)
{
    uint8_t ret = 0;
    for (int i = 0; i < 8; i++)
    {
        gpio_put(PS2_PIN_CLOCK, 0);
        if (cmd & (1 << i))
            gpio_put(PS2_PIN_COMMAND, 1);
        else
            gpio_put(PS2_PIN_COMMAND, 0);
        sleep_us(20);
        int v = gpio_get(PS2_PIN_DATA);
        if (v)
        {
            ret |= (0b10000000 >> i);
        }
        gpio_put(PS2_PIN_CLOCK, 1);
        sleep_us(20);
    }
    return ret;
}

int ps2_is_button_pressed(int button_type, uint8_t button)
{
    if (button_type == 1)
    {
        if (!(ps2_buttons1 & button))
            return 1;
    }else
    {
        if (!(ps2_buttons2 & button))
            return 1;
    }
    return 0;
}

int ps2_is_LEFT_pressed(void)
{
    return ps2_is_button_pressed(1, PS2_LEFT_BIT);
}

int ps2_is_DOWN_pressed(void)
{
    return ps2_is_button_pressed(1, PS2_DOWN_BIT);
}

int ps2_is_RIGHT_pressed(void)
{
    return ps2_is_button_pressed(1, PS2_RIGHT_BIT);
}

int ps2_is_UP_pressed(void)
{
    return ps2_is_button_pressed(1, PS2_UP_BIT);
}

int ps2_is_START_pressed(void)
{
    return ps2_is_button_pressed(1, PS2_START_BIT);
}

int ps2_is_R3_pressed(void)
{
    return ps2_is_button_pressed(1, PS2_R3_BIT);
}

int ps2_is_L3_pressed(void)
{
    return ps2_is_button_pressed(1, PS2_L3_BIT);
}

int ps2_is_SELECT_pressed(void)
{
    return ps2_is_button_pressed(1, PS2_SELECT_BIT);
}

int ps2_is_SQUARE_pressed(void)
{
    return ps2_is_button_pressed(2, PS2_SQUARE_BIT);
}

int ps2_is_CROSS_pressed(void)
{
    return ps2_is_button_pressed(2, PS2_CROSS_BIT);
}

int ps2_is_CIRCLE_pressed(void)
{
    return ps2_is_button_pressed(2, PS2_CIRCLE_BIT);
}

int ps2_is_TRIANGLE_pressed(void)
{
    return ps2_is_button_pressed(2, PS2_TRIANGLE_BIT);
}

int ps2_is_R1_pressed(void)
{
    return ps2_is_button_pressed(2, PS2_R1_BIT);
}

int ps2_is_L1_pressed(void)
{
    return ps2_is_button_pressed(2, PS2_L1_BIT);
}

int ps2_is_R2_pressed(void)
{
    return ps2_is_button_pressed(2, PS2_R2_BIT);
}

int ps2_is_L2_pressed(void)
{
    return ps2_is_button_pressed(2, PS2_L2_BIT);
}

int ps2_is_dengo_A_pressed(void)
{
    return ps2_is_button_pressed(2, PS2_DENGO_A_BIT);
}

int ps2_is_dengo_B_pressed(void)
{
    return ps2_is_button_pressed(2, PS2_DENGO_B_BIT);
}

int ps2_is_dengo_C_pressed(void)
{
    return ps2_is_button_pressed(2, PS2_DENGO_C_BIT);
}

int ps2_dengo_get_power(void)
{
    if (ps2_is_LEFT_pressed() && ps2_is_RIGHT_pressed() && !ps2_is_TRIANGLE_pressed())
        return 0;
    if (!ps2_is_LEFT_pressed() && ps2_is_RIGHT_pressed() && ps2_is_TRIANGLE_pressed())
        return 1;
    if (!ps2_is_LEFT_pressed() && ps2_is_RIGHT_pressed() && !ps2_is_TRIANGLE_pressed())
        return 2;
    if (ps2_is_LEFT_pressed() && !ps2_is_RIGHT_pressed() && ps2_is_TRIANGLE_pressed())
        return 3;
    if (ps2_is_LEFT_pressed() && !ps2_is_RIGHT_pressed() && !ps2_is_TRIANGLE_pressed())
        return 4;
    if (!ps2_is_LEFT_pressed() && !ps2_is_RIGHT_pressed() && ps2_is_TRIANGLE_pressed())
        return 5;
    return -1;
}

int ps2_dengo_get_break(void)
{
    if (!ps2_is_L1_pressed() && ps2_is_L2_pressed() && ps2_is_R1_pressed() && ps2_is_R2_pressed())
        return 0;
    if (ps2_is_L1_pressed() && !ps2_is_L2_pressed() && ps2_is_R1_pressed() && ps2_is_R2_pressed())
        return 1;
    if (!ps2_is_L1_pressed() && !ps2_is_L2_pressed() && ps2_is_R1_pressed() && ps2_is_R2_pressed())
        return 2;
    if (ps2_is_L1_pressed() && ps2_is_L2_pressed() && !ps2_is_R1_pressed() && ps2_is_R2_pressed())
        return 3;
    if (!ps2_is_L1_pressed() && ps2_is_L2_pressed() && !ps2_is_R1_pressed() && ps2_is_R2_pressed())
        return 4;
    if (ps2_is_L1_pressed() && !ps2_is_L2_pressed() && !ps2_is_R1_pressed() && ps2_is_R2_pressed())
        return 5;
    if (!ps2_is_L1_pressed() && !ps2_is_L2_pressed() && !ps2_is_R1_pressed() && ps2_is_R2_pressed())
        return 6;
    if (ps2_is_L1_pressed() && ps2_is_L2_pressed() && ps2_is_R1_pressed() && !ps2_is_R2_pressed())
        return 7;
    if (!ps2_is_L1_pressed() && ps2_is_L2_pressed() && ps2_is_R1_pressed() && !ps2_is_R2_pressed())
        return 8;
    if (!ps2_is_L1_pressed() && !ps2_is_L2_pressed() && !ps2_is_R1_pressed() && !ps2_is_R2_pressed())
        return 9;
    return -1;
}
