---
topic: sample
description: Contains a console test application and a series of iterative drivers for both KMDF and UMDF version 1.
languages:
- cpp
products:
- windows
---

<!---
    name: WDF Sample Driver Learning Lab for OSR USB-FX2
    platform: UMDF1 KMDF
    language: cpp
    category: USB
    description: Contains a console test application and a series of iterative drivers for both KMDF and UMDF version 1.
    samplefwlink: https://go.microsoft.com/fwlink/?linkid=856746
--->

# WDF Sample Driver Learning Lab for OSR USB-FX2

The wdf\_osrfx2\_lab sample contains a console test application and a series of iterative drivers for both Kernel-Mode Driver Framework (KMDF) and User-Mode Driver Framework (UMDF) version 1.

In the Windows Driver Kit (WDK) for Windows 7 and earlier versions of Windows, the osrusbfx2 sample demonstrated how to perform bulk and interrupt data transfers to an USB device. The sample was written for the OSR USB-FX2 Learning Kit.

Starting in Windows 8.1, the osrusbfx2 sample has been divided into these samples:

-   wdf\_osrfx2: This sample is a series of iterative drivers that demonstrate how to write a "Hello World" driver and adds additional features in each step.

-   [kmdf\_fx2](gallery_samples.123a_gallery#1): This sample is the final version of kernel-mode wdf\_osrfx2 driver. The sample demonstrates KMDF methods.

-   [umdf\_fx2](http://msdn.microsoft.com/en-us/library/windows/hardware/): This sample is the final version of the user-mode driver wdf\_osrfx2. The sample demonstrates UMDF methods.

This sample is written for the OSR USB-FX2 Learning Kit. The specification for the device is at <http://www.osronline.com/hardware/OSRFX2_32.pdf>.


Related topics
--------------

[kmdf\_fx2](http://msdn.microsoft.com/en-us/library/windows/hardware/)

[umdf\_fx2](http://msdn.microsoft.com/en-us/library/windows/hardware/)


Build the sample
----------------

The default Solution build configuration is Windows 8.1 Debug and Win32. You can change the default configuration to build for Windows 8 or Windows 7 version of the operating system.

**To select a configuration and build a driver**

1.  Open the driver project or solution in Visual Studio 2013 (find *filtername*.sln or *filtername*.vcxproj).
2.  Right-click the solution in the **Solutions Explorer** and select **Configuration Manager**.
3.  From the **Configuration Manager**, select the **Active Solution Configuration** (for example, Windows 8.1 Debug or Windows 8.1 Release) and the **Active Solution Platform** (for example, Win32) that correspond to the type of build you are interested in.
4.  Each driver project in this iterative sample creates a binary with the same name, osrusbfx2.sys. As a result, you can build only the single project you're currently working on, as well as the package project. You can do this by selecting only these two projects in **Configuration Manager**.
5.  From the **Build** menu, click **Build Solution** (Ctrl+Shift+B).

Overview
--------

Here is the overview of the device:

-   The device is based on the development board supplied with the Cypress EZ-USB FX2 Development Kit (CY3681).
-   It contains 1 interface and 3 endpoints (Interrupt IN, Bulk Out, Bulk IN).
-   Firmware supports vendor commands to query or set LED Bar graph display and 7-segment LED display, and to query toggle switch states.
-   Interrupt Endpoint:
    -   Sends an 8-bit value that represents the state of the switches.
    -   Sent on startup, resume from suspend, and whenever the switch pack setting changes.
    -   Firmware does not de-bounce the switch pack.
    -   One switch change can result in multiple bytes being sent.
    -   Bits are in the reverse order of the labels on the pack (for example, bit 0x80 is labeled 1 on the pack).
-   Bulk Endpoints are configured for loopback:
    -   The device moves data from IN endpoint to OUT endpoint.
    -   The device does not change the values of the data it receives nor does it internally create any data.
    -   Endpoints are always double buffered.
    -   Maximum packet size depends on speed (64 full speed, 512 high speed).

Sample Contents for KMDF
------------------------

The KMDF sample contains a console test application and a series of drivers. The driver is iterative as a series of steps, starting with a basic "Hello World" driver. Each step is describe in the following table.

<table>
<colgroup>
<col width="50%" />
<col width="50%" />
</colgroup>
<thead>
<tr class="header">
<th align="left">Folder
Description</th>
</tr>
</thead>
<tbody>
<tr class="odd">
<td align="left">usb\wdf_osrfx2_lab\kmdf\step1
The most basic step. The source file contains a minimal amount of code to get the driver loaded in memory and respond to PnP and Power events. You can install, uninstall, disable, enable, suspend, and resume the system.</td>
<td align="left">usb\wdf_osrfx2_lab\kmdf\step2
<ol>
<li>Creates a context with the WDFDEVICE object.</li>
<li>Initializes the USB device by registering a <em>EvtPrepareHardware</em> callback.</li>
<li>Registers an interface so that application can open a handle to the device.</li>
</ol></td>
</tr>
</tbody>
</table>

Testing the driver
------------------

The sample includes a test application, osrusbfx2.exe, that you can use to test the device. This console application enumerates the interface registered by the driver and opens the device to send read, write, or IOCTL requests based on the command line options.

Usage for Read/Write test:

-   -r [*n*], where *n* is number of bytes to read.
-   -w [*n*], where *n* is number of bytes to write.
-   -c [*n*], where *n* is number of iterations (default = 1).
-   -v, shows verbose read data.
-   -p, plays with Bar Display, Dip Switch, 7-Segment Display.
-   -a, performs asynchronous I/O operation.
-   -u, dumps USB configuration and pipe information.

**Playing with the 7 segment display, toggle switches and bar graph display**

Use the command **osrusbfx2.exe -p** with options 1-9 to set and clear bar graph display, set and get 7-segment state, and read the toggle switch states. The following list shows the function options:

1.  Light bar
2.  Clear bar
3.  Light entire bar graph
4.  Clear entire bar graph
5.  Get bar graph state
6.  Get switch state
7.  Get switch interrupt message
8.  Get 7-segment state
9.  Set 7-segment state
10. Reset the device
11. Re-enumerate the device

0. Exit

Selection:

**Reset and re-enumerate the device**

Use the command **osrusbfx2.exe -p** with option 10 and 11 to either reset the device or re-enumerate the device.

**Read and write to bulk endpoints**

The following commands send read and write requests to the device's bulk endpoint.

-   `osrusbfx2.exe -r 64`

    The preceding command reads 64 bytes to the bulk IN endpoint.

-   `osrusbfx2.exe -w 64 `

    The preceding command writes 64 bytes to the bulk OUT endpoint.

-   `osrusbfx2.exe -r 64 -w 64 -c 100 -v`

    The preceding command first writes 64 bytes of data to bulk OUT endpoint (Pipe 1), then reads 64 bytes from bulk IN endpoint (Pipe 2), and then compares the read buffer with write buffer to see if they match. If the buffer contents match, it repeats this operation 100 times.

-   `osrusbfx2.exe -a`

    The preceding command reads and writes to the device asynchronously in an infinite loop.

The bulk endpoints are double buffered. Depending on the operational speed (full or high), the buffer size is either 64 bytes or 512 bytes, respectively. A request to read data doesn't complete if the buffers are empty. If the buffers are full, a request to write data does not complete until the buffers are emptied. When you are doing a synchronous read, make sure the endpoint buffer has data (for example, when you send 512 bytes write request to the device operating in full speed mode). Because the endpoints are double buffered, the total buffer capacity is 256 bytes. The first 256 bytes fills the buffer and the write request waits in the USB stack until the buffers are emptied. If you run another instance of the application to read 512 bytes of data, both write and read requests complete successfully.

**Displaying descriptors**

The following command displays all the descriptors and endpoint information.

**osrusbfx2.exe -u**

If the device is operating in high speed mode, you get the following information:

`===================`

`USB_CONFIGURATION_DESCRIPTOR`

`bLength = 0x9, decimal 9`

`bDescriptorType = 0x2 ( USB_CONFIGURATION_DESCRIPTOR_TYPE )`

`wTotalLength = 0x27, decimal 39`

`bNumInterfaces = 0x1, decimal 1`

`bConfigurationValue = 0x1, decimal 1`

`iConfiguration = 0x4, decimal 4`

`bmAttributes = 0xa0 ( USB_CONFIG_BUS_POWERED )`

`MaxPower = 0x32, decimal 50`

`-----------------------------`

`USB_INTERFACE_DESCRIPTOR #0`

`bLength = 0x9`

`bDescriptorType = 0x4 ( USB_INTERFACE_DESCRIPTOR_TYPE )`

`bInterfaceNumber = 0x0`

`bAlternateSetting = 0x0`

`bNumEndpoints = 0x3`

`bInterfaceClass = 0xff`

`bInterfaceSubClass = 0x0`

`bInterfaceProtocol = 0x0`

`bInterface = 0x0`

`------------------------------`

`USB_ENDPOINT_DESCRIPTOR for Pipe00`

`bLength = 0x7`

`bDescriptorType = 0x5 ( USB_ENDPOINT_DESCRIPTOR_TYPE )`

`bEndpointAddress= 0x81 ( INPUT )`

`bmAttributes= 0x3 ( USB_ENDPOINT_TYPE_INTERRUPT )`

`wMaxPacketSize= 0x49, decimal 73`

`bInterval = 0x1, decimal 1`

`------------------------------`

`USB_ENDPOINT_DESCRIPTOR for Pipe01`

`bLength = 0x7`

`bDescriptorType = 0x5 ( USB_ENDPOINT_DESCRIPTOR_TYPE )`

`bEndpointAddress= 0x6 ( OUTPUT )`

`bmAttributes= 0x2 ( USB_ENDPOINT_TYPE_BULK )`

`wMaxPacketSize= 0x200, `

`decimal 512 bInterval = 0x0, `

`decimal 0`

`------------------------------`

`USB_ENDPOINT_DESCRIPTOR for Pipe02`

`bLength = 0x7`

`bDescriptorType = 0x5 ( USB_ENDPOINT_DESCRIPTOR_TYPE )`

`bEndpointAddress= 0x88 ( INPUT )`

`bmAttributes= 0x2 ( USB_ENDPOINT_TYPE_BULK )`

`wMaxPacketSize= 0x200, decimal 512`

`bInterval = 0x0, decimal 0`

If the device is operating in low speed mode, you will get the following information:

`===================`

`USB_CONFIGURATION_DESCRIPTOR`

`bLength = 0x9, decimal 9`

`bDescriptorType = 0x2 ( USB_CONFIGURATION_DESCRIPTOR_TYPE )`

`wTotalLength = 0x27, decimal 39`

`bNumInterfaces = 0x1, decimal 1`

`bConfigurationValue = 0x1, decimal 1`

`iConfiguration = 0x3, decimal 3`

`bmAttributes = 0xa0 ( USB_CONFIG_BUS_POWERED )`

`MaxPower = 0x32, decimal 50 `

`-----------------------------`

`USB_INTERFACE_DESCRIPTOR #0`

`bLength = 0x9`

`bDescriptorType = 0x4 ( USB_INTERFACE_DESCRIPTOR_TYPE )`

`bInterfaceNumber = 0x0 bAlternateSetting = 0x0`

`bNumEndpoints = 0x3`

`bInterfaceClass = 0xff`

`bInterfaceSubClass = 0x0`

`bInterfaceProtocol = 0x0`

`bInterface = 0x0`

`------------------------------`

`USB_ENDPOINT_DESCRIPTOR for Pipe00`

`bLength = 0x7`

`bDescriptorType = 0x5 ( USB_ENDPOINT_DESCRIPTOR_TYPE )`

`bEndpointAddress= 0x81 ( INPUT )`

`bmAttributes= 0x3 ( USB_ENDPOINT_TYPE_INTERRUPT )`

`wMaxPacketSize= 0x49, decimal 73`

`bInterval = 0x1, decimal 1`

`-------  -----------------------`

`USB_ENDPOINT_DESCRIPTOR for Pipe01`

`bLength = 0x7`

`bDescriptorType = 0x5 ( USB_ENDPOINT_DESCRIPTOR_TYPE )`

`bEndpointAddress= 0x6 ( OUTPUT )`

`bmAttributes= 0x2 ( USB_ENDPOINT_TYPE_BULK )`

`wMaxPacketSize= 0x40, decimal 64`

`bInterval = 0x0, decimal 0`

`------------------------------`

`USB_ENDPOINT_DESCRIPTOR for Pipe02`

`bLength = 0x7`

`bDescriptorType = 0x5 ( USB_ENDPOINT_DESCRIPTOR_TYPE )`

`bEndpointAddress= 0x88 ( INPUT )`

`bmAttributes= 0x2 ( USB_ENDPOINT_TYPE_BULK )`

`wMaxPacketSize= 0x40, decimal 64`

`bInterval = 0x0, decimal 0 `

Sample Contents for UMDF
------------------------

The UMDF sample driver is developed as a series of steps, starting with a basic "Hello World" driver. Each step progressively adds functionality to the previous step. Each step is described in the following table.

<table>
<colgroup>
<col width="50%" />
<col width="50%" />
</colgroup>
<thead>
<tr class="header">
<th align="left">Folder
Description</th>
</tr>
</thead>
<tbody>
<tr class="odd">
<td align="left">usb\wdf_osrfx2_lab\umdf\step1
The most basic step. The source file contains a minimal amount of code to get the driver loaded in memory and respond to PnP and Power events. You can install, uninstall, disable, enable, suspend, and resume the system.</td>
<td align="left">usb\wdf_osrfx2_lab\umdf\step2
<ol>
<li>The device registers a PnP device interface so that application can open a handle to the device.</li>
<li>The device object implements <strong>IPnpCallbackHardware</strong> interface and initializes USB I/O targets in <strong>IPnpCallbackHardware::OnPrepareHardware</strong> method.</li>
</ol></td>
</tr>
</tbody>
</table>