# NATSA: A Near-Data Processing Accelerator for Time Series Analysis

NATSA is the first near-data time series analysis accelerator based on Matrix Profile.

Ivan Fernandez, Ricardo Quislant, Christina Giannoula, Mohammed Alser, Juan Gómez-Luna, Eladio Gutiérrez, Oscar Plata, and Onur Mutlu,
"**NATSA: A Near-Data Processing Accelerator for Time Series Analysis**"
*Proceedings of the 38th IEEE International Conference on Computer Design (ICCD), Virtual, October 2020*.

[Slides (<a href="https://people.inf.ethz.ch/omutlu/pub/NATSA_time-series-analysis-near-data_iccd20-talk.pptx" target="_blank">pptx</a>) (<a href="https://people.inf.ethz.ch/omutlu/pub/NATSA_time-series-analysis-near-data_iccd20-talk.pdf" target="_blank">pdf</a>)]
[<a href="https://www.youtube.com/embed/PwhtSAVa_W4" target="_blank">Talk video</a> (10 minutes)]

NATSA's evaluation is performed using the following simulation environments:

ramulator-pim: https://github.com/CMU-SAFARI/ramulator-pim

gem5-aladdin: https://github.com/harvard-acc/gem5-aladdin

McPAT: https://github.com/HewlettPackard/mcpat


## Repository Structure and Usage

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
