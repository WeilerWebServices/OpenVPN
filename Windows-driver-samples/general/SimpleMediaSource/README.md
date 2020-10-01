<!---
    name: SimpleMediaSource Sample
    platform: UMDF2
    language: cpp
    category: general 
    description: Demonstrates how to write a custom media source and driver package.
    samplefwlink: 
--->

SimpleMediaSource Sample
========================
This sample demonstrates how to create a custom media source and driver package that can be installed as a Camera and produce frames. 

This sample has accompanying documentation here: https://docs.microsoft.com/en-us/windows-hardware/drivers/stream/frame-server-custom-media-source


Contents
--------
MediaSource - COM DLL project for the custom media source
SimpleMediaSourceDriver - UMDF driver install package 

Installation
------------------

1. Build the solution.

2. Navigate to the output folder, e.g. Windows-driver-samples\general\SimpleMediaSource\x64\Release, and the driver package will be in a directory also called SimpleMediaSourceDriver. Check that the folder has `SimpleMediaSource.dll`, `simplemediasourcedriver.cat`, `SimpleMediaSourceDriver.dll`, and `SimpleMediaSourceDriver.inf`. 

3. Deploy the driver package with 

`devcon dp_add SimpleMediaSourceDriver.inf`

4. In Device Manager, locate **SimpleMediaSource Capture Source**, under the Camera category. Open the Microsoft Camera App, switch cameras if necessary until the camera is streaming from the SimpleMediaSource. You should see a scrolling black and white gradient. 