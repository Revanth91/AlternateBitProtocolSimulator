/** \brief subnet_cadmium header file implements class Subnet
*
* The role of the subnet component is to just receive and send the packets
* after a specific delay. Only 95% of the packets will be send through 
* the subnet and the remain 5% will be lost through the subnet. This is 
* introduced to have unreliability in the network. 
*
* Whenever the subnet receives a packet from the sender, they
* will be in active phase and they will send a acknowledgement or packet
* with a probability of 95% after a specific time. Once the 
* acknowledgement or packet are sent, they will go to passive phase (default).
*
*/

/**
* Organization    : ARSLab - Carleton University
* Original author : Cristina Ruiz Martin
* Modified by     : Revanth Sridhar and Manoj Goli
*
*/

#ifndef __SUBNET_CADMIUM_HPP__
#define __SUBNET_CADMIUM_HPP__

#include <cadmium/modeling/ports.hpp>
#include <cadmium/modeling/message_bag.hpp>
#include <limits>
#include <math.h> 
#include <assert.h>
#include <memory>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <algorithm>
#include <limits>
#include <random>

#include "../data_structures/message.hpp"

using namespace cadmium;
using namespace std;

//Port definition
struct subnet_defs {
    struct out : public out_port<message_t> {
    };
    struct in : public in_port<message_t> {
    };
};

//This is a meta-model, 
//it should be overloaded for declaring the "id" parameter
template<typename TIME>
class Subnet {
    using defs = subnet_defs; // putting definitions in context
    public:
        //Parameters to be overwriten when instantiating the atomic model
        // default constructor
        Subnet() noexcept {
            state.transmiting = false;
            state.index = 0;
        }
                
        // state definition
        struct state_type {
            bool transmiting;
            int packet;
            int index;
        }; 
        state_type state;
                
        // ports definition
        using input_ports = std::tuple<typename defs::in>;
        using output_ports = std::tuple<typename defs::out>;

        // internal transition
        void internal_transition() {
            state.transmiting = false;  
        }

        // external transition
        void external_transition(TIME e, 
                                 typename make_message_bags<input_ports>::type mbs) { 
            state.index ++;
            if (get_messages<typename defs::in>(mbs).size() > 1) {
                assert(false && "One message at a time");                
            }
            for (const auto &x : get_messages<typename defs::in>(mbs)) {
                state.packet = static_cast<int>(x.value);
                state.transmiting = true; 
            }               
        }

        // confluence transition
        void confluence_transition(TIME e, 
                                   typename make_message_bags<input_ports>::type mbs) {
            internal_transition();
            external_transition(TIME(), std::move(mbs));
        }

        // output function
        typename make_message_bags<output_ports>::type output() const {
            typename make_message_bags<output_ports>::type bags;
            message_t out;
            if ((double)rand()/(double)RAND_MAX  < 0.95) {
                out.value = state.packet;
                get_messages<typename defs::out>(bags).push_back(out);
            }
            return bags;
        }

        // time_advance function
        TIME time_advance() const {
            std::default_random_engine generator;
            std::normal_distribution<double> distribution(3.0, 1.0); 
            TIME next_internal;
            if (state.transmiting) {
                std::initializer_list<int> time 
                    = {0, 0, static_cast<int>(round(distribution(generator)))};
                // time is hour min and second
                next_internal = TIME(time);
            }
            else {
                next_internal = std::numeric_limits<TIME>::infinity();
            }    
            return next_internal;
        }

        friend std::ostringstream& operator<<(std::ostringstream& output_stream, 
                                              const typename Subnet<TIME>::state_type& i) {
            output_stream<<"index: "<<i.index<<" & transmiting: "<<i.transmiting; 
            return output_stream;
        }
};    

#endif // __SUBNET_CADMIUM_HPP__