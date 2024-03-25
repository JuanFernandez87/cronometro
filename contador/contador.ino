#include <TM1637Display.h>

#define CLK 2 // Define el pin de reloj (CLK) del TM1637
#define DIO 3 // Define el pin de datos (DIO) del TM1637
#define START 4 // Define el pin del botón de inicio
#define CHANGE 5 // Define el pin del botón para cambio de piloto, se agregan 4 minutos por unica vez

TM1637Display display(CLK, DIO); // Inicializa el display

unsigned long tiempoInicio = 0; // Tiempo en el que se inició el cronómetro
bool cronometroIniciado = false; // Estado del cronómetro
unsigned long tiempoAviso = 11 * 1000; // Tiempo objetivo en milisegundos (10 segundos)
unsigned long tiempoObjetivo = 20 * 1000; // Tiempo objetivo en milisegundos (20 segundos)
// unsigned long tiempoObjetivo = 2 * 60 * 1000; // Tiempo objetivo en milisegundos (2 minutos)
unsigned long tiempoFaltante = 0; // Tiempo restante
unsigned long tiempoCambioPiloto = 30 * 1000; // Tiempo objetivo en milisegundos (20 segundos)
bool cambioPiloto = false; // Cambio de piloto

void setup() {
  Serial.begin(9600); // Para conectar con terminal e imprimir
  pinMode(START, INPUT_PULLUP);
  pinMode(CHANGE, INPUT_PULLUP);
  display.setBrightness(4); // Establece el brillo del display en 4
  display.showNumberDec(0, false, 4, 0); // Inicializa el display mostrando "0000"
}

void loop() {
  /* Cronometro en espera de inicio */
  if (digitalRead(START) == 0 && !cronometroIniciado) { // Si se presiona el botón y el cronómetro no está iniciado
    tiempoInicio = millis(); // Guarda el tiempo de inicio
    cronometroIniciado = true; // Marca el cronómetro como iniciado
  }
  
  /* Cronometro iniciado */
  if (cronometroIniciado) { // Si el cronómetro está iniciado
    unsigned long tiempoActual = millis(); // Obtiene el tiempo actual
    unsigned long tiempoTranscurrido = tiempoActual - tiempoInicio; // Calcula el tiempo transcurrido
    
    /* Si el tiempoTranscurrido alcanza el tiempoObjetivo */
    if (tiempoTranscurrido >= tiempoObjetivo) { // Si se alcanza el tiempo objetivo
      tiempoInicio = millis(); // Guarda el tiempo de inicio
      display.showNumberDec(0, false, 4, 0); // Inicializa el display mostrando "0000"
    } else {
      tiempoFaltante = tiempoObjetivo - tiempoTranscurrido; // Calcula el tiempo restante
      Serial.println(tiempoTranscurrido);
      /* Si faltan 10 segundos muestro la cuenta regresiva comenzando en 10 */
      if (tiempoFaltante <= tiempoAviso) { // Si faltan 10 segundos y el aviso no se ha mostrado
        // Si se alcanzo el tiempo 
        if (tiempoFaltante >= 0) {
          // Muestra el tiempo restante en el display
          int segundos = tiempoFaltante / 1000 % 60; // Calcula los segundos restantes
          display.showNumberDecEx(segundos); // Muestra los segundos y décimas en el display 
        }
      }

      /* Si se presiona el boton de start se detiene el parcial, 
      muestra la diferencia de tiempo que hubo entre tiempoTranscurrido y el tiempoObjetivo */
      if (digitalRead(START) == 0 && cronometroIniciado){
        // Muestra el tiempo que se marco en la vuelta
        unsigned long minutos = tiempoTranscurrido / 60000; // 1 minuto = 60000 milisegundos
        unsigned long segundos = (tiempoTranscurrido / 1000) % 60; // Calcula los segundos, 1 segundo = 1000 milisegundos
        unsigned long milisegundos = tiempoTranscurrido % 1000; // Calcula los milisegundos

        Serial.print("El tiempo marcado fue: ");
        Serial.println(minutos);
        Serial.println(segundos);
        Serial.println(milisegundos);

        unsigned long tiempoRestante = tiempoObjetivo - tiempoTranscurrido;
        unsigned long segundosRestantes = (tiempoRestante / 1000) % 60; // Calcula los segundos, 1 segundo = 1000 milisegundos
        unsigned long milisegundosRestantes = tiempoRestante % 1000; // Calcula los milisegundos

        Serial.print("La diferencia con el objetivo fue de: ");
        Serial.println(segundosRestantes);
        Serial.println(milisegundosRestantes);

        // Muestra los segundos y milisegundos en el display
        Serial.print("otro: ");
        Serial.println(segundosRestantes * 100 + milisegundosRestantes / 10);
        // display.showNumberDec(segundosRestantes * 1000 + milisegundosRestantes, false, 4, 1);

        if (cambioPiloto){ 
          cambioPiloto = false;
          tiempoObjetivo = 20 * 1000; // Tiempo objetivo en milisegundos (20 segundos)
        }
      }

      /* Si se presiona el boton de cambio de piloto se cambia el tiempoObjetivo */
      if (digitalRead(CHANGE) == 0){
        cambioPiloto = true;        
        tiempoObjetivo = tiempoCambioPiloto; // Agrego tiempo para cambio de piloto
      }
    }
  }
}