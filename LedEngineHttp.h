#pragma once

#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <LedEngine.h>

class LedEngineHttp {
public:
	/**
	 * Constuctor
	 *
	 * \param port HTTP server port
	 */
	LedEngineHttp (const uint16_t port, const LedEngine& led);

	/**
	 * HTTP server instance
	 */
	ESP8266WebServer server;

private:

	/**
	 * LedEngine instance
	 */
	const LedEngine& led_;

	void httpOnController_ ();
	void httpOffController_ ();
	void httpRawController_ ();
	void httpCie1976UcsController_ ();
	void httpColorTemperatureController_ ();
	void httpCalibrationController_ ();
};