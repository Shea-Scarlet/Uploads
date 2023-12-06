#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "wiringPi.h"
#include <unistd.h>
#include <ctype.h>

#define LED_GPIO_PIN 26

// Morse Code Mapping to the English Alphabet and Digits
const char *MORSE_CODE[] = {
    ".-",    // A
    "-...",  // B
    "-.-.",  // C
    "-..",   // D
    ".",     // E
    "..-.",  // F
    "--.",   // G
    "....",  // H
    "..",    // I
    ".---",  // J
    "-.-",   // K
    ".-..",  // L
    "--",    // M
    "-.",    // N
    "---",   // O
    ".--.",  // P
    "--.-",  // Q
    ".-.",   // R
    "...",   // S
    "-",     // T
    "..-",   // U
    "...-",  // V
    ".--",   // W
    "-..-",  // X
    "-.--",  // Y
    "--..",  // Z
    "-----", // 0
    ".----", // 1
    "..---", // 2
    "...--", // 3
    "....-", // 4
    ".....", // 5
    "-....", // 6
    "--...", // 7
    "---..", // 8
    "----."  // 9
};

void transmitMorseChar(char character){
    character = toupper(character);
    if (character == ' '){
        usleep(500000);
    }
    else{
        int index;
        if (isdigit(character)){
            index = character - '0' + 26; // Offset for digits
        }

        else{
            index = character - 'A'; // Offset for letters
        }

        if (index >= 0 && index < 36){
            const char *morseCode = MORSE_CODE[index];
            while (*morseCode){
                if (*morseCode == '.'){
                    digitalWrite(LED_GPIO_PIN, HIGH);
                    usleep(200000);
                }

                else if (*morseCode == '-'){
                    digitalWrite(LED_GPIO_PIN, HIGH);
                    usleep(600000);}

                digitalWrite(LED_GPIO_PIN, LOW);
                usleep(200000);
                morseCode++;
            }
            usleep(400000);
        }
    }
}
   void transmitMorseMessage(const char *message){
    int i = 0;
    while (message[i] != '\0'){
        transmitMorseChar(message[i]);
        i++;
    }
}

int main(int argc, char *argv[]){
    if (argc != 3){
        printf("Usage: %s <num_of_times_to_print_message> <message>\n", argv[0]);
        return 1;
    }

    if (wiringPiSetupGpio() == -1){
        fprintf(stderr, "Unable to initialize wiringPi\n");
        return -1;
    }

    pinMode(LED_GPIO_PIN, OUTPUT);
    int numberOfTimes = atoi(argv[1]);
    const char *message = argv[2];
    int i = 0;
    while (i < numberOfTimes){
        transmitMorseMessage(message);
        i++;
    }
    digitalWrite(LED_GPIO_PIN, LOW);
    return 0;
}
