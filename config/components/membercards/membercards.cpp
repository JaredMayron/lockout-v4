#include "membercards.h"
#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>
#include "esphome/core/hal.h"
#include "esphome/core/log.h"

namespace esphome {
namespace membercards {

static const char *const TAG = "membercards";
static bool READY = false;
void membercards::configure(const char *groups, const char *host) {
  strcpy(this->groups, groups);
  strcpy(this->host, host);
  ESP_LOGCONFIG(TAG, "Setting up LittleFS...");

  if (!LittleFS.begin()) {
    LittleFS.format();
    if (!LittleFS.begin()) {
        ESP_LOGE(TAG, "LittleFS mount failed");
        return;
    }
  }

  READY = true;
  ESP_LOGCONFIG(TAG, "OK");
  if(!LittleFS.exists("names")){
    fetch_needed = true;
    ESP_LOGCONFIG(TAG, "Fetch queued");
  }
  this->on_setup_trigger_->trigger();
}

void membercards::dump_config() {
  ESP_LOGCONFIG(TAG, "Config:");
  ESP_LOGCONFIG(TAG, "groups: %s", this->groups);
  ESP_LOGCONFIG(TAG, "host: %s", this->host);
}

bool membercards::fetch_new() {
  ESP_LOGD(TAG, "is ready %d", READY);
  if(!READY) return false;
  ESP_LOGD(TAG, "starting fetch");
  fetch_needed = false;
  WiFiClient wifiClient;
  HTTPClient http;
  http.begin(wifiClient, "http://" + String(this->host) + "/fetch.php?groups=" + String(this->groups));
  int ret = http.GET();
  if(ret == 200){
    ESP_LOGD(TAG, "GET 200");
    File file = LittleFS.open("nametemp", "w");
    if (!file) {
      ESP_LOGE(TAG, "Failed to open file for writing: nametemp");
      return false;
    }
    ESP_LOGD(TAG, "write stream");
    bool res = (http.writeToStream(&file) > 0);
    file.close();
    if(res){
      ESP_LOGD(TAG, "got some names %d", res);
      LittleFS.remove("names");
      LittleFS.rename("nametemp","names");
      file = LittleFS.open("names", "r");
      int lines = 0;
      while(file.available()){
        file.readStringUntil('\n');
        lines++;
      }
      file.close();
      ESP_LOGD(TAG, "total names %d", lines);
    } else {
      ESP_LOGE(TAG, "name failed %d", res);
      fetch_needed = true;
    }
    return res;
  } else {
    ESP_LOGE(TAG, "fetch_new failed status code %d", ret);
    fetch_needed = true;
    return false;
  }
}

bool membercards::findcard(const char *search) {
  if(!READY) return false;
  File file = LittleFS.open("names", "r");
  if (!file) {
    ESP_LOGE(TAG, "Failed to open file for reading: names");
    return "";
  }
  String s = String(search);
  s.toLowerCase();
  while(file.available()){
    String l = file.readStringUntil('\n');
    int x = sscanf(l.c_str(), "%d|%39[^|]|%39s", &id, name, card);
    //ESP_LOGD(TAG, "s %s line %s", s.c_str(), l.c_str());
    if(x == 3){
      String card_lower = String(card);
      card_lower.toLowerCase();
      //ESP_LOGD(TAG, "test %s against %s %d", s.c_str(), card_lower.c_str(), card_lower.indexOf(s));
      if(card_lower.indexOf(s) >= 0){ 
        file.close();
        ESP_LOGD(TAG, "num items %d %d, %s, %s", x, id, name, card);
        strcpy(card, search);
        file.close();
        return true;
      }
    }
  }
  file.close();

  return false;
}

} // namespace membercards
} // namespace esphome