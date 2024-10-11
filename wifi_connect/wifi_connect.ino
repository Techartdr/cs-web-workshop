#include <WiFi.h>
#include <WebSocketsServer.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP32Servo.h>

#define PIN 9
#define NUMPIXELS 2
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C
#define PIN_SG90 8

// Matrice des boutons
const int ROW_PINS[] = {7, 8, 1};  // Lignes
const int COL_PINS[] = {20, 10, 0};  // Colonnes

const char* ssid = "NAME";
const char* password = "PASS";

WebSocketsServer webSocket(80);
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

uint32_t selectedAngle = 180;
int selectedOption = 0; // Option sélectionnée
unsigned long lastDebounceTime = 0; // Pour la gestion du debounce
const unsigned long debounceDelay = 200; // Délai pour le debounce

Servo sg90;
int servoPosition = 0;  // Position actuelle du servo
int servoStep = 1;      // Incrément de la rotation du servo
unsigned long lastServoMove = 0;  // Pour le contrôle du timing du servo
const unsigned long servoMoveDelay = 20;  // Délai entre chaque mouvement du servo

void setup() {
  Serial.begin(115200);

  // Initialisation du WiFi
  connectToWiFi();

  // Initialisation de l'écran OLED
  Wire.begin(5, 6);
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    while (true);
  }
  display.clearDisplay();
  display.display();
  displayMessage("En attente");

  // Initialisation du serveur WebSocket
  webSocket.begin();
  webSocket.onEvent(onWebSocketEvent);

  // Configuration des boutons (matrice)
  setupButtonMatrix();

  // Initialisation du servomoteur
  sg90.setPeriodHertz(50); // Fréquence PWM pour le SG90
  sg90.attach(PIN_SG90, 500, 2400); // Largeur d'impulsion min et max (µs) pour 0° à 180°
}

void loop() {
  webSocket.loop();
  handleButtonPress();
  moveServo();  // Contrôler le servo sans bloquer le programme
}

// Fonction pour gérer la connexion WiFi
void connectToWiFi() {
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

// Gestion des événements WebSocket
void onWebSocketEvent(uint8_t num, WStype_t type, uint8_t* payload, size_t length) {
  if (type == WStype_TEXT) {
    String message = (char*)payload;
    Serial.println("Message received: " + message);
    displayMessage((char*)payload);

    if (message.startsWith("SERVO:")) {
      int n;
      sscanf(message.c_str(), "SERVO:%d", &n);
      selectedAngle = n;
      Serial.println(selectedAngle);
}
  }
}

// Fonction pour configurer la matrice de boutons
void setupButtonMatrix() {
  for (int row = 0; row < 3; row++) {
    pinMode(ROW_PINS[row], OUTPUT);
    digitalWrite(ROW_PINS[row], HIGH); // Initialise les lignes à HIGH
  }
  
  for (int col = 0; col < 3; col++) {
    pinMode(COL_PINS[col], INPUT_PULLUP); // Résistance de pull-up pour les colonnes
  }
}

// Gestion de la pression des boutons avec debounce
void handleButtonPress() {
  for (int row = 0; row < 3; row++) {
    digitalWrite(ROW_PINS[row], LOW); // Active la ligne actuelle

    for (int col = 0; col < 3; col++) {
      if (digitalRead(COL_PINS[col]) == LOW && (millis() - lastDebounceTime) > debounceDelay) {
        lastDebounceTime = millis(); // Mettre à jour le temps pour le debounce
        processButtonPress(row, col);
      }
    }
    
    digitalWrite(ROW_PINS[row], HIGH); // Désactive la ligne actuelle
  }
}

// Traitement des pressions de boutons en fonction de la position
void processButtonPress(int row, int col) {
  if (row == 0 && col == 0) {  // Bouton "Up"
    selectedOption = (selectedOption + 1) % 3;
  } else if (row == 2 && col == 0) {  // Bouton "Down"
    selectedOption = (selectedOption + 2) % 3;
  } else if (row == 0 && col == 2) {  // Bouton "A" pour valider
    if (selectedOption == 0) {
      webSocket.broadcastTXT("Satisfaction: Yes");
    } else if (selectedOption == 1) {
      webSocket.broadcastTXT("Satisfaction: No");
    }
    //selectedOption = 2; // En attente
  }
  updateDisplayState();
}

// Contrôler le servomoteur de manière non bloquante
void moveServo() {
  unsigned long currentMillis = millis();
  
  // Vérifiez si le délai entre les mouvements du servo est écoulé
  if (currentMillis - lastServoMove >= servoMoveDelay) {
    lastServoMove = currentMillis;  // Mettre à jour le dernier mouvement
    
    // Ajuster la position du servo
    sg90.write(servoPosition);
    servoPosition += servoStep;  // Incrément ou décrément
    
    // Inverser la direction du mouvement si les limites sont atteintes
    if (servoPosition >= selectedAngle || servoPosition <= 0) {
      servoStep = -servoStep;  // Inverser la direction
    }
  }
}

// Affichage du message à l'écran OLED
void updateDisplayState() {
  switch (selectedOption) {
    case 0:
      displayMessage("Oui");
      break;
    case 1:
      displayMessage("Non");
      break;
    case 2:
      displayMessage("En attente");
      break;
  }
}

// Affichage d'un message sur l'écran OLED
void displayMessage(const char* msg) {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 20);
  display.print(msg);
  display.display();
}
