/*
  ==============================================================================

    GuiParams.h
    Created: 7 Feb 2018 5:30:09pm
    Author:  Spenser

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"

#define NUMBER_SLIDERS 32 //Number of sliders which will appear on a single page.
#define NUMBER_CHANNELS 512 //Number of DMX channels to create (512 is the maximum for a single universe).
#define NUMBER_COLORS 9 //Number of color options for the GUI (buttons on top right).

//A class which holds and calculates gui parameters.
class GuiParams {
public:
	GuiParams() {
		numSliderColumns = NUMBER_SLIDERS / numSliderRows;
		width = sliderWidth * numSliderColumns + padding * (numSliderColumns + 1);
		consoleY = titleHeight + padding;
		channelSelectorY = consoleY + consoleHeight + padding;
		sliderLabel1Y = channelSelectorY + sliderLabelHeight;
		sliderRow1Y = sliderLabel1Y + sliderLabelHeight;
		sliderLabel2Y = sliderRow1Y + sliderHeight + padding;
		sliderRow2Y = sliderLabel2Y + sliderLabelHeight;
		footerY = sliderRow2Y + sliderHeight + padding;
		height = footerY + footerHeight;
		consoleWidth = width - connectButtonWidth * 2 - padding * 4;
	}

	//Designer Set GUI parameters
	int padding = 10;
	int titleHeight = 45;
	int consoleHeight = 70;
	int sliderWidth = 32;
	int sliderHeight = 150;
	int sliderLabelHeight = 25;
	int numSliderRows = 2;
	int footerHeight = 30;
	int connectButtonWidth = 110;
	int arrowButtonWidth = 20;

	//Fonts
	//Font font_h1 = Font("Val", 50, Font::plain);
	//Font font_h1 = Font("Raleway Thin", 50, Font::plain);
	Font font_h1 = Font("Caviar Dreams", 40, Font::plain);
	//Font font_h1 = Font("Lovelo Line", 50, Font::bold);
	//Font font_h1 = Font("Bebas Neue Book", 50, Font::bold);
	Font font_h2 = Font("Caviar Dreams", 25, Font::plain);
	Font font_h3 = Font("Caviar Dreams", 20, Font::plain);
	Font font_console = Font("Terminal", 16, Font::plain);

	//Colours
	Colour backgroundColour = Colour(50, 50, 50);
	Colour textColour = Colours::white;
	Colour transparentTextColour = Colour((uint8)255, (uint8)255, (uint8)255, (uint8)200);
	Colour consoleBackgroundColour = Colour(32, 32, 32);
	Colour pluginFaceColours[NUMBER_COLORS] = { Colours::darkcyan, Colour(180, 10, 91), Colour(103, 65, 114), Colour(1, 50, 67), Colour(68, 108,179), Colour(30, 130, 76), Colour(150, 75, 45), Colour(165, 127, 38), Colour(98, 110, 110) };

	//Calculated GUI parameters
	int numSliderColumns;
	int width;
	int height;
	int consoleY;
	int channelSelectorY;
	int sliderLabel1Y;
	int sliderRow1Y;
	int sliderLabel2Y;
	int sliderRow2Y;
	int footerY;
	int consoleWidth;

	//Strings
	String title = "  Garage Lights";
	String footer = "Created By Saling To Mars";
	String consoleNoDevices = "Found no or multiple devices. Please connect only 1 USB pro device.";
	String console1Device = "Found a device!\nPress the connect button to attempt a connection! -->";
	String consoleSuccessfulConnection = "USB Pro DMX Device connected successfully!\nFirmware Version: ";
	String consoleUnsuccessfulConnection = "USB Pro DMX Device could not be connected.";

	void drawBackground(Graphics& g, int _colourId) {
		// (Our component is opaque, so we must completely fill the background with a solid colour)
		g.fillAll(backgroundColour);

		//Draw plugin faceplate background;
		g.setColour(pluginFaceColours[_colourId]);
		g.fillRoundedRectangle(0, titleHeight, width, height - titleHeight - footerHeight, padding);

		//Draw console background
		g.setColour(consoleBackgroundColour);
		g.fillRoundedRectangle(padding * 2 + connectButtonWidth, consoleY, consoleWidth, consoleHeight, padding);

		//Draw Dividing line
		g.setColour(textColour);
		g.drawLine(30, channelSelectorY + sliderLabelHeight / 2, width - 30, channelSelectorY + sliderLabelHeight / 2);
		g.setColour(pluginFaceColours[_colourId]);
		g.fillRect(width * 1 / 3, channelSelectorY, width / 3, sliderLabelHeight);
		g.setColour(textColour);
	}

	void drawStaticText(Graphics& g) {
		//Draw Title Text
		g.setColour(textColour);
		g.setFont(font_h1);
		g.drawFittedText(title, padding, 0, width - padding * 2, titleHeight, Justification::centredLeft, 1);

		//Draw Footer Text
		g.setFont(font_h2);
		g.drawFittedText(footer, padding, footerY, width - padding * 2, footerHeight, Justification::centredRight, 1);

		//Draw Channel Title Text
		g.drawFittedText("Channels", 0, channelSelectorY, width, sliderLabelHeight, Justification::centred, 1);
	}

	void drawConsoleText(Graphics& g, String _consoleText) {
		g.setFont(font_console);
		g.drawFittedText(_consoleText, padding * 2 + connectButtonWidth, consoleY, consoleWidth, consoleHeight, Justification::centred, 1);
	}

	void drawChannelLabels(Graphics& g, int _channelPage) {
		int startingChannel = (_channelPage - 1) * 32 + 1;
		int channelLabel = startingChannel;
		g.setFont(font_h3);

		for (int row = 0; row < numSliderRows; row++) {

			int yValue;
			if (row == 0) { yValue = sliderLabel1Y; }
			else { yValue = sliderLabel2Y; }

			for (int column = 0; column < numSliderColumns; column++) {

				int xValue = padding * (column + 1) + sliderWidth * column;

				g.setColour(textColour);
				g.drawFittedText((String)channelLabel, xValue, yValue, sliderWidth, sliderLabelHeight, Justification::centred, 1);
				g.setColour(Colour(255.0f, 255.0f, 255.0f, 0.2f));
				g.fillRect(xValue, yValue + sliderLabelHeight, sliderWidth, sliderHeight);

				++channelLabel;
			}
		}
	}

private:

};

//Create a custom LookAndFeel class to define custom button drawing methods.
class MyLookAndFeel : public LookAndFeel_V4 {
public:

	void drawButtonBackground(Graphics& g, Button & button, const Colour &backgroundColour, bool isMouseOverButton, bool isButtonDown) override {
		g.setColour(backgroundColour);
		if (isMouseOverButton) {
			g.setColour(Colour(150, 150, 150));
		}
		if (isButtonDown) {
			g.setColour(Colour(0, 0, 0));
		}
		g.fillRect(button.getLocalBounds());
	}

	void drawButtonText(Graphics& g, TextButton & button, bool isMouseOverButton, bool isButtonDown) override {
		g.setColour(Colours::white);
		g.setFont(Font("Terminal", 16, Font::plain));
		g.drawFittedText(button.getButtonText(), button.getLocalBounds(), Justification::centred, 2);
	}

private:

};