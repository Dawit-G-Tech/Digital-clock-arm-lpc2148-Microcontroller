#include <NXP/iolpc2148.h>
#include <stdio.h>

#define RS (1<<24)
#define EN (1<<25)
#define LCD_DATA_MASK 0xFF0000  // P1.16–P1.23

#define SET  (1<<11)
#define INC  (1<<12)
#define DEC  (1<<13)
#define SAVE (1<<14)
int prev_set = 1, prev_save = 1, prev_inc = 1, prev_dec = 1;


unsigned int hour = 12, minute = 0, second = 0;
unsigned int day = 1, month = 1, year = 2025;

int edit_mode = 0;
int edit_index = 0;

void delay_ms(unsigned int ms) {
    for (unsigned int i = 0; i < ms * 150; i++);
}

void lcd_enable() {
    IO1SET = EN;
    delay_ms(2);
    IO1CLR = EN;
}

void lcd_cmd(unsigned char cmd) {
    IO1CLR = RS;
    IO1CLR = LCD_DATA_MASK;
    IO1SET = (cmd << 16) & LCD_DATA_MASK;
    lcd_enable();
}




void lcd_data(unsigned char data) {
    IO1SET = RS;
    IO1CLR = LCD_DATA_MASK;
    IO1SET = (data << 16) & LCD_DATA_MASK;
    lcd_enable();
}

void lcd_init() {
    IO1DIR |= RS | EN | LCD_DATA_MASK;
    delay_ms(20);
    lcd_cmd(0x38); // 8-bit, 2-line
    lcd_cmd(0x0C); // Display ON, cursor OFF
    lcd_cmd(0x01); // Clear display
    lcd_cmd(0x06); // Entry mode
}

void lcd_print2digit(int num) {
    lcd_data((num / 10) + '0');
    lcd_data((num % 10) + '0');
}

void update_lcd_full() {
    lcd_cmd(0x80); // Line 1 start
    lcd_print("Time: ");
    lcd_print2digit(hour);
    lcd_data(':');
    lcd_print2digit(minute);
    lcd_data(':');
    lcd_print2digit(second);

    lcd_cmd(0xC0); // Line 2 start
    lcd_print("Date: ");
    lcd_print2digit(day);
    lcd_data('/');
    lcd_print2digit(month);
    lcd_data('/');
    lcd_print2digit(year);
}

void update_lcd_field() {
    switch (edit_index) {
        case 0: lcd_cmd(0x86); lcd_print2digit(hour); break;
        case 1: lcd_cmd(0x89); lcd_print2digit(minute); break;
        case 2: lcd_cmd(0x8C); lcd_print2digit(second); break;
        case 3: lcd_cmd(0xC6); lcd_print2digit(day); break;
        case 4: lcd_cmd(0xC9); lcd_print2digit(month); break;
        case 5: lcd_cmd(0xCC); lcd_print2digit(year); break;
    }
}

void lcd_gotoxy(unsigned int x, unsigned int y) {
    unsigned char pos = (y == 1) ? 0xC0 : 0x80;
    lcd_cmd(pos + x);
}

void lcd_puts(const char* str) {
    while (*str) lcd_data(*str++);
}

void display_time_date() {
    char buf[17];
    lcd_cmd(0x80); // 1st line
    sprintf(buf, "%02d:%02d:%02d", hour, minute, second);
    lcd_puts(buf);
    
    lcd_cmd(0xC0); // 2nd line
    sprintf(buf, "%02d/%02d/%04d", day, month, year);
    lcd_puts(buf);
}

void clear_cursor() {
    lcd_cmd(0x0C); // Cursor OFF
}

void show_cursor() {
    lcd_cmd(0x0F); // Cursor ON and blink
}

int is_pressed(unsigned int pin) {
    return !(IO0PIN & pin); // Active LOW
}

void update_cursor_position() {
    const unsigned char cursor_pos[] = {
        0, 1, 3, 4, 6, 7,     // hh:mm:ss (positions on line 1)
        0x40, 0x41, 0x43, 0x44, 0x46 // dd/mm/yyyy (line 2 starts at 0x40)
    };
    lcd_cmd(0x80 + cursor_pos[edit_index]);
    show_cursor();
}

void increment_field() {
    switch (edit_index) {
        case 0: hour = (hour + 10) % 24; break;
        case 1: hour = (hour + 1) % 24; break;
        case 2: minute = (minute + 10) % 60; break;
        case 3: minute = (minute + 1) % 60; break;
        case 4: second = (second + 10) % 60; break;
        case 5: second = (second + 1) % 60; break;
        case 6: day = (day % 31) + 1; break;
        case 7: day = (day % 31) + 1; break;
        case 8: month = (month % 12) + 1; break;
        case 9: month = (month % 12) + 1; break;
        case 10: year++; break;
    }
}

void decrement_field() {
    switch (edit_index) {
        case 0: hour = (hour >= 10) ? hour - 10 : 23; break;
        case 1: hour = (hour > 0) ? hour - 1 : 23; break;
        case 2: minute = (minute >= 10) ? minute - 10 : 59; break;
        case 3: minute = (minute > 0) ? minute - 1 : 59; break;
        case 4: second = (second >= 10) ? second - 10 : 59; break;
        case 5: second = (second > 0) ? second - 1 : 59; break;
        case 6: day = (day > 1) ? day - 1 : 31; break;
        case 7: day = (day > 1) ? day - 1 : 31; break;
        case 8: month = (month > 1) ? month - 1 : 12; break;
        case 9: month = (month > 1) ? month - 1 : 12; break;
        case 10: if (year > 2000) year--; break;
    }
}

void blink_cursor_at_field() {
    const unsigned char positions[] = {0x86, 0x89, 0x8C, 0xC6, 0xC9, 0xCC};
    lcd_cmd(0x0F); // Cursor ON, blink ON
    lcd_cmd(positions[edit_index]);
}


void check_buttons() {
    int cur_set = IO0PIN & SET;
    int cur_inc = IO0PIN & INC;
    int cur_dec = IO0PIN & DEC;
    int cur_save = IO0PIN & SAVE;

    // SET: Only advance on falling edge
    if (!cur_set && prev_set) {
        if (!edit_mode) {
            edit_mode = 1;
            edit_index = 0;
        } else {
            edit_index++;
            if (edit_index > 10) edit_index = 10;
        }
        delay_ms(200);
    }

    // INC
    if (!cur_inc && prev_inc) {
        if (edit_mode) {
            increment_field();
            delay_ms(200);
        }
    }

    // DEC
    if (!cur_dec && prev_dec) {
        if (edit_mode) {
            decrement_field();
            delay_ms(200);
        }
    }

    // SAVE: exit edit mode
    if (!cur_save && prev_save) {
        edit_mode = 0;
        clear_cursor();
        delay_ms(300);
    }

    // Store current states for edge detection
    prev_set = cur_set;
    prev_inc = cur_inc;
    prev_dec = cur_dec;
    prev_save = cur_save;
}

int main() {
    IO0DIR &= ~(SET | INC | DEC | SAVE); // Buttons as input
    lcd_init();

    while (1) {
    check_buttons();  // Handle button logic

    if (!edit_mode) {
        update_lcd_full(); // Normal display
        lcd_cmd(0x0C); // Cursor OFF
        delay_ms(500);
        increment_time(); // Simulate real-time update
    } else {
        update_lcd_field(); // Only show the selected editable field
        blink_cursor_at_field(); // Optional: Add cursor blinking logic
    }
}

}
