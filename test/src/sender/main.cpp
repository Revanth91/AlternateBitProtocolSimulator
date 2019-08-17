/** \brief This file is for sender main.cpp



*/


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
#include "../../../include/atomics/sendercadmium.hpp"
#include "../../../src/transform.cpp"

using namespace std;
using hclock = chrono::high_resolution_clock;
using TIME = NDTime;

/***** SETING INPUT PORTS FOR COUPLEDs *****/
struct inp_controll : public cadmium::in_port<message_t> {
};
struct inp_ack : public cadmium::in_port<message_t> {
};

/***** SETING OUTPUT PORTS FOR COUPLEDs *****/
struct outp_ack : public cadmium::out_port<message_t> {
};
struct outp_data : public cadmium::out_port<message_t> {
};
struct outp_pack : public cadmium::out_port<message_t> {
};

/********************************************/
/****** APPLICATION GENERATOR *******************/
/********************************************/
template<typename T>
class ApplicationGen : public iestream_input<message_t, T> {
    public:
        ApplicationGen() = default;
        ApplicationGen(const char* file_path) : iestream_input<message_t, T>(
            file_path
        ) {}
};

int main() {
    auto start = hclock::now(); //to measure simulation execution time

    /*************** Loggers *******************/
    static std::ofstream out_data(
        "../test/data/sender/sender_test_output.txt"
    );
    struct oss_sink_provider {
        static std::ostream& sink() {
            return out_data;
        }
    };

    using info = cadmium::logger::logger<
        cadmium::logger::logger_info, 
        cadmium::dynamic::logger::formatter<TIME>, 
        oss_sink_provider
    >;
    using debug = cadmium::logger::logger<
        cadmium::logger::logger_debug,
        cadmium::dynamic::logger::formatter<TIME>,
        oss_sink_provider
    >;
    using state = cadmium::logger::logger<
        cadmium::logger::logger_state,
        cadmium::dynamic::logger::formatter<TIME>, 
        oss_sink_provider
    >;
    using log_messages = cadmium::logger::logger<
        cadmium::logger::logger_messages,
        cadmium::dynamic::logger::formatter<TIME>,
        oss_sink_provider
    >;
    using routing = cadmium::logger::logger<
        cadmium::logger::logger_message_routing,
        cadmium::dynamic::logger::formatter<TIME>,
        oss_sink_provider
    >;
    using global_time = cadmium::logger::logger<
        cadmium::logger::logger_global_time,
        cadmium::dynamic::logger::formatter<TIME>,
        oss_sink_provider
    >;
    using local_time = cadmium::logger::logger<
        cadmium::logger::logger_local_time,
        cadmium::dynamic::logger::formatter<TIME>,
        oss_sink_provider
    >;
    using log_all = cadmium::logger::multilogger<
        info, debug, state, log_messages, routing,
        global_time, local_time
    >;
    using logger_top = cadmium::logger::multilogger<
        log_messages, global_time
    >;

    /*******************************************/
    /********************************************/
    /****** APPLICATION GENERATOR *******************/
    /********************************************/
    string input_data_control 
        = "../test/data/sender/sender_input_test_control_in.txt";
    const char * i_input_data_control = input_data_control.c_str();

    std::shared_ptr<cadmium::dynamic::modeling::model>generator_con 
        = cadmium::dynamic::translate::make_dynamic_atomic_model
          <ApplicationGen, TIME, const char*>(
          "generator_con", std::move(i_input_data_control)
        );

    string input_data_ack = "sender_input_test_ack_In.txt";
    const char * i_input_data_ack = input_data_ack.c_str();

    std::shared_ptr<cadmium::dynamic::modeling::model>generator_ack 
        = cadmium::dynamic::translate::make_dynamic_atomic_model
          <ApplicationGen, TIME, const char*>(
          "generator_ack", std::move(i_input_data_ack)
        );

    /********************************************/
    /****** SENDER *******************/
    /********************************************/

    std::shared_ptr<cadmium::dynamic::modeling::model>sender1 
        = cadmium::dynamic::translate::make_dynamic_atomic_model
          <Sender, TIME>("sender1");

    /************************/
    /*******TOP MODEL********/
    /************************/
    cadmium::dynamic::modeling::Ports in_ports_top = {
    };
    cadmium::dynamic::modeling::Ports out_ports_top = {
        typeid(outp_data), typeid(outp_pack), typeid(outp_ack)
    };
    cadmium::dynamic::modeling::Models submodels_top = {
        generator_con, generator_ack, sender1
    };
    cadmium::dynamic::modeling::EICs eics_top = {
    };
    cadmium::dynamic::modeling::EOCs eocs_top = {
        cadmium::dynamic::translate::make_EOC<
            sender_defs::packet_sent_out, outp_pack
        >("sender1"),
        cadmium::dynamic::translate::make_EOC<
        sender_defs::ack_received_out, outp_ack
        >("sender1"),
        cadmium::dynamic::translate::make_EOC<
        sender_defs::data_out, outp_data
        >("sender1")
    };
    cadmium::dynamic::modeling::ICs ics_top = {
        cadmium::dynamic::translate::make_IC<
            iestream_input_defs<message_t>::out, sender_defs::control_in
        >("generator_con", "sender1"),
        cadmium::dynamic::translate::make_IC<
            iestream_input_defs<message_t>::out, sender_defs::acknowledgement_in
        >("generator_ack", "sender1")
    };
    std::shared_ptr<cadmium::dynamic::modeling::coupled<TIME>>TOP 
        = std::make_shared<cadmium::dynamic::modeling::coupled<TIME>>(
          "TOP", submodels_top, in_ports_top, out_ports_top, eics_top, 
          eocs_top, ics_top
        );

    ///****************////
    auto elapsed1 = std::chrono::duration_cast<
        std::chrono::duration<double, std::ratio<1>>
    >(hclock::now() - start).count();
    cout<<"Model Created. Elapsed time: "<<elapsed1<<"sec"<<endl;
    cadmium::dynamic::engine::runner<NDTime, logger_top>r(TOP, {0});
    elapsed1 = std::chrono::duration_cast<
        std::chrono::duration<double, std::ratio<1>>
    >(hclock::now() - start).count();
    cout<<"Runner Created. Elapsed time: "<<elapsed1<<"sec"<<endl;
    cout<<"Simulation starts"<<endl;
    r.run_until(NDTime("04:00:00:000"));
    auto elapsed = std::chrono::duration_cast<
        std::chrono::duration<double, std::ratio<1>>
    >(hclock::now() - start).count();
    cout<<"Simulation took:"<<elapsed<<"sec"<<endl;

    FILE *input;
    FILE *output;
    input = fopen("../test/data/sender/sender_test_output.txt","r");
    output = fopen("../test/data/sender/sender_test_processed_output.csv","w");
    filter(input,output);

    return 0;
}