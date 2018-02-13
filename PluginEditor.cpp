/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
GarageLightsAudioProcessorEditor::GarageLightsAudioProcessorEditor (GarageLightsAudioProcessor& p, AudioProcessorValueTreeState& vts)
    : AudioProcessorEditor (&p), processor (p), valueTreeState(vts)
{
	setSize(gp.width, gp.height);

	addAndMakeVisible(connectUsbButton);
	connectUsbButton.setBounds(gp.consoleWidth + gp.padding*3 + gp.connectButtonWidth, gp.consoleY, gp.connectButtonWidth, (gp.consoleHeight+gp.padding)/2);
	connectUsbButton.setSize(gp.connectButtonWidth, (gp.consoleHeight - gp.padding) / 2);
	connectUsbButton.setLookAndFeel(&myLookAndFeel);
	connectUsbButton.setButtonText("Connect USB");
	connectUsbButton.addListener(this);

	addAndMakeVisible(disconnectButton);
	disconnectButton.setBounds(gp.consoleWidth + gp.padding * 3 + gp.connectButtonWidth, gp.consoleY + (gp.consoleHeight + gp.padding) / 2, gp.connectButtonWidth, (gp.consoleHeight + gp.padding) / 2);
	disconnectButton.setSize(gp.connectButtonWidth, (gp.consoleHeight - gp.padding) / 2);
	disconnectButton.setLookAndFeel(&myLookAndFeel);
	disconnectButton.setButtonText("Disconnect");
	disconnectButton.addListener(this);

	addAndMakeVisible(blackoutButton);
	blackoutButton.setBounds(gp.padding, gp.consoleY, gp.connectButtonWidth, gp.consoleHeight);
	blackoutButton.setSize(gp.connectButtonWidth, gp.consoleHeight);
	blackoutButton.setLookAndFeel(&myLookAndFeel);
	blackoutButton.setButtonText("Blackout");
	blackoutButton.addListener(this);

	//Add page change buttons
	addAndMakeVisible(pageLeftButton);
	pageLeftButton.setSize(gp.arrowButtonWidth, gp.arrowButtonWidth);
	pageLeftButton.setBounds(gp.width * 23 / 60 - gp.arrowButtonWidth/2, gp.channelSelectorY + gp.sliderLabelHeight/2 - gp.arrowButtonWidth/2, gp.arrowButtonWidth, gp.arrowButtonWidth);
	pageLeftButton.addListener(this);

	addAndMakeVisible(pageRightButton);
	pageRightButton.setSize(gp.arrowButtonWidth, gp.arrowButtonWidth);
	pageRightButton.setBounds(gp.width * 37 / 60 - gp.arrowButtonWidth / 2, gp.channelSelectorY + gp.sliderLabelHeight / 2 - gp.arrowButtonWidth / 2, gp.arrowButtonWidth, gp.arrowButtonWidth);
	pageRightButton.addListener(this);
	
	//Add color buttons
	Path square;
	square.addRectangle(0, 0, gp.arrowButtonWidth, gp.arrowButtonWidth);
	for (int i = 0; i < NUMBER_COLORS; i++) {

		colorSelectButtons.add(new ShapeButton("hello", gp.pluginFaceColours[i], gp.pluginFaceColours[i], gp.pluginFaceColours[i]));
		addAndMakeVisible(colorSelectButtons[i]);
		colorSelectButtons[i]->setShape(square, true, true, true);
		colorSelectButtons[i]->addListener(this);

		int xValue = gp.width - gp.padding * (NUMBER_COLORS + 1) - gp.arrowButtonWidth * NUMBER_COLORS + gp.arrowButtonWidth * i + gp.padding*i;
		int yValue = gp.titleHeight / 2 - gp.arrowButtonWidth / 2;
		colorSelectButtons[i]->setBounds(xValue, yValue, gp.arrowButtonWidth, gp.arrowButtonWidth);
	}

	//Add sliders
	for (int slider = 0; slider < 512 ; slider++) {

		//Set slider settings.
		addChildComponent(dmxLevelSlider[slider]);
		dmxLevelSlider[slider].setSliderStyle(Slider::SliderStyle::LinearBarVertical);
		dmxLevelSliderAttachment[slider] = new SliderAttachment(valueTreeState, (String)(slider + 1), dmxLevelSlider[slider]);

		//Set Slider positions.
		int xValue = gp.padding * (slider%16 + 1) + gp.sliderWidth * (slider%16);
		int yValue;
		if (slider%32 < gp.numSliderColumns) { yValue = gp.sliderRow1Y; }
		else { yValue = gp.sliderRow2Y; }
		dmxLevelSlider[slider].setBounds(xValue, yValue, gp.sliderWidth, gp.sliderHeight);
	}

	pageChange(0);
	connectAttempt = false;

}

