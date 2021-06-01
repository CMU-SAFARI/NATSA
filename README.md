# NATSA: A Near-Data Processing Accelerator for Time Series Analysis

<p align="center">
  <img width="722" height="140" src="https://github.com/CMU-SAFARI/NATSA/blob/main/images/natsa_logo.png">
</p>

NATSA is the first near-data time series analysis accelerator based on Matrix Profile (SCRIMP). The key idea behind NATSA is to exploit modern 3D-stacked High Bandwidth Memory (HBM) to enable efficient and fast matrix profile computation near memory. NATSA provides three key benefits: 1) quickly computing the matrix profile for a wide range of applications by building specialized energy-efficient floating-point arithmetic processing units close to HBM, 2) improving the energy effciency and execution time by reducing the need for data movement over slow and energy-hungry buses between the computation units and the memory units, and 3) analyzing time seriesdata at scale by exploiting low-latency, high-bandwidth, and energy-efficient memory access provided by HBM.

<p align="center">
  <img width="368" height="335" src="https://github.com/CMU-SAFARI/NATSA/blob/main/images/natsa_diagram.png">
</p>


## Citation

Please cite the following paper if you find NATSA useful:

Ivan Fernandez, Ricardo Quislant, Christina Giannoula, Mohammed Alser, Juan Gómez-Luna, Eladio Gutiérrez, Oscar Plata, and Onur Mutlu,
"**NATSA: A Near-Data Processing Accelerator for Time Series Analysis**"
*Proceedings of the 38th IEEE International Conference on Computer Design (ICCD), Virtual, October 2020*.


## Repository Structure and Usage


> :warning: This repository contains the codes used in NATSA's performance and energy evaluation for our ICCD 2020 paper using architectural simulation frameworks. **If you are interested on performing motif/discord discovery in time series using Matrix Profile**, please visit https://www.cs.ucr.edu/~eamonn/MatrixProfile.html, where you will find ready-to-use implementations for CPU and GPU.

We point out next the repository structure and some important folders and files.
```
.
+-- README.md
+-- aladdin/
|   +-- config_files/
|   +-- scrimp_src/
+-- gpu/
+-- timeseries/
+-- xeonphi/
+-- zsimramulator/
|   +-- config_files/
|	|	+-- scrimp/
|   +-- scrimp_src/
```

### Prerequisites

NATSA's evaluation requires the following simulation frameworks. Please refer to their corresponding documentation to install them and their dependencies. 
* ramulator-pim: https://github.com/CMU-SAFARI/ramulator-pim
* gem5-aladdin: https://github.com/harvard-acc/gem5-aladdin
* McPAT: https://github.com/HewlettPackard/mcpat

Additionaly, the Xeon Phi code requires a supported processor (e.g., Intel Xeon Phi 7210) and the GPU code requires a CUDA-capable GPU.



## Other Resources

[Slides (<a href="https://people.inf.ethz.ch/omutlu/pub/NATSA_time-series-analysis-near-data_iccd20-talk.pptx" target="_blank">pptx</a>) (<a href="https://people.inf.ethz.ch/omutlu/pub/NATSA_time-series-analysis-near-data_iccd20-talk.pdf" target="_blank">pdf</a>)]
[<a href="https://www.youtube.com/embed/PwhtSAVa_W4" target="_blank">Talk video</a> (10 minutes)]
