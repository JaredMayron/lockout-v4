#!/bin/bash

docker compose exec -it esphome /entrypoint.sh run --no-logs --device OTA /config/backdoor.yaml /config/frontdoor.yaml
