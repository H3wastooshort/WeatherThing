//C to F
float celsius_to_farenheit(float c) return (1.8*c) + 32;

//mBar to inHg
double pascal_to_inHg(double pa) return pa / double(3386.389);

//dewpt in C
float dewpointC(float c, float h, float pa) return c - (100 - h) / 5;

//does what its name says
String uint64ToString(uint64_t input) {
  String result = "";
  uint8_t base = 10;

  do {
    char c = input % base;
    input /= base;

    if (c < 10)
      c += '0';
    else
      c += 'A' - 10;
    result = c + result;
  } while (input);
  return result;
}