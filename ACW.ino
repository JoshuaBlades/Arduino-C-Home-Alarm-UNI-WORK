**********************************
Electronics ACW by 514380 & 514413
**********************************
#include <IRremote.h>
#include <IRremoteInt.h>
#include <LiquidCrystal.h>
#include <EEPROM.h>
////////////////////////////////////////
//These variables are for the IR receiver
#define IRpin_PIN PIND
#define IRpin 4
//Pins for LCD Panel
LiquidCrystal lcd(13, 12, 11, 10, 9, 8);
//Pin for the IR receiver
IRrecv IR(IRpin);
//Values from the IR input
decode_results results;
//counter for the press of a button on the remote
uint8_t remotepress = 1;
//These are the towo code storing arrays.
//The default one is the current active passcode.
//The attempted one is the one we're checking against the
//default passcode with.
////////////////////////////////////////
uint8_t defaultPasscode[4] {0, 0, 0, 0};
uint8_t attemptPasscode[4] { };
////////////////////////////////////////
//These variables are for the button press arm and disarm
bool lastButtonPress;
//Boolean value for if someone has pressed the button
int buttonState = 0;
//The state we use to check the current state to the last one
int lastButtonState = 0;
//The last button state variable we use to check against the current state
//If it is different then the state hs changed. We don't need to know what it
//has changed to, just need to know it has changed
int counter = 1;
//The counter we use to change the state of the LED
//Variables we use to store the differet LED colours.
////////////////////////////////////////
int ledB = 3;
int ledG = 6;
int ledR = 5;
////////////////////////////////////////
//Variables for the motion sensor
int pir = 7;
int pirState = LOW;
int pirStatus = 0;
////////////////////////////////////////
//Variable for the buzzer
int buzzer = 18;
//LED pin number
int button = 2;
//Button pin number
bool armed = false;
//Bool to check if the alarm is armed
////////////////////////////////////////////////////////
//All the code which will run once at the start,
//including the passcode load from the EEPROM.
void setup(void) {
  // put your setup code here, to run once:
  pinMode(ledG, OUTPUT);
  pinMode(ledB, OUTPUT);
  pinMode(ledR, OUTPUT);
  pinMode(pir, INPUT);
  pinMode(buzzer, OUTPUT);
  Serial.begin(9600);
  IR.enableIRIn();
  lcd.begin(16, 2);
  lcd.print("Enter Passcode");
  setColor(0, 255, 0);
  if (EEPROM.read (0) != 0xff)
  {
    for (int i = 0; i < 4; i++)
    {
      defaultPasscode[i] = EEPROM.read(i);
    }
  }
}
/////////////////////////////////////////////////////////////
//This is the method we use to save the passcode
//to the EEPROM.
void SavePasscode()
{
  for (int i = 0; i < 4; i++)
  {
    EEPROM.write(i, defaultPasscode[i]);
  }
}

/////////////////////////////////////////////////////////////
//This method actives the LED and the buzzer
//if the alarm has been rasied. It also listens out for
//the disarm command on the IR remote.
void activeLED()
{
  while (armed == true)
  {
    setColor(255, 0, 0);
    tone(buzzer, 700, 500);
    delay(1000); lcd.clear();
    lcd.print("DETECTED!");
    setColor(0, 0, 0);
    tone(buzzer, 700, 500);
    delay(1000);


    if (IR.decode(&results))
    {
      if (results.value == 16769565)
      {
        pirState = LOW;
        pirStatus = 0;
        deactivate();
      }
      delay(100);
      IR.resume();
    }
  }
}

