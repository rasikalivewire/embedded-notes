#ifndef SECRETS_H
#define SECRETS_H

#include <pgmspace.h>

// =====================================================
// WIFI
// =====================================================

#define WIFI_SSID "IOT"
#define WIFI_PASSWORD "12345678"


// =====================================================
// AWS IoT
// =====================================================

#define THINGNAME "ESP"

#define MQTT_HOST "ait43kyp0vy5k-ats.iot.us-east-1.amazonaws.com"

#define TIME_ZONE 5.5


// =====================================================
// AMAZON ROOT CA 1
// =====================================================

static const char cacert[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----

-----END CERTIFICATE-----
)EOF";


// =====================================================
// DEVICE CERTIFICATE
// =====================================================

static const char client_cert[] PROGMEM = R"KEY(
-----BEGIN CERTIFICATE-----

-----END CERTIFICATE-----

)KEY";


// =====================================================
// PRIVATE KEY
// =====================================================

static const char privkey[] PROGMEM = R"KEY(
-----BEGIN RSA PRIVATE KEY-----

)KEY";

#endif
