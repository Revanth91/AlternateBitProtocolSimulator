/** \brief subnet_cadmium header file implements class Subnet
*
* The role of the subnet component is to receive the packets and send the 
* packets after a specific duration (i.e., delay). Only 95% of the packets 
* will be send through the subnet and the remaining 5% will be lost through 
* the subnet. This is introduced to have unreliability in the network. 
*
* Whenever the subnet receives a packet from the sender, they
* will be in active phase and they will send a acknowledgement or packet
* with a probability of 95% after a specific delay. Once the 
* acknowledgement or packet are sent, they will go to passive phase (default).
*/

/**
* Organization    : ARSLab - Carleton University
* Original author : Cristina Ruiz Martin
* Modified by     : Revanth Sridhar and Manoj Goli
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

/**
* Below are the structures used to define the input and output ports.
*/
struct subnet_defs {
    struct out : public out_port<message_t> {
    };
    struct in : public in_port<message_t> {
    };
};

/**
* This is a meta-model,it should be overloaded for declaring the "id" parameter.
* The Subnet class passes the packets between sender and receiver after a 
* specific delay. 
*/ 
template<typename TIME>
class Subnet {

    /**
    * Putting definitions in context. 
    */
    using defs = subnet_defs; 
    public:
        
        /**
        * Parameters to be overwriten when instantiating the atomic model.
        */

        /**
        * Subnet() is a default constructor to this class. 
        * It is used to initialize Subnet components transmitting status to false, 
        * and state index to zero. 
        */
        Subnet() noexcept {
            state.transmiting = false;
            state.index = 0;
        }
                
        /**
        * state_type structure is to provide the state definition and it uses/
        * declares three variables namely transmitting, packet, index. 
        */ 
        struct state_type {
            bool transmiting; /**< Variable transmitting is of boolean type
                                   which sets the subnet to either to active
                                   or passive phase.
                              */
            int packet;       /**< It is used to provide the packet number. */
            int index;        /**< It is used to provide index number. */
        }; 
        state_type state;
                
        /** 
        * input and output ports definition.
        */
        using input_ports = std::tuple<typename defs::in>;
        using output_ports = std::tuple<typename defs::out>;

        /**
        * internal transition function sets the state of trasmitting 
        * variable to false (i.e., passive).
        */
        void internal_transition() {
            state.transmiting = false;  
        }

        /**
        * external transition function  calculates the size of messages. If 
        * size is greater than 1, it asserts with a output as one message 
        * at a time. If the condition fails, it provides the packet value 
        * based on the message. Subsequently, it sets the state transmitting
        * variable to true i.e., active.
        * @param e is the first arguement
        * @param mbs is the second arguement 
        */
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

        /** 
        * confluence transition function is used to execute internal_transition
        * function (takes no arguements) and then external_transition function
        * (take two arguements e and mbs).
        * @param e is the first arguement.
        * @param mbs is the second arguement.
        */
        void confluence_transition(TIME e, 
                                   typename make_message_bags<input_ports>::type mbs) {
            internal_transition();
            external_transition(TIME(), std::move(mbs));
        }

        /**
        * output function is used to transmit the packet value through ports.
        * Before sending the value, it divides the current random number with 
        * the maximum random number and if the results is less the 95%, the
        * packet value will be transmitted. Meaning that the other 5% will not 
        * be transmitted. This is used to introduce the unpredictability of 
        * subnet in transmitting the packets.
        * It sends the results via the output ports.
        * @return returns bags.
        */
        typename make_message_bags<output_ports>::type output() const {
            typename make_message_bags<output_ports>::type bags;
            message_t out;
            if ((double)rand()/(double)RAND_MAX  < 0.95) {
                out.value = state.packet;
                get_messages<typename defs::out>(bags).push_back(out);
            }
            return bags;
        }

        /** 
        * time_advance function is used to set the next_internal time by 
        * checking whether the state transmitting is true/false 
        * (i.e.,active/passive). If it is true, then a delay is introduced 
        * which is a non-deterministic value expressed by normal distribution
        * with mean as 3.0 and standard deviation as 1.0. After the delay, 
        * the time is set to next_internal. If the condition is false, 
        * then next_internal is set to infinity.
        * @return returns next_internal.
        */
        TIME time_advance() const {
            std::default_random_engine generator;
            std::normal_distribution<double> distribution(3.0, 1.0); 
            TIME next_internal;
            if (state.transmiting) {
                std::initializer_list<int> time 
                    = {0, 0, static_cast<int>(round(distribution(generator)))};
                /**
                * Time is in HH:MM:SS format.
                */
                next_internal = TIME(time);
            }
            else {
                next_internal = std::numeric_limits<TIME>::infinity();
            }    
            return next_internal;
        }

        /**
        * friend function is used to provide the index number and 
        * transmitting state. 
        * @param output_stream.
        * @param i.
        * @return returns output_stream.
        */ 
        friend std::ostringstream& operator<<(std::ostringstream& output_stream, 
                                              const typename Subnet<TIME>::state_type& i) {
            output_stream<<"index: "<<i.index<<" & transmiting: "<<i.transmiting; 
            return output_stream;
        }
};    

#endif // __SUBNET_CADMIUM_HPP__