/*
  ==============================================================================

    CompressorView.h
    Created: 29 Aug 2020 11:21:37pm
    Author:  Jesse Chappell

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include "SonobusPluginProcessor.h"
#include "SonoLookAndFeel.h"
#include "SonoDrawableButton.h"

//==============================================================================
/*
*/
class ExpanderView    : public Component, public Slider::Listener, public Button::Listener
{
public:
    ExpanderView() : sonoSliderLNF(14), smallLNF(12)
    {
        // In your constructor, you should add any child components, and
        // initialise any special settings that your component needs.

        thresholdSlider.setName("thresh");
        thresholdSlider.setRange(-96.0f, 0.0f, 1);
        thresholdSlider.setSkewFactor(1.5);
        thresholdSlider.setTextValueSuffix(" dB");
        thresholdSlider.setDoubleClickReturnValue(true, -60.0);
        configKnobSlider(thresholdSlider);
        thresholdSlider.addListener(this);

        thresholdLabel.setText(TRANS("Noise Floor"), dontSendNotification);
        configLabel(thresholdLabel);

           
        ratioSlider.setName("ratio");
        ratioSlider.setRange(1.0f, 20.0f, 0.1);
        ratioSlider.setSkewFactor(0.5);
        ratioSlider.setTextValueSuffix(" : 1");
        ratioSlider.setDoubleClickReturnValue(true, 2.0);
        configKnobSlider(ratioSlider);
        ratioSlider.addListener(this);
        
        ratioLabel.setText(TRANS("Ratio"), dontSendNotification);
        configLabel(ratioLabel);

           
        attackSlider.setName("attack");
        attackSlider.setRange(1.0f, 1000.0f, 1);
        attackSlider.setSkewFactor(0.5);
        attackSlider.setTextValueSuffix(" ms");
        attackSlider.setDoubleClickReturnValue(true, 1.0);
        configKnobSlider(attackSlider);
        attackSlider.addListener(this);
        
        attackLabel.setText(TRANS("Attack"), dontSendNotification);
        configLabel(attackLabel);

           
        releaseSlider.setName("release");
        releaseSlider.setRange(1.0f, 1000.0f, 1);
        releaseSlider.setSkewFactor(0.5);
        releaseSlider.setTextValueSuffix(" ms");
        releaseSlider.setDoubleClickReturnValue(true, 200.0);
        configKnobSlider(releaseSlider);
        releaseSlider.addListener(this);
        
        releaseLabel.setText(TRANS("Release"), dontSendNotification);
        configLabel(releaseLabel);

           
        std::unique_ptr<Drawable> powerimg(Drawable::createFromImageData(BinaryData::power_svg, BinaryData::power_svgSize));
        std::unique_ptr<Drawable> powerselimg(Drawable::createFromImageData(BinaryData::power_sel_svg, BinaryData::power_sel_svgSize));
        enableButton.setImages(powerimg.get(), nullptr, nullptr, nullptr, powerselimg.get());
        enableButton.addListener(this);
        enableButton.setClickingTogglesState(true);
        //enableButton.setColour(TextButton::buttonOnColourId, Colour::fromFloatRGBA(0.2, 0.5, 0.7, 0.8));
        //enableButton.setColour(TextButton::buttonOnColourId, Colour::fromFloatRGBA(0.2, 0.2, 0.2, 0.7));
        enableButton.setColour(TextButton::buttonColourId, Colours::transparentBlack);
        enableButton.setColour(TextButton::buttonOnColourId, Colours::transparentBlack);
        enableButton.setColour(DrawableButton::backgroundColourId, Colours::transparentBlack);
        enableButton.setColour(DrawableButton::backgroundOnColourId, Colours::transparentBlack);
        
        titleLabel.setText(TRANS("Noise Gate"), dontSendNotification);

           

        addAndMakeVisible(thresholdSlider);
        addAndMakeVisible(thresholdLabel);
        addAndMakeVisible(ratioSlider);
        addAndMakeVisible(ratioLabel);
        addAndMakeVisible(attackSlider);
        addAndMakeVisible(attackLabel);
        addAndMakeVisible(releaseSlider);
        addAndMakeVisible(releaseLabel);

        headerComponent.addAndMakeVisible(enableButton);
        headerComponent.addAndMakeVisible(titleLabel);

        headerComponent.addMouseListener(this, true);

        
        setupLayout();
        
        updateParams(mParams);
    }

    ~ExpanderView()
    {
    }

    class HeaderComponent : public Component
    {
    public:
        HeaderComponent(ExpanderView & parent_) : parent(parent_) {
            
        }
        ~HeaderComponent() {}
        
