# ALTERNATE BIT PROTOCOL

## Introduction 

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

## Contributions 