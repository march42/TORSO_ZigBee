# RF frequencies

## ISM radio bands

| center frequency | band width | notes |
| --- | --- | --- |
| 6.78 MHz | 30 kHz | |
| 13.56 MHz | 14 kHz | |
| 27.12 MHz | 326 kHz | |
| 40.68 MHz | 40 kHz | |
| 169.606 MHz | 412.5 kHz | only in EU |
| 433.92 MHz | 1.74 MHz | region 1 only |
| 915 MHz | 26 MHz | region 2 only |
| 2.45 GHz | 100 MHz | |
| 5.8 GHz | 150 MHz | |
| 24.125 GHz | 250 MHz | |
| 61.25 GHz | 500 MHz | |
| 122.5 GHz | 1 GHz | |
| 245 GHz | 2 GHz | |

## BlueTooth

- frequency hopping mechanism
- 2.4 GHz spectrum devided into 79 distinct RF channels 1 MHz band width
- channel n = 0 to 78; f=2.402 + n, MHz

| BlueTooth Channel | Frequency |
| --- | --- |
| ch.0 | 2402 MHz |
| ch.1 | 2403 MHz |
| ch.2 | 2404 MHz |
| ... | ... |
| ch.39 | 2441 MHz |
| ch.40 | 2442 MHz |
| ch.41 | 2443 MHz |
| ... | ... |
| ch.76 | 2478 MHz |
| ch.77 | 2479 MHz |
| ch.78 | 2480 MHz |

## BLE Bluetooth Low Energy

- 2.4 GHz spectrum; 40 channels; 2 MHz spread
- 3 advertising channels; 37 data channels
- advertising channels outside of 802.11 RF spectrum

| BLE Channel | Frequency | notes |
| --- | --- | --- |
| ch.37 | 2402 MHz | advertising channel |
| ch.0 | 2404 MHz | |
| ch.1 | 2406 MHz | |
| ... | ... | ... |
| ch.10 | 2424 MHz | |
| ch.38 | 2426 MHz | advertising channel |
| ch.11 | 2428 MHz | |
| ... | ... | ... |
| ch.36 | 2478 MHz | |
| ch.39 | 2480 MHz | advertising channel |

## ZigBee

868MHz band

| Zigbee Channel | Frequency |
| --- | --- |
| ch.0 | 868 MHz |

915MHz band

| Zigbee Channel | Frequency |
| --- | --- |
| ch.01 | 906 MHz |
| ch.02 | 908 MHz |
| ch.03 | 910 MHz |
| ch.04 | 912 MHz |
| ch.05 | 914 MHz |
| ch.06 | 916 MHz |
| ch.07 | 918 MHz |
| ch.08 | 920 MHz |
| ch.09 | 922 MHz |
| ch.10 | 924 MHz |

ZigBee 2.4 GHz channels have

- band width 2 MHz
- spacing 5 MHz

| Zigbee Channel | Frequency |
| --- | --- |
| ch.11 | 2405 MHz |
| ch.12 | 2410 MHz |
| ch.13 | 2415 MHz |
| ch.14 | 2420 MHz |
| ch.15 | 2425 MHz |
| ch.16 | 2430 MHz |
| ch.17 | 2435 MHz |
| ch.18 | 2440 MHz |
| ch.19 | 2445 MHz |
| ch.20 | 2450 MHz |
| ch.21 | 2455 MHz |
| ch.22 | 2460 MHz |
| ch.23 | 2465 MHz |
| ch.24 | 2470 MHz |
| ch.25 | 2475 MHz |
| ch.25 | 2480 MHz |

## WiFi

| WiFi channel | Frequency | notes |
| --- | --- | --- |
| ch.1 | 2412 MHz | |
| ch.2 | 2417 MHz | |
| ch.3 | 2422 MHz | |
| ch.4 | 2427 MHz | |
| ch.5 | 2432 MHz | |
| ch.6 | 2437 MHz | |
| ch.7 | 2442 MHz | |
| ch.8 | 2447 MHz | |
| ch.9 | 2452 MHz | |
| ch.10 | 2457 MHz | |
| ch.11 | 2462 MHz | |
| ch.12 | 2467 MHz | avoided in north America |
| ch.13 | 2472 MHz | avoided in north America |

| ch.14 | 2484 MHz | Japan 802.11b only |

| ch. | 24 MHz | |
| ch. | 24 MHz | |

### 802.11/11b DSSS method

The DSSS method used by legacy 802.11 and 802.11b (and the 11b-compatible rates of 11 g) occupies 22 MHz of bandwidth. This is from the 11 MHz chip rate used by the coding system. No guard band is prescribed;[17] the channel definition provides 3 MHz between 1, 6, and 11.

### 802.11g/11n OFDM method

The OFDM method used by 802.11g and 802.11n occupies a bandwidth of 16.25 MHz. The nameplate bandwidth is set to be 20 MHz, rounding up to a multiple of channel width and providing some guard band for signal to attenuate along the edge of the band.[18] This guardband is mainly used to accommodate older routers with modem chipsets prone to full channel occupancy, as most modern Wi‑Fi routers are not prone to excessive channel occupancy.
