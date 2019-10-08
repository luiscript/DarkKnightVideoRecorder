/*
 Copyright (C) 2019 Luis Fernando García [http://luiscript.com]
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
 */



#include "DarkKnightVideoRecorder.hpp"


void DarkKnightVideoRecorder::setup()
{
    drawFbo = false;
    fbo = nullptr;
    fps = 30;
    seconds = 10;
    exporting = false;
    exportingPath = "export/";
    prefix = "frame_";
    addInputConnection(ConnectionType::DK_FBO);
    recorder = make_shared<ofxFboRecorder>(seconds, fps);
}

void DarkKnightVideoRecorder::update()
{
    if(recorder->isRecording())
    {
        recorder->addFrame(*fbo, exportingPath + prefix);
    }
    else if(!exporting)
    {
        gui->getHeader()->setLabel("VIDEO RECORDER");
    }
}

void DarkKnightVideoRecorder::draw()
{
    
}

void DarkKnightVideoRecorder::setFbo(ofFbo * fboPtr)
{
    fbo  = fboPtr;
    drawFbo = fboPtr != nullptr;
}

void DarkKnightVideoRecorder::addModuleParameters()
{
    ofxDatGuiButton * record = gui->addButton("START RECORDING");
    record->setLabelAlignment(ofxDatGuiAlignment::CENTER);
    record->onButtonEvent(this, &DarkKnightVideoRecorder::startRecording);
    
    ofxDatGuiButton * exportFrames = gui->addButton("EXPORT VIDEO");
    exportFrames->setLabelAlignment(ofxDatGuiAlignment::CENTER);
    exportFrames->onButtonEvent(this, &DarkKnightVideoRecorder::exportVideo);
    
    auto config = gui->addFolder("SETTINGS");
    
    ofxDatGuiTextInput * fpsInput = config->addTextInput("FPS", "60");
    fpsInput->onTextInputEvent(this, &DarkKnightVideoRecorder::onFpsChange);
    ofxDatGuiTextInput * secondsInput = config->addTextInput("SECONDS", "10");
    secondsInput->onTextInputEvent(this, &DarkKnightVideoRecorder::onSecondsChange);
}

void DarkKnightVideoRecorder::onFpsChange(ofxDatGuiTextInputEvent e)
{
    fps = ofToInt(e.target->getText());
}

void DarkKnightVideoRecorder::onSecondsChange(ofxDatGuiTextInputEvent e)
{
    seconds = ofToInt(e.target->getText());
}

void DarkKnightVideoRecorder::startRecording(ofxDatGuiButtonEvent e)
{
    recorder = make_shared<ofxFboRecorder>(seconds, fps);
    gui->getHeader()->setLabel("VIDEO RECORDER: RECORDING");
    recorder->start();
}

void DarkKnightVideoRecorder::exportVideo(ofxDatGuiButtonEvent e)
{
    gui->getHeader()->setLabel("VIDEO RECORDER: EXPORTING");
    exporting = true;
    startThread();
}

void DarkKnightVideoRecorder::threadedFunction()
{
    string path = ofFilePath::getAbsolutePath( ofToDataPath("") );
    string command = "/libs/ffmpeg -framerate " + ofToString(fps) + " -i " + path + "export/frame_%d.png -crf 10 -pix_fmt yuv420p -c:a copy -tune grain -c:v hap -format hap_alpha " + path + "export.mov";
    string full = path + command;
    system(full.c_str());
    stopThread();
    exporting = false;
    gui->getHeader()->setLabel("VIDEO RECORDER");
}
