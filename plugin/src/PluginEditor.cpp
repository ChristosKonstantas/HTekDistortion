#include "PluginEditor.hpp"

HTekDistortionAudioProcessorEditor::HTekDistortionAudioProcessorEditor (HTekDistortionAudioProcessor& p)
: AudioProcessorEditor (&p), _proc (p)
{
    styleSlider (_drive,           "Drive (dB)");    addAndMakeVisible(_drive);
    styleSlider (_preHPF,          "preHPF (Hz)");   addAndMakeVisible(_preHPF);
    styleSlider (_threshold,       "Threshold");     addAndMakeVisible(_threshold);
    styleSlider (_knee,            "Knee");          addAndMakeVisible(_knee);
    styleSlider (_bias,            "Bias");          addAndMakeVisible(_bias);
    styleSlider (_postLPF,         "postLPF (Hz)");  addAndMakeVisible(_postLPF);
    styleSlider (_mix,             "Mix");           addAndMakeVisible(_mix);
    styleSlider (_output,          "Output (dB)");   addAndMakeVisible(_output);

                                   
    styleLabel(_driveLabel,        "Drive (dB)");    addAndMakeVisible(_driveLabel);
    styleLabel(_preHPFLabel,       "preHPF (Hz)");   addAndMakeVisible(_preHPFLabel);
    styleLabel(_thresholdLabel,    "Threshold");     addAndMakeVisible(_thresholdLabel);
    styleLabel(_kneeLabel,         "Knee");          addAndMakeVisible(_kneeLabel);
    styleLabel(_biasLabel,         "Bias");          addAndMakeVisible(_biasLabel);
    styleLabel(_postLPFLabel,      "postLPF (Hz)");  addAndMakeVisible(_postLPFLabel);
    styleLabel(_mixLabel,          "Mix");           addAndMakeVisible(_mixLabel);
    styleLabel(_outputLabel,       "Output (dB)");   addAndMakeVisible(_outputLabel);

    styleLabel(_oversamplingLabel, "Oversampling");  addAndMakeVisible(_oversamplingLabel);

    auto makeYellow = [](juce::Slider& s)
    {
        s.setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::yellow);
        s.setColour(juce::Slider::thumbColourId, juce::Colours::yellow);
        s.setColour(juce::Slider::textBoxTextColourId, juce::Colours::yellow);
        s.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::yellow);
    };
    
    auto makePurple = [](juce::Slider& s)
    {
        s.setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::orange);
        s.setColour(juce::Slider::thumbColourId, juce::Colours::orange);
        s.setColour(juce::Slider::textBoxTextColourId, juce::Colours::orange);
        s.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::orange);
    };
    
    makeYellow(_preHPF);
    makeYellow(_postLPF);
    makePurple(_threshold);
    makePurple(_drive);
    makePurple(_knee);
    makePurple(_bias);
    makePurple(_mix);
    makePurple(_output);
    
    _preHPFLabel.setColour(juce::Label::textColourId, juce::Colours::yellow);
    _postLPFLabel.setColour(juce::Label::textColourId, juce::Colours::yellow);
    _driveLabel.setColour(juce::Label::textColourId, juce::Colours::orange);    
    _thresholdLabel.setColour(juce::Label::textColourId, juce::Colours::orange);
    _kneeLabel.setColour(juce::Label::textColourId, juce::Colours::orange);
    _biasLabel.setColour(juce::Label::textColourId, juce::Colours::orange);
    _mixLabel.setColour(juce::Label::textColourId, juce::Colours::orange);
    _outputLabel.setColour(juce::Label::textColourId, juce::Colours::orange);

    _oversampling.addItem ("Off", 1);
    _oversampling.addItem ("2x",  2);
    _oversampling.addItem ("4x",  3);
    _oversampling.addItem ("8x",  4);
    _oversampling.setJustificationType (juce::Justification::centred);
    addAndMakeVisible (_oversampling);
    
    // The following attachments are bound to _proc.apvts thus the attachments update the APVTS parameters and not the DSP state. 
    // Only the audio thread touches DSP objects. UI only touches APVTS (data-races prevented).
    _aDrive     = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>   (_proc.apvts, "driveDb",      _drive);
    _apreHPF    = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>   (_proc.apvts, "preHPFHz",     _preHPF);
    _aThreshold = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>   (_proc.apvts, "threshold",    _threshold);
    _aKnee      = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>   (_proc.apvts, "knee",         _knee);
    _aBias      = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>   (_proc.apvts, "bias",         _bias);
    _apostLPF   = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>   (_proc.apvts, "postLPFHz",    _postLPF);
    _aMix       = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>   (_proc.apvts, "mix",          _mix);
    _aOutput    = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>   (_proc.apvts, "outputDb",     _output);
    _aOs        = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment> (_proc.apvts, "oversampling", _oversampling);

    setSize (640, 350);
}

void HTekDistortionAudioProcessorEditor::styleSlider (juce::Slider& s, const juce::String& name)
{
    s.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    s.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 80, 18);
    s.setName (name);
}

void HTekDistortionAudioProcessorEditor::styleLabel(juce::Label& l, const juce::String& text)
{
    l.setText(text, juce::dontSendNotification);
    l.setJustificationType(juce::Justification::centred);
    l.setColour(juce::Label::textColourId, juce::Colours::white);
}

void HTekDistortionAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll(juce::Colours::black);

}

void HTekDistortionAudioProcessorEditor::resized()
{
    juce::Rectangle<int> r = getLocalBounds().reduced (10);
    r.removeFromTop (10);

    juce::Rectangle<int> bottomBar = r.removeFromBottom (30);

    juce::Rectangle<int> topRow = r.removeFromTop (r.getHeight() / 2);
    juce::Rectangle<int> botRow = r;

    auto placeKnobWithLabel = [] (juce::Slider& s, juce::Label& l, juce::Rectangle<int> area)
    {
        auto labelArea = area.removeFromTop (18);
        l.setBounds (labelArea);
        s.setBounds (area);
    };

    auto layoutRow4 = [&] (juce::Rectangle<int> row,
                           juce::Slider& a, juce::Label& la,
                           juce::Slider& b, juce::Label& lb,
                           juce::Slider& c, juce::Label& lc,
                           juce::Slider& d, juce::Label& ld)
    {
        constexpr int cols = 4;
        const int w = row.getWidth() / cols;
        const int used = w * cols;
        const int pad = (row.getWidth() - used) / 2;
        row.removeFromLeft (pad);

        placeKnobWithLabel (a, la, row.removeFromLeft(w).reduced(6));
        placeKnobWithLabel (b, lb, row.removeFromLeft(w).reduced(6));
        placeKnobWithLabel (c, lc, row.removeFromLeft(w).reduced(6));
        placeKnobWithLabel (d, ld, row.removeFromLeft(w).reduced(6));
    };

    layoutRow4 (topRow,
                _drive,     _driveLabel,
                _preHPF,    _preHPFLabel,
                _threshold, _thresholdLabel,
                _knee,      _kneeLabel);

    layoutRow4 (botRow,
                _bias,    _biasLabel,
                _postLPF, _postLPFLabel,
                _mix,     _mixLabel,
                _output,  _outputLabel);

    // Oversampling label + box
    juce::Rectangle<int> osArea = bottomBar.removeFromRight (220);
    _oversamplingLabel.setBounds (osArea.removeFromLeft (110).reduced (2));
    _oversampling.setBounds (osArea.reduced (2));
}
