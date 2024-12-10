int rcPins[3] = {3, 5, 6}; // Pines de entrada para los canales

void setup() {
  Serial.begin(9600); // Inicializa la comunicación serie
  for (int i = 0; i < 3; i++) {
    pinMode(rcPins[i], INPUT); // Configura los pines como entradas
  }
}

void loop() {
  for (int i = 0; i < 3; i++) {
    int value = pulseIn(rcPins[i], HIGH, 30000); // Lee el ancho del pulso en microsegundos
    Serial.print("Canal ");
    Serial.print(i + 1);
    Serial.print(": ");
    Serial.println(value); // Imprime el valor leído
  }
  Serial.println("----"); // Separador para cada iteración
  delay(1000); // Espera 1 segundo antes de leer nuevamente
}
