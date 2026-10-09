#include <SPI.h>
#include <MFRC522.h>
#include <Keypad.h>
#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define SS_PIN 10
#define RST_PIN 9

MFRC522 mfrc522(SS_PIN, RST_PIN);

Servo lockServo;
LiquidCrystal_I2C lcd(0x27,16,2);

// Servo & Outputs
#define SERVO_PIN 6
#define RED_LED 8
#define GREEN_LED 7
#define BUZZER A0

// Keypad
const byte ROWS = 4;
const byte COLS = 4;

char keys[ROWS][COLS]={
{'1','2','3','A'},
{'4','5','6','B'},
{'7','8','9','C'},
{'*','0','#','D'}
};

byte rowPins[ROWS]={A1,A2,A3,0}; 
byte colPins[COLS]={2,3,4,5};    

Keypad keypad = Keypad(makeKeymap(keys),rowPins,colPins,ROWS,COLS);

String password="1234";
String inputPassword="";

byte validUID[4]={0x01,0x02,0x03,0x04};   // Replace with your RFID UID

void setup()
{
    Serial.begin(9600);

    SPI.begin();
    mfrc522.PCD_Init();

    lcd.init();
    lcd.backlight();

    pinMode(RED_LED,OUTPUT);
    pinMode(GREEN_LED,OUTPUT);
    pinMode(BUZZER,OUTPUT);

    lockServo.attach(SERVO_PIN);
    lockServo.write(0);
    Serial.println("Servo attached on pin " + String(SERVO_PIN) + ", set to 0");

    digitalWrite(RED_LED,HIGH);

    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("SMART DOOR");
    lcd.setCursor(0,1);
    lcd.print("LOCK SYSTEM");
    delay(2000);

    lcd.clear();
}

void loop()
{
    lcd.setCursor(0,0);
    lcd.print("Who goes there?");
    checkRFID();
    checkPassword();
}

void checkRFID()
{
    if(!mfrc522.PICC_IsNewCardPresent()) return;
    if(!mfrc522.PICC_ReadCardSerial()) return;

    bool access=true;

    for(byte i=0;i<4;i++)
    {
        if(mfrc522.uid.uidByte[i]!=validUID[i])
        access=false;
    }

    if(access)
    {
        lcd.clear();
        lcd.print("RFID Accepted");
        unlockDoor();
    }
    else
    {
        lcd.clear();
        lcd.print("YOU SHALL NOT");
        lcd.setCursor(0,1);
        lcd.print("PASS!");
        denied();
    }

    mfrc522.PICC_HaltA();     
    mfrc522.PCD_StopCrypto1();  

    delay(1500);
    lcd.clear();
}

void checkPassword()
{
    char key=keypad.getKey();

    if(key)
    {
        Serial.print("Key pressed: ");
        Serial.println(key);

        if(key=='#')
        {
            Serial.println("Submitted: [" + inputPassword + "]  Expected: [" + password + "]");

            if(inputPassword==password)
            {
                lcd.clear();
                lcd.print("Correct");
                Serial.println("Password match -> unlocking");
                unlockDoor();
            }
            else
            {
                lcd.clear();
                lcd.print("Wrong Password");
                denied();
            }

            inputPassword="";
            delay(1500);
            lcd.clear();
        }

        else if(key=='*')
        {
            inputPassword="";
            lcd.clear();
        }

        else
        {
            inputPassword+=key;
            lcd.setCursor(0,1);
            for(int i=0;i<inputPassword.length();i++)
            lcd.print("*");
        }
    }
}

void unlockDoor()
{
    digitalWrite(RED_LED,LOW);
    digitalWrite(GREEN_LED,HIGH);

    tone(BUZZER,1000,200);

    Serial.println("Writing servo angle 90");
    lockServo.write(90);

    lcd.clear();
    lcd.print("Enter, mortal!");

    delay(5000);

    lockServo.write(0);

    digitalWrite(GREEN_LED,LOW);
    digitalWrite(RED_LED,HIGH);

    lcd.clear();
    lcd.print("Door Locked");
    delay(1500);

    lcd.clear();
}

void denied()
{
    digitalWrite(GREEN_LED,LOW);
    digitalWrite(RED_LED,HIGH);

    for(int i=0;i<3;i++)
    {
        tone(BUZZER,500);
        delay(250);
        noTone(BUZZER);
        delay(250);
    }
}
