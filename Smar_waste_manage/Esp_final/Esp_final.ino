#include <SPI.h>
#include <LoRa.h>
#include <WiFi.h>  // Add Wi-Fi library
#include <HTTPClient.h>
#include <vector>

// LoRa module pins
#define SS 5
#define RST 14
#define DIO0 2

// Firebase details
const char* FIREBASE_HOST = "https://sensordata-44cc7-default-rtdb.asia-southeast1.firebasedatabase.app/";
const char* FIREBASE_AUTH = "T50DavCgS73cwkv9CsuRxuXpk6HLiPsddS82S3OV";

// Wi-Fi credentials
const char* WIFI_SSID = "Galaxyadi";
const char* WIFI_PASSWORD = "ciib1438";

// SIM module pins
#define RXD2 27
#define TXD2 26
#define POWER_PIN 4
#define SerialAT Serial1

const uint8_t SYNC_WORD = 0xF3;
const char* AUTH_KEY = "AUTH123";

// Struct for child node data
struct ChildNode {
    String UID;
    int percentage;
};
std::vector<ChildNode> childNodes;

// Flag to switch between SIM and Wi-Fi
bool useWiFi = true;  // Set to false to use SIM module

void setup() {
    Serial.begin(115200);
    LoRa.setPins(SS, RST, DIO0);
    while (!LoRa.begin(433E6)) {
        Serial.println("Initializing LoRa...");
        delay(500);
    }
    LoRa.setSyncWord(SYNC_WORD);
    Serial.println("Master Node Initialized");

    pinMode(POWER_PIN, OUTPUT);
    digitalWrite(POWER_PIN, LOW);
    SerialAT.begin(115200, SERIAL_8N1, RXD2, TXD2);
    delay(20000);
    initSIMModule();

    if (useWiFi) {
        connectToWiFi();
    }
}

void loop() {
    int packetSize = LoRa.parsePacket();
    if (packetSize) {
        String message = "";
        while (LoRa.available()) {
            message += (char)LoRa.read();
        }
        processMessage(message);
    }
}

void processMessage(String message) {
    if (!message.startsWith("AUTH:" + String(AUTH_KEY))) {
        Serial.println("Invalid message: " + message);
        return;
    }

    int uidStart = message.indexOf(":", 10) + 1;
    int uidEnd = message.indexOf(":", uidStart);
    String UID = message.substring(uidStart, uidEnd);
    int percentage = message.substring(uidEnd + 1).toInt();

    // Update existing node or add a new one
    bool found = false;
    for (auto& node : childNodes) {
        if (node.UID == UID) {
            node.percentage = percentage;
            found = true;
            break;
        }
    }
    if (!found) {
        childNodes.push_back({UID, percentage});
    }

    // Send individual child data to Firebase
    if (useWiFi) {
        sendToFirebaseWiFi(UID, percentage);
    } else {
        sendToFirebaseSIM(UID, percentage);
    }
}

void sendToFirebaseWiFi(String UID, int percentage) {
    HTTPClient http;
    String url = String(FIREBASE_HOST) + "/sensorData/" + UID + ".json?auth=" + FIREBASE_AUTH;
    String payload = "{\"data\": " + String(percentage) + ", \"timestamp\": " + String(millis()) + "}";

    http.begin(url);
    http.addHeader("Content-Type", "application/json");
    int httpResponseCode = http.POST(payload);

    if (httpResponseCode > 0) {
        Serial.println("Data sent to Firebase via Wi-Fi: " + payload);
    } else {
        Serial.println("Error sending data via Wi-Fi: " + String(httpResponseCode));
    }
    http.end();
}

void sendToFirebaseSIM(String UID, int percentage) {
    String url = String(FIREBASE_HOST) + "/sensorData/" + UID + ".json?auth=" + FIREBASE_AUTH;
    String payload = "{\"data\": " + String(percentage) + ", \"timestamp\": " + String(millis()) + "}";

    SerialAT.println("AT+HTTPINIT");
    delay(200);
    SerialAT.println("AT+HTTPPARA=\"URL\",\"" + url + "\"");
    delay(200);
    SerialAT.println("AT+HTTPPARA=\"CONTENT\",\"application/json\"");
    delay(200);
    SerialAT.println("AT+HTTPDATA=" + String(payload.length()) + ",5000");
    delay(200);
    SerialAT.println(payload);
    delay(200);
    SerialAT.println("AT+HTTPACTION=1");
    delay(1000);
    SerialAT.println("AT+HTTPTERM");
    delay(200);

    Serial.println("Data sent to Firebase via SIM: " + payload);
}

void initSIMModule() {
    SerialAT.println("AT+CRESET");
    delay(1000);
    SerialAT.println("ATE0");
    delay(1000);
    SerialAT.println("AT+CPIN?");
    delay(1000);
    String rxString = SerialAT.readString();
    if (rxString.indexOf("+CPIN: READY") != -1) {
        Serial.println("SIM Card Ready");
    } else {
        Serial.println("SIM Card Not Ready");
    }
}

void connectToWiFi() {
    Serial.print("Connecting to WiFi");
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("Connected to WiFi");
}