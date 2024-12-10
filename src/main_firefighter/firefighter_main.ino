


// Pines del receptor
int receiverPins[3] = {3, 5, 6}; // Pines conectados a los canales del receptor (Dirección, Throttle, Switches)

// Pines del controlador L298N
const int EN_A = 11;   // Control de velocidad Motor A
const int IN1 = A0;    // Dirección Motor A
const int IN2 = A1;    // Dirección Motor A

const int EN_B = 10;  // Control de velocidad Motor B
const int IN3 = A2;    // Dirección Motor B
const int IN4 = A3;    // Dirección Motor B

const int PUMP_PIN = 8; // Pin para la bomba

// Pines para LEDs y buzzer
const int RED_LED = 7;  // Pin para LED rojo
const int BLUE_LED = 9; // Pin para LED azul
const int BUZZER = 12;  // Pin para el buzzer

unsigned long previousMillis = 0; // Variable para temporizador
const long interval = 500; // Intervalo de parpadeo (500ms)
bool ledState = false; // Estado actual de los LEDs

void setup() {
  Serial.begin(9600);

  // Configurar pines del controlador como salida
  pinMode(EN_A, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(EN_B, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  pinMode(PUMP_PIN, OUTPUT); // Configurar pin de la bomba como salida
  digitalWrite(PUMP_PIN, LOW); // Asegurarse de que la bomba esté apagada al inicio

    // Configurar pines para LEDs y buzzer como salida
  pinMode(RED_LED, OUTPUT);
  pinMode(BLUE_LED, OUTPUT);
  pinMode(BUZZER, OUTPUT);

  // Apagar LEDs y buzzer al inicio
  digitalWrite(RED_LED, LOW);
  digitalWrite(BLUE_LED, LOW);
  digitalWrite(BUZZER, LOW);
}

void loop() {
  // Leer señal de los canales del receptor
  int directionValue = pulseIn(receiverPins[0], HIGH, 30000); // Canal 1 (Dirección)
  int throttleValue = pulseIn(receiverPins[1], HIGH, 30000);  // Canal 2 (Throttle)
  int switchesValue = pulseIn(receiverPins[2], HIGH, 30000);  // Canal 3 (Switches)

  // Mapear valores del receptor
  int motorSpeed = map(throttleValue, 995, 1990, 0, 255); // Throttle mapeado a PWM
  motorSpeed = constrain(motorSpeed, 0, 255); // Limitar valores al rango válido

  int directionPWM = map(directionValue, 995, 1990, -255, 255); // Dirección mapeada a -255 a 255
  directionPWM = constrain(directionPWM, -255, 255); // Limitar valores al rango válido

  // Detectar switches con margen de error
  bool reverseMode = (switchesValue >= 1295 - 10 && switchesValue <= 1295 + 10) || (switchesValue >= 1495 - 10 && switchesValue <= 1495 + 10) ; // Switch izquierdo posición 1
  bool pumpActive = (switchesValue >= 1195 - 10 && switchesValue <= 1195 + 10) || (switchesValue >= 1495 - 10 && switchesValue <= 1495 + 10);  // Switch derecho posición 1

  // Controlar motores en función de dirección y throttle
  int leftMotorSpeed, rightMotorSpeed;

  if (motorSpeed == 0) {
    // Giro sobre sí mismo
    leftMotorSpeed = directionPWM;
    rightMotorSpeed = -directionPWM;
  } else {
    // Ajuste de velocidad para giros suaves 
    leftMotorSpeed = motorSpeed + (directionPWM / 2);
    rightMotorSpeed = motorSpeed - (directionPWM / 2);
  }

  // Limitar velocidades
  leftMotorSpeed = constrain(leftMotorSpeed, -255, 255);
  rightMotorSpeed = constrain(rightMotorSpeed, -255, 255);

  // Cambiar dirección si está en modo reversa
  if (reverseMode) {
    leftMotorSpeed = -leftMotorSpeed;
    rightMotorSpeed = -rightMotorSpeed;
  }

  // Configurar dirección y velocidad de Motor A
  if (leftMotorSpeed > 0) {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
  } else {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
  }

  // Velocidad mimima para que se mueve el motor por construcción
  if(abs(leftMotorSpeed) > 75){
    analogWrite(EN_A, abs(leftMotorSpeed));
  } else {
    analogWrite(EN_A, 0);
  }


  // Configurar dirección y velocidad de Motor B
  if (rightMotorSpeed > 0) {
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
  } else {
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
  }

  // Velocidad mimima para que se mueve el motor por construcción
  if(abs(rightMotorSpeed) > 75){
    analogWrite(EN_B, abs(rightMotorSpeed));
  } else {
    analogWrite(EN_B, 0);
  }

  // Controlar bomba
  if (pumpActive) {
    digitalWrite(PUMP_PIN, HIGH); // Activar bomba
  } else {
    digitalWrite(PUMP_PIN, LOW); // Apagar bomba
  }

    // Controlar LEDs y buzzer
  if (rightMotorSpeed < 50 && leftMotorSpeed < 50 && !pumpActive){
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;
      ledState = !ledState; // Cambiar estado de los LEDs
  
      // Alternar LEDs
      digitalWrite(RED_LED, ledState);
      digitalWrite(BLUE_LED, !ledState);
  
      // Generar sonido de sirena en el buzzer
      tone(BUZZER, ledState ? 1000 : 500, 500); // Alternar entre 1000Hz y 500Hz cada medio segundo
    }
  }

  // Debug para monitorear valores
  Serial.print("Throttle: ");
  Serial.print(throttleValue);
  Serial.print(" -> Speed: ");
  Serial.println(motorSpeed);

  Serial.print("Direction: ");
  Serial.print(directionValue);
  Serial.print(" -> Left: ");
  Serial.print(leftMotorSpeed);
  Serial.print(", Right: ");
  Serial.println(rightMotorSpeed);

  Serial.print("Switches: ");
  Serial.print(switchesValue);
  Serial.print(" -> Reverse Mode: ");
  Serial.print(reverseMode);
  Serial.print(", Pump: ");
  Serial.println(pumpActive);

  Serial.println("----");

  delay(20); // Pequeño retraso para estabilidad
}
