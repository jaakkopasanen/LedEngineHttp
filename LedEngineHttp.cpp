#include "LedEngineHttp.h"

LedEngineHttp::LedEngineHttp (const uint16_t port, const LedEngine& led) : led_(led) {
	ESP8266WebServer server(port);

	server.on("/on", HTTP_GET, httpOnController_);
	server.on("/off", HTTP_GET, httpOffController_);
	server.on("/raw", HTTP_GET, httpRawController_);
	server.on("/cie1976Ucs", HTTP_GET, httpCie1976UcsController_);
	server.on("/colorTemperature", HTTP_GET, httpColorTemperatureController_);
	server.on("/calibrate", HTTP_GET, httpCalibrationController_);
};

void LedEngineHttp::httpOnController_ () {
	led_.setOnOff(true);

	// JSON response
	char response[40];
	sprintf(response, "{\n  \"onOff\": %s}", "true");

	// Send response
	server.send(200, "application/json", response);
}

void LedEngineHttp::httpOffController_ () {
	led_.setOnOff(false);

	// JSON response
	char response[40];
	sprintf(response, "{\n  \"onOff\": %s\n}", "false");

	// Send response
	server.send(200, "application/json", response);
}

void LedEngineHttp::httpRawController_ () {
	float R = -1, G = -1, B = -1;

	// Parse args
	for (uint8_t i = 0; i < server.args(); ++i) {
		if (server.argName(i) == "R") {
			R = server.arg(i).toFloat();
		}
		else if (server.argName(i) == "G") {
			G = server.arg(i).toFloat();
		}
		else if (server.argName(i) == "B") {
			B = server.arg(i).toFloat();
		}
	}

	int httpStatus = 200;

	// All parameters must be given for write
	if (R >= 0 && G >= 0 && B >= 0) {
		// All parameters given -> set new values
		RGB raw = { R, G, B };
		led_.setRaw(raw);
	}

	// Read (updated) values
	RGB raw = led_.getRaw();

	// JSON response
	char response[60];
	char strR[10]; dtostrf(raw.R, 6, 4, strR);
	char strG[10]; dtostrf(raw.G, 6, 4, strG);
	char strB[10]; dtostrf(raw.B, 6, 4, strB);
	sprintf(response, "{\n  \"R\": %s,\n  \"G\": %s,\n  \"B\": %s\n}", strR, strG, strB);

	// Send response
	server.send(httpStatus, "application/json", response);
}

void LedEngineHttp::httpCie1976UcsController_ () {
	float L = -1, u = -1, v = -1;
	// Parse args
	for (uint8_t i = 0; i < server.args(); ++i) {
		if (server.argName(i) == "L") {
			L = server.arg(i).toFloat();
		}
		else if (server.argName(i) == "u") {
			u = server.arg(i).toFloat();
		}
		else if (server.argName(i) == "v") {
			v = server.arg(i).toFloat();
		}
	}

	// All or none of the parameters must be missing
	if (L >= 0 && u >= 0 && v >= 0) {
		// All parameters given -> set new color
		Luv luv = { L, u, v };
		led_.setCie1976Ucs(luv);
	}

	// Read (updated) color
	Luv luv = led_.getCie1976Ucs();
	L = luv.L;
	u = luv.u;
	v = luv.v;

	// JSON response
	char response[60];
	char strL[10]; dtostrf(L, 6, 4, strL);
	char strU[10]; dtostrf(u, 6, 4, strU);
	char strV[10]; dtostrf(v, 6, 4, strV);
	sprintf(response, "{\n  \"L\": %s,\n  \"u\": %s,\n  \"v\": %s\n}", strL, strU, strV);

	// Send response
	server.send(200, "application/json", response);
}

