#include <EEPROM.h>

#define STRING_ADDRESS 0  // Starting address for string in EEPROM
#define MAX_STRING_LENGTH 42 // Maximum allowed string length

char myString[MAX_STRING_LENGTH];

void setup() {
  Serial.begin(115200);
  
  // Initialize EEPROM with the maximum string length
  if (!EEPROM.begin(MAX_STRING_LENGTH)) {
    Serial.println("Failed to initialize EEPROM");
    while (1); // Halt if EEPROM initialization fails
  }

  // Read existing string from EEPROM (optional)
  readStringFromEEPROM();
}

void loop() {
  // Example string to write
  String dataToWrite = "Hello from ESP32!                        ";

  // Check if string length exceeds limit
  if (dataToWrite.length() > MAX_STRING_LENGTH - 1) {
    Serial.println("String too long for EEPROM!");
    return;
  }

  // Convert string to null-terminated character array
  dataToWrite.toCharArray(myString, MAX_STRING_LENGTH);

  // Write string to EEPROM character by character
  for (int i = 0; i < dataToWrite.length(); i++) {
    EEPROM.write(STRING_ADDRESS + i, myString[i]);
  }

  // Commit the write operation (important!)
  EEPROM.commit();
  Serial.println("String written to EEPROM");
  delay(5000);

  // Read string from EEPROM and print it
  readStringFromEEPROM();
  delay(5000);
}

void readStringFromEEPROM() {
  char tempString[MAX_STRING_LENGTH];

  // Read characters from EEPROM
  for (int i = 0; i < MAX_STRING_LENGTH; i++) {
    tempString[i] = EEPROM.read(STRING_ADDRESS + i);
    
    // Check for null terminator (end of string)
    if (tempString[i] == '\0') {
      break;
    }
  }

  // Print the retrieved string
  Serial.print("String read from EEPROM: ");
  Serial.println(tempString);
}
