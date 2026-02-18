#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>

#include "effects/EffectChain.hpp"
#include "effects/Distortion.hpp"

//==============================================================================
class HTekDistortionAudioProcessor final : public juce::AudioProcessor
{
    public:
        //==============================================================================
        HTekDistortionAudioProcessor();
        ~HTekDistortionAudioProcessor() override;

        //==============================================================================
        void prepareToPlay (double sampleRate, int samplesPerBlock) override;
        void releaseResources() override;

        bool isBusesLayoutSupported (const BusesLayout& layouts) const override;

        void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
        using AudioProcessor::processBlock;

        //==============================================================================
        juce::AudioProcessorEditor* createEditor() override;
        bool hasEditor() const override;

        //==============================================================================
        const juce::String getName() const override;

        bool acceptsMidi() const override;
        bool producesMidi() const override;
        bool isMidiEffect() const override;
        double getTailLengthSeconds() const override;

        //==============================================================================
        int getNumPrograms() override;
        int getCurrentProgram() override;
        void setCurrentProgram (int index) override;
        const juce::String getProgramName (int index) override;
        void changeProgramName (int index, const juce::String& newName) override;

        //==============================================================================
        void getStateInformation (juce::MemoryBlock& destData) override;
        void setStateInformation (const void* data, int sizeInBytes) override;
        
        //==============================================================================
        juce::AudioProcessorValueTreeState apvts;

        static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    private:

        int _currentOsIndex = 0; // 0 = off, 1 = 2x, 2 = 4x, 3 = 8x
        int _maxBlockSize = 0;

        EffectChain _chain;
        HTekDistortionEffect _distortion;

        std::atomic<float>* _driveDb      = nullptr;
        std::atomic<float>* _preHPFHz     = nullptr;
        std::atomic<float>* _threshold    = nullptr;
        std::atomic<float>* _knee         = nullptr;
        std::atomic<float>* _bias         = nullptr;
        std::atomic<float>* _postLPFHz    = nullptr;
        std::atomic<float>* _mix          = nullptr;
        std::atomic<float>* _outputDb     = nullptr;
        std::atomic<float>* _oversampling = nullptr;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HTekDistortionAudioProcessor)
};
