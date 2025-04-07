#include <WiFi.h>
#include <HTTPClient.h>
#include <HTTPUpdate.h>
#include "ota_update.h"
#include "ota_config.h"

bool check_and_update_firmware(const char* localVersion) {
  HTTPClient http;
  http.begin(VERSION_URL);

  int httpCode = http.GET();
  if (httpCode != 200) {
    Serial.printf("Error al obtener la versión remota: %d\n", httpCode);
    http.end();
    return false;
  }

  String remoteVersion = http.getString();
  remoteVersion.trim();
  Serial.printf("Versión local: %s | Versión remota: %s\n", localVersion, remoteVersion.c_str());

  if (remoteVersion != String(localVersion)) {
    http.end();

    // Construir URL dinámica del bin
    String fullBinUrl = String(BASE_BIN_URL) + "firmware_v" + remoteVersion + ".bin";
    Serial.println("Descargando firmware desde: " + fullBinUrl);

    WiFiClient client;
    t_httpUpdate_return ret = httpUpdate.update(client, fullBinUrl);

    if (ret == HTTP_UPDATE_OK) {
      Serial.println("Actualización OTA exitosa.");
      return true;
    } else {
      Serial.printf("Error OTA: %d - %s\n", httpUpdate.getLastError(), httpUpdate.getLastErrorString().c_str());
      return false;
    }
  } else {
    Serial.println("Firmware ya actualizado.");
    http.end();
    return false;
  }
}
