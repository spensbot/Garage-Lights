/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"
#include "GuiParams.h"

//==============================================================================
/**
*/

class GarageLightsAudioProcessorEditor  : public AudioProcessorEditor, public Button::Listener
{
public:
	GarageLightsAudioProcessorEditor(GarageLightsAudioProcessor&, AudioProcessorValueTreeState& vts);
    ~GarageLightsAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

	void buttonClicked(Button* button) override;

	typedef AudioProcessorValueTreeState::SliderAttachment SliderAttachment;

private:

	void pageChange(int);

	void determineConnectionStatus();

    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    GarageLightsAudioProcessor& processor;

	AudioProcessorValueTreeState& valueTreeState;

	GuiParams gp;

	MyLookAndFeel myLookAndFeel;

	Slider dmxLevelSlider[NUMBER_CHANNELS];
	ScopedPointer<SliderAttachment> dmxLevelSliderAttachment[NUMBER_CHANNELS];

	TextButton connectUsbButton;
	TextButton blackoutButton;
	TextButton disconnectButton;
	ArrowButton pageRightButton{ "pageRightButton", 0.0f, Colours::white };
	ArrowButton pageLeftButton{ "pageLeftButton", 0.5f, Colours::white };
	OwnedArray<ShapeButton> colorSelectButtons;

	String consoleText;

	boolean connectAttempt;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GarageLightsAudioProcessorEditor)
};
