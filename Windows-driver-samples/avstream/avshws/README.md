---
topic: sample
description: A simulated hardware sample driver providing a pin-centric capture driver to simulate AV capture hardware.
languages:
- cpp
products:
- windows
---

<!---
    name: AVStream simulated hardware sample driver (Avshws)
    platform: WDM
    language: cpp
    category: Camera
    description: A simulated hardware sample driver providing a pin-centric capture driver to simulate AV capture hardware.
    samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=620185
--->

# AVStream simulated hardware sample driver (Avshws)

The AVStream simulated hardware sample driver (Avshws) provides a pin-centric [AVStream](http://msdn.microsoft.com/en-us/library/windows/hardware/ff554240) capture driver for a simulated piece of hardware. This streaming media driver performs video captures at 320 x 240 pixels in either RGB24 or YUV422 format using direct memory access (DMA) into capture buffers. The purpose of the sample is to demonstrate how to write a pin-centric AVStream minidriver. The sample also shows how to implement DMA by using the related functionality provided by the AVStream class driver.

This sample features enhanced parameter validation and overflow detection.

Provision a target computer
---------------------------

After you've installed the sample on your host computer, run Visual Studio, and from the **File** menu, select **Open**, then **Project/Solution...**, navigate to the directory where you've copied the Avshws sample, then to the C++ folder, and select **avshws.vcxproj** (the VC++ Project).

In the **Solution Explorer** pane in Visual Studio, at the top is **Solution 'avshws'**. Right-click this and select **Configuration Manager**. Follow the instructions in [Building a Driver with the WDK](http://msdn.microsoft.com/en-us/library/windows/hardware/ff554644) to set the platform, operating system, and debug configuration you want to use, and to build the sample. This sample project will automatically sign the driver package.

Provision your target computer using instructions in, for example, [Preparing a Computer for Provisioning](http://msdn.microsoft.com/en-us/library/windows/hardware/dn265573). Ensure that in the **Network and Sharing Center** control panel your target computer has **Network Discovery** and **File and Printer Sharing** enabled.

Deploy the driver to the target computer
----------------------------------------

Now you can deploy the Avshws driver that you've just built to the target computer, using guidance in [Deploying a Driver to a Test Computer](http://msdn.microsoft.com/en-us/library/windows/hardware/hh454834). Specifically, find the package file under the **Package** folder in the Avshws solution. Right-click **package** and select **Properties**. Under Configuration Properties, click **Driver install** and then **Deployment**. Here you must click the check box for **Enable deployment**, and then click the button to the right of **\<Configure Computer...\>**. In the next dialog you enter the **Target Computer Name** and can let the host computer automatically provision the target computer and set up debugger options.

Finally, in Visual Studio, from the **Build** menu select **Deploy Solution** to deploy the sample to the target computer. On the target computer, you can see the deployed package in the **%Systemdrive%\\drivertest\\drivers** folder.

Install the driver
------------------

On the target computer, open Device Manager, and follow these steps:

1.  In the **Action** menu, click **Add Legacy Hardware**, and the **Add Hardware Wizard** appears. Click **Next** and then **Next** again.
2.  In the **Add Hardware** window, select **Show All Devices**.
3.  In the **Manufacturer** list in the left pane, click **Microsoft**.
4.  You should see the **AVStream Simulated Hardware Sample** in the **Model** pane on the right. Click this and then click **Next**.
5.  Click **Next** again to install the driver, and then click **Finish** to exit the wizard.

The sample driver now appears in the Device Manager console tree under **Sound, video and game controllers**. The Avshws INF file will be on the system drive at, for example, **...windows\\System32\\DriverStore\\FileRepository\\**.

Sample code hierarchy
---------------------

[**DriverEntry**](http://msdn.microsoft.com/en-us/library/windows/hardware/ff558717) in Device.cpp is the initial point of entry into the driver. This routine passes control to AVStream by calling the [**KsInitializeDriver**](http://msdn.microsoft.com/en-us/library/windows/hardware/ff562683) function. In this call, the minidriver passes the device descriptor, an AVStream structure that recursively defines the AVStream object hierarchy for a driver. This is common behavior for an AVStream minidriver.

At device start time, a simulated piece of capture hardware is created (the **CHardwareSimulation** class), and a DMA adapter is acquired from the operating system and is registered with AVStream by calling the [**KsDeviceRegisterAdapterObject**](http://msdn.microsoft.com/en-us/library/windows/hardware/ff561687) function. This call is required for a sample that performs DMA access directly into the capture buffers, instead of using DMA access to write to a common buffer. The driver creates the [KS Filter](http://msdn.microsoft.com/en-us/library/windows/hardware/ff567644) for this device dynamically by calling the [**KsCreateFilterFactory**](http://msdn.microsoft.com/en-us/library/windows/hardware/ff561650) function.

Filter.cpp is where the sample lays out the [**KSPIN\_DESCRIPTOR\_EX**](http://msdn.microsoft.com/en-us/library/windows/hardware/ff563534) structure for the single video pin. In addition, a [**KSFILTER\_DISPATCH**](http://msdn.microsoft.com/en-us/library/windows/hardware/ff562554) structure and a [**KSFILTER\_DESCRIPTOR**](http://msdn.microsoft.com/en-us/library/windows/hardware/ff562553) structure are provided in this source file. The filter dispatch provides only a create dispatch, a routine that is included in Filter.cpp. The process dispatch is provided on the pin because this is a pin-centric sample.

Capture.cpp contains source for the video capture pin on the capture filter. This is where the [**KSPIN\_DISPATCH**](http://msdn.microsoft.com/en-us/library/windows/hardware/ff563535) structure for the unique pin is provided. This dispatch structure specifies a *Process* callback routine, also defined in this source file. This routine is where stream pointer manipulation and cloning occurs.

The process callback is one of two routines of interest in Capture.cpp that demonstrate how to perform DMA transfers with AVStream functionality. The other is the **CCapturePin::CompleteMappings** method. These two methods show how to use the queue, obtain clone pointers, use scatter/gather lists, and perform other DMA-related tasks.

For more information, see the comments in all .cpp files.

Run the sample
--------------

Follow these steps to see how the sample driver functions:

1.  After installation has completed, access the driver through the Graphedt tool. Graphedt.exe is available in the *tools* directory of the WDK.
2.  Before running GraphEdit, use the regsvr32 utility to register the proppage.dll DLL and to enable GraphEdit to display property pages for some of the built-in Microsoft DirectShow filters. Open an elevated command window with Administrator privileges, and navigate to the WDK or SDK *tools* directory that contains proppage.dll.
3.  On the command line, type regsvr32 proppage.dll. If the registration succeeds, you'll get a message, "DllRegisterServer in proppage.dll succeeded." Click OK.
4.  In the Graphedt tool, click the **Graph** menu and click **Insert Filters**. The sample appears under "WDM Streaming Capture Devices" as "avshws Source."
5.  Click **Insert Filter**. The sample appears in the graph as a single filter labeled, "avshws Source." There is one output pin, which is the video capture pin. This pin emits video in YUY2 format.
6.  Attach this filter to either a DirectShow Video Renderer or to the VMR default video renderer. Then click **Play**.

The output that is produced by the sample is a 320 x 240 pixel image of standard EIA-189-A color bars. In the middle of the image near the bottom, a clock appears over the image. This clock displays the elapsed time since the graph was introduced into the run state following the last stop. The clock display format is MINUTES:SECONDS.HUNDREDTHS.

In the upper-left corner of the image, a counter counts the number of frames that have been dropped since the graph was introduced into the run state after the last stop.

Code tour
---------

### File Manifest

File | Description
-----|------------
Avshws.h | Main header file for the sample
Avshws.inf | Sample installation file
