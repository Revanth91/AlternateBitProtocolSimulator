/** \brief This main file is for the implementing the operations of Sender file
 *
 * This following main.cpp file prepares the application generator for 
 * control and acknowledgment and sends input as filepath, 
 * stores the output.Later, generates all the data logs using 
 * the cadmium library files. It executes as per input provided in the input 
 * file and performs simulation till mentioned 04:00:00:000 time is reaches.
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

#include "../../../lib/vendor/ndtime.hpp"
#include "../../../lib/vendor/iestream.hpp"
#include "../../../include/data_structures/message.hpp"
#include "../../../include/atomics/sendercadmium.hpp"

using namespace std;
using hclock = chrono::high_resolution_clock;
using TIME = NDTime;

/**
* Setting up in_ports for messages 
*/

struct inp_controll : public cadmium::in_port<message_t> {
};
struct inp_ack : public cadmium::in_port<message_t> {
};

/**
* Setting up out_ports for messages 
*/

struct outp_ack : public cadmium::out_port<message_t> {
};
struct outp_data : public cadmium::out_port<message_t> {
};
struct outp_pack : public cadmium::out_port<message_t> {
};

/**
 * The application generator is the class that takes respective file path
 * parameter and waits for input
 * @tParam message T
 */
template<typename T>
class ApplicationGen : public iestream_input<message_t, T> {
    public:
        ApplicationGen() = default;
        /**
         * This is a contructor with parameters for the class of application 
         * generator takes the input file path which contains 
         * the input that is fed in to the application to execute
         * @param file_path
         */
        ApplicationGen(const char* file_path) : iestream_input<message_t, T>(
            file_path
        ) {}
};

int main() {
    /**
     * to measure simulation execution time
     */
    auto start = hclock::now(); 

    /**
     * Generating messages and operation logs which are being fed
     * at execution time of this application and storing them 
     * in the respective path as mentioned to sender_test_output 
     * file as indicated in the out_data.
     */
    static std::ofstream out_data(
        "../test/data/sender/sender_test_output.txt"
    );

    /**
     * Sink provider structure which is helpfull to
     * call the ostream which is the output stream
     * and returns data stored out_data in the respective file
     */
    struct oss_sink_provider {
        static std::ostream& sink() {
            return out_data;
        }
    };

    /**
     * Loggers below mentioned are helpfull with cadmium 
     * to call source loggers strcture which can generate the log 
     * files in a specific mentioned formats. Later, store
     * it in variables and also to be logged to the file
     */

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

    /**
     * Obtaining sender_input_test_control_in text file for execution and
     * execut for number of input times 
     */
    string input_data_control 
        = "../test/data/sender/sender_input_test_control_in.txt";
    const char * i_input_data_control = input_data_control.c_str();

    /**
     * Initialized generator_ con consists of path of output file, TIME and 
     * given input which generates the output
     */

    std::shared_ptr<cadmium::dynamic::modeling::model>generator_con 
        = cadmium::dynamic::translate::make_dynamic_atomic_model
          <ApplicationGen, TIME, const char*>(
          "generator_con", std::move(i_input_data_control)
        );

    /**
     * This will get sender_input_test_ack_In file for execution and
     * runs the execution for number of i_input_data_ack times
     */

    string input_data_ack = "sender_input_test_ack_In.txt";
    const char * i_input_data_ack = input_data_ack.c_str();

    /**
     * Initializes the generator_ack  where it has the output file specific 
     * path, TIME along with given input generates the output
     */

    std::shared_ptr<cadmium::dynamic::modeling::model>generator_ack 
        = cadmium::dynamic::translate::make_dynamic_atomic_model
          <ApplicationGen, TIME, const char*>(
          "generator_ack", std::move(i_input_data_ack)
        );

    /**
     * Identification of the output data, which been sent from sender1
     */

    std::shared_ptr<cadmium::dynamic::modeling::model>sender1 
        = cadmium::dynamic::translate::make_dynamic_atomic_model
          <Sender, TIME>("sender1");

    /**
     * Top model operations were been stored and performed 
     * each time frame and then store in respective output file
     */

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

    /**
     * Generating model and elapsed time is measured in 
     * seconds. Generate runner and elapsed time measurement in
     * seconds. Later, Simulation begins and the time to complete 
     * the simulation. Simulation runs until 04:00:00:000 time period.
     */
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
    return 0;
}