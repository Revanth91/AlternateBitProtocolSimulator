This folder contains a simulator for an ALTERNATE BIT PROTOCOL implemented in Cadmium

/**************************/
/****FILES ORGANIZATION****/
/**************************/

README.txt	

bin [This folder is created automatically during execution and it contains executable files]

build [This folder is created automatically during execution and it contains object files]

data [This folder contains the input and output files for the simulator]
	input [This folder contains the input files]
		input_abp_0.txt
		input_abp_1.txt
	output [This folder contains the output files]
		abp_output.txt

doc [This folder contains the documentation]
	alternatebitprot.pdf
	Cadmium_Documentation_Ubuntu.pdf
	Cadmium_Windows.pdf

include [This folder contains the various header files used in the project]
	atomics [This folder contains the atomics header files ]
		receiverCadmium.hpp
		senderCadmium.hpp
		subnetCadmium.hpp
	data_structures [This folder contains the data structures headers]
		message.hpp	

lib [This folder contains the third party library files]
	cadmium [This folder contains the cadmium (third party) library files]
	DESTimes [This folder contains the DESTimes (third party) library files]
	vendor [This folder contains vendor (third party) header files]
		iestream.hpp
		NDTime.hpp
	
src [This folder contains the source files used for the simualator] 
	data_structures [This folder contains the source files from data_structures]
		message.cpp
	top_model [This folder contains the source files from top_model]
		main.cpp

test [This folder contains files realted to the unit tests]
	data [This folder contains the input and output files for the unit tests]
		sender [This folder contains some data for sender testing and simulator outputs]
			sender_input_test_ack_In.txt
			sender_input_test_control_In.txt
			sender_test_output.txt
		receiver [This folder contains some data for receiver testing and simulator outputs] 
			receiver_input_test.txt
			receiver_test_output.txt
		subnet [This folder contains some data for subnet testing and simulator outputs]
			subnet_input_test.txt
			subnet_test_output.txt

	include [This folder will include header files related to unit tests]

	src [This folder contains the source code files for the unit tests]
		sender [This folder contains source code for the sender component]
			main.cpp
		receiver [This folder contains source code for the receiver component] 
			main.cpp
		subnet [This folder contains source code for the subnet component]
			main.cpp

	
/*************/
/****STEPS****/
/*************/

0 - alternatebitprotocol.doc contains the explanation of this simulator

1 - Update include paths in the makefile of this folder. You need to update the following lines:
	INCLUDECADMIUM=-I lib/cadmium/include
    Update the relative path to cadmium/include from the folder where the makefile is. You need to take into account where you copied the folder during the installation process
	Example: INCLUDECADMIUM=-I ../../cadmium/include

2 - Run the simulator
	2.1 - Open the terminal. Press in your keyboard Ctrl+Alt+t
	2.2 - Set the command prompt in the AlternateBitProtocol folder. To do so, type in the terminal the path to this folder.
		Example: cd ../../AlternateBitProtocol
	2.3 - To compile the project, type in the terminal:
		make clean; make all
	2.4 - To run the simulation, go the folder with executable files (i.e., bin folder). To do so, type in the terminal the path to this folder.
		Example: cd bin
	2.5 - In the terminal type "./NAME_OF_THE_COMPILED_FILE NAME_OF_THE_INPUT_FILE". For this test you need to type:
		./ABP ../data/input/input_abp_1.txt
	2.6 - To check the output of the simulation, open "../data/output/abp_output.txt"
	2.7 - To execute the simulator with different inputs
		2.7.1. Create new .txt files with the same structure as input_abp_0.txt or input_abp_1.txt
		2.7.2. Run the simulator using the instructions in step 4
		2.7.3. If you want to keep the output, rename abp_output.txt. To do so, type in the terminal: "mv abp_output.txt NEW_NAME"
		Example: mv ../data/output/abp_output.txt ../data/output/abp_output_0.txt

3 - Run the unit tests
	3.1. Run subnet test
		3.1.1 - Open the terminal. Press in your keyboard Ctrl+Alt+t
		3.1.2 - To run the unit tests, go the folder with executable files (i.e., bin folder). To do so, type in the terminal the path to this folder.
			Example: cd ../alternatebitprotocolsimulator/bin
		3.1.3 - To run the test, type in the terminal "./NAME_OF_THE_COMPILED_FILE". For this specific test you need to type:
			./SUBNET_TEST
		3.1.4 - To check the output of the test, open "../test/data/subnet/subnet_test_output.txt"
	3.2. To run receiver and sender tests, the steps are analogous to 3.1
