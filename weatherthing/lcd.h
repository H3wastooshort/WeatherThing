#define H_LCD

LiquidCrystal_I2C lcd(LCD_ADDRESS,20,4);

void lcd_clear_line(uint8_t len = 20, char c = ' ') {
  for (; len > 0; len--) lcd.write(c);
}

byte gfx_antenna[8] = {
  0b01110,
  0b00001,
  0b01101,
  0b00101,
  0b10000,
  0b00000,
  0b10000,
  0b10000
};

byte gfx_drop[8] = {
  0b00100,
  0b00100,
  0b01110,
  0b01110,
  0b10111,
  0b11111,
  0b01110,
  0b00000
};

byte gfx_airTemp[8] = {
  0b00000,
  0b01010,
  0b10100,
  0b00000,
  0b00010,
  0b00010,
  0b00111,
  0b00010
};

byte gfx_indoorTemp[8] = {
  0b01000,
  0b00000,
  0b01000,
  0b01000,
  0b00010,
  0b00010,
  0b00111,
  0b00010
};

byte gfx_soilTemp[8] = {
  0b00000,
  0b11100,
  0b11100,
  0b00000,
  0b00010,
  0b00010,
  0b00111,
  0b00010
};

byte gfx_sun[8] = {
  0b11011,
  0b11000,
  0b00010,
  0b01001,
  0b00100,
  0b10100,
  0b10010,
  0b00000
};
