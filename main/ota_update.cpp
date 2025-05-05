#include <WiFi.h>
#include <HTTPClient.h>
#include <HTTPUpdate.h>
#include "ota_update.h"

bool check_and_update_firmware(const char* localVersion) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Error: WiFi no conectado");
    return false;
  }

  Serial.printf("Memoria libre antes de OTA: %d bytes\n", ESP.getFreeHeap());

  HTTPClient http;
  http.begin(VERSION_URL);
  http.setConnectTimeout(30000);
  http.setTimeout(30000);

  int httpCode = http.GET();
  if (httpCode != 200) {
    Serial.printf("Error al obtener versión remota: %d\n", httpCode);
    http.end();
    return false;
  }

  String remoteVersion = http.getString();
  remoteVersion.trim();
  Serial.printf("Versión local: %s | Remota: %s\n", localVersion, remoteVersion.c_str());

  if (remoteVersion != String(localVersion)) {
    http.end();

    String fullBinUrl = String(BASE_BIN_URL) + "firmware_v" + remoteVersion + ".bin";
    Serial.println("Descargando desde: " + fullBinUrl);

    WiFiClient client;
    httpUpdate.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
    
    // Configurar timeouts para httpUpdate
    httpUpdate.rebootOnUpdate(false);
    httpUpdate.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
    
    // Mostrar progreso
    httpUpdate.onProgress([](int cur, int total) {
      Serial.printf("Progreso: %d%%\r", (cur * 100) / total);
    });

    t_httpUpdate_return ret = httpUpdate.update(client, fullBinUrl);

    switch (ret) {
      case HTTP_UPDATE_FAILED:
        Serial.printf("OTA falló (%d): %s\n", 
                     httpUpdate.getLastError(), 
                     httpUpdate.getLastErrorString().c_str());
        break;
      case HTTP_UPDATE_NO_UPDATES:
        Serial.println("No hay actualizaciones");
        break;
      case HTTP_UPDATE_OK:
        Serial.println("OTA exitoso, reiniciando...");
        ESP.restart();
        return true;
    }
  } else {
    Serial.println("Ya está actualizado");
  }
  
  http.end();
  return false;
}