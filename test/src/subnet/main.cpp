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

#include "../../../lib/DESTimes/include/NDTime.hpp"
#include "../../../lib/iestream.hpp"
#include "../../../include/data_structures/message.hpp"
#include "../../../include/atomics/subnet_cadmium.hpp"
#include "../../../src/transform.cpp"

#define SUBNET_INPUT_FILEPATH "../test/data/subnet/subnet_input_test.txt"
#define SUBNET_OUTPUT_FILEPATH "../test/data/subnet/subnet_test_output.txt"

using namespace std;
using hclock = chrono::high_resolution_clock;
using TIME = NDTime;

/***** SETING INPUT PORTS FOR COUPLEDs *****/
struct inp_in : public cadmium::in_port<message_t> {};

/***** SETING OUTPUT PORTS FOR COUPLEDs *****/
struct outp_out : public cadmium::out_port<message_t> {};

/********************************************/
/****** APPLICATION GENERATOR *******************/
/********************************************/
template<typename T>
class ApplicationGen : public iestream_input<message_t,T> {
    public:
    ApplicationGen() = default;
    ApplicationGen(const char* file_path):iestream_input<message_t,T>(file_path) {}
};

int main() {
    auto start = hclock::now(); //to measure simulation execution time

/*************** Loggers *******************/
    static std::ofstream out_data(SUBNET_OUTPUT_FILEPATH);
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
string input_data = SUBNET_INPUT_FILEPATH;
const char * i_input_data = input_data.c_str();

std::shared_ptr<cadmium::dynamic::modeling::model> generator = cadmium::dynamic::translate::make_dynamic_atomic_model<ApplicationGen, TIME, const char* >("generator" , std::move(i_input_data));

/********************************************/
/****** SUBNET *******************/
/********************************************/

std::shared_ptr<cadmium::dynamic::modeling::model> subnet1 = cadmium::dynamic::translate::make_dynamic_atomic_model<Subnet, TIME>("subnet1");

/************************/
/*******TOP MODEL********/
/************************/
cadmium::dynamic::modeling::Ports in_ports_top = {};
cadmium::dynamic::modeling::Ports out_ports_top
 = {
    typeid(outp_out)
};
cadmium::dynamic::modeling::Models submodels_top = {generator, subnet1};
cadmium::dynamic::modeling::EICs eics_top = {};
cadmium::dynamic::modeling::EOCs eocs_top = {
    cadmium::dynamic::translate::make_EOC<subnet_defs::out,outp_out>("subnet1")
};
cadmium::dynamic::modeling::ICs ics_top = {
    cadmium::dynamic::translate::make_IC<iestream_input_defs<message_t>::out,subnet_defs::in>("generator","subnet1")
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
    input = fopen("../test/data/subnet/subnet_test_output.txt","r");
    output = fopen("../test/data/subnet/subnet_test_processed_output.csv","w");
    filter(input,output);
    
    return 0;
}