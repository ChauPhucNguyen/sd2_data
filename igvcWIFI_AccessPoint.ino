#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>

// WiFi Configuration
const char* ssid = "IGVC_WIFI";
const char* password = "igvcpassword";

// Motor Control Pins
#define MOTOR_PIN1 2
#define MOTOR_PIN2 19

// Sensor Pins (Don't even know if they exist, need to ask dezean, placeholder for now)
#define BATTERY_PIN 34
#define TEMPERATURE_PIN 35

// Create WebServer instance
WebServer server(80);

// Sensor Data Structure
struct SensorData {
  float batteryVoltage;
  float temperature;
  String motorStatus;
};

// Global sensor data object
SensorData currentData;

void setup() {
  // Initialize Serial
  Serial.begin(115200);

  // Set pin modes
  pinMode(MOTOR_PIN1, OUTPUT);
  pinMode(MOTOR_PIN2, OUTPUT);
  pinMode(BATTERY_PIN, INPUT);
  pinMode(TEMPERATURE_PIN, INPUT);

  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Connected");
  Serial.println("IP Address: " + WiFi.localIP().toString());

  // Define server routes
  server.on("/", handleRoot);
  server.on("/data", handleSensorData);
  server.on("/command", handleCommand);
  server.onNotFound(handleNotFound);

  // Start server
  server.begin();
  Serial.println("HTTP Server Started");
}

void loop() {
  // Handle client requests
  server.handleClient();

  // Update sensor data periodically
  updateSensorReadings();
}

void updateSensorReadings() {
  // Read battery voltage (example conversion)
  int rawBattery = analogRead(BATTERY_PIN);
  currentData.batteryVoltage = rawBattery * (3.3 / 4096.0) * 2; // Voltage divider adjustment

  // Read temperature (example - replace with actual sensor reading)
  int rawTemp = analogRead(TEMPERATURE_PIN);
  currentData.temperature = rawTemp * 0.1; // Example conversion

  // Update motor status
  currentData.motorStatus = digitalRead(MOTOR_PIN1) == HIGH ? "Forward" : 
                             digitalRead(MOTOR_PIN2) == HIGH ? "Backward" : "Stopped";
}

void handleRoot() {
  String html = R"(
<!DOCTYPE html>
<html>
<head>
    <title>ESP32 Robot Control</title>
</head>
<body>
    <h1>Robot Control Panel</h1>
    <div id="sensorData">Loading...</div>
    <button onclick="sendCommand('FORWARD')">Forward</button>
    <button onclick="sendCommand('BACKWARD')">Backward</button>
    <button onclick="sendCommand('LEFT')">Left</button>
    <button onclick="sendCommand('RIGHT')">Right</button>

    <script>
        function updateData() {
            fetch('/data')
                .then(response => response.json())
                .then(data => {
                    document.getElementById('sensorData').innerHTML = 
                        `Battery: ${data.batteryVoltage.toFixed(2)}V<br>` +
                        `Temperature: ${data.temperature.toFixed(1)}°C<br>` +
                        `Motor Status: ${data.motorStatus}`;
                });
        }

        function sendCommand(cmd) {
            fetch(`/command?action=${cmd}`, { method: 'POST' })
                .then(response => updateData());
        }

        // Update data every 2 seconds
        setInterval(updateData, 2000);
        updateData(); // Initial update
    </script>
</body>
</html>
  )";
  server.send(200, "text/html", html);
}

void handleSensorData() {
  // Create JSON response with sensor data
  StaticJsonDocument<200> doc;
  doc["batteryVoltage"] = currentData.batteryVoltage;
  doc["temperature"] = currentData.temperature;
  doc["motorStatus"] = currentData.motorStatus;

  String jsonResponse;
  serializeJson(doc, jsonResponse);

  server.send(200, "application/json", jsonResponse);
}

void handleCommand() {
  if (server.method() != HTTP_POST) {
    server.send(405, "text/plain", "Method Not Allowed");
    return;
  }

  String action = server.arg("action");

  // Reset all motor pins
  digitalWrite(MOTOR_PIN1, LOW);
  digitalWrite(MOTOR_PIN2, LOW);

  // Process command
  if (action == "FORWARD") {
    digitalWrite(MOTOR_PIN1, HIGH);
    digitalWrite(MOTOR_PIN2, HIGH);
  } else if (action == "BACKWARD") {
    digitalWrite(MOTOR_PIN1, LOW);
    digitalWrite(MOTOR_PIN2, LOW);
  } else if (action == "LEFT") {
    digitalWrite(MOTOR_PIN1, HIGH);
    digitalWrite(MOTOR_PIN2, LOW);
  } else if (action == "RIGHT") {
    digitalWrite(MOTOR_PIN1, LOW);
    digitalWrite(MOTOR_PIN2, HIGH);
  }

  server.send(200, "text/plain", "Command Executed: " + action);
}

void handleNotFound() {
  server.send(404, "text/plain", "404: Not Found");
}