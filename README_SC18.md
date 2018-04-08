# SC18 Paper Reproducibility Instructions

## Getting Started
First, follow the installation instructions in the [README](README.md)

For the analysis scripts, you will need a Python (2 or 3) installation that also has the Seaborn module (for plotting).

### Datasets for the experiments

Following are the steps to setup RMAT network and real network for running Dynamic SSSP and Galois

1. Compile RMAT graph generator (in external/RMAT) by issuing "make" command (without quotes).

2. Generate RMAT networks for scale 24 for different probablities, by issuing following commands 
"./driverForRmat 24 24 0.25 0.25 0.25 0.25 data/RMAT24.er"
"./driverForRmat 24 24 0.45 0.15 0.15 0.25 data/RMAT24.gr"

3. For real world networks (https://snap.stanford.edu/data/), following command were used from unix machine-
"wget https://snap.stanford.edu/data/com-LiveJournal.html", "wget https://snap.stanford.edu/data/com-Youtube.html", and "https://snap.stanford.edu/data/soc-pokec.html"

4. After getting the networks from step3, unzip all the networks.  

