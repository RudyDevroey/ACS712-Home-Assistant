# ACS712_HA
ACS712 sensor with ESP32 and MQTT Home Assistant

Connect an ACS712 sensor to Home Assistant through MQTT.
Don't forget to add the mqtt sensors in you configuration.yaml!!

    - name: ACS712watt
      unique_id: 'sensor.ACS712watt'
      device_class: power
      state_class: measurement
      unit_of_measurement: 'kW'
      value_template: "{{ value|float / 1000 }}"        
      state_topic: "homeassistant/sensor/stroomsensor/watt"

    - name: ACS712amps
      unique_id: 'sensor.ACS712amp'
      device_class: current
      state_class: measurement
      unit_of_measurement: 'A'     
      state_topic: "homeassistant/sensor/stroomsensor/amps"
