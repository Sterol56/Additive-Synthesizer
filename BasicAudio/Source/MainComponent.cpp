// Music 256a / CS 476a | fall 2016
// CCRMA, Stanford University
//
// Author: Romain Michon (rmichonATccrmaDOTstanfordDOTedu)
// Description: Simple JUCE sine wave synthesizer

#ifndef MAINCOMPONENT_H_INCLUDED
#define MAINCOMPONENT_H_INCLUDED



#include "../JuceLibraryCode/JuceHeader.h"
#include "Sine.h"
#define oscNum 7 //number of the oscs that you're using

class MainContentComponent :
    public AudioAppComponent,
    private Slider::Listener,
    private ToggleButton::Listener
{
public:
    MainContentComponent() :  onOff (0), samplingRate(0.0)
    {

        //use for loop to configure multiple oscs
		for (int i = 0; i < oscNum; i++) {
		    // configuring frequency slider and adding it to the main window
			addAndMakeVisible(frequencySlider[i]);
			frequencySlider[i].setRange(50.0, 5000.0);
			frequencySlider[i].setSkewFactorFromMidPoint(500.0);
			frequencySlider[i].setValue(1000); // will also set the default frequency of the sine osc
			frequencySlider[i].addListener(this);

			// configuring frequency label box and adding it to the main window
			addAndMakeVisible(frequencyLabel[i]);
			frequencyLabel[i].setText("Frequency", dontSendNotification);
			frequencyLabel[i].attachToComponent(&frequencySlider[i], true);


			// configuring gain slider and adding it to the main window
			addAndMakeVisible(gainSlider[i]);
			gainSlider[i].setRange(0.0, 1.0);
			gain[i] = 0.0; // initialize gain value
			gainSlider[i].setValue(0.0); // will alsi set the default gain of the sine osc
			gainSlider[i].addListener(this);


			// configuring gain label and adding it to the main window
			addAndMakeVisible(gainLabel[i]);
			gainLabel[i].setText("Gain", dontSendNotification);
			gainLabel[i].attachToComponent(&gainSlider[i], true);

			//configuring mute button and add it to the main window
			addAndMakeVisible(muteButton[i]);
			onMute[i] = 0;
			muteButton[i].addListener(this);

			//configuring mute label and add it to the main window
			addAndMakeVisible(muteLabel[i]);
			muteLabel[i].setText("Mute", dontSendNotification);
			muteLabel[i].attachToComponent(&muteButton[i], true);
		}
        
        
        // configuring on/off button and adding it to the main window
        addAndMakeVisible(onOffButton);
        onOffButton.addListener(this);
        
        
        // configuring on/off label and adding it to the main window
        addAndMakeVisible(onOffLabel);
        onOffLabel.setText ("On/Off", dontSendNotification);
        onOffLabel.attachToComponent (&onOffButton, true);

        setSize (600, oscNum*80 + 56 + 40);
        nChans = 2;
        setAudioChannels (0, nChans); // no inputs, one output
    }
    
    ~MainContentComponent()
    {
        shutdownAudio();
    }
    
    void resized() override
    {
        // placing the UI elements in the main window
        // getWidth has to be used in case the window is resized by the user
        const int sliderLeft = 80;
		for (int i = 0; i < oscNum; i++) {
			frequencySlider[i].setBounds(sliderLeft, 10 + i*80 , getWidth() - sliderLeft - 20, 20);
			gainSlider[i].setBounds(sliderLeft, 40 + i*80, getWidth() - sliderLeft - 20, 20);
			muteButton[i].setBounds(sliderLeft, 60 + i * 80, 40, 20);
		}
		onOffButton.setBounds(sliderLeft, 26 + oscNum*80, getWidth() - sliderLeft - 20, 20);
    }
    
    void sliderValueChanged (Slider* slider) override
    {
        if (samplingRate > 0.0){
			for (int i = 0; i < oscNum; i++) {
				if (slider == &frequencySlider[i]) {
					sine[i].setFrequency(frequencySlider[i].getValue());
				}
				else if (slider == &gainSlider[i]) {
					gain[i] = gainSlider[i].getValue();
				}
			}

        }
    }//change later
    
    void buttonClicked (Button* button) override
    {
        // turns audio on or off
        if(button == &onOffButton && onOffButton.getToggleState()){
            onOff = 1;
        }
        else if(button == &onOffButton && !onOffButton.getToggleState() ){
            onOff = 0;
        }
        //mute one osc
		for (int i = 0; i < oscNum; i++) {
			if (button == &muteButton[i] && muteButton[i].getToggleState()) {
				onMute[i] = 1;
			}
			else if(button == &muteButton[i] && !muteButton[i].getToggleState()){
				onMute[i] = 0;
			}
		}
    }
    
    void prepareToPlay (int /*samplesPerBlockExpected*/, double sampleRate) override
    {
		for (int i = 0; i < oscNum; i++) {
			samplingRate = sampleRate;
			sine[i].setSamplingRate(sampleRate);
		}
    }
    
    void releaseResources() override
    {
    }
    
    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) override
    {
        // getting the audio output buffer to be filled
        float* const buffer0 = bufferToFill.buffer->getWritePointer (0, bufferToFill.startSample);
		float* const buffer1 = bufferToFill.buffer->getWritePointer(1, bufferToFill.startSample);
        
        // computing one block
        for (int sample = 0; sample < bufferToFill.numSamples; ++sample)
        {
			if (onOff == 1) {
					for (int i = 0; i < oscNum; i++) {
						if (onMute[i] == 0) {
							buffer0[sample] += sine[i].tick() * gain[i];
						}
					}
				buffer1[sample] = buffer0[sample];
			}
			else {
				buffer0[sample] = 0.0;
				buffer1[sample] = 0.0;
			}
        }
    }
    
    
private:
    // UI Elements
    Slider frequencySlider[oscNum];
    Slider gainSlider[oscNum];
    ToggleButton onOffButton;
	ToggleButton muteButton[oscNum];
    
    Label frequencyLabel[oscNum], gainLabel[oscNum], onOffLabel, muteLabel[oscNum];
    
    Sine sine[oscNum]; // the sine wave oscillator
    
    // Global Variables
    float gain[oscNum];
    int onOff, samplingRate, nChans;
	int onMute[oscNum];
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainContentComponent)
};

Component* createMainContentComponent()     { return new MainContentComponent(); }


#endif  // MAINCOMPONENT_H_INCLUDED
