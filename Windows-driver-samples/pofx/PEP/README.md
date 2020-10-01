---
topic: sample
description: Demonstrates an interface which allows PEP to implement ACPI runtime methods natively via a driver.
languages:
- cpp
products:
- windows
---

<!---
    name: Power Engine Plugin (PEP) ACPI Sample
    platform: KMDF
    language: cpp
    category: ACPI Power
    description: Demonstrates an interface which allows PEP to implement ACPI runtime methods natively via a driver.
    samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=620311
--->

# PEP ACPI Sample

The Power Engine Plugin (PEP) provides interfaces for platform power management including device power management (DPM), processor power management (PPM), and, starting with Windows 10, ACPI runtime methods. This sample documents the latter: an interface which allows a PEP to implement ACPI runtime methods natively via a Windows driver rather than firmware (AML). A PEP using the ACPI interface is often called a Platform Extension. Note that this interface can be used independently or in conjunction with the DPM and/or PPM interfaces as appropriate.

Use the PEP ACPI interface if:

* You want to write peripheral (off-SoC) device power management in C rather than in ACPI Source Language (ASL).
* You need to override an ACPI method which exists in a platform's DSDT or SSDT firmware tables.
* Shipping, maintaining, and updating a driver binary suits your platform better than firmware updates (note that you will still need FADT, MADT, DBG2, and so on in firmware - this interface is only for runtime methods).
