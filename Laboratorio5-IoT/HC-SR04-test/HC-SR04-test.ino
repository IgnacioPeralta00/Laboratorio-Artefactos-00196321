#define TRIG_PIN 18
#define ECHO_PIN 19

void setup() {
  Serial.begin(115200);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
}

void loop() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  // Enviar pulso de 10 microsegundos
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);

  digitalWrite(TRIG_PIN, LOW);

  // Leer el tiempo de viaje del sonido de ida y vuelta
  long duracion = pulseIn(ECHO_PIN, HIGH);

  if (duracion == 0) {
    Serial.println("No se detectó eco");
  } else {
    // Calcular distancia basándose en la velocidad del sonido
    float distancia = duracion * 0.0343 / 2.0;

    Serial.print("Distancia: ");
    Serial.print(distancia);
    Serial.println(" cm");
  }

  delay(500);
}
