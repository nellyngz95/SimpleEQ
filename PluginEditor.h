/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class SimpleEQAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    SimpleEQAudioProcessorEditor (SimpleEQAudioProcessor&);
    ~SimpleEQAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    
    juce::Slider LowFrequency;
    juce::Label LowFrequencyLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> LowFrequencyAttachment;
    
    juce::Slider LowGain;
    juce::Label LowGainLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> LowGainAttachment;
    
    juce::Slider LowQ;
    juce::Label LowQLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> LowQAttachment;
    
    //MID
    juce::Slider MidFrequency;
    juce::Label MidFrequencyLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> MidFrequencyAttachment;
    
    juce::Slider MidGain;
    juce::Label MidGainLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> MidGainAttachment;
    
    juce::Slider MidQ;
    juce::Label MidQLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> MidQAttachment;
    
    //High
    juce::Slider HighFrequency;
    juce::Label HighFrequencyLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> HighFrequencyAttachment;
    
    juce::Slider HighGain;
    juce::Label HighGainLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> HighGainAttachment;
    
    juce::Slider HighQ;
    juce::Label HighQLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> HighQAttachment;

private:
   
    
    SimpleEQAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SimpleEQAudioProcessorEditor)
};
