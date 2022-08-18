    #include <Arduino.h>
#include <SPI.h>
#include "rtc.hpp"
#include "delay.hpp"
#include "waterpump.hpp"
#include "display.hpp"

#define TICK_MS 100
#define WATER_PUMP_PIN 2
#define PANIC_BUTTON_PIN 3


using greenhouse::rtc::DateTime;

greenhouse::rtc::Rtc<> rtc;
greenhouse::waterpump::WaterPump<WATER_PUMP_PIN> waterpump;
greenhouse::pin::DigitalInput<PANIC_BUTTON_PIN> panic_button;
greenhouse::display::Display<> display;

String prev_message = "";

void setup();
void loop();
String get_status(bool is_waterpump_on, bool is_panic);

void setup() {
    Serial.begin(9600);

    rtc.begin();
    waterpump.begin();
    panic_button.begin();
    display.begin();

    display.hello();
    rtc.set_alarm(DateTime(0, 0, 0, 21, 45, 00), DS3231_A1_Hour);
}

void loop() {
    auto ms = millis();

    bool is_panic = panic_button.get() == HIGH;
    String status = get_status(waterpump.is_on(), is_panic);
    String message = greenhouse::rtc::format_date_time(rtc.now(), false) + "Status: " + status;
    

    waterpump.tick(ms);
    panic_button.read();

    if (rtc.alarm_fired())
    {
        waterpump.on();
        rtc.clear_alarm();
    }

    if (is_panic) {
        Serial.println("Panic!");
        waterpump.off();
    }

    if (message != prev_message) {
        display.display(message);
        prev_message = message;
    } 

    // Some debugging messages
    Serial.println(greenhouse::rtc::format_date_time(rtc.now()));

    delay(TICK_MS);
}

String get_status(bool is_waterpump_on, bool is_panic) {
    String status = "OK";

    if (waterpump.is_on())
    {
        status = "Pump ON";
    }

    if (is_panic) {
        status = "Panic!";
    }

    return status;
}