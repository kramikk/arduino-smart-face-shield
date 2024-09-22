#include <NewPing.h>
#include <Adafruit_MLX90614.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "RTClib.h"

// Global variables and definitions
char *typeName[] = {"Object:", "Ambient:"};
Adafruit_MLX90614 mlx = Adafruit_MLX90614();
RTC_DS3231 rtc;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

#define SCREEN_WIDTH 128      // OLED display width, in pixels
#define SCREEN_HEIGHT 64      // OLED display height, in pixels
#define OLED_RESET 4          // Reset pin for the OLED display
#define VIBRATION_MOTOR 10    // Pin for the vibration motor
#define TRIGGER_PIN 11        // Ultrasonic sensor trigger pin
#define ECHO_PIN 12           // Ultrasonic sensor echo pin
#define MAX_DISTANCE 100      // Maximum distance for the ultrasonic sensor in cm
#define VCC2 2                // Additional VCC (5V) for the ultrasonic sensor

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); 
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
  // Setup pins and initialize components
  pinMode(VCC2, OUTPUT);                // Extra VCC for ultrasonic sensor
  digitalWrite(VCC2, HIGH);             // Set extra VCC to HIGH (5V)
  pinMode(VIBRATION_MOTOR, OUTPUT);     // Pin for vibration motor
  Serial.begin(9600);                   // Start serial communication
  delay(3000);                          // Wait for console opening
  
  // Initialize RTC
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }
  if (rtc.lostPower()) {
    Serial.println("RTC lost power, setting time.");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); // Set RTC to compile time
  }
  
  // Initialize OLED display
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // Initialize with I2C address 0x3C
  display.clearDisplay();
  display.display();                           // Display buffer content
  display.setTextColor(WHITE, BLACK);
  display.drawRect(95, 30, 3, 3, WHITE);      // Degree symbol ( ° )
  draw_text(60, 57, "cm", 1);
  draw_text(100, 30, "C", 1);
  
  // Check temperature and control vibration motor
  if (mlx.readObjectTempC() >= 37.5) {
    digitalWrite(VIBRATION_MOTOR, HIGH);
  } else {
    digitalWrite(VIBRATION_MOTOR, LOW);
  }
}

void loop() {
  DateTime now = rtc.now();
  int distanceCM = sonar.ping_cm();           // Get distance in cm
  
  // Display date
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print(daysOfTheWeek[now.dayOfTheWeek()]);
  
  char currentDate[16];
  sprintf(currentDate, "%02d/%02d/", now.day(), now.month()); // Leading zeros for day and month
  display.setCursor(62, 0);
  display.print(currentDate);
  
  display.setCursor(102, 0);
  display.print(now.year(), DEC);
  
  // Display time
  char buffer[16];
  sprintf(buffer, "%02d:%02d:%02d", now.hour(), now.minute(), now.second());
  display.setCursor(40, 13);
  display.setTextSize(1);
  display.print(buffer);
  
  // Display temperature
  display.setTextSize(2);
  display.setCursor(33, 30);
  display.print(mlx.readObjectTempC());
  display.display();
  
  // Display distance
  display.setTextSize(1);
  display.setCursor(45, 57);
  display.print(distanceCM);
  display.display();

  // Output to Serial
  Serial.println(distanceCM);
  Serial.println(mlx.readObjectTempC());
  
  delay(500);  // Delay for half a second
}

// Function to display text on the OLED
void draw_text(byte x_pos, byte y_pos, char *text, byte text_size) {
  display.setCursor(x_pos, y_pos);
  display.setTextSize(text_size);
  display.print(text);
  display.display();
}
