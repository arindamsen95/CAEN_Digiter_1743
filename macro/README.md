# Long Scintillator Detector Analysis Framework

## Overview

This repository contains ROOT macros for the analysis and characterization of a dual-ended scintillator detector instrumented with SiPM readout and digitized using a WaveCatcher DAQ system.

The analysis framework converts raw WaveCatcher ASCII data into ROOT TTrees, reconstructs event-level quantities, and provides tools for studying:

* Charge response
* Timing response
* Signal attenuation
* Effective propagation velocity
* Position reconstruction
* Position resolution
* Timing resolution
* Light collection uniformity

The workflow is designed for scintillator detectors with readout on both ends and an external reference detector.

---

# Analysis Workflow

```text
WaveCatcher ASCII Files
           |
           v
      data_decoder.C
           |
           v
      ROOT File
  (EventTree, ChannelTree)
           |
           v
 build_analysis_tree.C
           |
           v
      AnalysisTree
           |
           +--> Charge Analysis
           +--> Timing Analysis
           +--> Position Reconstruction
           +--> Resolution Studies
```

---

# 1. data_decoder.C

## Purpose

Converts WaveCatcher ASCII files into ROOT format.

The macro scans one or more directories, decodes event and channel information, and stores the results in ROOT TTrees.

---

## Output Trees

### EventTree

Contains event-level information.

| Branch   | Description         |
| -------- | ------------------- |
| eventID  | Global event number |
| unixTime | Event timestamp     |
| tdc      | FPGA TDC value      |

---

### ChannelTree

Contains channel-level information.

| Branch       | Description             |
| ------------ | ----------------------- |
| eventID      | Event number            |
| channel      | Channel number          |
| fcr          | WaveCatcher FCR value   |
| trigCount    | Trigger count           |
| timeCount    | Time count              |
| baseline     | Signal baseline         |
| amplitude    | Pulse amplitude         |
| charge       | Integrated pulse charge |
| leadingTime  | Leading edge timestamp  |
| trailingTime | Trailing edge timestamp |
| waveform     | Full waveform samples   |

---

## Typical Usage

```cpp
.L data_decoder.C+

data_decoder(
    "output.root",
    {"run_directory"}
);
```

---

# 2. build_analysis_tree.C

## Purpose

Builds a compact event-based analysis tree from the decoded data.

The macro matches detector channels belonging to the same event and computes derived quantities used throughout the analysis.

---

## Required Channels

| Channel | Description        |
| ------- | ------------------ |
| 0       | Reference detector |
| 9       | Left SiPM          |
| 11      | Right SiPM         |

Only events containing all required channels are stored.

---

# AnalysisTree

The output tree contains one entry per reconstructed event.

---

## Charge Variables

### Q9

Charge measured by the left SiPM.

Used for:

* Charge spectra
* Attenuation studies
* Position reconstruction

---

### Q11

Charge measured by the right SiPM.

Used for:

* Charge spectra
* Attenuation studies
* Position reconstruction

---

### Qsum

Total collected charge.

[
Qsum = Q9 + Q11
]

Used for:

* Light collection studies
* Detector response uniformity
* Energy-related observables

---

### Qratio

Logarithmic charge ratio.

[
Qratio = ln(Q9 / Q11)
]

Used for:

* Effective attenuation length determination
* Charge-based position reconstruction

Typical analysis macros:

```text
plot_Qratio_vs_position.C
plot_charge_position_resolution.C
```

---

### Asymmetry

Charge asymmetry.

[
Asymmetry (A) = (Q9 - Q11)/(Q9 + Q11)
]

Used for:

* Position reconstruction
* Alternative attenuation studies
* Position resolution measurements

Typical analysis macros:

```text
plot_asymmetry_vs_position.C
plot_asymmetry_position_resolution.C
```

---

## Timing Variables

### T0

Reference detector timestamp.

Used for:

* Timing resolution studies
* Time-of-flight measurements

---

### T9

Timestamp from the left SiPM.

---

### T11

Timestamp from the right SiPM.

---

### dtLR

Left-right timing difference.

[
dtLR = T9 - T11
]

Used for:

* Effective signal propagation velocity
* Timing-based position reconstruction

Typical analysis macros:

```text
plot_dtLR_vs_position.C
plot_timing_position_resolution.C
```

---

### Tavg

Average detector timestamp.

[
Tavg = (T9 + T11)/2
]

Used for:

* Event timing
* Reference timing comparisons

---

### dtRef

Detector timing relative to the reference detector.

[
dtRef = Tavg - T0
]

Used for:

* Timing resolution studies
* Detector timing performance

Typical analysis macros:

```text
plot_dtRef.C
plot_timing_resolution.C
```

---

# Analysis Macros

## Charge Analysis

### Charge Spectra

Plots charge distributions for individual channels.

Typical variables:

```cpp
charge
Q9
Q11
Qsum
```

Output examples:

```text
charge_spectra.pdf
Qsum_distributions.pdf
```

---

### Qsum vs Position

Studies detector response uniformity.

Observable:

[
Q_{sum}=Q_9+Q_{11}
]

Purpose:

* Light collection studies
* Response uniformity checks

---

## Attenuation Studies

### Qratio vs Position

Fits:

[
Q_{ratio}=a+bx
]

Used to extract:

[
lambda_{eff}=  2 / |slope(b)|
]

Macro:

```text
plot_Qratio_vs_position.C
```

---

## Timing Studies

### dtLR vs Position

Fits:

[
dtLR=a+bx
]

Used to determine:

[
v_{eff}= 2/b
]

Macro:

```text
plot_dtLR_vs_position.C
```

---

### Timing Resolution

Uses dtRef distributions.

Procedure:

1. Build dtRef histogram.
2. Fit Gaussian distribution.
3. Extract timing resolution.

Macro:

```text
plot_timing_resolution.C
```

---

## Position Reconstruction

### Timing-Based Method

Calibration:

[
x=f(dtLR)
]

Uses:

```text
dtLR
```

Macro:

```text
plot_timing_position_resolution.C
```

---

### Charge-Ratio Method

Calibration:

[
x=f(Q_{ratio})
]

Uses:

```text
Qratio
```

Macro:

```text
plot_charge_position_resolution.C
```

---

### Asymmetry Method

Calibration:

[
x=f(A)
]

Uses:

```text
Asymmetry
```

Macros:

```text
plot_asymmetry_vs_position.C
plot_asymmetry_position_resolution.C
```

---

# Future Extensions

The framework can be extended to include:

* Single-photoelectron calibration
* Gain measurements
* Photoelectron yield estimation
* Light-yield measurements
* Cosmic-muon calibration
* Time-of-flight studies
* Multi-bar detector systems

---

# Requirements

* ROOT 6.36+
* C++17
* std::filesystem support

---

# Repository Structure

```text
.
├── data_decoder.C
├── build_analysis_tree.C
├── plot_Qsum_vs_position.C
├── plot_Qratio_vs_position.C
├── plot_dtLR_vs_position.C
├── plot_timing_position_resolution.C
├── plot_charge_position_resolution.C
├── plot_asymmetry_vs_position.C
├── plot_asymmetry_position_resolution.C
└── README.md
```

---

# Author

Arindam Sen

Detector characterization framework for dual-ended scintillator detectors using WaveCatcher digitizers and SiPM readout.

