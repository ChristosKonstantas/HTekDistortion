#include "PluginEditor.hpp"

HTekDistortionAudioProcessorEditor::HTekDistortionAudioProcessorEditor (HTekDistortionAudioProcessor& p)
: AudioProcessorEditor (&p), _proc (p)
{
    styleSlider (_drive,        "Drive (dB)");
    styleSlider (_preHPF,       "preHPF (Hz)");
    styleSlider (_threshold,    "Threshold");
    styleSlider (_knee,         "Knee");
    styleSlider (_bias,         "Bias");
    styleSlider (_postLPF,      "postLPF (Hz)");
    styleSlider (_mix,          "Mix");
    styleSlider (_output,       "Output (dB)");

    styleLabel(_driveLabel,     "Drive (dB)");    addAndMakeVisible(_driveLabel);
    styleLabel(_preHPFLabel,    "preHPF (Hz)");   addAndMakeVisible(_preHPFLabel);
    styleLabel(_thresholdLabel, "Threshold");     addAndMakeVisible(_thresholdLabel);
    styleLabel(_kneeLabel,      "Knee");          addAndMakeVisible(_kneeLabel);
    styleLabel(_biasLabel,      "Bias");          addAndMakeVisible(_biasLabel);
    styleLabel(_postLPFLabel,   "postLPF (Hz)");  addAndMakeVisible(_postLPFLabel);
    styleLabel(_mixLabel,       "Mix");           addAndMakeVisible(_mixLabel);
    styleLabel(_outputLabel,    "Output (dB)");   addAndMakeVisible(_outputLabel);

    styleLabel(_oversamplingLabel, "Oversampling"); addAndMakeVisible(_oversamplingLabel);

    addAndMakeVisible (_drive);
    addAndMakeVisible (_preHPF);
    addAndMakeVisible (_threshold);
    addAndMakeVisible (_knee);
    addAndMakeVisible (_bias);
    addAndMakeVisible (_postLPF);
    addAndMakeVisible (_mix);
    addAndMakeVisible (_output);

    _oversampling.addItem ("Off", 1);
    _oversampling.addItem ("2x",  2);
    _oversampling.addItem ("4x",  3);
    _oversampling.addItem ("8x",  4);
    _oversampling.setJustificationType (juce::Justification::centred);
    addAndMakeVisible (_oversampling);

    _aDrive     = std::make_unique<Attachment> (_proc.apvts, "driveDb",   _drive);
    _apreHPF    = std::make_unique<Attachment> (_proc.apvts, "preHPFHz",  _preHPF);
    _aThreshold = std::make_unique<Attachment> (_proc.apvts, "threshold", _threshold);
    _aKnee      = std::make_unique<Attachment> (_proc.apvts, "knee",      _knee);
    _aBias      = std::make_unique<Attachment> (_proc.apvts, "bias",      _bias);
    _apostLPF   = std::make_unique<Attachment> (_proc.apvts, "postLPFHz", _postLPF);
    _aMix       = std::make_unique<Attachment> (_proc.apvts, "mix",       _mix);
    _aOutput    = std::make_unique<Attachment> (_proc.apvts, "outputDb",  _output);
    _aOs        = std::make_unique<ChoiceAttachment> (_proc.apvts, "oversampling", _oversampling);

    setSize (640, 320);
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
    g.fillAll (juce::Colours::black);
    g.setColour (juce::Colours::white);
    g.setFont (16.0f);
    g.drawFittedText ("HTekDistortion", getLocalBounds().removeFromTop(24), juce::Justification::centred, 1);
}

void HTekDistortionAudioProcessorEditor::resized()
{
    juce::Rectangle<int> r = getLocalBounds().reduced(10);

    r.removeFromTop(24);

    juce::Rectangle<int> bottomBar = r.removeFromBottom(30);
    juce::Rectangle<int> topRow = r.removeFromTop(r.getHeight() / 2);
    juce::Rectangle<int> botRow = r;

    auto cell = [] (juce::Rectangle<int>& row, int n)
    {
        const int w = row.getWidth() / n;
        return row.removeFromLeft(w).reduced(6);
    };

    auto placeKnobWithLabel = [] (juce::Slider& s, juce::Label& l, juce::Rectangle<int> area)
    {
        juce::Rectangle<int> labelArea = area.removeFromTop(18);
        l.setBounds(labelArea);
        s.setBounds(area);
    };

    juce::Rectangle<int> tr = topRow;
    placeKnobWithLabel(_drive,     _driveLabel,     cell(tr, 4));
    placeKnobWithLabel(_preHPF,    _preHPFLabel,    cell(tr, 4));
    placeKnobWithLabel(_threshold, _thresholdLabel, cell(tr, 4));
    placeKnobWithLabel(_knee,      _kneeLabel,      cell(tr, 4));

    juce::Rectangle<int> br = botRow;
    placeKnobWithLabel(_bias,    _biasLabel,    cell(br, 4));
    placeKnobWithLabel(_postLPF, _postLPFLabel, cell(br, 4));
    placeKnobWithLabel(_mix,     _mixLabel,     cell(br, 4));
    placeKnobWithLabel(_output,  _outputLabel,  cell(br, 4));

    // Oversampling label + box (algorithm pending)
    juce::Rectangle<int> osArea = bottomBar.removeFromRight(220);
    _oversamplingLabel.setBounds(osArea.removeFromLeft(110).reduced(2));
    _oversampling.setBounds(osArea.reduced(2));
}
