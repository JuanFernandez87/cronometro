#include <TM1637Display.h>

#define CLK 2 // Define el pin de reloj (CLK) del TM1637
#define DIO 3 // Define el pin de datos (DIO) del TM1637
#define BOTON_PIN 4 // Define el pin del botón

TM1637Display display(CLK, DIO); // Inicializa el display

unsigned long tiempoInicio = 0; // Tiempo en el que se inició el cronómetro
bool cronometroIniciado = false; // Estado del cronómetro
unsigned long tiempoAviso = 11 * 1000; // Tiempo objetivo en milisegundos (10 segundos)
unsigned long tiempoObjetivo = 20 * 1000; // Tiempo objetivo en milisegundos (20 segundos)
// unsigned long tiempoObjetivo = 118000; // Tiempo objetivo en milisegundos (30 segundos)
// unsigned long tiempoObjetivo = 2 * 60 * 1000; // Tiempo objetivo en milisegundos (2 minutos)
unsigned long tiempoFaltante = 0; // Tiempo restante
bool avisoMostrado = false; // Estado del aviso

void setup() {
  Serial.begin(9600);
  pinMode(BOTON_PIN, INPUT_PULLUP);
  display.setBrightness(4); // Establece el brillo del display en 4
  display.showNumberDec(0, false, 4, 0); // Inicializa el display mostrando "0000"
}

void loop() {
  if (digitalRead(BOTON_PIN) == 0 && !cronometroIniciado) { // Si se presiona el botón y el cronómetro no está iniciado
    tiempoInicio = millis(); // Guarda el tiempo de inicio
    cronometroIniciado = true; // Marca el cronómetro como iniciado
  }
  
  if (cronometroIniciado) { // Si el cronómetro está iniciado
    unsigned long tiempoActual = millis(); // Obtiene el tiempo actual
    unsigned long tiempoTranscurrido = tiempoActual - tiempoInicio; // Calcula el tiempo transcurrido
    
    if (tiempoTranscurrido >= tiempoObjetivo) { // Si se alcanza el tiempo objetivo
      tiempoInicio = millis(); // Guarda el tiempo de inicio
      avisoMostrado = false; // Reinicia el aviso
      display.showNumberDec(0, false, 4, 0); // Inicializa el display mostrando "0000"
    } else {
      tiempoFaltante = tiempoObjetivo - tiempoTranscurrido; // Calcula el tiempo restante
      Serial.print("El tiempo faltante es: ");
      Serial.println(tiempoFaltante);
      
      if (tiempoFaltante <= tiempoAviso && !avisoMostrado) { // Si faltan menos de 10 segundos y el aviso no se ha mostrado
        if (tiempoFaltante >= 0) {
          // Muestra el tiempo restante en el display
          int segundos = tiempoFaltante / 1000 % 60; // Calcula los segundos restantes
          int decimas = tiempoFaltante / 100 % 10; // Calcula las décimas de segundo restantes
          display.showNumberDecEx(segundos); // Muestra los segundos y décimas en el display 
        } else {
          avisoMostrado = true; // Marca el aviso como mostrado
        }
      }
    }
  }
}