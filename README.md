# CAEN_Digiter_1743

A project for digitizing, processing, and analyzing data from the CAEN 1743 digitizer.

## Author

Arindam Sen

## Features

* Data acquisition from CAEN 1743 hardware
* Signal waveform processing
* Data storage and export
* Configurable acquisition parameters
* Analysis and visualization support

## Requirements

* Git
* Python 3.x (if applicable)
* CAEN drivers and libraries
* Additional dependencies listed in the project configuration files

## Installation

Clone the repository:

```bash
git clone git@github.com:arindamsen95/CAEN_Digiter_1743.git
cd CAEN_Digiter_1743
```

Install dependencies: C++, snap, root 

```bash
sudo apt install g++ build-essential
sudo apt install snapd   
sudo snap install root-framework
```

## Usage

Run the application:

```bash
# Example
root macro.C
```


## Project Structure

CAEN_Digiter_1743/
├── src/
├── config/
├── data/
├── docs/
├── tests/
├── README.md
└── .gitignore


## Development

Create a feature branch:

```bash
git checkout -b feature/my-feature
```

Commit changes:

```bash
git commit -m "Add new feature"
```

Push changes:

```bash
git push origin feature/my-feature
```


