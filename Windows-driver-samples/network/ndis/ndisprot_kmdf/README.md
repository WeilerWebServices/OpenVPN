---
topic: sample
description: Demonstrates a connection-less NDIS 6.0 protocol KMDF driver.
languages:
- cpp
products:
- windows
---

<!---
    name: Connection-less NDIS 6.0 Protocol KMDF Sample Driver
    platform: KMDF
    language: cpp
    category: Network
    description: Demonstrates a connection-less NDIS 6.0 protocol KMDF driver.
    samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=620197
--->

# Connection-less NDIS 6.0 Protocol KMDF Sample Driver

This sample demonstrates a connection-less NDIS 6.0 protocol KMDF driver.

The driver supports sending and receiving raw Ethernet frames using ReadFile/WriteFile calls from user-mode. As an NDIS protocol, it illustrates how to establish and tear down bindings to Ethernet adapters, i.e. those that export medium type *NdisMedium802\_3*. It shows how to set a packet filter, send and receive data, and handle plug-and-play events.

The sample also demonstrates how to write a Notify Object dll. The Notify Object is used for calling into the Wdf Coinstaller to install and load the framework library.

Related technologies
--------------------

[Creating Framework-based Miniport Drivers](http://msdn.microsoft.com/en-us/library/windows/hardware/ff540778)

Build the sample
----------------

For information on how to build a driver solution using Microsoft Visual Studio, see [Building a Driver](http://msdn.microsoft.com/en-us/library/windows/hardware/ff554644).

The 60 subdirectory (\\ndisprot\_kmdf\\60) indicates that the built sample will be NDIS 6.0 compatible.

Installation
------------

Use the following steps to install the sample.

1.  When you build the sample, the build engine produces ndisprot.inf in the build target directory. Copy nprt6wdf.sys, protnotify.dll, and ndisprot.inf to a directory.
2.  Copy the KMDF coinstaller (wdfcoinstaller*MMmmm*.dll) to the same directory.

    **Note** You can obtain redistributable framework updates by downloading the *wdfcoinstaller.msi* package from [WDK 8 Redistributable Components](http://go.microsoft.com/fwlink/p/?LinkID=226396). This package performs a silent install into the directory of your Windows Driver Kit (WDK) installation. You will see no confirmation that the installation has completed. You can verify that the redistributables have been installed on top of the WDK by ensuring there is a redist\\wdf directory under the root directory of the WDK, %ProgramFiles(x86)%\\Windows Kits\\8.0.

3.  In Control Panel, in the **Network and Internet** group, open **Network Connections**, select an adapter, and then open **Properties**.

4.  Click **Install**, and then click **Protocol**.

5.  Click **Add**, and then click **Have disk**.

6.  Point to the location of the INF file and driver, click **Sample NDIS Protocol Driver**, and then click **OK**.

7.  After installing the protocol, copy the test application Uiotest.exe to a convenient location. Note that the driver service has been set to manual start in the INF file. As a result, it doesn't get loaded automatically when you install the driver.

Usage
-----

From an administrator command prompt, to start the driver, type **Net start ndisprot**.

To stop the driver, type **Net stop ndisprot**.

You can build Prottest.exe from source code located in the \\ndisprot\\6x\\test directory.

To test the NDIS 6.0 driver, run prottest. For help on usage, run **prottest -?**.

```
usage: PROTTEST [options] <devicename>
options:
 -e: Enumerate devices
 -r: Read
 -w: Write (default)
 -l <length>: length of each packet (default: 100)
 -n <count>: number of packets (defaults to infinity)
 -m <MAC address> (defaults to local MAC)
 -f Use a fake address to send out the packets.
```


Prottest exercises the IOCTLs supported by NDISPROT, and sends and/or receives data on the selected device. In order to use Prottest, the user must have administrative privilege. Users should pass down a buffer that is large enough to contain the data returned. If the length of the buffer passed down is smaller than the length of the received data, NDISPROT will only copy part of the data and discard the rest when the given buffer is full.

For an NDIS 6.0 driver, use the -e option on prottest to enumerate all devices to which NDISPROT is bound:

```
C:\prot>prottest -e
 0. \DEVICE\{9273DA7D-5275-4B9A-AC56-68A49D121F1F}
 - Intel-Based 10/100 Ethernet Card
```

The following command sends and receives 2 packets on a device. Since these packets are sent to the local MAC address (default), both packets are received. The device name parameter to prottest is picked up from the output of **prottest -e** (see above).

```
C:\prot>prottest -n 2 \DEVICE\{9273DA7D-5275-4B9A-AC56-68A49D121F1F}
DoWriteProc: finished sending 2 packets of 100 bytes each
DoReadProc finished: read 2 packets
```

For security reasons, this driver does not allow packets with fake MAC addresses to be sent from user-mode applications.

With a checked version of ndisprot.sys, you can control the volume of debug information generated by changing the variable **ndisprotDebugLevel**. Refer to debug.h for more information.

File Manifest
-------------

**Directory: 60**

File | Description 
-----|------------
debug.c | Routines to aid debugging
debug.h | Debug macro definitions
excallbk.c | Handles load order dependency between this sample and NDISWDM sample
macros.h | Spinlock, event, referencing macros
ndisbind.c | NDIS protocol entry points to handle binding/unbinding from adapters
ndisprot.h | Data structure definitions
precomp.h | Contains the precompiled headers
protuser.h | Has the definitions of ioctls issued by protuser.exe application used on NDIS 6.0 
nuiouser.h | Has the definitions of ioctls issued by nuiouser.exe application used on NDIS 5.0 
ndisprot.inf | INF file for installing NDISPROT
ntdisp.c | NT Entry points and dispatch routines for NDISPROT
recv.c | NDIS protocol entry points for receiving data, and IRP_MJ_READ processing

**Directory: NotifyOb**

File | Description 
-----|------------
Common.hpp | Header file containing the common include files for the project
dllmain.cpp |  Handles loading/unloading of Wdf Coinstaller and the notify object dll
ProtNotify.idl |  Defines the interfaces for the notify object dll
ProtNotify.rc |  Resource file for the notify object dll
