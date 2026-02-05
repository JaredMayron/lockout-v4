#pragma once
#include <FS.h>
#include <LittleFS.h>
#include <ESP8266HTTPClient.h>
#include "esphome/core/automation.h"
#include "esphome/core/component.h"

namespace esphome {
namespace membercards {

class membercards : public Component {
public:
  void configure(const char *groups, const char *host);
  void dump_config() override;
  float get_setup_priority() const override { return setup_priority::DATA; }

  bool fetch_new();
  bool findcard(const char *search);

  Trigger<> *get_connect_trigger() const { return this->on_setup_trigger_; };

  bool fetch_needed = false;
  int id;
  char card[40];
  char name[40];
  char groups[10];
  char host[40];

protected:
  Trigger<> *on_setup_trigger_{new Trigger<>()};

};



} // namespace membercards
} // namespace esphome