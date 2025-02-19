#include <Arduino.h>

#define S1 D1
#define S2 D2
#define DIRA D3
#define DIRB D4
#define MOTOR_1_PWM D5

#define STEP_MARGIN_X 1L       //10 - 1000 (2)
#define MIN_DUTYCYCLE_X 0    //0 - 255 (125)
#define MAX_DUTYCYCLE_X 255    //0 - 255 (255)

unsigned long stepTime = 1;
unsigned long millisStart = 0, millisEnd = 0;
long MxSetPoint = 0;
long MxActualPoint = 0;
byte MxStep = 0;
byte MxStepDone = 0;
byte readByte = 0;
byte readByteNext = 0;
byte bufferSize = 0;

int MxDutyCycle = 100; // 10 - 255

int MxSensorRead1;
int MxSensorRead2;

String inputString = "";      // Almacena la cadena de entrada desde el puerto serial
bool stringComplete = false;  // Indica si se ha completado la cadena de entrada
bool moveComplete = false; 


void serialEvent() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    Serial.print(inChar);
    if (inChar == '\n') { // Si se recibe un salto de línea, la cadena está completa
      stringComplete = true;
    } else {
      inputString += inChar; // Agregar el carácter a la cadena de entrada
    }
  }
}


void printValues() {
  Serial.print(MxActualPoint);
  Serial.print(", ");
  Serial.print(MxSetPoint);
  Serial.print(", ");
  Serial.print(MxStepDone);  
  Serial.print(", ");
  Serial.println(MxStep);
}


void setup() {

  inputString.reserve(50); // Reservar memoria para la cadena de entrada
  Serial.begin(115200);
  pinMode(S1, INPUT);
  pinMode(S2, INPUT);
  digitalWrite(DIRA, 0);
  digitalWrite(DIRB, 0);
  analogWrite(MOTOR_1_PWM, 0);
  pinMode(DIRA, OUTPUT);
  pinMode(DIRB, OUTPUT);
  pinMode(MOTOR_1_PWM, OUTPUT);

}

void loop() {

   millisStart = millis();
   /*
   if((millisStart - millisEnd)>500){
    millisEnd = millisStart;
    //printValues();
   }*/

    do{
      MxSensorRead1 = digitalRead(S1);
      MxSensorRead2 = digitalRead(S2);

     if(MxSensorRead2 == 0 && MxSensorRead1 == 0){
        if(MxStep == 3){
          MxActualPoint++;
          //printValues();
        }
        if(MxStep == 1){
          MxActualPoint--;
          //printValues();
        }
        MxStep = 0;
      }

      if(MxSensorRead2 == 1 && MxSensorRead1 == 0){
        if(MxStep == 0){
          MxActualPoint++;
          //printValues();
        }
        if(MxStep == 2){
          MxActualPoint--;
          //printValues();
        }
        MxStep = 1;
      }

      if(MxSensorRead2 == 1 && MxSensorRead1 == 1){
        if(MxStep == 1){
          MxActualPoint++;
          //printValues();
        }
        if(MxStep == 3){
          MxActualPoint--;
          //printValues();
        }
        MxStep = 2;
      }

      if(MxSensorRead2 == 0 && MxSensorRead1 == 1){
        if(MxStep == 2){
          MxActualPoint++;
          //printValues();
        }
        if(MxStep == 0){
          MxActualPoint--;
          //printValues();
        }
        MxStep = 3;
      }

      if(abs(MxSetPoint - MxActualPoint) < STEP_MARGIN_X){
              analogWrite(MOTOR_1_PWM, 255);
              digitalWrite(DIRA, 0);
              digitalWrite(DIRB, 0);
              MxStepDone = 1;
              //MxDutyCycle = MIN_DUTYCYCLE_X;
      }
      else{
              if(MxActualPoint < MxSetPoint){
                digitalWrite(DIRA, 0);
                digitalWrite(DIRB, 1);
                analogWrite(MOTOR_1_PWM, MxDutyCycle);
              }
              if(MxActualPoint > MxSetPoint){
                digitalWrite(DIRA, 1);
                digitalWrite(DIRB, 0);
                analogWrite(MOTOR_1_PWM, MxDutyCycle);
              }
      }
    yield();
    }while((MxStepDone == 0) || ((millis() - millisStart) < stepTime));

    if(moveComplete == false){
      moveComplete = true;
      printValues();
    }

    if (stringComplete) {
        MxSetPoint =  inputString.toInt(); // Convertir la cadena a un entero
        MxDutyCycle = MAX_DUTYCYCLE_X;
        MxStepDone = 0;

        inputString = "";         // Limpiar la cadena de entrada
        stringComplete = false;   // Reiniciar el indicador de cadena completa
        moveComplete = false;
    }

}