# ALTERNATE BIT PROTOCOL

## Introduction 

Alternate Bit Protocol (ABP) simulator is a communication protocol to ensure reliable transmission through an unreliable network. It consists of three components: sender, network (decomposed further into two subnets) and receiver. The sender sends a packet and waits for an acknowledgement. If the acknowledgement doesn't arrive within a predefined time, the sender re-sends this packet until it receives an expected acknowledgement and then sends the next packet. In order to distinguish two consecutive packets, the sender adds an additional bit on each packet (called alternating bit because the sender uses 0 and 1 alternatively). 

The behavior of receiver is to receive the data and send back an acknowledgement extracted from the received data after a time period. The subnets just pass the packets after a time delay. However, in order to simulate the unreliability of the network, only 95% of the packets will be passed in each of the subnet, i.e. 5% of the data will be lost through the subnet.

This ABP simulator is implemented in Cadmium. The behaviour and specification of the simulator is explained in detail in the alternatebitprot.pdf found in [documentation](https://github.com/Revanth91/AlternateBitProtocolSimulator/tree/master/doc) section. In this project, we focus on reviewing the code and following the standard coding conventions, modifying documentation as per proposed coding standard and improving the performance of the simulator.

## File Organization

### data [This folder contains the input and output files for the simulator]
1. input [This folder contains the input files]
	- input_abp_0.txt
2. output [This folder contains the output files]
	- abp_output.txt

### doc [This folder contains the documentation]
1. alternatebitprot.pdf
2. Cadmium_Documentation_Ubuntu.pdf
3. Cadmium_Windows.pdf

### include [This folder contains the various header files used in the project]
1. atomics [This folder contains the atomics header files ]
	- receiverCadmium.hpp
	- senderCadmium.hpp
	- subnetCadmium.hpp
2. data_structures [This folder contains the data structures header files]
 	- message.hpp	

### lib [This folder contains the third party library files]
1. cadmium [This folder contains the cadmium library files]
2. DESTimes [This folder contains the DESTimes library files]
3. vendor [This folder contains vendor header files]
	- iestream.hpp
	- NDTime.hpp
	
### src [This folder contains the source files used for the simualator] 
1. data_structures [This folder contains the source files from data_structures]
	- message.cpp
2. top_model [This folder contains the source files for top_model]
	- main.cpp

### test [This folder contains files realted to the unit tests]
1. data [This folder contains the input and output files for the unit tests]
	1. sender [This folder contains data for sender testing and simulator outputs]
		- sender_input_test_ack_In.txt
		- sender_input_test_control_In.txt
		- sender_test_output.txt
	2. subnet [This folder contains data for subnet testing and simulator outputs]
	    - subnet_input_test.txt
	    - subnet_test_output.txt
	3. receiver [This folder contains data for receiver testing and simulator outputs]
	    - receiver_input_test.txt
	    - receiver_test_output.txt


2. src [This folder contains the source code files for the unit tests]
	1. sender [This folder contains source code for the sender component]
		- main.cpp
	2. subnet [This folder contains source code for the subnet component]
		- main.cpp
	3. receiver [This folder contains source code for the receiver component]
		- main.cpp


## Instructions

### Installing dependencies

Below are the instructions to install dependencies on **Windows operating system** and the same is available in the [documentation](https://github.com/Revanth91/AlternateBitProtocolSimulator/tree/master/doc) section.

1. apt-cyg installations: Open Cygwin in administrator mode and type the below commands, 
	1. `wget rawgit.com/transcode-open/apt-cyg/master/apt-cyg`
	2. `install apt-cyg/bin`
	3. `apt-cyg install git` Please use this command if you don't have git pre-installed in your system.

2. Cadmium dependencies installations: 
	1. Open the Cygwin terminal in the location where you have downloaded the setup (.exe) file for cygwin. 
	2. Type `./setup-x86_64.exe -q -P libboost-devel` or `apt-cyg install libboost-devel`. This will install boost library.
	3. Select a folder to download the Cadmium and DESTimes files for the project. 
	4. Open the Cygwin terminal inside this folder location and type `git clone https://github.com/SimulationEverywhere/cadmium` to download the Cadmium dependencies.  
	5. Type `git clone https://github.com/Laouen/DESTimes.git` to download the DESTimes dependencies. 

For **Linux operating system**, there is a seperate set of instructions available in the [documentation](https://github.com/Revanth91/AlternateBitProtocolSimulator/tree/master/doc) section.

### Run the simulator
1. Open the terminal. Press in your keyboard `Ctrl+Alt+t`
2. Set the command prompt in the AlternateBitProtocol folder. To do so, type in the terminal the path to this folder.
		 Example: cd ../../AlternateBitProtocol
3. To compile the project, type in the terminal
		 `make clean; make all`
4. To run the simulation, go the folder with executable files (i.e., bin folder). To do so, type in the terminal the path to this folder.
		 Example: cd bin
5. In the terminal type "./NAME_OF_THE_COMPILED_FILE NAME_OF_THE_INPUT_FILE". For this test you need to type:
		 `./ABP ../data/input/input_abp_1.txt`
6. To check the output of the simulation, open `"../data/output/abp_output.txt"`
7. To execute the simulator with different inputs,
	1. Create new .txt files with the same structure as input_abp_0.txt or input_abp_1.txt
	2. Run the simulator using the instructions in step 4 of this [section](#Run-the-simulator).
	3. If you want to keep the output, rename abp_output.txt. To do so, type the below in the terminal
	  `"mv abp_output.txt NEW_NAME"`.
		Example: mv ../data/output/abp_output.txt ../data/output/abp_output_0.txt

### Run the unit tests
1. Run sender test
	1. Open the terminal. Press in your keyboard `Ctrl+Alt+t`
	2. To run the unit tests, go the folder with executable files (i.e., bin folder). To do so, type in the 
	   terminal the path to this folder. 
	      Example: cd ../alternatebitprotocolsimulator/bin
	3. To run the test, type in the terminal "./NAME_OF_THE_COMPILED_FILE". For this specific test you need 
	   to type:
		  `./SENDER_TEST`
	4. To check the output of the test, open `"../test/data/subnet/subnet_test_output.txt"`
2. To run subnet and receiver tests, the steps are analogous to step 1 of this [section](#Run-the-unit-tests).

## Contributions 

### Organization 
Our sincere thanks to [Carleton University](https://carleton.ca/) and Dr. [Cristina Ruiz Martin](https://github.com/cruizm) for providing an opportunity to work on this project. 
### Authors
This project was originally developed by Dr. [Cristina Ruiz Martin](https://github.com/cruizm) and it has been modified by [Revanth Sridhar](https://github.com/Revanth91), [Manoj Goli](https://github.com/manoj-goli).