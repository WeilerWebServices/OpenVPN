---
topic: sample
description: This filter is a transaction-aware file scanner that examines data in files.
languages:
- cpp
products:
- windows
---

<!---
    name: AvScan File System Minifilter Driver
    platform: WDM
    language: cpp
    category: FileSystem
    description: This filter is a transaction-aware file scanner that examines data in files.
    samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=617644
--->

# AvScan File System Minifilter Driver

The AvScan minifilter is a transaction-aware file scanner. This is an example for developers who intend to write filters that examine data in files. Typically, anti-virus products fall into this category.

## Universal Windows Driver Compliant

This sample builds a Universal Windows Driver. It uses only APIs and DDIs that are included in OneCoreUAP.
