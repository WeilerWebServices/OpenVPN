---
topic: sample
description: Demonstrates a simple controller-less modem driver.
languages:
- cpp
products:
- windows
---

<!---
    name: Fakemodem Driver
    platform: KMDF
    language: cpp
    category: Network
    description: Demonstrates a simple controller-less modem driver.
    samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=617733
--->

# Fakemodem Driver

The Fakemodem sample demonstrates a simple controller-less modem driver. This driver supports sending and receiving AT commands using the `ReadFile`/`WriteFile` calls or via a TAPI interface using an application such as *HyperTerminal.*

## Universal Windows Driver Compliant

This sample builds a Universal Windows Driver. It uses only APIs and DDIs that are included in OneCoreUAP.
