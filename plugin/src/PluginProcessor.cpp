#include "PluginProcessor.hpp"
#include "PluginEditor.hpp"
#include <cmath>

//==============================================================================
HTekDistortionAudioProcessor::HTekDistortionAudioProcessor()
   : AudioProcessor (BusesProperties()
    #if ! JucePlugin_IsMidiEffect
    #if ! JucePlugin_IsSynth
                    .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
    #endif
                    .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
    #endif
                  )
    , apvts (*this, nullptr, "PARAMS", createParameterLayout())
{
    _driveDb      = apvts.getRawParameterValue ("driveDb");
    _preHPFHz     = apvts.getRawParameterValue ("preHPFHz");
    _threshold    = apvts.getRawParameterValue ("threshold");
    _knee         = apvts.getRawParameterValue ("knee");
    _bias         = apvts.getRawParameterValue ("bias");
    _postLPFHz    = apvts.getRawParameterValue ("postLPFHz");
    _mix          = apvts.getRawParameterValue ("mix");
    _outputDb     = apvts.getRawParameterValue ("outputDb");
    _oversampling = apvts.getRawParameterValue ("oversampling");

    _chain.clear();
    _chain.addEffect (&_distortion);
}

HTekDistortionAudioProcessor::~HTekDistortionAudioProcessor() = default;

//==============================================================================
const juce::String HTekDistortionAudioProcessor::getName() const
{
    return "HTekDistortion";
}

bool HTekDistortionAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool HTekDistortionAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool HTekDistortionAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double HTekDistortionAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int HTekDistortionAudioProcessor::getNumPrograms()           { return 1; }
int HTekDistortionAudioProcessor::getCurrentProgram()        { return 0; }
void HTekDistortionAudioProcessor::setCurrentProgram (int i) { juce::ignoreUnused (i); }
const juce::String HTekDistortionAudioProcessor::getProgramName (int i)
{
    juce::ignoreUnused (i);
    return {};
}
void HTekDistortionAudioProcessor::changeProgramName (int i, const juce::String& n)
{
    juce::ignoreUnused (i, n);
}

//==============================================================================
bool HTekDistortionAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    const auto mainIn  = layouts.getMainInputChannelSet();
    const auto mainOut = layouts.getMainOutputChannelSet();

    if (mainOut.isDisabled()) return false;

   #if ! JucePlugin_IsSynth
    if (mainIn.isDisabled()) return false;
    if (mainIn != mainOut)   return false;
   #endif

    return (mainOut == juce::AudioChannelSet::mono()
         || mainOut == juce::AudioChannelSet::stereo());
}

//==============================================================================
void HTekDistortionAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    _maxBlockSize = samplesPerBlock;

    const int numChannels = getTotalNumOutputChannels();
    jassert (numChannels > 0);

    _chain.prepare (sampleRate, samplesPerBlock , numChannels);
    _chain.reset();
}

void HTekDistortionAudioProcessor::releaseResources()
{
    _chain.reset();
    // oversampling.reset();
    // setLatencySamples (0);
}

//==============================================================================
void HTekDistortionAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                                juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused (midiMessages);
    juce::ScopedNoDenormals noDenormals;

    const int numSamples = buffer.getNumSamples();
    if (buffer.getNumChannels() == 0 || numSamples == 0)
        return;

    for (int ch = getTotalNumInputChannels(); ch < getTotalNumOutputChannels(); ++ch)
        buffer.clear (ch, 0, numSamples);

    HTekDistortionEffect::Params p;
    p.driveDb   = _driveDb->load();
    p.preHPFHz  = _preHPFHz->load();
    p.threshold = _threshold->load();
    p.knee      = _knee->load();
    p.bias      = _bias->load();
    p.postLPFHz = _postLPFHz->load();
    p.mix       = _mix->load();
    p.outputDb  = _outputDb->load();

    _distortion.setParams (p);

    juce::dsp::AudioBlock<float> baseBlock (buffer);

    _chain.process (baseBlock);
}

//==============================================================================
bool HTekDistortionAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* HTekDistortionAudioProcessor::createEditor()
{
    return new HTekDistortionAudioProcessorEditor (*this);
}

//==============================================================================
juce::AudioProcessorValueTreeState::ParameterLayout HTekDistortionAudioProcessor::createParameterLayout()
{
    using APVTS = juce::AudioProcessorValueTreeState;
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back (std::make_unique<juce::AudioParameterFloat>(
        "driveDb", "Drive",
        juce::NormalisableRange<float> (0.0f, 36.0f, 0.01f), 18.0f));

    params.push_back (std::make_unique<juce::AudioParameterFloat>(
        "preHPFHz", "preHPF (HPF)",
        juce::NormalisableRange<float> (20.0f, 700.0f, 0.01f, 0.5f), 80.0f));

    params.push_back (std::make_unique<juce::AudioParameterFloat>(
        "threshold", "Threshold",
        juce::NormalisableRange<float> (0.05f, 1.0f, 0.0001f), 0.7f));

    params.push_back (std::make_unique<juce::AudioParameterFloat>(
        "knee", "Knee",
        juce::NormalisableRange<float> (0.0f, 0.80f, 0.0001f), 0.12f));

    params.push_back (std::make_unique<juce::AudioParameterFloat>(
        "bias", "Bias",
        juce::NormalisableRange<float> (-0.5f, 0.5f, 0.0001f), 0.08f));

    params.push_back (std::make_unique<juce::AudioParameterFloat>(
        "postLPFHz", "postLPF (LPF)",
        juce::NormalisableRange<float> (3000.0f, 20000.0f, 0.01f, 0.5f), 12000.0f));

    params.push_back (std::make_unique<juce::AudioParameterFloat>(
        "mix", "Mix",
        juce::NormalisableRange<float> (0.0f, 1.0f, 0.0001f), 1.0f));

    params.push_back (std::make_unique<juce::AudioParameterFloat>(
        "outputDb", "Output",
        juce::NormalisableRange<float> (-24.0f, 24.0f, 0.01f), -1.0f));
    
    //TODO: Add oversampling!
    params.push_back (std::make_unique<juce::AudioParameterChoice>(
        "oversampling", "Oversampling",
        juce::StringArray { "Off", "2x", "4x", "8x" }, 0));

    return { params.begin(), params.end() };
}

//==============================================================================
void HTekDistortionAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void HTekDistortionAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
    if (xmlState == nullptr) return;

    if (xmlState->hasTagName (apvts.state.getType()))
        apvts.replaceState (juce::ValueTree::fromXml (*xmlState));
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new HTekDistortionAudioProcessor();
}