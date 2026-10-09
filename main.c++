#include <Keypad.h>
#include <Servo.h>

// Servo & Outputs
#define SERVO_PIN 6
#define RED_LED 8

Servo lockServo;

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

void setup()
{
    Serial.begin(9600);

    pinMode(RED_LED,OUTPUT);

    lockServo.attach(SERVO_PIN);
    lockServo.write(0);
    Serial.println("Servo attached on pin " + String(SERVO_PIN) + ", set to 0");

    digitalWrite(RED_LED,HIGH);
}

void loop()
{
    checkPassword();
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
                Serial.println("Password match -> unlocking");
                unlockDoor();
            }
            else
            {
                digitalWrite(RED_LED,HIGH);
                Serial.println("Wrong password");
            }

            inputPassword="";
        }
        else if(key=='*')
        {
            inputPassword="";
            Serial.println("Input cleared");
        }
        else
        {
            inputPassword+=key;
            Serial.print("*");
        }
    }
}

void unlockDoor()
{
    Serial.println("Writing servo angle 90");
    lockServo.write(90);
}
