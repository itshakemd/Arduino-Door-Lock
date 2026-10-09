#include <Keypad.h>

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
            }
            else
            {
                Serial.println("Wrong password");
            }

            inputPassword="";
        }
        else
        {
            inputPassword+=key;
        }
    }
}
