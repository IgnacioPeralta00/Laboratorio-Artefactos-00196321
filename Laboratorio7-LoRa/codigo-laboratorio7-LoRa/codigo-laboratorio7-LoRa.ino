#include <SPI.h>
#include <LoRa.h> // Asumido por el uso de funciones LoRa en el documento

// ======================================================
// IDENTIFICACION
// ======================================================
const int MI_ID = 1;
const int DESTINO_ID = 2;
String mensajeOriginal = "TENGO SUEÑO";
// ======================================================
// PINES
// ======================================================
#define MISO 19
#define MOSI 23
#define SCK 18
#define NSS 5

#define TOUCH_PIN 27
#define UMBRAL_TOUCH 300 

bool toqueRegistrado = false; // Variable de estado para evitar envíos múltiples

// ======================================================
// ATBASH
// ======================================================
char convertirAtbash(char letra) {
  if (letra >= 'A' && letra <= 'Z') {
    return 'Z' - (letra - 'A');
  }
  if (letra >= 'a' && letra <= 'z') {
    return 'z' - (letra - 'a');
  }
  return letra;
}

String cifrarAtbash(String mensaje) {
  String mensajeCifrado = "";
  for (int i = 0; i < mensaje.length(); i++) {
    mensajeCifrado += convertirAtbash(mensaje.charAt(i));
  }
  return mensajeCifrado;
}

// ======================================================
// ENVIAR MENSAJE
// ======================================================
void enviarMensaje() {
  String mensajeCifrado = cifrarAtbash(mensajeOriginal);

  Serial.println();
  Serial.println("TOUCH DETECTADO");

  Serial.print("Mensaje original: ");
  Serial.println(mensajeOriginal);

  Serial.print("Mensaje cifrado: ");
  Serial.println(mensajeCifrado);

  // Formato del paquete: ORIGEN|DESTINO|MENSAJE
  LoRa.beginPacket();
  LoRa.print(MI_ID);
  LoRa.print("|");
  LoRa.print(DESTINO_ID);
  LoRa.print("|");
  LoRa.print(mensajeCifrado);
  LoRa.endPacket();

  Serial.print("Paquete enviado: ");
  Serial.print(MI_ID);
  Serial.print("|");
  Serial.print(DESTINO_ID);
  Serial.print("|");
  Serial.println(mensajeCifrado);

  Serial.println("----------------------------");
}

// ======================================================
// SETUP
// ======================================================
void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("TRANSMISOR LORA");
  Serial.print("Mi ID: ");
  Serial.println(MI_ID);
  Serial.print("Destino: ");
  Serial.println(DESTINO_ID);

  // Iniciar el bus SPI usando los pines definidos
  SPI.begin(SCK, MISO, MOSI, NSS);

  // Indicar a la librería los pines del módulo (NSS, RESET, DIO0)
  LoRa.setPins(NSS, -1, -1);

  // Iniciar LoRa a 433 MHz
  if (!LoRa.begin(433E6)) {
    Serial.println("ERROR: No se pudo iniciar LoRa");
    while (true);
  }

  Serial.println("LoRa iniciado correctamente");
  Serial.println("Esperando toque...");
}

// ======================================================
// LOOP
// ======================================================
void loop() {
  int valorTouch = touchRead(TOUCH_PIN);

  // Detectar el toque si el valor es menor al umbral
  if (valorTouch < UMBRAL_TOUCH) {
    if (!toqueRegistrado) {
      enviarMensaje();
      toqueRegistrado = true; // Bloquea el envío continuo mientras mantienes el dedo
    }
  } else {
    // Reinicia el estado cuando quitas el dedo
    toqueRegistrado = false; 
  }

  delay(100);
}