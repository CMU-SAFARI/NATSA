# NATSA: A Near-Data Processing Accelerator for Time Series Analysis

<p align="center">
  <img width="722" height="140" src="https://github.com/CMU-SAFARI/NATSA/blob/main/images/natsa_logo.png">
</p>

**NATSA** is the first **N**ear-Data Processing **A**ccelerator for **T**ime **S**eries **A**nalysis. We enable high-performance and energy-efficient time series analysis for a wide range of applications, by minimizing the overheads of data movement. This can enable efficient time series analysis on large-scale systems as well as embedded and mobile devices, where power consumption is a critical constraint (e.g., heart beat analysis on a mobile medical device to predict a heart attack). 

***:bulb:Watch our [talk about NATSA](https://www.youtube.com/watch?v=PwhtSAVa_W4)!*** 
Slides: (<a href="https://people.inf.ethz.ch/omutlu/pub/NATSA_time-series-analysis-near-data_iccd20-talk.pptx" target="_blank">pptx</a>) (<a href="https://people.inf.ethz.ch/omutlu/pub/NATSA_time-series-analysis-near-data_iccd20-talk.pdf" target="_blank">pdf</a>)

<p align="center">
    <a href="https://www.youtube.com/watch?v=PwhtSAVa_W4" target="_blank"><img src="http://img.youtube.com/vi/PwhtSAVa_W4/0.jpg" 
alt="Watch our explanation of NATSA" width="440" border="10" /></a>
</p>

## Table of Contents
- [Time Series Analysis](#timeseries)
- [Key Idea and Benefits of NATSA](#idea)
- [Repository Structure and Usage](#structure)
- [Prerequisites](#prerequisites)
  - [General-purpose Core Simulation](#gpcsim)
  - [NATSA Simulation](#natsasim)
  - [Xeon Phi and GPU Execution](#xeonsim)
- [Other Resources](#resources)
- [Getting Help](#contact)
- [Citing NATSA](#cite)


## <a name="timeseries"></a>Time Series Analysis
Time series analysis is a key technique for extracting and predicting events in domains as diverse as epidemiology, genomics, neuroscience, environmental sciences, economics, and more. Matrix profile, the state-of-the-art algorithm to perform time series analysis, computes the most similar subsequence for a given query subsequence within a sliced time series. Matrix profile has low arithmetic intensity, but it typically operates on large amounts of time series data. In current computing systems, this data needs to be moved between the off-chip memory units and the on-chip computation units for performing matrix profile. This causes a major performance bottleneck as data movement is extremely costly in terms of both execution time and energy. 

## <a name="idea"></a>Key Idea and Benefits of NATSA
The key idea of NATSA is to exploit modern 3D-stacked High Bandwidth Memory (HBM) to enable efficient and fast specialized matrix profile computation near memory, where time series data resides. NATSA provides three key benefits: 1) quickly computing the matrix profile for a wide range of applications by building specialized energy-efficient floating-point arithmetic processing units close to HBM, 2) improving the energy efficiency and execution time by reducing the need for data movement over slow and energy-hungry buses between the computation units and the memory units, and 3) analyzing time series data at scale by exploiting low-latency, high-bandwidth, and energy-efficient memory access provided by HBM. Our experimental evaluation shows that NATSA improves performance by up to 14.2× (9.9× on average) and reduces energy by up to 27.2× (19.4× on average), over the state-of-the-art multi-core implementation. NATSA also improves performance by 6.3× and reduces energy by 10.2× over a general-purpose NDP platform with 64 in-order cores.


## <a name="structure"></a>Repository Structure and Usage
> :warning: This repository contains the codes used in NATSA's performance and energy evaluation for our ICCD 2020 paper using architectural simulation frameworks. **If you are interested in performing motif/discord discovery in time series using Matrix Profile**, please visit https://www.cs.ucr.edu/~eamonn/MatrixProfile.html, where you will find ready-to-use implementations for CPU and GPU.

We point out next the repository structure and some important folders and files.
```
.
+-- README.md
+-- aladdin/
|   +-- config_files/
|   +-- scrimp_src/
+-- gpu/
+-- images/
+-- mcpat/
+-- timeseries/
+-- xeonphi/
+-- zsimramulator/
|   +-- config_files/
|	|	+-- scrimp/
|   +-- scrimp_src/
```


## <a name="prerequisites"></a>Prerequisites
NATSA's evaluation requires the following simulation frameworks. Please refer to their corresponding documentation to install them and their dependencies. 
* ramulator-pim: https://github.com/CMU-SAFARI/ramulator-pim
* McPAT: https://github.com/HewlettPackard/mcpat
* gem5-aladdin: https://github.com/harvard-acc/gem5-aladdin


Additionaly, the Xeon Phi code requires a supported processor (e.g., Intel Xeon Phi 7210) and the GPU code requires a CUDA-capable GPU.

### <a name="gpcsim"></a>General-purpose Core Simulation
The general-purpose core performance can be simulated using ramulator-pim environment and the files provided in `zsimramulator` folder. Please refer to ramulator-pim documentation to set up the environment. The source code of SCRIMP, ready to be used in ramulator-pim is under the folder `zsimramulator/scrimp_src`, which has to be compiled before running the simulator. Once the desired configuration file is properly adjusted with the users's paths, the simulator can be started as follows:

```
./build/opt/zsim config_files/scrimp/262144/2048/arm/hbm/64/scrimp_arm_hbm.cfg
```
This example will evaluate SCRIMP in a arm-like 64-core configuration with HBM, using a time series of 262144 elements and a window size of 2048.

The general-purpose core energy and area can be estimated using McPAT and the config files provided in `mcpat` folder. Please refer to McPAT documentation to set up the environment. The following example estimates the energy and area for the ARM-like configuration:

```
./mcpat -infile ARM_64.xml
```


### <a name="natsasim"></a>NATSA Simulation
The performance, area and energy of NATSA can be simulated using gem5-aladdin simulation framework and the files provided in `aladdin` folder. Please refer to gem5-aladdin documentation to set up the environment. The source file of SCRIMP, optimized for NATSA is located under `aladdin/scrimp_src`, which can be tunned according to the desired time series parameters.

To run it, simply launch gem5 with default HBM memory model and pass the `.cfg`file to aladdin.


### <a name="xeonsim"></a>Xeon Phi and GPU Execution
The code to perform the Xeon Phi executions is located under `xeonphi` folder, simply `make` it and run it with the desired time series (example time series are located under `timeseries` folder. This is an example execution for a time series of 524288 elements, window size of 4096 and 256 threads:

```
./scrimp_xeonphi 4096 randomSerie524288.txt 4096 256
```

The code to perform the GPU executions is located under `gpu` folder, which requires a CUDA-capable GPU to run. Once compiled using the provided `Makefile`, it can be executed as follows:

```
./SCRIMP 4096 randomSerie524288.txt out.txt
```

## <a name="resources"></a>Other Resources
* Slides used in our ICCD 2020 presentation: (<a href="https://people.inf.ethz.ch/omutlu/pub/NATSA_time-series-analysis-near-data_iccd20-talk.pptx" target="_blank">pptx</a>) (<a href="https://people.inf.ethz.ch/omutlu/pub/NATSA_time-series-analysis-near-data_iccd20-talk.pdf" target="_blank">pdf</a>)
* <a href="https://www.youtube.com/embed/PwhtSAVa_W4" target="_blank">Talk video</a> (10 minutes)

## <a name="contact"></a>Getting Help
If you have any suggestion for improvement, please contact ivanferveg [at] gmail.com
If you encounter bugs or have further questions or requests, you can raise an issue at the [issue page][issue].

[issue]: https://github.com/CMU-SAFARI/NATSA/issues


## <a name="cite"></a>Cite NATSA

Please cite the following paper if you find NATSA useful:

Ivan Fernandez, Ricardo Quislant, Christina Giannoula, Mohammed Alser, Juan Gómez-Luna, Eladio Gutiérrez, Oscar Plata, and Onur Mutlu.
"**NATSA: A Near-Data Processing Accelerator for Time Series Analysis**"
*Proceedings of the 38th IEEE International Conference on Computer Design (ICCD), Virtual, October 2020*.

Below is bibtex format for citation.

```bibtex
@inproceedings{fernandez2020natsa,
  title={NATSA: A Near-Data Processing Accelerator for Time Series Analysis},
  author={Fernandez, Ivan and Quislant, Ricardo and Guti{\'e}rrez, Eladio and Plata, Oscar and Giannoula, Christina and Alser, Mohammed and G{\'o}mez-Luna, Juan and Mutlu, Onur},
  booktitle={2020 IEEE 38th International Conference on Computer Design (ICCD)},
  pages={120--129},
  year={2020},
  organization={IEEE}
}
```