/////////////////////////////////////////////////////////////
//This method converts the results number from the IR
//to one we can understand from 0-9.
int Passcode(uint32_t convert)
{
  switch (convert)
  {
    case 16738455:
      return 0;
      break;

    case 16724175:

      return 1;
      break;

    case 16718055:

      return 2;
      break;

    case 16743045:

      return 3;
      break;

    case 16716015:

      return 4;
      break;

    case 16726215:

      return 5;
      break;

    case 16734885:

      return 6;
      break;

    case 16728765:

      return 7;
      break;

    case 16730805:

      return 8;
      break;

    case 16732845:

      return 9;
      break;

    default:
      //Reject button presses that
      //are not 0-9.
      return 10;
      break;
  }
}
/////////////////////////////////////////////////////////////
//This allowes us to change the passcode.
void changePasscode()
{
  int buttonPressValueChange = 0;
  int buttonPress;
  int correctNumberPressed = 0;
  lcd.clear();
  while (buttonPressValueChange < 5) {
    if (IR.decode(&results))
    {
      buttonPressValueChange++;

      if (results.value != 16736925)
      {
        if (Passcode(results.value) != 10)
        {
          Serial.println(Passcode(results.value));
          lcd.print(Passcode(results.value));
          defaultPasscode[correctNumberPressed] = Passcode(results.value);
          correctNumberPressed++;
        }
        else
        {
          buttonPressValueChange--;
        }
      }
      delay(100);
      IR.resume();
    }
  }
}
/////////////////////////////////////////////////////////////
//This method makes sure that the entered pascode is the correct one.
//Anything else will be rejected.
bool validatePasscode()
{
  int correctElementCounter = 0;
  for (int i = 0; i < 4; i++)
  {
    if (defaultPasscode[i] == attemptPasscode[i])
    {
      correctElementCounter++;
    }
    else
    {
      Serial.println("Passcode Invalid");
      clearAttemptPasscode();
      return false;
      break;
    }
  }

  if (correctElementCounter == 4)
  {
    Serial.println("Passcode Valid!");
    clearAttemptPasscode();
    return true;
  }
}
/////////////////////////////////////////////////////////////
//This method clears the attempted passcode, ready for the
//next attempt.
void clearAttemptPasscode()
{
  for (int i = 0; i < 4; i++)
  {
    attemptPasscode[i] = 0;
  }
}
/////////////////////////////////////////////////////////////
//This populates the attempt passcode array, ready
//to check it against the current in-use passcode.
void checkPasscode()
{
  int buttonPressValueChange = 0;
  int buttonPress;
  int correctNumberPressed = 0;
  lcd.clear();

  while (buttonPressValueChange < 5) {
    if (IR.decode(&results))
    {
      buttonPressValueChange++;
      if (results.value != 16753245 && results.value != 16769565)
      {
        if (Passcode(results.value) != 10)
        {
          Serial.println(Passcode(results.value));
          lcd.print(Passcode(results.value));
          attemptPasscode[correctNumberPressed] = Passcode(results.value);
          correctNumberPressed++;
        }
        else
        {
          buttonPressValueChange--;
        }
      }
      delay(100);
      IR.resume();
    }
  }
}
/////////////////////////////////////////////////////////////
//This allowes us to change the colour of the LED.
void setColor(int red, int green, int blue)
{
  analogWrite(ledR, red);
  analogWrite(ledG, green);
  analogWrite(ledB, blue);
}
/////////////////////////////////////////////////////////////
//This method activates the device, from idle to armed.
void activate()
{
  checkPasscode();
  if (validatePasscode() == true)
  {
    Serial.println("DEVICE ARMING...");
    armed = true;
    setColor(255, 0, 0);
    if (armed == true)
    {
      lcd.clear();
      lcd.print("DEVICE ARMED!");
    }
    else
    {
      lcd.clear();
      lcd.print("WRONG PASSCODE!");
    }
  }
}
/////////////////////////////////////////////////////////////
//This method deactivates the device, from armed/raised to idle.
void deactivate()
{
  checkPasscode();
  if (validatePasscode() == true)
  {
    Serial.println("DEVICE DISARMING...");
    armed = false;
    setColor(0, 255, 0);
    lcd.clear();
  }
  else
  {
    lcd.clear();
    lcd.print("WRONG PASSCODE!");
  }
}
/////////////////////////////////////////////////////////////
//This is the main loop which will run all of the code, and
//listen out for an IR signal or a button press. Only if the
//device is armed will it listen to the motion sensor state
//changes.
void loop(void) {
  // put your main code here, to run repeatedly:
  pirStatus = digitalRead(pir);
  buttonState = digitalRead(button);
  if (IR.decode(&results))
  {
    if (results.value == 16753245)
    {
      Serial.println("DEVICE ARMING...");
      activate();
      if (armed == true)
      {
        Serial.println("DEVICE ARMED!");
        lcd.clear();
        lcd.print("DEVICE ARMED!");
      }
      else
      {
        lcd.clear();
        lcd.print("WRONG PASSCODE!");
        Serial.println("WRONG PASSCODE!");
        delay(2000);
        lcd.clear();
        lcd.print("Enter passcode");
      }
    }
    else if (results.value == 16769565)
    {
      Serial.println("DEVICE DISARMING...");
      deactivate();
      if (armed == false)
      {
        Serial.println("DEVICE DISARMED!");
        lcd.clear();
        lcd.print("DEVICE DISARMED!");
        delay(2000);
        lcd.clear();
        lcd.print("Enter passcode");
      }
      else
      {
        lcd.clear();
        lcd.print("WRONG PASSCODE!");
        Serial.println("WRONG PASSCODE!");
        delay(2000);
        lcd.clear();
        lcd.print("DEVICE ARMED!");
      }
    }
    else if (results.value == 16736925)
    {
      if (armed == false)
      {
        Serial.println("Chaning passcode...");
        lcd.clear();
        lcd.print("Changing passcode...");
        setColor(0, 0, 255);
        changePasscode();
        Serial.println("Passcode changed successfully");
        SavePasscode();
        lcd.clear();
        lcd.print("Passcode changed!");
        setColor(0, 255, 0);
        delay(2000);
        lcd.clear();
        lcd.print("Enter passcode");
      }
    }
    delay(100);
    IR.resume();
  }

  if (armed == true)
  {
    if (pirStatus == HIGH)
    {
      if (pirState == LOW)
      {
        Serial.println("INTRUDER DETECTED!");
        activeLED();
        lcd.clear();
        lcd.print("DEVICE DISARMED!");
        delay(2000);
        lcd.clear();
        lcd.print("Enter passcode");
      }
    }
  }
  if (armed == false)
  {
    if (buttonState == lastButtonState)
    {
      armed = true;
      setColor(255, 0, 0);
      Serial.println("DEVICE ARMED!");
      lcd.clear();
      lcd.print("DEVICE ARMED!");
    }
    buttonState = lastButtonState;
  }
}
