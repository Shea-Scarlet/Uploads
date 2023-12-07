#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "wiringPi.h"
#include <unistd.h>
#include <ctype.h>

#define LED_GPIO_PIN 26
#define SPACE_DELAY 500000
#define DOT_DELAY 200000
#define DASH_DELAY 600000

// Enumerating indices for letters and digits in Morse code array
enum MorseIndices {
    A = 0, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
    ZERO = 26, ONE, TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE
};

// Morse Code Mapping to the English Alphabet and Digits
const char *MORSE_CODE[] = {
    ".-", "-...", "-.-.", "-..", ".", "..-.", "--.", "....", "..", ".---", "-.-", ".-..", "--", "-.", "---", ".--.", "--.-", ".-.", "...", "-", "..-", "...-", ".--", "-..-", "-.--", "--..",
    "-----", ".----", "..---", "...--", "....-", ".....", "-....", "--...", "---..", "----."
};

// Transmit Morse code for a single character
void transmitMorseChar(char character) {
    character = toupper(character);
    if (character == ' ') {
        // If the character is a space, delay accordingly
        usleep(SPACE_DELAY);
    } else {
        int index;
        if (isdigit(character)) {
            // Offset for digits
            index = character - '0' + ZERO;
        } else {
            // Offset for letters
            index = character - 'A';
        }

        if (index >= 0 && index < 36) {
            const char *morseCode = MORSE_CODE[index];
            while (*morseCode) {
                // Transmit Morse code
                if (*morseCode == '.') {
                    digitalWrite(LED_GPIO_PIN, HIGH);
                    usleep(DOT_DELAY);
                } else if (*morseCode == '-') {
                    digitalWrite(LED_GPIO_PIN, HIGH);
                    usleep(DASH_DELAY);
                }
                // Turn off LED after Morse code element
                digitalWrite(LED_GPIO_PIN, LOW);
                usleep(DOT_DELAY);
                morseCode++;
            }
            // Delay between characters
            usleep(DASH_DELAY);
        }
    }
}

// Transmit Morse code for an entire message
void transmitMorseMessage(const char *message) {
    int messageIndex = 0;
    while (message[messageIndex] != '\0') {
        // Transmit Morse code for each character in the message
        transmitMorseChar(message[messageIndex]);
        messageIndex++;
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        // Print usage information if the command-line arguments are not correct
        fprintf(stderr, "Usage: %s <num_of_times_to_print_message> <message>\n", argv[0]);
        return 1;
    }

    if (wiringPiSetupGpio() == -1) {
        // Print error message if wiringPi initialization fails
        fprintf(stderr, "Unable to initialize wiringPi\n");
        return -1;
    }

    // Set the LED GPIO pin as an output pin
    pinMode(LED_GPIO_PIN, OUTPUT);

    // Parse command-line arguments
    int numberOfTimes = atoi(argv[1]);
    const char *message = argv[2];

    // Transmit the Morse code message multiple times
    int i = 0;
    while (i < numberOfTimes) {
        transmitMorseMessage(message);
        i++;
    }

    // Turn off the LED at the end
    digitalWrite(LED_GPIO_PIN, LOW);
    return 0;
}