        void paint (Graphics& g) override
        {
            if (parent.enableButton.getToggleState()) {
                g.setColour(Colour::fromFloatRGBA(0.2f, 0.5f, 0.7f, 0.5f));                
            } else {
                g.setColour(Colour(0xff2a2a2a));                
            }

            auto bounds = getLocalBounds().withTrimmedTop(2).withTrimmedBottom(2);
            g.fillRoundedRectangle(bounds.toFloat(), 6.0);
        }
        
        void resized() override {
            auto bounds = getLocalBounds().withTrimmedTop(4).withTrimmedBottom(4);
            headerBox.performLayout(bounds);
        }
        
        FlexBox headerBox;
        ExpanderView & parent;
    };
    
    Component * getHeaderComponent() {
        
        return &headerComponent;
    }
    
    void mouseUp (const MouseEvent& event) override {
        if (event.eventComponent == &headerComponent) {
            if (!event.mouseWasDraggedSinceMouseDown()) {                
                listeners.call (&ExpanderView::Listener::expanderHeaderClicked, this, event);
            }
        }
    }

    
    void paint (Graphics& g) override
    {
       
    }
    
    class Listener {
    public:
        virtual ~Listener() {}
        virtual void expanderParamsChanged(ExpanderView *comp, SonobusAudioProcessor::CompressorParams &params) {}
        virtual void expanderHeaderClicked(ExpanderView *comp, const MouseEvent & event) {}
    };
    
    void addListener(Listener * listener) { listeners.add(listener); }
    void removeListener(Listener * listener) { listeners.remove(listener); }
    
    
    void setupLayout()
    {
        int minKnobWidth = 54;
        int minitemheight = 32;
        int knoblabelheight = 18;
        int knobitemheight = 62;

#if JUCE_IOS
        // make the button heights a bit more for touchscreen purposes
        minitemheight = 40;
        knobitemheight = 80;
#endif
        
        threshBox.items.clear();
        threshBox.flexDirection = FlexBox::Direction::column;
        threshBox.items.add(FlexItem(minKnobWidth, knoblabelheight, thresholdLabel).withMargin(0).withFlex(0));
        threshBox.items.add(FlexItem(minKnobWidth, knobitemheight, thresholdSlider).withMargin(0).withFlex(1));
        
        ratioBox.items.clear();
        ratioBox.flexDirection = FlexBox::Direction::column;
        ratioBox.items.add(FlexItem(minKnobWidth, knoblabelheight, ratioLabel).withMargin(0).withFlex(0));
        ratioBox.items.add(FlexItem(minKnobWidth, knobitemheight, ratioSlider).withMargin(0).withFlex(1));
        
        attackBox.items.clear();
        attackBox.flexDirection = FlexBox::Direction::column;
        attackBox.items.add(FlexItem(minKnobWidth, knoblabelheight, attackLabel).withMargin(0).withFlex(0));
        attackBox.items.add(FlexItem(minKnobWidth, knobitemheight, attackSlider).withMargin(0).withFlex(1));
        
        releaseBox.items.clear();
        releaseBox.flexDirection = FlexBox::Direction::column;
        releaseBox.items.add(FlexItem(minKnobWidth, knoblabelheight, releaseLabel).withMargin(0).withFlex(0));
        releaseBox.items.add(FlexItem(minKnobWidth, knobitemheight, releaseSlider).withMargin(0).withFlex(1));

        
        
        checkBox.items.clear();
        checkBox.flexDirection = FlexBox::Direction::row;
        checkBox.items.add(FlexItem(5, 5).withMargin(0).withFlex(0));
        checkBox.items.add(FlexItem(minitemheight, minitemheight, enableButton).withMargin(0).withFlex(0));
        checkBox.items.add(FlexItem(2, 5).withMargin(0).withFlex(0));
        checkBox.items.add(FlexItem(100, minitemheight, titleLabel).withMargin(0).withFlex(1));

        headerComponent.headerBox.items.clear();
        headerComponent.headerBox.flexDirection = FlexBox::Direction::column;
        headerComponent.headerBox.items.add(FlexItem(150, minitemheight, checkBox).withMargin(0).withFlex(1));

        
        knobBox.items.clear();
        knobBox.flexDirection = FlexBox::Direction::row;
        knobBox.items.add(FlexItem(6, 5).withMargin(0).withFlex(0));
        knobBox.items.add(FlexItem(minKnobWidth, knobitemheight + knoblabelheight, threshBox).withMargin(0).withFlex(1));
        knobBox.items.add(FlexItem(minKnobWidth, knobitemheight + knoblabelheight, ratioBox).withMargin(0).withFlex(1));
        knobBox.items.add(FlexItem(minKnobWidth, knobitemheight + knoblabelheight, attackBox).withMargin(0).withFlex(1));
        knobBox.items.add(FlexItem(minKnobWidth, knobitemheight + knoblabelheight, releaseBox).withMargin(0).withFlex(1));
        knobBox.items.add(FlexItem(6, 5).withMargin(0).withFlex(0));
        
        mainBox.items.clear();
        mainBox.flexDirection = FlexBox::Direction::column;
        //mainBox.items.add(FlexItem(150, minitemheight, checkBox).withMargin(0).withFlex(0));
        //mainBox.items.add(FlexItem(6, 2).withMargin(0).withFlex(0));
        mainBox.items.add(FlexItem(100, knoblabelheight + knobitemheight, knobBox).withMargin(0).withFlex(1));
        mainBox.items.add(FlexItem(6, 2).withMargin(0).withFlex(0));
        
        minBounds.setSize(jmax(180, minKnobWidth * 4 + 16), knobitemheight + knoblabelheight + 2);
        minHeaderBounds.setSize(jmax(180, minKnobWidth * 5 + 16),  minitemheight + 8);

    }

