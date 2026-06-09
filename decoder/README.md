# WaveCatcher ASCII to ROOT Converter

## Description

`data_decoder.C` converts WaveCatcher ASCII output files into a ROOT file containing two TTrees:

### EventTree

Stores event-level information:

* `eventID`
* `unixTime`
* `tdc`

### ChannelTree

Stores channel-level information:

* `eventID`
* `channel`
* `fcr`
* `trigCount`
* `timeCount`
* `baseline`
* `amplitude`
* `charge`
* `leadingTime`
* `trailingTime`
* `waveform`

---

## Input Files

The macro searches each specified directory for files whose names begin with:

```
wavecatcher_run1_Ascii.dat
```

Examples:

```
wavecatcher_run1_Ascii.dat
wavecatcher_run1_Ascii.dat_0001
wavecatcher_run1_Ascii.dat_0002
...
```

Files are processed in sorted order.

---

## Multiple Directory Support

The macro can process multiple directories in a single run.

Events from later directories are automatically renumbered so that event IDs remain unique.

Example:

Directory 1:

```
EVENT 0
...
EVENT 500
```

Directory 2:

```
EVENT 0
...
EVENT 400
```

Output ROOT file:

```
Directory 1 -> Event IDs 0 ... 500
Directory 2 -> Event IDs 501 ... 901
```

The event numbering is propagated to both:

* EventTree.eventID
* ChannelTree.eventID

ensuring consistency between the trees.

---

## Function Signature

```cpp
void data_decoder(
    TString outfile = "wavecatcher_output.root",
    std::vector<std::string> directories = {"."}
)
```

Parameters:

| Parameter   | Description                                            |
| ----------- | ------------------------------------------------------ |
| outfile     | Output ROOT filename                                   |
| directories | List of directories containing WaveCatcher ASCII files |

---

## Compilation

Start ROOT:

```bash
root
```

Compile the macro:

```cpp
.L data_decoder.C+
```

The `+` instructs ROOT/ACLiC to compile the macro.

---

## Running the Macro

### Process Current Directory

```cpp
data_decoder();
```

Output:

```
wavecatcher_output.root
```

---

### Specify Output File

```cpp
data_decoder(
    "output.root"
);
```

---

### Process One Directory

```cpp
std::vector<std::string> dirs = {
    "wavecatcher_run1"
};

data_decoder(
    "output.root",
    dirs
);
```

---

### Process Multiple Directories

```cpp
std::vector<std::string> dirs = {
    "run1",
    "run2",
    "run3"
};

data_decoder(
    "output.root",
    dirs
);
```

---

## Batch Execution

ROOT can execute the macro directly from the command line:

```bash
root -l
```

Then:

```cpp
.L data_decoder+

std::vector<std::string> dirs = {
    "run1",
    "run2"
};

data_decoder(
    "output.root",
    dirs
);
```

Or:

```bash
root -l 'data_decoder("output.root",{"dir1","dir2",...})'
```

---

## Output

The resulting ROOT file contains:

```
EventTree
ChannelTree
```

which can be inspected with:

```cpp
TFile f("wave.root");

f.ls();

EventTree->Print();
ChannelTree->Print();
```


## Quick Data Analysis

Start ROOT and open the output file:

```bash
root wavecatcher_output.root
```

Retrieve the trees:

```cpp
TTree *eventTree = (TTree*)_file0->Get("EventTree");
TTree *chTree    = (TTree*)_file0->Get("ChannelTree");
```

Inspect available branches:

```cpp
eventTree->Print();
chTree->Print();
```

Plot common quantities:

```cpp
chTree->Draw("amplitude");
chTree->Draw("charge");
chTree->Draw("baseline");
chTree->Draw("channel");
```

Apply selections:

```cpp
chTree->Draw("charge","channel==0");
chTree->Draw("amplitude","eventID==100");
```

Display the first few entries:

```cpp
eventTree->Scan("*","","",10);
chTree->Scan("*","","",10);
```

Access waveform data:

```cpp
std::vector<double> *waveform = nullptr;

chTree->SetBranchAddress("waveform",&waveform);
chTree->GetEntry(0);

for(auto v : *waveform)
    std::cout << v << std::endl;
```

The `eventID` branch can be used to correlate information between `EventTree` and `ChannelTree`.


---

## Notes

* Files are automatically sorted before processing.
* Event numbering is continuous across all directories.
* Waveforms are stored as `std::vector<double>`.
* Requires ROOT 6 and C++17 support (`std::filesystem`).
* Tested with ROOT 6.36.

