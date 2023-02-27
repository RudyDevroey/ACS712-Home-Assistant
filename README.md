# ACS712_HA
ACS712 sensor with ESP32 and MQTT Home Assistant

Calibrating ACS712 current sensor:
At no load situation open serial monitor and check weither the amp values are near to Zero. If the values are not near to zero, and if the values are positive then substract the error value from the line 86, if the values are negative then add the error value till you reach the near zero level.

Calibration is important to get the nearly accurate results, You can also calibrate with some known current loads and correct the error through the code.

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
