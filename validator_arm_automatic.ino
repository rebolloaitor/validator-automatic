#include <Servo.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Configuración de la pantalla OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Configuración del joystick
const int pinJoystickX = A0;  // Eje X del joystick
const int pinJoystickButton = 7;  // Botón del joystick

// Configuración del servo
Servo servoMotor;
const int pinServo = 9;

// Variables de control
bool modoAutomatico = false;
int anguloServo = 90;  // Posición inicial del servo en 90 grados

// Variables para el modo automático
int direccion = 1;  // 1 para girar hacia la derecha, -1 para girar hacia la izquierda
unsigned long tiempoAnterior = 0;
const int INTERVALO_AUTOMATICO = 2; // Tiempo en milisegundos para cada paso en automático


unsigned long tiempoActual = 0;

const int DURACION_PAUSA = 3000; // Tiempo en milisegundos para cada paso en automático
unsigned long startPause = 0;
unsigned long timepoPausa = 0;

int valorJoystickX = 0;

void mostrarModo() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 10);
  if (modoAutomatico) {
    display.print("Auto:");
  } else {
    display.print("Manual:");
  }
  /*display.setCursor(10, 30);
  display.print(anguloServo);
  display.print(":");
  display.print(valorJoystickX);*/
  display.display();
}

void setup() {
  // Inicializar pantalla OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("Error al inicializar pantalla OLED"));
    for(;;);
  }
  display.clearDisplay();
  
  // Inicializar el servo
  servoMotor.attach(pinServo);
  servoMotor.write(anguloServo);  // Posicionar servo en el centro (90 grados)

  // Configurar el pin del botón del joystick
  pinMode(pinJoystickButton, INPUT_PULLUP);
  
  // Mostrar el modo inicial en la pantalla
  mostrarModo();
}

void loop() {
  // Leer el estado del botón del joystick
  if (digitalRead(pinJoystickButton) == LOW) {
    delay(200);  // Anti-rebote
    modoAutomatico = !modoAutomatico;  // Cambiar entre modo manual y automático
    mostrarModo();  // Actualizar la pantalla OLED
  }

  if (modoAutomatico) {
    tiempoActual = millis();
    if (tiempoActual - tiempoAnterior >= INTERVALO_AUTOMATICO && startPause == 0) {
      tiempoAnterior = tiempoActual;
      
      // Actualizar el ángulo del servo
      anguloServo += direccion;
      if (anguloServo >= 200 || anguloServo <= 10) {
        if(anguloServo <= 10)
          startPause = millis();
        direccion = -direccion;  // Cambiar la dirección del movimiento
      }
      servoMotor.write(anguloServo);
    }
    else
    {
      timepoPausa = millis();
      if (timepoPausa - startPause >= DURACION_PAUSA) {
        startPause = 0;
        timepoPausa = 0;
      }
    }
  }
  else 
  {
     valorJoystickX = (analogRead(pinJoystickX) * -1) + 590;
     
    if(valorJoystickX < 0)
      valorJoystickX*=2;
    if(valorJoystickX < -800)
      valorJoystickX = -800;
    // Mapear el valor del joystick (0 a 1023) a un ángulo del servo (0 a 180)
    int nuevoAngulo = map(valorJoystickX, 0, 1023, 0, 180);
    nuevoAngulo += 195;
    
    // Solo actualizar el ángulo si es diferente al actual, para evitar sobreescritura constante
    if (nuevoAngulo != anguloServo) {
      anguloServo = nuevoAngulo;    // Actualizar el ángulo actual del servo
      servoMotor.write(anguloServo);
    }
  }
  //mostrarModo();
}
