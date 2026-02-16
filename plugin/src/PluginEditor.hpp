#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.hpp"

class HTekDistortionAudioProcessorEditor final : public juce::AudioProcessorEditor
{
public:
    explicit HTekDistortionAudioProcessorEditor (HTekDistortionAudioProcessor&);
    ~HTekDistortionAudioProcessorEditor() override = default;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    HTekDistortionAudioProcessor& _proc;
    juce::Label    _driveLabel, _preHPFLabel,  _thresholdLabel, _kneeLabel;
    juce::Label    _biasLabel,  _postLPFLabel, _mixLabel,      _outputLabel;
    juce::Label    _oversamplingLabel;
    juce::Slider   _drive, _preHPF, _threshold, _knee, _bias, _postLPF, _mix, _output;
    juce::ComboBox _oversampling;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> _aDrive, _apreHPF, _aThreshold, _aKnee, _aBias, _apostLPF, _aMix, _aOutput;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> _aOs;

    void styleSlider (juce::Slider& s, const juce::String& name);
    void styleLabel(juce::Label&l, const juce::String& text);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HTekDistortionAudioProcessorEditor)
};
