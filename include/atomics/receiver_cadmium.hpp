/** \brief receiver_cadmium header file implements class Receiver.
*
* The role of the receiver component is to receive the packet through
* input ports from subnet and send back an acknowledgement through the 
* output ports after a specific time duration (i.e.,delay). 
* The delay in responding back to the received packet is constant.
*
* Whenever the receiver receives a packet from the sender via subnet, it
* will be in active phase and will send a acknowledgement after a delay. Once 
* the acknowledgement is sent, it will go back to passive phase (default). 
*/

/**
* Organization    : ARSLab - Carleton University
* Original author : Cristina Ruiz Martin
* Modified by     : Revanth Sridhar and Manoj Goli
*/

#ifndef __RECEIVER_CADMIUM_HPP__
#define __RECEIVER_CADMIUM_HPP__

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

#define PREPARATION_TIME_CONSTANT "00:00:10"

using namespace cadmium;
using namespace std;

/**
* Below are the structures used to define the input and output ports.
*/
struct receiver_defs {
    struct out : public out_port<message_t> {
    };
    struct in : public in_port<message_t> {
    };
};

/**
* The Receiver class receives the packet from subnet and provides the replies with 
* an acknowledgement_number through ports.
*/   
template<typename TIME>
class Receiver {

    /**
    * Putting definitions in context. 
    */
    using defs = receiver_defs; 
    public:
        
        /**
        * Parameters to be overwriten when instantiating the atomic model.
        */
        TIME PREPARATION_TIME; /**< PREPARATION_TIME is a constant in 
                                    HH:MM:SS format. This is a delay which 
                                    the receiver takes to provide 
                                    acknowledgement whenever it receives 
                                    a packet.
                               */
        
        /**
        * Receiver() is a default constructor to this class. 
        * It is to used initialize recevier components sending status to false, 
        * acknowledgement_number to 0 and PREPARATION_TIME to 00:00:10.
        */
        Receiver() noexcept {
            PREPARATION_TIME = TIME(PREPARATION_TIME_CONSTANT);
            state.acknowledgement_number = 0;
            state.sending = false;
        }
            
        /**
        * state_type structure is to provide the state definition and it uses/
        * declares two variables namely acknowledgement_number and sending.
        */    
        struct state_type {
            int acknowledgement_number;  /**< it is used to provide 
                                              acknowledgement for receiving a
                                              packet from sender.
                                         */ 
            bool sending;                /**< variable sending is of boolean 
                                              type which sets the receiver to
                                              either to active or passive phase.
                                         */
        }; 
        state_type state;
        
        /** 
        * input and output ports definition.
        */
        using input_ports = std::tuple<typename defs::in>;
        using output_ports = std::tuple<typename defs::out>;

        /**
        * internal transition function sets the state of 
        * sending variable to false (i.e., passive).
        */
        void internal_transition() {
            state.sending = false; 
        }
 
        /**
        * external transition function  calculates the size of messages. If 
        * size is greater than 1, it asserts with an output as one message 
        * per unit time. If the condition is false,it provides the 
        * acknowledgement number based on the message. 
        * It also sets the state sending variable to true i.e., active.
        * @param e is the first arguement.
        * @param mbs is the second arguement.
        */
        void external_transition(TIME e, 
                                 typename make_message_bags<input_ports>::type mbs) { 
            if (get_messages<typename defs::in>(mbs).size() > 1) {
                assert(false && "one message per time uniti");    
            } 
            for (const auto &x : get_messages<typename defs::in>(mbs)) {
                state.acknowledgement_number = static_cast<int>(x.value);
                state.sending = true;
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
        * output function is used to calculate the value of acknowledgement. 
        * It is obtained by performing modulo operation on acknowledgement_number. 
        * It sends the results via the output ports.
        * @return returns bags.
        */
        typename make_message_bags<output_ports>::type output() const {
            typename make_message_bags<output_ports>::type bags;
            message_t out;              
            out.value = state.acknowledgement_number % 10;
            get_messages<typename defs::out>(bags).push_back(out);       
            return bags;
        }

        /** 
        * time_advance function is used to set the next_internal time by 
        * checking whether the state sending is true/false (i.e.,active/passive).
        * if it is true, then preparation time is set to next_internal. If the 
        * condition is false, then next_internal is set to infinity.
        * @return returns next_internal. 
        */
        TIME time_advance() const {  
            TIME next_internal;
            if (state.sending) {
                next_internal = PREPARATION_TIME;
            }
            else {
                next_internal = std::numeric_limits<TIME>::infinity();
            }    
            return next_internal;
        }

        /**
        * friend function is used to provide the acknowledgement number. 
        * @param output_stream.
        * @param i.
        * @return returns output_stream.
        */    
        friend std::ostringstream& operator<<(std::ostringstream& output_stream, 
                                              const typename Receiver<TIME>::state_type& i) {
            output_stream<<"acknowledgement_number: "<<i.acknowledgement_number; 
            return output_stream;
        }
};     

#endif /** __RECEIVER_CADMIUM_HPP__ */
