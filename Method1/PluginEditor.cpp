/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SimpleEQAudioProcessorEditor::SimpleEQAudioProcessorEditor (SimpleEQAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    
    //LOW BAND
    LowFrequency.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
    LowFrequency.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 50, 10);
    LowFrequency.setRange(0.0,24.0,0.0);
    addAndMakeVisible(LowFrequency);
    LowFrequencyAttachment=std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.pvms, "LowFrequency", LowFrequency);
    
    LowGain.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
    LowGain.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 50, 10);
    LowGain.setRange(0.0,24.0,0.0);
    addAndMakeVisible(LowGain);
    LowGainAttachment=std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.pvms, "LowGain", LowGain);
    
    LowQ.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
    LowQ.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 50, 10);
    LowQ.setRange(0.0,24.0,0.0);
    addAndMakeVisible(LowQ);
    LowQAttachment=std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.pvms, "LowQ", LowQ);
    
    // === MID BAND ===
        MidFrequency.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
        MidFrequency.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 50, 10);
        MidFrequency.setRange(0.0,24.0,0.0);
        addAndMakeVisible(MidFrequency);
        MidFrequencyAttachment=std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.pvms, "MidFrequency", MidFrequency);

        MidGain.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
        MidGain.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 50, 10);
        MidGain.setRange(0.0,24.0,0.0);
        addAndMakeVisible(MidGain);
        MidGainAttachment=std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.pvms, "MidGain", MidGain);

        MidQ.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
        MidQ.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 50, 10);
        MidQ.setRange(0.0,24.0,0.0);
        addAndMakeVisible(MidQ);
        MidQAttachment=std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.pvms, "MidQ", MidQ);

        // === HIGH BAND ===
        HighFrequency.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
        HighFrequency.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 50, 10);
        HighFrequency.setRange(0.0,24.0,0.0);
        addAndMakeVisible(HighFrequency);
        HighFrequencyAttachment=std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.pvms, "HighFrequency", HighFrequency);

        HighGain.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
        HighGain.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 50, 10);
        HighGain.setRange(0.0,24.0,0.0);
        addAndMakeVisible(HighGain);
        HighGainAttachment=std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.pvms, "HighGain", HighGain);

        HighQ.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
        HighQ.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 50, 10);
        HighQ.setRange(0.0,24.0,0.0);
        addAndMakeVisible(HighQ);
        HighQAttachment=std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.pvms, "HighQ", HighQ);
        
    setSize (400, 300);
}

SimpleEQAudioProcessorEditor::~SimpleEQAudioProcessorEditor()
{
}

//==============================================================================
void SimpleEQAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (juce::FontOptions (15.0f));
}

void SimpleEQAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();
    const int columnWidth = bounds.getWidth() / 3; // Divide the width into 3 columns
    const int controlHeight = bounds.getHeight() / 3; // Divide the height for 3 controls per band
        
        
    auto lowBounds  = bounds.removeFromLeft(columnWidth);
    auto midBounds  = bounds.removeFromLeft(columnWidth);
    auto highBounds = bounds.removeFromLeft(columnWidth); // The rest of the bounds
        
    // === LOW BAND (Left Column) ===
    LowFrequency.setBounds(lowBounds.removeFromTop(controlHeight));
    LowGain.setBounds(lowBounds.removeFromTop(controlHeight));
    LowQ.setBounds(lowBounds);
    //Mid Band
    MidFrequency.setBounds(midBounds.removeFromTop(controlHeight));
    MidGain.setBounds(midBounds.removeFromTop(controlHeight));
    MidQ.setBounds(midBounds);
    //HIgh band
    HighFrequency.setBounds(highBounds.removeFromTop(controlHeight));
    HighGain.setBounds(highBounds.removeFromTop(controlHeight));
    HighQ.setBounds(highBounds);
}
