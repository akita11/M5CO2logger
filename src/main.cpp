#include "M5Unified.h"
#include "M5UnitENV.h"

SCD4X scd4x;

void setup() {
    Serial.begin(115200);

	M5.begin();
    if (!scd4x.begin(&Wire, SCD4X_I2C_ADDR, 32, 33, 400000U)) {
        Serial.println("Couldn't find SCD4X");
        while (1) delay(1);
    }

    uint16_t error;
    error = scd4x.stopPeriodicMeasurement();
    if (error) printf("Error trying to execute stopPeriodicMeasurement():\n");

    error = scd4x.startPeriodicMeasurement();
    if (error) printf("Error trying to execute startPeriodicMeasurement():\n");

    printf("Waiting for first measurement... (5 sec)\n");
}

uint16_t px = 0;
#define X 320
uint16_t val[X][8]; // 8 values: CO2, temp, RH
uint16_t color[] = {RED, YELLOW, CYAN};

uint16_t conv_value(float value, float min, float max) {
	int16_t v = (value - min) / (max - min) * 240;
	if (v < min) v = min;
	else if (v > max) v = max;
	return(v);
}	


void loop() {
    float co2, temp, rh;
    if (scd4x.update()){
        co2 = scd4x.getCO2();
        temp = scd4x.getTemperature();
        rh = scd4x.getHumidity();
        printf("%f,%f,%f\n", co2, temp, rh);   
		val[px][0] = conv_value(co2, 0, 5000);
		val[px][1] = conv_value(temp, 0, 100);
		val[px][2] = conv_value(rh, 0, 100);
		px = (px + 1) % X;
		uint16_t x, p;
		p = px;
		for (x = 0; x < X; x++){
			uint8_t t = 0;
			M5.Display.drawFastVLine(x, 0, 240, BLACK);
			M5.Display.drawPixel(x, 120, LIGHTGREY);
			if (x % 4 == 0){
				M5.Display.drawPixel(x, 60, LIGHTGREY);
				M5.Display.drawPixel(x, 180, LIGHTGREY);
			}
			for (t = 0; t < 3; t++) {
				M5.Display.drawPixel(x, 240 - val[p][t], color[t]);
			}
			p = (p + 1) % X;
		}
		M5.Lcd.setCursor(0,0);
		M5.Lcd.setTextColor(color[0]); M5.Lcd.printf("CO2(0-5000)\n");
		M5.Lcd.setTextColor(color[1]); M5.Lcd.printf("Temp(0-100)\n");
		M5.Lcd.setTextColor(color[2]); M5.Lcd.printf("Hum(0-100)\n");
	}
    delay(1000);
}