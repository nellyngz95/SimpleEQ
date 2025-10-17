/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SimpleEQAudioProcessor::SimpleEQAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ), pvms(*this, nullptr, "params", createParameterLayout())
#endif
{
}

SimpleEQAudioProcessor::~SimpleEQAudioProcessor()
{
}

//==============================================================================
const juce::String SimpleEQAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool SimpleEQAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool SimpleEQAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool SimpleEQAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double SimpleEQAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int SimpleEQAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int SimpleEQAudioProcessor::getCurrentProgram()
{
    return 0;
}

void SimpleEQAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String SimpleEQAudioProcessor::getProgramName (int index)
{
    return {};
}

void SimpleEQAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void SimpleEQAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    lowFilters.clear();
      for (int i = 0; i < getTotalNumInputChannels(); ++i) {
        juce::IIRFilter* filter;
        lowFilters.add(filter = new juce::IIRFilter());
      }
    
    midFilters.clear();
      for (int i = 0; i < getTotalNumInputChannels(); ++i) {
        juce::IIRFilter* filter;
        midFilters.add(filter = new juce::IIRFilter());
      }
    
      highFilters.clear();
      for (int i = 0; i < getTotalNumInputChannels(); ++i) {
        juce::IIRFilter* filter;
        highFilters.add(filter = new juce::IIRFilter());
      }
}

void SimpleEQAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool SimpleEQAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
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

void SimpleEQAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    auto numSamples = buffer.getNumSamples();
    float sampleRate = (float)juce::AudioProcessor::getSampleRate();
    
    //LOW :
    float NewLowGain = 0.0f;
    auto* lowGain=  pvms.getRawParameterValue("LowGain");
    NewLowGain=pow(10,lowGain->load()/20.0);
   
    
    float NewLowFreq=0.0f;
    auto *LowFreq=pvms.getRawParameterValue("LowFrequency");
    NewLowFreq=LowFreq->load();
    
    float NewLowQ=0.0f;
    auto *LowQ=pvms.getRawParameterValue("LowQ");
    NewLowQ=LowQ->load();
    //MID
    float NewMidGain = 0.0f;
    auto* MidGain=  pvms.getRawParameterValue("MidGain");
    NewMidGain=pow(10,MidGain->load()/20.0);
   
    
    float NewMidFreq=0.0f;
    auto *MidFreq=pvms.getRawParameterValue("MidFrequency");
    NewMidFreq=MidFreq->load();
    
    float NewMidQ=0.0f;
    auto *MidQ=pvms.getRawParameterValue("MidQ");
    NewMidQ=MidQ->load();

    //High
    float NewHighGain = 0.0f;
    auto* HighGain=  pvms.getRawParameterValue("HighGain");
    NewHighGain=pow(10,HighGain->load()/20.0);
   
    
    float NewHighFreq=0.0f;
    auto *HighFreq=pvms.getRawParameterValue("HighFrequency");
    NewHighFreq=HighFreq->load();
    
    float NewHighQ=0.0f;
    auto *HighQ=pvms.getRawParameterValue("HighQ");
    NewHighQ=HighQ->load();
    
    
    //COEFFICIENTS LOW-MID&HIGH:
    auto lowCoefficients = juce::IIRCoefficients::makeLowShelf(sampleRate, NewLowFreq,NewLowQ, NewLowGain);
    for (int i = 0; i < lowFilters.size(); i++) lowFilters[i]->setCoefficients(lowCoefficients);
    auto MidCoefficients = juce::IIRCoefficients::makePeakFilter(sampleRate, NewMidFreq,NewMidQ, NewMidGain);
    for (int i = 0; i < midFilters.size(); i++) midFilters[i]->setCoefficients(MidCoefficients);
    auto HighCoefficients = juce::IIRCoefficients::makeHighShelf(sampleRate, NewHighFreq,NewHighQ, NewHighGain);
    for (int i = 0; i < highFilters.size(); i++) highFilters[i]->setCoefficients(HighCoefficients);
    
    
    for (int channel = 0; channel < totalNumInputChannels; ++channel) {
          float* channelData = buffer.getWritePointer(channel);
          for (int i = 0; i < numSamples; ++i) channelData[i] = 2.0f * rand() / (float)RAND_MAX - 1.0f;
           lowFilters[channel]->processSamples(channelData, numSamples);
            midFilters[channel]->processSamples(channelData, numSamples);
            highFilters[channel]->processSamples(channelData, numSamples);
        }

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i) buffer.clear (i, 0, buffer.getNumSamples());
}

//==============================================================================
bool SimpleEQAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* SimpleEQAudioProcessor::createEditor()
{
    return new SimpleEQAudioProcessorEditor (*this);
}

//==============================================================================
void SimpleEQAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void SimpleEQAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SimpleEQAudioProcessor();
}

juce::AudioProcessorValueTreeState::ParameterLayout SimpleEQAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params; //POINTER
    
   params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{"LowFrequency", 1}, "Low Frequency",10.0f, 600.0f, 150.0f));
   
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{"LowGain", 1}, "Low Gain",-24.0f, 12.0f, 2.0f));
    
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{"LowQ", 1}, "Low Q", 0.1f, 20.0f, 2.0f));
   //MID
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{"MidFrequency", 1}, "Mid Frequency",150.0f, 3000.0f, 750.0f));
    
     params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{"MidGain", 1}, "Mid Gain",-24.0f, 12.0f, 2.0f));
     
     params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{"MidQ", 1}, "Mid Q", 0.1f, 20.0f, 2.0f));
     //High
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{"HighFrequency", 1}, "High Frequency",450.0f, 18000.0f, 3000.0f));
    
     params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{"HighGain", 1}, "High Gain",-24.0f, 12.0f, 2.0f));
     
     params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{"HighQ", 1}, "High Q", 0.1f, 20.0f, 2.0f));
     

    return { params.begin(), params.end()};
    
}
