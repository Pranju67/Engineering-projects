#include <SPI.h>
#include <LoRa.h>
#include <Servo.h>

#define SS 10
#define RST 9
#define DIO0 2

const String UID = "CHILD_2";
const String AUTH_KEY = "AUTH123";
const uint8_t SYNC_WORD = 0xF3;

#define TRIG_PIN1 4
#define ECHO_PIN1 3
#define TRIG_PIN2 19
#define ECHO_PIN2 20
#define TRIG_PIN3 5
#define ECHO_PIN3 7
#define SERVO_PIN 6

const float SCALE_X = 1.89;
const float SCALE_Y = 1.89;
const unsigned long SEND_INTERVAL = 5000;

unsigned long lastSendTime = 0;
Servo myServo;
bool lidOpen = false;
bool objectDetected = false;

void setup() {
    Serial.begin(115200);
    while (!Serial);
    
    LoRa.setPins(SS, RST, DIO0);
    while (!LoRa.begin(433E6)) {
        Serial.println("Initializing LoRa...");
        delay(500);
    }
    LoRa.setSyncWord(SYNC_WORD);
    Serial.println("Child Node Initialized: " + UID);

    pinMode(TRIG_PIN1, OUTPUT);
    pinMode(ECHO_PIN1, INPUT);
    pinMode(TRIG_PIN2, OUTPUT);
    pinMode(ECHO_PIN2, INPUT);
    pinMode(TRIG_PIN3, OUTPUT);
    pinMode(ECHO_PIN3, INPUT);

    myServo.attach(SERVO_PIN);
    myServo.write(0);
    Serial.println("Servo Initialized at 0°");
}

void loop() {
    if (millis() - lastSendTime > SEND_INTERVAL) {
        sendData();
        lastSendTime = millis();
    }
    operateServo();
}

void sendData() {
    delay(1000);
    float distance = getEnhancedUltrasonicData();
    int percentage = calculatePercentage(distance);
    
    Serial.print("Measured Distance: ");
    Serial.print(distance);
    Serial.println(" cm");

    Serial.print("Calculated Percentage: ");
    Serial.print(percentage);
    Serial.println(" %");

    String message = "AUTH:" + AUTH_KEY + ":" + UID + ":" + String(percentage);
    
    LoRa.beginPacket();
    LoRa.print(message);
    LoRa.endPacket();
    
    Serial.println("Sent via LoRa: " + message);
    delay(1000);
}

float getEnhancedUltrasonicData() {
    float d1 = measureDistance(TRIG_PIN1, ECHO_PIN1);
    float d2 = measureDistance(TRIG_PIN2, ECHO_PIN2);

    Serial.print("Ultrasonic 1 Distance: ");
    Serial.print(d1);
    Serial.println(" cm");

    Serial.print("Ultrasonic 2 Distance: ");
    Serial.print(d2);
    Serial.println(" cm");

    return (SCALE_X * d1 + SCALE_Y * d2) / 2.0;
}

int calculatePercentage(float distance) {
    if (distance > 50 && distance <= 1000) return 30;
    else if (distance > 20 && distance <= 50) return 60;
    else if (distance > 0 && distance <= 20) return 90;
    return 0;
}

void operateServo() {
    float distance = measureDistance(TRIG_PIN3, ECHO_PIN3);
    
    Serial.print("Lid Sensor Distance: ");
    Serial.print(distance);
    Serial.println(" cm");

    if (distance > 0 && distance < 55) {
        if (!objectDetected) {
            Serial.println("Object detected! Keeping lid at 120°");
            myServo.write(120);  // Keep the lid at 120° when an object is detected
            objectDetected = true;
        }
    } else if (distance >= 55 && objectDetected) {
        Serial.println("Object moved away! Closing lid to 0°");
        myServo.write(0);  // Move to 0° only when the object moves away
        objectDetected = false;
    }
}

float measureDistance(int trigPin, int echoPin) {
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    long duration = pulseIn(echoPin, HIGH);
    return (duration * 0.034) / 2.0;
}
