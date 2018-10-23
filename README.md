# Linux kernel FL2000DX/IT66121FN dongle DRM driver

Clean re-implementation of FrescoLogic FL2000DX DRM driver and ITE Tech IT66121F driver, allowing to enable full display controller capabilities for [USB-to-HDMI dongle based on such chips](https://www.aliexpress.com/item/HD-1080P-USB-3-0-To-HDMI-External-Video-Graphic-Card-Multi-Display-Cable-Adapter-Converter/32808836824.html?spm=a2g0s.9042311.0.0.4a9f4c4dow19O6) in Linux

## FL2000DX
The FL2000DX is Fresco Logic’s USB 3.0 Display device controller. It integrates Fresco Logic’s display transfer engine, USB 3.0 device controller, USB 3.0 transceiver, and a VGA (D-Sub) DAC. Fresco Logic’s display transfer engine is designed with Fresco Logic’s proprietary architecture and processes the video stream optimally for USB 3.0 bandwidth. The high performance video DAC provides clear and crisp display quality, and supports full HD (1920×1080) resolution. The integrated USB 3.0 device controller and transceiver were developed in conjunction with the de-facto standard Fresco USB 3.0 host controllers, which ensures the best performance and compatibility.

## IT66121FN
The IT66121 is a high-performance and low-power single channel HDMI transmitter, fully compliant with HDMI 1.3a, HDCP 1.2 and backward compatible to DVI 1.0 specifications. IT66121 also provide the HDMI1.4 3D feature, which enables direct 3D displays through an HDMI link. The IT66121 serves to provide the most cost-effective HDMI solution for DTV-ready consumer electronics such as set-top boxes, DVD players and A/V receivers, as well as DTV-enriched PC products such as notebooks and desktops, without compromising the performance. Its backward compatibility to DVI standard allows connectivity to myriad video displays such as LCD and CRT monitors, in addition to the ever-so-flourishing Flat Panel TVs.

## Limitations
 * USB2.0 is not supported
 * Connecting more than one dongles to the same bus is not supported
 * HDMI CEC is not supported
 * HDMI Audio is not supported
 * HDCP is not supported
 * Internal SPI flash access is not supported
 * Only big-endian hosts are supported
 * 32-bit hosts are not supported

## Upstreaming
This is not going to happen - both chips are poorly supported by their vendors, if at all

## Sources
 * original driver by FrescoLogic: https://github.com/FrescoLogic/FL2000
 * major clean-up by Hans Ulli Kroll: https://github.com/ulli-kroll/FL2000
 * reference IT66121FN driver ftom RK3188 Android KK kernel repositpry: https://github.com/phjanderson/Kernel-3188
