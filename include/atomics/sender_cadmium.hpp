/** \brief The header file sender_cadmium implements the class sender.
*
*When received an external signal, the sender switches from passive 
*phase to active phase. Once it is activated, it will start sending 
*the packets in the form of alternating bits. Everytime when a packet 
*is sent, this waits for a specific amount of waiting time (i.e delay). 
*And if acknowledgement is not received with in the a specific time window, 
*the packet will be re sent again by the sender  with an alternating bit. 
*However, if it receives the expected outputs, the sender will 
*transmit the next packet. If there are no more packets to transmit 
*the sender will go to the passive state (Default) again.
*/

/**
* Organization    : ARSLab - Carleton University
* Original author : Cristina Ruiz Martin
* Modified by     : Manoj Goli and Revanth Sridhar
*/

#ifndef __SENDER_CADMIUM_HPP__
#define __SENDER_CADMIUM_HPP__

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
* Below are the structure Port definitions for packets sent out and 
* acknowldgement received, data ouput, control input and
* incoming acknowldgement
*/
struct sender_defs {
    struct packet_sent_out : public out_port<message_t> {
    };
    struct ack_received_out : public out_port<message_t> {
    };
    struct data_out : public out_port<message_t> {
    };
    struct control_in : public in_port<message_t> {
    };
    struct acknowledgement_in : public in_port<message_t> {
    };
};

/** 
* The Sender class receives the message and sends out respective acknowledgement.
*/
template<typename TIME>
class Sender {
    /**
    * putting definitions in context
    */
    using defs = sender_defs;
    public:
        /**Parameters to be overwriten when instantiating the atomic model
        */
        TIME PREPARATION_TIME;/**< PREPARATION_TIME is a constant 
                                   in HH:MM:SS format which contains the time
                                   delay from acknowledgement to output. 
                                */
        TIME TIME_OUT;/**< TIME_OUT is a constant that contains time 
                           delay from output to acknowledgement 
                        */
        
        /** 
        * Default constructor for the Receiver class.
        * Initializes  PREPARATION_TIME delay constant, TIME_OUT constant, 
        * set alternate bit state to 0 and also initializing next internal state
        * and set model active state as false
        */
        Sender() noexcept {
            PREPARATION_TIME = TIME(PREPARATION_TIME_CONSTANT);
            TIME_OUT = TIME("00:00:20");
            state.alt_bit = 0;
            state.next_internal = std::numeric_limits<TIME>::infinity();
            state.model_active = false;
        }
            
        /**
        * state_type structure provides the state definition 
        *for the below state variables.
        */  
        struct state_type {
            bool ack;
            int packet_number;
            int total_packet_number;
            int alt_bit;
            bool sending;
            bool model_active;
            TIME next_internal;
        }; 
        state_type state;
        
        /** 
        * input and output ports definition.
        */
        using input_ports = std::tuple<
            typename defs::control_in, 
            typename defs::acknowledgement_in
        >;
        using output_ports = std::tuple<
            typename defs::packet_sent_out, 
            typename defs::ack_received_out, 
            typename defs::data_out
        >;

        /**
        * internal_transitions is a Function that 
        * performs internal transition where it sets the 
        * next state based on the current state.
        */
        void internal_transition() {
            if (state.ack) {
                if (state.packet_number < state.total_packet_number) {
                    state.packet_number ++;
                    state.ack = false;
                    state.alt_bit = (state.alt_bit + 1) % 2;
                    state.sending = true;
                    state.model_active = true; 
                    state.next_internal = PREPARATION_TIME;   
                } 
                else {
                    state.model_active = false;
                    state.next_internal 
                        = std::numeric_limits<TIME>::infinity();
                }
            } 
            else {
                if (state.sending) {
                    state.sending = false;
                    state.model_active = true;
                    state.next_internal = TIME_OUT;
                } 
                else {
                    state.sending = true;
                    state.model_active = true;
                    state.next_internal = PREPARATION_TIME;
                } 
            }
        }

