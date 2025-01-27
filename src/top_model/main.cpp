#include <iostream>
#include <chrono>
#include <algorithm>
#include <string>

#include <cadmium/modeling/coupling.hpp>
#include <cadmium/modeling/ports.hpp>
#include <cadmium/modeling/dynamic_model_translator.hpp>
#include <cadmium/concept/coupled_model_assert.hpp>
#include <cadmium/modeling/dynamic_coupled.hpp>
#include <cadmium/modeling/dynamic_atomic.hpp>
#include <cadmium/engine/pdevs_dynamic_runner.hpp>
#include <cadmium/logger/tuple_to_ostream.hpp>
#include <cadmium/logger/common_loggers.hpp>


#include "../../lib/DESTimes/include/NDTime.hpp"
#include "../../lib/iestream.hpp"

#include "../../include/data_structures/message.hpp"

#include "../../include/atomics/sender_cadmium.hpp"
#include "../../include/atomics/receiver_cadmium.hpp"
#include "../../include/atomics/subnet_cadmium.hpp"

#define MAIN_OUTPUT_FILEPATH "../data/output/abp_output.txt"

#include "../transform.cpp"
#include "../user_output_query.cpp"

using namespace std;

using hclock=chrono::high_resolution_clock;
using TIME = NDTime;


/***** SETING INPUT PORTS FOR COUPLEDs *****/
struct inp_control : public cadmium::in_port<message_t>{};
struct inp_1 : public cadmium::in_port<message_t>{};
struct inp_2 : public cadmium::in_port<message_t>{};
/***** SETING OUTPUT PORTS FOR COUPLEDs *****/
struct outp_ack : public cadmium::out_port<message_t>{};
struct outp_1 : public cadmium::out_port<message_t>{};
struct outp_2 : public cadmium::out_port<message_t>{};
struct outp_pack : public cadmium::out_port<message_t>{};


/********************************************/
/****** APPLICATION GENERATOR *******************/
/********************************************/
template<typename T>
class ApplicationGen : public iestream_input<message_t,T> {
public:
  ApplicationGen() = default;
  ApplicationGen(const char* file_path) : iestream_input<message_t,T>(file_path) {}
};


