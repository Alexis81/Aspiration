// Bouton poussoir : https://github.com/pololu/pushbutton-arduino
// Gestion moteur pas à pas :


#include "Ticker.h"
#include <AccelStepper.h>
#include <PushButton.h>
#include <avr/wdt.h>

# define enabled 10
# define pasScie 0
# define pasDegau 1068
# define pasRuban 2136

// Déclaration des constantes
const long maxSpeed = 4000;
const long setSpedd = 3000;
const long setAcceleration = 1200;
const byte ledPin = 13;
const byte interruptPin = 3;
const byte pinStep = 9;
const byte pinDir = 8;

// Déclaration des variables
long initialHoming = -1;
int lecture = 0;
bool ledState;

// Déclaration des fonctions
void blinkLed();
void resetPoition();
void scie();
void degau();
void bande();
void software_Reboot();

//AccelStepper stepper(AccelStepper::DRIVER, stepper_pin_step, stepper_pin_dir);
AccelStepper stepper(AccelStepper::DRIVER, pinStep, pinDir);

Ticker timer1(blinkLed, 500);

// Déclare le brochage des boutons
PushButton button_Scie(11);
PushButton button_Degau(12);
PushButton button_Bande(7);


void setup()
{
  // Init de la liaison série
  Serial.begin(115200);

  Serial.println("*******************************************************");
  Serial.println("              Gestion Aspiration V1");
  Serial.println("");
  Serial.println("               By : Alexis OLIVAN");
  Serial.println("*******************************************************");
  Serial.println("");
  Serial.println("Init de la carte");

  // Initialise les pin
  pinMode(ledPin, OUTPUT);
  pinMode(interruptPin, INPUT);
  pinMode(11, INPUT_PULLUP);
  pinMode(12, INPUT_PULLUP);
  pinMode(7, INPUT_PULLUP);
  pinMode(enabled, OUTPUT);

  // Lance le timer pour faire clignoter la led
  timer1.start();



  // Définir les paramètres du moteur
  stepper.setMaxSpeed(maxSpeed);
  stepper.setAcceleration(setAcceleration);
  stepper.setSpeed(setSpedd);

  // Cherche le hall sensor
  resetPoition();

}

void loop()
{
  // Mettre à jour l'état des boutons
  button_Scie.update();
  button_Degau.update();
  button_Bande.update();

  // Permet de mettre à jour le timer1 pour faire clignoter la led
  timer1.update();


  // Test l'état des boutons
  if (button_Scie.isDoubleClicked()) {
      timer1.start();
       resetPoition();
     }

  if (button_Scie.isReleased()) {
     timer1.start();
     scie();
  }

   if (button_Degau.isReleased()) {
        timer1.start();
        degau();
      }

  if (button_Bande.isReleased()) {
       timer1.start();
       bande();
  }

  if (button_Bande.isDoubleClicked()) {
    Serial.println("!!!! Reboot device...");
       software_Reboot();
  }


}

//-- Fonction pour aller à la position de la scie
void scie() {
  Serial.println("Ouverture de la scie à format...");
  digitalWrite(enabled, LOW);
  stepper.enableOutputs();
  stepper.setSpeed(setSpedd);
  stepper.moveTo(pasScie);
  while (stepper.currentPosition() != stepper.targetPosition()) {
    timer1.update();
    stepper.run();
}

stepper.disableOutputs();
timer1.stop();
digitalWrite(ledPin, false);
digitalWrite(enabled, HIGH);

}

//-- Fonction pour aller à la position de la dégauchisseuse
void degau() {
  Serial.println("Ouverture de la dégauchisseuse...");
  digitalWrite(enabled, LOW);
  stepper.enableOutputs();
  stepper.setSpeed(setSpedd);
  stepper.moveTo(pasDegau);
  while (stepper.currentPosition() != stepper.targetPosition()) {
    timer1.update();
    stepper.run();
  }

  stepper.disableOutputs();
  timer1.stop();
  digitalWrite(ledPin, false);
  digitalWrite(enabled, HIGH);

}

//-- Fonction pour aller à la position de la scie à ruban
void bande() {
  Serial.println("Ouverture de la scie à ruban...");
  digitalWrite(enabled, LOW);
  stepper.enableOutputs();
  stepper.setSpeed(setSpedd);
  stepper.moveTo(pasRuban);
  while (stepper.currentPosition() != stepper.targetPosition()) {
    timer1.update();
    stepper.run();
  }

  stepper.disableOutputs();
  timer1.stop();
  digitalWrite(ledPin, false);
  digitalWrite(enabled, HIGH);

}

// Permet de faire clignoter la led
void blinkLed() {
  digitalWrite(ledPin, ledState);
  ledState = !ledState;
}

// Permet de faire le initialHoming
void resetPoition() {
  Serial.println("Recherche du homing...");
  digitalWrite(enabled, LOW);
  stepper.enableOutputs();
  stepper.setSpeed(setSpedd);
  stepper.setCurrentPosition(0);

  while (digitalRead(interruptPin)) {
    timer1.update();
    stepper.moveTo(50000);
    stepper.run();
  }

  timer1.stop();
  digitalWrite(ledPin, false);

  // Reset the position to 0:
  stepper.setCurrentPosition(0);
  stepper.disableOutputs();
  digitalWrite(enabled, HIGH);
}

// Permet de faire un softReset de l'arduino
void software_Reboot()
{
  wdt_enable(WDTO_15MS);

  while(1)
  {

  }
}
