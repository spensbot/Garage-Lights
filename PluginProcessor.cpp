/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
CustomLightShowAudioProcessor::CustomLightShowAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
	, parameters(*this, nullptr)
{
	for (int i = 0; i < 512; i++) {
		String channelNumber = (String)(i + 1);
		parameters.createAndAddParameter(channelNumber, "Channel " + channelNumber, String(), NormalisableRange<float>(0.0f, 255.0f, 1.0f), 0.0f, nullptr, nullptr);
		parameters.addParameterListener(channelNumber, this);
	}
	
	parameters.state = ValueTree(Identifier("DmxUniverse"));

	usbDmxPro = new UsbDmxPro(&parameters, &blackout);

	channelPage = 1;
	colourId = 0;
	blackout = false;
}

CustomLightShowAudioProcessor::~CustomLightShowAudioProcessor()
{
	usbDmxPro->stopTimer();
	usbDmxPro->disconnect();
}

//==============================================================================
const String CustomLightShowAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool CustomLightShowAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool CustomLightShowAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool CustomLightShowAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double CustomLightShowAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int CustomLightShowAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int CustomLightShowAudioProcessor::getCurrentProgram()
{
    return 0;
}

void CustomLightShowAudioProcessor::setCurrentProgram (int index)
{
}

const String CustomLightShowAudioProcessor::getProgramName (int index)
{
    return {};
}

void CustomLightShowAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void CustomLightShowAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
	
}

void CustomLightShowAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool CustomLightShowAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void CustomLightShowAudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
	buffer.clear();


}

//==============================================================================
bool CustomLightShowAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* CustomLightShowAudioProcessor::createEditor()
{
    return new CustomLightShowAudioProcessorEditor (*this, parameters);
}

//==============================================================================
void CustomLightShowAudioProcessor::getStateInformation (MemoryBlock& destData)
{
	ScopedPointer<XmlElement> xml(parameters.state.createXml());
	copyXmlToBinary(*xml, destData);
}

void CustomLightShowAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
	ScopedPointer<XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
	if (xmlState != nullptr)
		if (xmlState->hasTagName(parameters.state.getType()))
			parameters.state = ValueTree::fromXml(*xmlState);
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new CustomLightShowAudioProcessor();
}

void CustomLightShowAudioProcessor::parameterChanged(const String & parameterID, float newValue) {
	usbDmxPro->myDmx[parameterID.getIntValue()] = (int)newValue;
}