    juce::Rectangle<int> getMinimumContentBounds() const {
        return minBounds;
    }

    juce::Rectangle<int> getMinimumHeaderBounds() const {

        return minHeaderBounds;
    }

    
    void resized() override
    {
        mainBox.performLayout(getLocalBounds());
    }

    void buttonClicked (Button* buttonThatWasClicked) override
    {
        if (buttonThatWasClicked == &enableButton) {
            mParams.enabled = enableButton.getToggleState();
            headerComponent.repaint();
        }
        listeners.call (&ExpanderView::Listener::expanderParamsChanged, this, mParams);
        
    }
    
    void sliderValueChanged (Slider* slider) override
    {
        if (slider == &thresholdSlider) {
            mParams.thresholdDb = slider->getValue();
        }
        else if (slider == &ratioSlider) {
            mParams.ratio = slider->getValue();
        }
        else if (slider == &attackSlider) {
            mParams.attackMs = slider->getValue();
        }
        else if (slider == &releaseSlider) {
            mParams.releaseMs = slider->getValue();
        }
        listeners.call (&ExpanderView::Listener::expanderParamsChanged, this, mParams);
        
    }

    void updateParams(const SonobusAudioProcessor::CompressorParams & params) {
        mParams = params;
        
        thresholdSlider.setValue(mParams.thresholdDb, dontSendNotification);
        ratioSlider.setValue(mParams.ratio, dontSendNotification);
        attackSlider.setValue(mParams.attackMs, dontSendNotification);
        releaseSlider.setValue(mParams.releaseMs, dontSendNotification);
        
        enableButton.setToggleState(mParams.enabled, dontSendNotification);
    }
    
    const SonobusAudioProcessor::CompressorParams & getParams() const { 
        return mParams;         
    }
    
   
    
private:
    
    SonoBigTextLookAndFeel sonoSliderLNF;
    SonoBigTextLookAndFeel smallLNF;

    ListenerList<Listener> listeners;
    juce::Rectangle<int> minBounds;
    juce::Rectangle<int> minHeaderBounds;

    void configKnobSlider(Slider & slider) 
    {
        slider.setSliderStyle(Slider::SliderStyle::RotaryHorizontalVerticalDrag);
        slider.setTextBoxStyle(Slider::TextBoxAbove, true, 60, 14);
        slider.setMouseDragSensitivity(128);
        slider.setScrollWheelEnabled(false);
        slider.setTextBoxIsEditable(true);
        slider.setSliderSnapsToMousePosition(false);
        //slider->setPopupDisplayEnabled(true, false, this);
        slider.setColour(Slider::textBoxBackgroundColourId, Colours::transparentBlack);
        slider.setColour(Slider::textBoxOutlineColourId, Colours::transparentBlack);
        slider.setColour(Slider::textBoxTextColourId, Colour(0x90eeeeee));
        slider.setColour(TooltipWindow::textColourId, Colour(0xf0eeeeee));
        slider.setLookAndFeel(&sonoSliderLNF);
    }
    
    void configLabel(Label & label) 
    {
        label.setFont(12);
        label.setColour(Label::textColourId, Colour(0xa0eeeeee));
        label.setJustificationType(Justification::centred);
        label.setMinimumHorizontalScale(0.3);
    }
    
    SonoDrawableButton enableButton = { "enable", DrawableButton::ButtonStyle::ImageFitted };
    
    Slider thresholdSlider;
    Slider ratioSlider;
    Slider attackSlider;
    Slider releaseSlider;

    Label titleLabel;
    Label thresholdLabel;
    Label ratioLabel;
    Label attackLabel;
    Label releaseLabel;

    HeaderComponent headerComponent = { *this };

    
    FlexBox mainBox;
    FlexBox checkBox;
    FlexBox knobBox;
    FlexBox threshBox;
    FlexBox ratioBox;
    FlexBox attackBox;
    FlexBox releaseBox;


    SonobusAudioProcessor::CompressorParams mParams;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ExpanderView)
};