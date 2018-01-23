#include <SoftwareSerial.h>  

// To do wireless update over bluetooth, connect to robot with bluetooth, set baud rate to 19200,
// then upload code and finally do a hardware reset(can be triggered soely by software)

int bluetoothTx = 2;  // TX-O pin of bluetooth mate, Arduino D2
int bluetoothRx = 3;  // RX-I pin of bluetooth mate, Arduino D3

#define pwm_2 5 
#define pwm_1 6 
#define dir_2 4 
#define dir_1 7



SoftwareSerial bluetooth(bluetoothTx, bluetoothRx);

struct BtMessage {
    char type[32];
    char speed[32];
};

struct BtMessage parseMessage(char message[])
{
    struct BtMessage btMessage;
    strcpy(btMessage.type, strtok(message, "_"));
    
    if(strcmp(btMessage.type, NULL) != 0){
        strcpy(btMessage.speed, strtok(NULL, "_"));
    }
    else
    {
        strcpy(btMessage.speed, "255");
    }

    return btMessage;
}

void setup() {
    Serial.begin(9600);  // Begin the serial monitor at 9600bps
    bluetooth.begin(115200);  // The Bluetooth Mate defaults to 115200bps
    bluetooth.print("$");  // Print three times individually
    bluetooth.print("$");
    bluetooth.print("$");  // Enter command mode
    delay(100);  // Short delay, wait for the Mate to send back CMD
    bluetooth.println("U,9600,N");  // Temporarily Change the baudrate to 9600, no parity
    // 115200 can be too fast at times for NewSoftSerial to relay the data reliably
    bluetooth.begin(9600);  // Start bluetooth serial at 9600

    pinMode(pwm_2, OUTPUT);
    pinMode(pwm_1, OUTPUT);
    pinMode(dir_1, OUTPUT);
    pinMode(dir_2, OUTPUT);
}

// Limit max string size to 32 char
void getMessage(char text[32])
{
    unsigned i=0;      /*array index variable */
    /*character is received until end of the string received */

    char letter = (char)bluetooth.read();
    text[0] = letter;
    i++;
    
    while(letter != '\0' && i<30)
    {
        if(bluetooth.available()){
            letter = (char)bluetooth.read();
            text[i]=letter;
            i++;
        }
    }
    text[i] = '\0';
    //return text;   /* return the base address of received string */
}


void loop() {
    
     if(bluetooth.available())  // If the bluetooth sent any characters
     {
        char message[32];
        getMessage(message);
        Serial.println("type is:");
        Serial.println(message);
        
        struct BtMessage btMessage = parseMessage(message);
        unsigned speed  = 255;
        char *ptr;
        
       
        if(strcmp(btMessage.type, "S") != 0){
            speed = strtoul(btMessage.speed, &ptr, 10);
            
        }

        
        
        if(strcmp(btMessage.type, "U") == 0){ //UP
            digitalWrite(dir_1,HIGH);
            digitalWrite(dir_2, HIGH);
            analogWrite(pwm_1, speed);
            analogWrite(pwm_2, speed);
        }else if(strcmp(btMessage.type, "S") == 0){ //STOP
            analogWrite(pwm_1,0);
            analogWrite(pwm_2,0);
        }else if(strcmp(btMessage.type, "L") == 0){ //LEFT SPIN
            digitalWrite(dir_1,LOW);
            digitalWrite(dir_2,HIGH);
            analogWrite(pwm_1, speed);
            analogWrite(pwm_2,speed);
        }
        else if(strcmp(btMessage.type, "R") == 0){ // RIGHT SPIN
            digitalWrite(dir_1,HIGH);
            digitalWrite(dir_2,LOW);
            analogWrite(pwm_1, speed);
            analogWrite(pwm_2, speed);
        } else if(strcmp(btMessage.type, "B") == 0){ //Backwards
            //Serial.print("BACKWARDS");
            digitalWrite(dir_1,LOW);
            digitalWrite(dir_2,LOW);
            analogWrite(pwm_1, speed);
            analogWrite(pwm_2, speed);
        }else if(strcmp(btMessage.type, "1") == 0){ // top left
            digitalWrite(dir_1,HIGH);
            //digitalWrite(dir_2,LOW);
            analogWrite(pwm_1, speed);
            analogWrite(pwm_2, 0);
        }else if(strcmp(btMessage.type, "2") == 0){ // top right
            //digitalWrite(dir_1,LOW);
            digitalWrite(dir_2,HIGH);
            analogWrite(pwm_2, 0);
            analogWrite(pwm_2, speed);
        }else if(strcmp(btMessage.type, "3") == 0){ // bottom left
            digitalWrite(dir_1,LOW);
            //digitalWrite(dir_2,LOW);
            analogWrite(pwm_1,speed);
            analogWrite(pwm_2,0);
        }else if(strcmp(btMessage.type, "4") == 0){ // bottom right
            //digitalWrite(pwm_1,LOW);
            digitalWrite(pwm_2,LOW);
            analogWrite(pwm_1,0);
            analogWrite(pwm_2,speed);
        }
    }
}