void LedEngineHttp::httpColorTemperatureController_ () {
	int T = -1; float L = -1;
	// Parse args
	for (uint8_t i = 0; i < server.args(); ++i) {
		if (server.argName(i) == "T") {
			T = server.arg(i).toInt();
		}
		else if (server.argName(i) == "L") {
			L = server.arg(i).toFloat();
		}
	}

	// Set color temperature
	if (T >= 1000 && L >= 0) {
		led_.setColorTemperature(L, T);
	}

	// Read (updated) color temperature
	T = led_.getColorTemperature();

	// Read (updated) lightness
	Luv luv = led_.getCie1976Ucs();
	L = luv.L;

	// JSON response
	char response[60];
	char strL[10]; dtostrf(L, 6, 4, strL);
	sprintf(response, "{\n  \"L\": %s,\n  \"T\": %d\n}", strL, T);

	// Send response
	server.send(200, "application/json", response);
}

void LedEngineHttp::httpCalibrationController_ () {
	float redU = -1, redV = -1, greenU = -1, greenV = -1, blueU = -1, blueV = -1;
	float redLum = -1, greenLum = -1, blueLum = -1;
	float redP1 = -10000, redP2 = -10000, redQ1 = -10000;
	float greenP1 = -10000, greenP2 = -10000, greenQ1 = -10000;
	float blueP1 = -10000, blueP2 = -10000, blueQ1 = -10000;

	// Parse args
	for (uint8_t i = 0; i < server.args(); ++i) {
		if (server.argName(i) == "redU") {
			redU = server.arg(i).toFloat();
		}
		else if (server.argName(i) == "redV") {
			redV = server.arg(i).toFloat();
		}
		else if (server.argName(i) == "greenU") {
			greenU = server.arg(i).toFloat();
		}
		else if (server.argName(i) == "greenV") {
			greenV = server.arg(i).toFloat();
		}
		else if (server.argName(i) == "blueU") {
			blueU = server.arg(i).toFloat();
		}
		else if (server.argName(i) == "blueV") {
			blueV = server.arg(i).toFloat();
		}
		else if (server.argName(i) == "redLum") {
			redLum = server.arg(i).toFloat();
		}
		else if (server.argName(i) == "greenLum") {
			greenLum = server.arg(i).toFloat();
		}
		else if (server.argName(i) == "blueLum") {
			blueLum = server.arg(i).toFloat();
		}
		else if (server.argName(i) == "redP1") {
			redP1 = server.arg(i).toFloat();
		}
		else if (server.argName(i) == "redP2") {
			redP2 = server.arg(i).toFloat();
		}
		else if (server.argName(i) == "redQ1") {
			redQ1 = server.arg(i).toFloat();
		}
		else if (server.argName(i) == "greenP1") {
			greenP1 = server.arg(i).toFloat();
		}
		else if (server.argName(i) == "greenP2") {
			greenP2 = server.arg(i).toFloat();
		}
		else if (server.argName(i) == "greenQ1") {
			greenQ1 = server.arg(i).toFloat();
		}
		else if (server.argName(i) == "blueP1") {
			blueP1 = server.arg(i).toFloat();
		}
		else if (server.argName(i) == "blueP2") {
			blueP2 = server.arg(i).toFloat();
		}
		else if (server.argName(i) == "blueQ1") {
			blueQ1 = server.arg(i).toFloat();
		}
	}

	if (redU > 0 && redV > 0 && greenU > 0 && greenV > 0 && blueU > 0 && blueV > 0 && redLum > 0 &&
		greenLum > 0 && blueLum > 0 && redP1 > -10000 && redP2 > -10000 && redQ1 > -10000 && greenP1 > -10000 &&
		greenP2 > -10000 && greenQ1 > -10000 && blueP1 > -10000 && blueP2 > -10000 && blueQ1 > -10000) {
		Luv redUv = { 100, redU, redV }, greenUv = { 100, greenU, greenV }, blueUv = { 100, blueU, blueV };
		float redToGreenFit[3] = { redP1, redP2, redQ1 };
		float greenToBlueFit[3] = { greenP1, greenP2, greenQ1 };
		float blueToRedFit[3] = { blueP1, blueP2, blueQ1 };
		led_.calibrate(redUv, greenUv, blueUv, redLum, greenLum, blueLum, redToGreenFit, greenToBlueFit, blueToRedFit);
	}

	// Send response
	server.send(200, "application/json", "");
}