---
topic: sample
description: A driver MFT for use with a camera's Windows device app.
languages:
- cpp
products:
- windows
---

<!---
    name: Driver MFT Sample
    platform: WDM
    language: cpp
    category: Camera
    description: A driver MFT for use with a camera's UWP device app.
    samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=617126
--->

# Driver MFT Sample

Provides a *driver MFT* for use with a camera's UWP device app.A *driver MFT* is a Media Foundation Transform that's used with a specific camera when capturing video. The driver MFT is also known as MFT0 because it is the first MFT applied to the video stream captured from the camera. This MFT can provide a video effect or other processing when capturing photos or video from the camera. It can be distributed along with the driver package for a camera.

In this sample, the driver MFT, when enabled, replaces a portion of the captured video with a green box. To test this sample, download the [UWP device app for camera sample](http://go.microsoft.com/fwlink/p/?linkid=249442) and the [Camera Capture UI sample](http://go.microsoft.com/fwlink/p/?linkid=249441). The [UWP device app for camera sample](http://go.microsoft.com/fwlink/p/?linkid=249442) provides a *UWP device app* that controls the effect implemented by the driver MFT. The [Camera Capture UI sample](http://go.microsoft.com/fwlink/p/?linkid=249441) provides a way to invoke the *UWP device app*.

This sample is designed to be used with a specific camera. To run the sample, you need the your camera's device ID and device metadata package.

Related topics
--------------

**Concepts**

[UWP device apps for cameras](http://go.microsoft.com/fwlink/p/?LinkId=306683)

[Media Foundation Transforms](http://msdn.microsoft.com/en-us/library/windows/hardware/ms703138)

[Roadmap for Developing Streaming Media Drivers](http://msdn.microsoft.com/en-us/library/windows/hardware/ff568130)

[Universal camera driver design guide for Windows 10](https://msdn.microsoft.com/en-us/Library/Windows/Hardware/dn937080)

**Samples**

[Device app for camera sample](http://go.microsoft.com/fwlink/p/?linkid=249442)

[Camera Capture UI sample](http://go.microsoft.com/fwlink/p/?linkid=249441%20)