GarageLightsAudioProcessorEditor::~GarageLightsAudioProcessorEditor()
{
}

//==============================================================================
void GarageLightsAudioProcessorEditor::paint (Graphics& g)
{
	gp.drawBackground(g, processor.colourId);

	gp.drawStaticText(g);

	determineConnectionStatus();
	
	gp.drawConsoleText(g, consoleText);
	
	gp.drawChannelLabels(g, processor.channelPage);
	
}

void GarageLightsAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}

void GarageLightsAudioProcessorEditor::buttonClicked(Button* button) {
	if (button == &connectUsbButton) {
		if (!processor.usbDmxPro->connected) {
			processor.usbDmxPro->connect();
			connectAttempt = true;
		}
	}
	else if (button == &pageLeftButton) {
		pageChange(-1);
	}
	else if (button == &pageRightButton) {
		pageChange(1);
	}
	else if (button == &blackoutButton) {
		if (blackoutButton.getToggleState()) {
			blackoutButton.setToggleState(false, juce::NotificationType::dontSendNotification);
			processor.blackout = false;
		}
		else {
			blackoutButton.setToggleState(true, juce::NotificationType::dontSendNotification);
			processor.blackout = true;
		}
	}
	else if (button == &disconnectButton) {
		processor.usbDmxPro->disconnect();
	}
	for (int i = 0; i < NUMBER_COLORS; i++) {
		if (button == colorSelectButtons[i]) {
			processor.colourId = i;
		}
	}
	repaint();
}

void GarageLightsAudioProcessorEditor::pageChange(int direction) {

	//Turn off visiblility for the current page's sliders.
	int startingChannel = (processor.channelPage - 1) * 32 + 1;
	for (int slider = startingChannel; slider < startingChannel + 32; slider++) {
		dmxLevelSlider[slider - 1].setVisible(false);
	}
	
	//Update channel page.
	processor.channelPage += direction;
	if (processor.channelPage < 1) {
		processor.channelPage = 16;
	}
	if (processor.channelPage > 16) {
		processor.channelPage = 1;
	}

	//Turn on visibility for the new page's sliders.
	startingChannel = (processor.channelPage - 1) * 32 + 1;
	for (int slider = startingChannel; slider < startingChannel + 32; slider++) {
		dmxLevelSlider[slider - 1].setVisible(true);
	}

	repaint();
}

void GarageLightsAudioProcessorEditor::determineConnectionStatus() {

	if (connectAttempt) {
		if (processor.usbDmxPro->connected) {
			connectUsbButton.setButtonText("Connected");
		}
		else {
			connectUsbButton.setButtonText("Connect USB");
			consoleText = "Connection failed.\nMake sure you have an Open DMX compatible device.\nTry disconnecting, then connecting again.";
		}
	}
	connectAttempt = false;


	if (processor.usbDmxPro->connected) {
		consoleText = processor.usbDmxPro->getConnectionInfoString();
	}
	else if (processor.usbDmxPro->numDevices == 0) {
		consoleText = "No connection.\nCould not find any compatible devices.\nPlease connect a USB Dmx pro compatible device and retry.\nMake sure your FTDI driver is installed.";
	}
	else if (processor.usbDmxPro->numDevices == 1) {
		consoleText = "No connection.\nFound a compatible device!\nClick 'Connect USB' to attempt connection";
	}
	else {
		consoleText = "No connection.\nFound " + (String)processor.usbDmxPro->numDevices + " compatible devices.\nPlease disconnect all devices except the one you wish to use.\nAttempting connection with multiple devices available will connect to a random device.";
	}
}

