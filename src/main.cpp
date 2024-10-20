#include <Arduino.h>
#include <lvgl.h>
#include <ATD3.5-S3.h>
#include "gui/ui.h"

#define RX_PIN (2) // กำหนดขาต่อ RX VC-02-Kit
#define TX_PIN (1) // กำหนดขาต่อ TX VC-02-Kit

#define LED_Y_PIN (5)

void setup() {
  Serial.begin(115200);
  
  // Setup peripherals
  Display.begin(0); // rotation number 0
  Touch.begin();
  Sound.begin();
  // Card.begin(); // uncomment if you want to Read/Write/Play/Load file in MicroSD Card
  pinMode(LED_Y_PIN, OUTPUT);
  digitalWrite(LED_Y_PIN, HIGH); // off LED Y
  Serial.begin(115200); // ใช้งาน Serial Monitor
  Serial2.begin(115200, SERIAL_8N1, RX_PIN, TX_PIN); // ใช้งาน Serial2
  Serial2.setTimeout(50); // กำหนดเวลา Timeout เป็น 50 mS
  
  // Map peripheral to LVGL
  Display.useLVGL(); // Map display to LVGL
  Touch.useLVGL(); // Map touch screen to LVGL
  Sound.useLVGL(); // Map speaker to LVGL
  // Card.useLVGL(); // Map MicroSD Card to LVGL File System

  Display.enableAutoSleep(120); // Auto off display if not touch in 2 min
  
  // Add load your UI function
  ui_init();

  // Add event handle
  lv_obj_add_event_cb(ui_led_switch, [](lv_event_t * e) { // Add click LED switch event handle
    lv_obj_t * target = lv_event_get_target(e);
    if (lv_obj_has_state(target, LV_STATE_CHECKED)) { // if switch on
      digitalWrite(LED_Y_PIN, LOW); // ON LED Y
    } else {
      digitalWrite(LED_Y_PIN, HIGH); // OFF LED Y
    }
  }, LV_EVENT_CLICKED, NULL);
}

void loop() {
  Display.loop(); // Keep GUI work

  if (Serial2.available()) { // ถ้ามีข้อมูลเข้ามาที่ Serial2
    String command_code = Serial2.readString(); // อ่านค่าจาก Serial2
    if (command_code == "\x5A\x00\x00\x00\x5A") { // Wake Up
      Serial.println("Wake Up");
    } else if (command_code == "\x5A\x01\x00\x00\x5B") { // Standby
      Serial.println("Standby");
    } else if (command_code == "\x5A\x27\x00\x00\x81") { // turn on the light
      Serial.println("LED ON");
      lv_obj_add_state(ui_led_switch, LV_STATE_CHECKED);
      digitalWrite(LED_Y_PIN, LOW); // ON LED Y
    } else if (command_code == "\x5A\x28\x00\x00\x82") { // turn off the light
      Serial.println("LED OFF");
      lv_obj_clear_state(ui_led_switch, LV_STATE_CHECKED);
      digitalWrite(LED_Y_PIN, HIGH); // OFF LED Y
    } else {
      Serial.print("Unknow command : ");
      for (uint8_t i=0;i<command_code.length();i++) {
        Serial.print("0x");
        Serial.print((int) command_code.charAt(i), HEX);
        Serial.print(" ");
      }
      Serial.println();
    }
  }
}