int main(int argc, char ** argv) {

  if (argc < 2) {
   cout << "you are using this program with wrong parameters. Te program should be invoked as follow:";
   cout << argv[0] << " path to the input file " << endl;
   return 1; 
  }

  auto start = hclock::now(); //to measure simulation execution time

/*************** Loggers *******************/
  static std::ofstream out_data(MAIN_OUTPUT_FILEPATH);
    struct oss_sink_provider{
        static std::ostream& sink(){          
            return out_data;
        }
    };

using info=cadmium::logger::logger<cadmium::logger::logger_info, cadmium::dynamic::logger::formatter<TIME>, oss_sink_provider>;
using debug=cadmium::logger::logger<cadmium::logger::logger_debug, cadmium::dynamic::logger::formatter<TIME>, oss_sink_provider>;
using state=cadmium::logger::logger<cadmium::logger::logger_state, cadmium::dynamic::logger::formatter<TIME>, oss_sink_provider>;
using log_messages=cadmium::logger::logger<cadmium::logger::logger_messages, cadmium::dynamic::logger::formatter<TIME>, oss_sink_provider>;
using routing=cadmium::logger::logger<cadmium::logger::logger_message_routing, cadmium::dynamic::logger::formatter<TIME>, oss_sink_provider>;
using global_time=cadmium::logger::logger<cadmium::logger::logger_global_time, cadmium::dynamic::logger::formatter<TIME>, oss_sink_provider>;
using local_time=cadmium::logger::logger<cadmium::logger::logger_local_time, cadmium::dynamic::logger::formatter<TIME>, oss_sink_provider>;
using log_all=cadmium::logger::multilogger<info, debug, state, log_messages, routing, global_time, local_time>;

using logger_top=cadmium::logger::multilogger<log_messages, global_time>;


/*******************************************/



/********************************************/
/****** APPLICATION GENERATOR *******************/
/********************************************/
string input_data_control = argv[1];
const char * i_input_data_control = input_data_control.c_str();

std::shared_ptr<cadmium::dynamic::modeling::model> generator_con = cadmium::dynamic::translate::make_dynamic_atomic_model<ApplicationGen, TIME, const char* >("generator_con" , std::move(i_input_data_control));


/********************************************/
/****** SENDER *******************/
/********************************************/

std::shared_ptr<cadmium::dynamic::modeling::model> sender1 = cadmium::dynamic::translate::make_dynamic_atomic_model<Sender, TIME>("sender1");

/********************************************/
/****** RECIEVER *******************/
/********************************************/

std::shared_ptr<cadmium::dynamic::modeling::model> receiver1 = cadmium::dynamic::translate::make_dynamic_atomic_model<Receiver, TIME>("receiver1");



/********************************************/
/****** SUBNET *******************/
/********************************************/

std::shared_ptr<cadmium::dynamic::modeling::model> subnet1 = cadmium::dynamic::translate::make_dynamic_atomic_model<Subnet, TIME>("subnet1");
std::shared_ptr<cadmium::dynamic::modeling::model> subnet2 = cadmium::dynamic::translate::make_dynamic_atomic_model<Subnet, TIME>("subnet2");

/************************/
/*******NETWORK********/
/************************/
cadmium::dynamic::modeling::Ports iports_Network = {typeid(inp_1),typeid(inp_2)};
cadmium::dynamic::modeling::Ports oports_Network = {typeid(outp_1),typeid(outp_2)};
cadmium::dynamic::modeling::Models submodels_Network = {subnet1, subnet2};
cadmium::dynamic::modeling::EICs eics_Network = {
  cadmium::dynamic::translate::make_EIC<inp_1, subnet_defs::in>("subnet1"),
  cadmium::dynamic::translate::make_EIC<inp_2, subnet_defs::in>("subnet2")
};
cadmium::dynamic::modeling::EOCs eocs_Network = {
  cadmium::dynamic::translate::make_EOC<subnet_defs::out,outp_1>("subnet1"),
cadmium::dynamic::translate::make_EOC<subnet_defs::out,outp_2>("subnet2")
};
cadmium::dynamic::modeling::ICs ics_Network = {};
std::shared_ptr<cadmium::dynamic::modeling::coupled<TIME>> NETWORK = std::make_shared<cadmium::dynamic::modeling::coupled<TIME>>(
 "Network", 
 submodels_Network, 
 iports_Network, 
 oports_Network, 
 eics_Network, 
 eocs_Network, 
 ics_Network 
  );

/************************/
/*******ABPSimulator********/
/************************/
cadmium::dynamic::modeling::Ports iports_ABPSimulator = {typeid(inp_control)};
cadmium::dynamic::modeling::Ports oports_ABPSimulator = {typeid(outp_ack),typeid(outp_pack)};
cadmium::dynamic::modeling::Models submodels_ABPSimulator = {sender1, receiver1, NETWORK};
cadmium::dynamic::modeling::EICs eics_ABPSimulator = {
  cadmium::dynamic::translate::make_EIC<inp_control, sender_defs::control_in>("sender1")
};
cadmium::dynamic::modeling::EOCs eocs_ABPSimulator = {
  cadmium::dynamic::translate::make_EOC<sender_defs::packet_sent_out,outp_pack>("sender1"),
cadmium::dynamic::translate::make_EOC<sender_defs::ack_received_out,outp_ack>("sender1")
};
cadmium::dynamic::modeling::ICs ics_ABPSimulator = {
  cadmium::dynamic::translate::make_IC<sender_defs::data_out, inp_1>("sender1","Network"),
  cadmium::dynamic::translate::make_IC<outp_2, sender_defs::acknowledgement_in>("Network","sender1"),
  cadmium::dynamic::translate::make_IC<receiver_defs::out, inp_2>("receiver1","Network"),
  cadmium::dynamic::translate::make_IC<outp_1, receiver_defs::in>("Network","receiver1")
};
std::shared_ptr<cadmium::dynamic::modeling::coupled<TIME>> ABPSimulator = std::make_shared<cadmium::dynamic::modeling::coupled<TIME>>(
 "ABPSimulator", 
 submodels_ABPSimulator, 
 iports_ABPSimulator, 
 oports_ABPSimulator, 
 eics_ABPSimulator, 
 eocs_ABPSimulator, 
 ics_ABPSimulator 
  );


/************************/
/*******TOP MODEL********/
/************************/
cadmium::dynamic::modeling::Ports in_ports_top = {};
cadmium::dynamic::modeling::Ports out_ports_top = {typeid(outp_pack),typeid(outp_ack)};
cadmium::dynamic::modeling::Models submodels_top = {generator_con, ABPSimulator};
cadmium::dynamic::modeling::EICs eics_top = {};
cadmium::dynamic::modeling::EOCs eocs_top = {
  cadmium::dynamic::translate::make_EOC<outp_pack,outp_pack>("ABPSimulator"),
cadmium::dynamic::translate::make_EOC<outp_pack,outp_ack>("ABPSimulator")
};
cadmium::dynamic::modeling::ICs ics_top = {
  cadmium::dynamic::translate::make_IC<iestream_input_defs<message_t>::out,inp_control>("generator_con","ABPSimulator")
};
std::shared_ptr<cadmium::dynamic::modeling::coupled<TIME>> TOP = std::make_shared<cadmium::dynamic::modeling::coupled<TIME>>(
 "TOP", 
 submodels_top, 
 in_ports_top, 
 out_ports_top, 
 eics_top, 
 eocs_top, 
 ics_top 
  );

///****************////

    auto elapsed1 = std::chrono::duration_cast<std::chrono::duration<double, std::ratio<1>>>(hclock::now() - start).count();
    cout << "Model Created. Elapsed time: " << elapsed1 << "sec" << endl;
    
    cadmium::dynamic::engine::runner<NDTime, logger_top> r(TOP, {0});
    elapsed1 = std::chrono::duration_cast<std::chrono::duration<double, std::ratio<1>>>(hclock::now() - start).count();
    cout << "Runner Created. Elapsed time: " << elapsed1 << "sec" << endl;

    cout << "Simulation starts" << endl;

    r.run_until(NDTime("04:00:00:000"));
    auto elapsed = std::chrono::duration_cast<std::chrono::duration<double, std::ratio<1>>>(hclock::now() - start).count();
    cout << "Simulation took:" << elapsed << "sec" << endl;

    /**
     * File pointers has been initiated to handle the file operation fopen() 
     * to read from .txt files and write the formatted outputs in .csv files.
     * The formatting is performed with the help of filter() function 
     * by passing the arguments input and output
     */
    FILE *input;
    FILE *output;
    input = fopen("../data/output/abp_output.txt","r");
    output = fopen("../data/output/abp_processed_output.csv","w");
    filter(input,output);
    fclose(input);
    fclose(output);
    

    /**
     * File pointers has been initiated to handle the file operation fopen() 
     * to read from .txt files and write the formatted outputs in .csv files 
     * for the inputs (start_time, end_time, components) specified by the user
     * during the run-time of the program. The formatting is performed with 
     * the help of user_filter() function by passing the arguments 
     * input and output. 
     */
    FILE *fpp;
    FILE *fppout;
    fpp = fopen("../data/output/abp_output.txt","r");
    fppout = fopen("../data/output/abp_query_output.csv","w");
    main_test(fpp,fppout);
    fclose(fpp);
    fclose(fppout);
    
    return 0;
}
