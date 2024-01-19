#include <key_pad.h>
#include "./../config.h"

//char obtained_pass_code[6];
char obtained_pass_code[PASS_NUM_LENGTH];
const byte ROWS = 4; //four rows
const byte COLS = 3; //four columns
//define the cymbols on the buttons of the keypads
char hexaKeys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};
byte rowPins[ROWS] = {32, 33, 34, 35};  //connect to the row pinouts of the keypad
byte colPins[COLS] = {25, 26, 27}; //connect to the column pinouts of the keypad
Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 

bool get_user_passcode()
{
    int num_of_keys = 0;
    char code_init[2];

    char key = '\0';
    while(key == '\0'){
       // key = customKeypad.getKey();
       if(Serial.available() > 0)
       {
            key = Serial.read();
            Serial.println(key);
       }
    }
    if(key == '*') 
    {
        key = '\0';
        while(key == '\0')
        {
          //  key = customKeypad.getKey();
            if(Serial.available() > 0)
            {
                key = Serial.read();
            }
        }
        Serial.println(key);
        if(key == '#')
        {
                while(num_of_keys < 6)
                {
                   // key = customKeypad.getKey();
                   if(Serial.available() > 0)
                   {
                        key = Serial.read();
                        if(key != '\0')
                        {
                            obtained_pass_code[num_of_keys] = key;
                            Serial.println(key);
                            num_of_keys++;
                        }
                    }
                }
                return true;
        }
        else
            return false;
    }
    else
        return false;

}
bool get_password_from_keypad(void)
{
    char key = '\0';
    static int num_of_keys = 0;

    if(Serial.available() > 0)
       {
            key = Serial.read();
            Serial.println(key);
       }
    // key = customKeypad.getKey();

    if(key != '\0')
    {
        switch(num_of_keys)
        {
            case 0:
                if(key == '*')
                {
                    num_of_keys++;
                }
                else
                    Serial.println("Enter password starting with *#");
            break;
            case 1:
                if(key == '#')
                {
                    num_of_keys++;
                }
                else
                    Serial.println("Enter password starting with *#");
            break;
            default:
                if(num_of_keys < (PASS_NUM_LENGTH + 1))  //6 digit passcode, 2 digit *# - not considering null char here
                {
                  //  num_of_keys++;
                    Serial.print("num_of_keys: ");
                    Serial.println(num_of_keys);
                    obtained_pass_code[num_of_keys - 2] = key;
                    if(num_of_keys >= PASS_NUM_LENGTH)   // (0 - 7)  get 8 digits from keypad
                    {
                        num_of_keys = 0;
                        obtained_pass_code[PASS_NUM_LENGTH - 1] = '\0';
                        return true;
                    }    
                    num_of_keys++;    
                }

            break;
        }
    }
    return false;
}
