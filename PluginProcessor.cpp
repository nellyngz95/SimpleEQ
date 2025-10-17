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
    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize=(juce::uint32)samplesPerBlock;
    spec.sampleRate=sampleRate;
    spec.numChannels=(juce::uint32)getTotalNumOutputChannels();
    processors.clear();
    for(int i=0;i<getTotalNumOutputChannels();++i)
    {
        processors.add(new ChannelFilterChain());
        processors.getLast()->prepare(spec);
    }
}

void SimpleEQAudioProcessor::releaseResources()
{
    processors.clear();
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
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
        {
            float* channelData = buffer.getWritePointer(channel);
            for (int i = 0; i < numSamples; ++i)
            {
                // Generates noise between -1.0f and 1.0f
                channelData[i] = 2.0f * rand() / (float)RAND_MAX - 1.0f;
            }
        }
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
    auto lowCoefficients = juce::dsp::IIR::Coefficients<float>::makeLowShelf(sampleRate, NewLowFreq, NewLowQ, NewLowGain);
    auto midCoefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate, NewMidFreq, NewMidQ, NewMidGain);
    auto highCoefficients = juce::dsp::IIR::Coefficients<float>::makeHighShelf(sampleRate, NewHighFreq, NewHighQ, NewHighGain);
    
    juce::dsp::AudioBlock<float> block (buffer);
    
    for (size_t channel = 0; channel < block.getNumChannels(); ++channel)
        {
            auto& chain = *processors[(int)channel];
           

            chain.get<0>().coefficients=lowCoefficients;
            chain.get<1>().coefficients=midCoefficients;
            chain.get<2>().coefficients=highCoefficients;
            
            auto channelBlock = block.getSingleChannelBlock ((int)channel);
            auto context = juce::dsp::ProcessContextReplacing<float>{ channelBlock };
            chain.process(context);
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
