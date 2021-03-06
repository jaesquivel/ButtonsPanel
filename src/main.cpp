// Buttons Panel

// Let the IDE point to the Souliss framework
#include <SoulissFramework.h>

// Configure the framework
#include "bconf/StandardArduino.h"				// Use a standard Arduino
#include "conf/usart.h"							// USART RS485

/*************/
// Use the following if you are using an RS485 transceiver with
// transmission enable pin, otherwise delete this section.
//
#define USARTDRIVER_INSKETCH
#define USART_TXENABLE			1
#define USART_TXENPIN			3
#define USARTDRIVER				Serial
/*************/

// Include framework code and libraries
#include <SPI.h>
#include <Souliss.h>

#include "../../ControllerGateway/include/types.h"
#include "../../ControllerGateway/include/constants.h"

// Pin assignment
#define BUTTON_1_PIN		4
#define BUTTON_2_PIN		5
#define LED_1_PIN			6
#define LED_2_PIN			7

#define BUTTON_PIN_START	4
#define NUM_BUTTONS			2

uint8_t buttonState[NUM_BUTTONS];

button_address	button_addr;

boolean buttonPressed(int pin) {
	if(digitalRead(pin) && (buttonState[pin-BUTTON_PIN_START] == LOW)) {
		buttonState[pin-BUTTON_PIN_START] = HIGH;
		return true;
	} else if(!digitalRead(pin))
		buttonState[pin-BUTTON_PIN_START] = LOW;
	return false;
}

void setup() {
	Initialize();

	// Set network parameters
	SetAddress(RS485_BUTTONS_PANEL_1_ADDR, VNET_SUBNET_MASK, RS485_GATEWAY_ADDR);

	// Define inputs, outputs pins and pulldown
	pinMode(BUTTON_1_PIN, INPUT);
	pinMode(BUTTON_2_PIN, INPUT);
	pinMode(LED_1_PIN, OUTPUT);
	pinMode(LED_2_PIN, OUTPUT);

	Set_T11(BUTTONS_PANEL_LED_1_SLOT);
	Set_T11(BUTTONS_PANEL_LED_2_SLOT);

	for(uint8_t i=0; i < NUM_BUTTONS; i++) {
		buttonState[i]  = LOW;
	}
	button_addr.button_panel_addr = RS485_BUTTONS_PANEL_1_ADDR;
}

void loop() {
	// Here we start to play
	EXECUTEFAST() {
		UPDATEFAST();

		FAST_10ms() {	// Process  logic and relevant input and output every 50 milliseconds

			button_addr.button = 1;
			if(buttonPressed(BUTTON_1_PIN))
				m_publishdata(BUTTONS_PANEL_MCAS_ADDR, BUTTON_PUSHED_EVENT, (uint8_t*)&button_addr, sizeof(button_addr));

			button_addr.button = 2;
			if(buttonPressed(BUTTON_2_PIN))
				m_publishdata(BUTTONS_PANEL_MCAS_ADDR, BUTTON_PUSHED_EVENT, (uint8_t*)&button_addr, sizeof(button_addr));

			// Execute the logic
			Logic_T11(BUTTONS_PANEL_LED_1_SLOT);
			Logic_T11(BUTTONS_PANEL_LED_2_SLOT);

			// Drive the LEDs
			DigOut(LED_1_PIN, Souliss_T1n_Coil, BUTTONS_PANEL_LED_1_SLOT);
			DigOut(LED_2_PIN, Souliss_T1n_Coil, BUTTONS_PANEL_LED_2_SLOT);

		}

		// Process the communication, this include the command that are coming from SoulissApp
		// or from the push-button located on the other node
		FAST_PeerComms();

	}

	EXECUTESLOW() {
		UPDATESLOW();
	}
}
