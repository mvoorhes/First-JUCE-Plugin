/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

const int HEIGHT = 600;
const int WIDTH = 800;

struct CustomRotarySlider : juce::Slider
{
    CustomRotarySlider() : juce::Slider(juce::Slider::SliderStyle::RotaryHorizontalDrag, juce::Slider::TextEntryBoxPosition::NoTextBox) {
        
    }
};

struct ResponseCurveComponent : juce::Component, juce::AudioProcessorParameter::Listener, juce::Timer
{
    ResponseCurveComponent(FirstJUCEpluginAudioProcessor&);
    ~ResponseCurveComponent();
    
    void parameterValueChanged(int parameterIndex, float newValue) override;
    void parameterGestureChanged(int parameterIndex, bool gestureIsStarting) override {};
    void timerCallback() override;
    void paint(juce::Graphics& g) override;
    
private:
    FirstJUCEpluginAudioProcessor& audioProcessor;
    juce::Atomic<bool> parametersChanged {false};
    MonoChain monoChain;
};

//==============================================================================
/**
*/
class FirstJUCEpluginAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    FirstJUCEpluginAudioProcessorEditor (FirstJUCEpluginAudioProcessor&);
    ~FirstJUCEpluginAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    
private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    FirstJUCEpluginAudioProcessor& audioProcessor;
    
//    juce::Atomic<bool> parametersChanged {false};
    
    CustomRotarySlider peakFreqSlider,
                        peakGainSlider,
                        peakQualitySlider,
                        lowCutFreqSlider,
                        highCutFreqSlider,
                        lowCutSlopeSlider,
                        highCutSlopeSlider;
    
    ResponseCurveComponent responseCurveComponent;
    
    using APVTS = juce::AudioProcessorValueTreeState;
    using Attachment = APVTS::SliderAttachment;
    
    Attachment peakFreqSliderAttachment,
                peakGainSliderAttachment,
                peakQualitySliderAttachment,
                lowCutFreqSliderAttachment,
                highCutFreqSliderAttachment,
                lowCutSlopeSliderAttachment,
                highCutSlopeSliderAttachment;
    
//    MonoChain monoChain;
    
    std::vector<juce::Component*> getComps();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FirstJUCEpluginAudioProcessorEditor)
};