        /**
         * Function that performs external transition.
         * the function retrieves the messages, messages count is
         * is more than one, it asserts that only one message is
         * expected per time unit. Later, it determines
         * and also sets the next state, based on the current state.
         * And it also sets the time of the next internal transition. 
         * @param e is of type time
         * @param mbs is of type message bags 
         */
        void external_transition(TIME e, 
                                 typename make_message_bags<input_ports>::type mbs  
        ) {
            if((get_messages<typename defs::control_in>(mbs).size()+
                    get_messages<typename defs::acknowledgement_in>(mbs).size()) > 1) {
                        assert(false && "one message per time uniti");
            }
            for(const auto &x : get_messages<typename defs::control_in>(mbs)) {
                if(state.model_active == false) {
                    state.total_packet_number = static_cast <int> (x.value);
                    if (state.total_packet_number > 0) {
                        state.packet_number = 1;
                        state.ack = false;
                        state.sending = true;
                        state.alt_bit = state.packet_number % 2;  //set initial alt_bit
                        state.model_active = true;
                        state.next_internal = PREPARATION_TIME;
                    }
                    else {
                        if (state.next_internal !=
                            std::numeric_limits<TIME>::infinity()
                        ) {
                            state.next_internal = state.next_internal - e;
                        }
                    }
                }
            }
            for(const auto &x : get_messages<typename defs::acknowledgement_in>(mbs)) {
                if(state.model_active == true) {
                    if (state.alt_bit == static_cast<int>(x.value)) {
                        state.ack = true;
                        state.sending = false;
                        state.next_internal = TIME("00:00:00");
                    } 
                    else {
                        if(state.next_internal != 
                            std::numeric_limits<TIME>::infinity()
                        ) {
                            state.next_internal = state.next_internal - e;
                        }
                    }
                }
            }  
        }

        /** 
         * Confluence transition is a function that calls internal 
         * transition fucntion which is followed by the external transition function.
         * @param e of type time 
         * @param mbs of type message bags
         */
        void confluence_transition(TIME e, 
                                   typename make_message_bags<input_ports>::type mbs
        ) {
            internal_transition();
            external_transition(TIME(), std::move(mbs));
        }

        /**
         * This a function that transmits the packet to the output port
         * When it is in sending state, the packet is calculated as
         * 10 times packet number added with alternating bit
         * and finally assigned to output. Later, output is pushed
         * to message bags. When in acknowledgement state, the alt_bit is 
         * assigned to output and the output is pushed
         * to message bags.
         * @return Message bags
         */
        typename make_message_bags<output_ports>::type output() const {
            typename make_message_bags<output_ports>::type bags;
            message_t out;
            if (state.sending) {
                out.value = state.packet_number * 10 + state.alt_bit;
                get_messages<typename defs::data_out>(bags).push_back(out);
                out.value = state.packet_number;
                get_messages<
                    typename defs::packet_sent_out
                >(bags).push_back(out);
            }
            else {
                if (state.ack) {
                    out.value = state.alt_bit;
                    get_messages<
                        typename defs::ack_received_out
                    >(bags).push_back(out);
                }
            }   
            return bags;
        }

        /**
        * This is a Function which returns the next internal transition time.
        * @return Next internal time
        */
        TIME time_advance() const {
            return state.next_internal;
        }

        /**
         * friend function is used to send the packet number and 
         * total packet number to ostring stream.
         * @param os the ostring stream
         * @param i structure state_type
         * @return os the ostring stream
         */
        friend std::ostringstream& operator<<(std::ostringstream& output_stream,
                                              const typename Sender<TIME>::state_type& i) {
            output_stream<<"packet_number: "<<i.packet_number<<" & total_packet_number: "
                                                  <<i.total_packet_number;
            return output_stream;
        }
};     

#endif // __SENDER_CADMIUM_HPP__