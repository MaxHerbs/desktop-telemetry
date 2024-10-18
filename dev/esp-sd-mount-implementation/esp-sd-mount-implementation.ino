#include "FS.h"
#include "SD.h"
#include "SPI.h"


#define sdCsPin 5
#define max_str_len 4096


void setup() {
  Serial.begin(115200);
  if (!SD.begin(sdCsPin)) {
    Serial.println("Card Mount Failed");
    return;
  }
  uint8_t cardType = SD.cardType();

  if (cardType == CARD_NONE) {
    Serial.println("No SD card attached");
    return;
  }

  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  Serial.printf("SD Card Size: %lluMB\n", cardSize);



  char* content = (char*)malloc(sizeof(char) * (max_str_len + 1));

  readFile(SD, "/test.txt", content);

  Serial.println("This is the file...");
  Serial.println(content);
  Serial.println("Done!");
  free(content);
}

void loop() {
  // put your main code here, to run repeatedly:

}


int readFile(fs::FS &fs, const char *path, char* content) {
    Serial.printf("Reading file: %s\n", path);

    File file = fs.open(path);
    if (!file) {
        Serial.println("Failed to open file for reading");
        return 1;
    }


    
    // Initialize fileStream with null characters
    memset(content, 0, max_str_len + 1);
    
    // Read the file content into fileStream
    size_t bytesRead = file.readBytes(content, max_str_len);
    content[bytesRead] = '\0'; // Null-terminate the string


    file.close();
    return 0;
}
