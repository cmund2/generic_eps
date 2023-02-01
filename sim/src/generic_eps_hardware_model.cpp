#include <generic_eps_hardware_model.hpp>

namespace Nos3
{
    REGISTER_HARDWARE_MODEL(Generic_epsHardwareModel,"GENERIC_EPS");

    extern ItcLogger::Logger *sim_logger;

    Generic_epsHardwareModel::Generic_epsHardwareModel(const boost::property_tree::ptree& config) : SimIHardwareModel(config) 
    {
        /* Get the NOS engine connection string */
        std::string connection_string = config.get("common.nos-connection-string", "tcp://127.0.0.1:12001"); 
        sim_logger->info("Generic_epsHardwareModel::Generic_epsHardwareModel:  NOS Engine connection string: %s.", connection_string.c_str());

        /* Get a data provider */
        std::string dp_name = config.get("simulator.hardware-model.data-provider.type", "GENERIC_EPS_PROVIDER");
        _generic_eps_dp = SimDataProviderFactory::Instance().Create(dp_name, config);
        sim_logger->info("Generic_epsHardwareModel::Generic_epsHardwareModel:  Data provider %s created.", dp_name.c_str());

        /* Get on a protocol bus */
        /* Note: Initialized defaults in case value not found in config file */
        std::string bus_name = "i2c_0";
        int bus_address = 0x2B;
        if (config.get_child_optional("simulator.hardware-model.connections")) 
        {
            /* Loop through the connections for hardware model */
            BOOST_FOREACH(const boost::property_tree::ptree::value_type &v, config.get_child("simulator.hardware-model.connections"))
            {
                /* v.second is the child tree (v.first is the name of the child) */
                if (v.second.get("type", "").compare("i2c") == 0)
                {
                    /* Configuration found */
                    bus_name = v.second.get("bus-name", bus_name);
                    bus_address = v.second.get("bus-address", bus_address);
                    break;
                }
            }
        }
        _i2c_slave_connection = new I2CSlaveConnection(this, bus_address, connection_string, bus_name);
        sim_logger->info("Generic_epsHardwareModel::Generic_epsHardwareModel:  Now on I2C bus name %s as address 0x%02x.", bus_name.c_str(), bus_address);

        /* Get on the command bus - EPS receives commands here */
        std::string time_bus_name = "command";
        if (config.get_child_optional("hardware-model.connections")) 
        {
            /* Loop through the connections for the hardware model connections */
            BOOST_FOREACH(const boost::property_tree::ptree::value_type &v, config.get_child("hardware-model.connections"))
            {
                /* v.first is the name of the child */
                /* v.second is the child tree */
                if (v.second.get("type", "").compare("time") == 0) // 
                {
                    time_bus_name = v.second.get("bus-name", "command");
                    /* Found it... don't need to go through any more items*/
                    break; 
                }
            }
        }
        _time_bus.reset(new NosEngine::Client::Bus(_hub, connection_string, time_bus_name));
        sim_logger->info("Generic_epsHardwareModel::Generic_epsHardwareModel:  Now on time bus named %s.", time_bus_name.c_str());

        /* Get on the sim bus - EPS sends commands here */
        _sim_bus.reset(new NosEngine::Client::Bus(_hub, connection_string, time_bus_name));
        sim_logger->info("Generic_epsHardwareModel::Generic_epsHardwareModel:  Now on bus named %s.", time_bus_name.c_str());

        /* Initialize status for each switch */
        _init_switch[0]._node_name = config.get("hardware-model.connections.switch_0.node-name", "switch_0");
        _init_switch[0]._voltage = config.get("hardware-model.connections.switch_0.voltage", "3.3");
        _init_switch[0]._current = config.get("hardware-model.connections.switch_0.current", "0.25");
        _init_switch[0]._state = config.get("hardware-model.connections.switch_0.hex_status", "0000");

        _init_switch[1]._node_name = config.get("hardware-model.connections.switch_1.node-name", "switch_1");
        _init_switch[1]._voltage = config.get("hardware-model.connections.switch_1.voltage", "3.3");
        _init_switch[1]._current = config.get("hardware-model.connections.switch_1.current", "0.1");
        _init_switch[1]._state = config.get("hardware-model.connections.switch_1.hex_status", "0000");

        _init_switch[2]._node_name = config.get("hardware-model.connections.switch_2.node-name", "switch_2");
        _init_switch[2]._voltage = config.get("hardware-model.connections.switch_2.voltage", "5.0");
        _init_switch[2]._current = config.get("hardware-model.connections.switch_2.current", "0.2");
        _init_switch[2]._state = config.get("hardware-model.connections.switch_2.hex_status", "0000");

        _init_switch[3]._node_name = config.get("hardware-model.connections.switch_3.node-name", "switch_3");
        _init_switch[3]._voltage = config.get("hardware-model.connections.switch_3.voltage", "5.0");
        _init_switch[3]._current = config.get("hardware-model.connections.switch_3.current", "0.3");
        _init_switch[3]._state = config.get("hardware-model.connections.switch_3.hex_status", "0000");

        _init_switch[4]._node_name = config.get("hardware-model.connections.switch_4.node-name", "switch_4");
        _init_switch[4]._voltage = config.get("hardware-model.connections.switch_4.voltage", "12.0");
        _init_switch[4]._current = config.get("hardware-model.connections.switch_4.current", "0.4");
        _init_switch[4]._state = config.get("hardware-model.connections.switch_4.hex_status", "0000");

        _init_switch[5]._node_name = config.get("hardware-model.connections.switch_5.node-name", "switch_5");
        _init_switch[5]._voltage = config.get("hardware-model.connections.switch_5.voltage", "12.0");
        _init_switch[5]._current = config.get("hardware-model.connections.switch_5.current", "0.5");
        _init_switch[5]._state = config.get("hardware-model.connections.switch_5.hex_status", "0000");

        _init_switch[6]._node_name = config.get("hardware-model.connections.switch_6.node-name", "switch_6");
        _init_switch[6]._voltage = config.get("hardware-model.connections.switch_6.voltage", "3.3");
        _init_switch[6]._current = config.get("hardware-model.connections.switch_6.current", "0.6");
        _init_switch[6]._state = config.get("hardware-model.connections.switch_6.hex_status", "0000");

        _init_switch[7]._node_name = config.get("hardware-model.connections.switch_7.node-name", "switch_7");
        _init_switch[7]._voltage = config.get("hardware-model.connections.switch_7.voltage", "5.0");
        _init_switch[7]._current = config.get("hardware-model.connections.switch_7.current", "0.7");
        _init_switch[7]._state = config.get("hardware-model.connections.switch_7.hex_status", "0000");

        std::uint8_t i;
        for (i = 0; i < 8; i++)
        {
            _switch[i]._voltage = atoi((_init_switch[i]._voltage).c_str()) * 1000;
            _switch[i]._current = atoi((_init_switch[i]._current).c_str()) * 1000;
            _switch[i]._status = std::stoi((_init_switch[i]._state).c_str(), 0, 16);
        }

        /* Construction complete */
        sim_logger->info("Generic_epsHardwareModel::Generic_epsHardwareModel:  Construction complete.");
    }


    Generic_epsHardwareModel::~Generic_epsHardwareModel(void)
    {        
        /* Close the protocol bus */
       delete _i2c_slave_connection;
        _i2c_slave_connection = nullptr;

        /* Clean up the data provider */
        delete _generic_eps_dp;
        _generic_eps_dp = nullptr;

        /* The bus will clean up the time node */
    }


    /* Automagically set up by the base class to be called */
    void Generic_epsHardwareModel::command_callback(NosEngine::Common::Message msg)
    {
        /* Get the data out of the message */
        NosEngine::Common::DataBufferOverlay dbf(const_cast<NosEngine::Utility::Buffer&>(msg.buffer));
        sim_logger->info("Generic_epsHardwareModel::command_callback:  Received command: %s.", dbf.data);

        /* Do something with the data */
        std::string command = dbf.data;
        std::string response = "Generic_epsHardwareModel::command_callback:  INVALID COMMAND! (Try HELP)";
        boost::to_upper(command);
        if (command.compare("HELP") == 0) 
        {
            response = "Generic_epsHardwareModel::command_callback: Valid commands are HELP, ENABLE, DISABLE, STATUS=X, or STOP";
        }
        else if (command.compare("ENABLE") == 0) 
        {
            _enabled = GENERIC_EPS_SIM_SUCCESS;
            response = "Generic_epsHardwareModel::command_callback:  Enabled";
        }
        else if (command.compare("DISABLE") == 0) 
        {
            _enabled = GENERIC_EPS_SIM_ERROR;
            _count = 0;
            _config = 0;
            _status = 0;
            response = "Generic_epsHardwareModel::command_callback:  Disabled";
        }
        else if (command.substr(0,7).compare("STATUS=") == 0)
        {
            try
            {
                _status = std::stod(command.substr(7));
                response = "Generic_epsHardwareModel::command_callback:  Status set";
            }
            catch (...)
            {
                response = "Generic_epsHardwareModel::command_callback:  Status invalid";
            }            
        }
        else if (command.compare("STOP") == 0) 
        {
            _keep_running = false;
            response = "Generic_epsHardwareModel::command_callback:  Stopping";
        }

        /* Send a reply */
        sim_logger->info("Generic_epsHardwareModel::command_callback:  Sending reply: %s.", response.c_str());
        _command_node->send_reply_message_async(msg, response.size(), response.c_str());
    }

    std::uint8_t Generic_epsHardwareModel::generic_eps_crc8(const std::vector<uint8_t>& crc_data, std::uint32_t crc_size)
    {
        std::uint8_t crc = 0xFF;
        std::uint32_t i;
        std::uint32_t j;

        for (i = 0; i < crc_size; i++) 
        {
            crc ^= crc_data[i];
            for (j = 0; j < 8; j++) 
            {
                if ((crc & 0x80) != 0)
                {
                    crc = (uint8_t)((crc << 1) ^ 0x31);
                }
                else
                {
                    crc <<= 1;
                }
            }
        }
        return crc;
    }

    /* Custom function to prepare the Generic_eps Data */
    void Generic_epsHardwareModel::create_generic_eps_data(std::vector<uint8_t>& out_data)
    {
        boost::shared_ptr<Generic_epsDataPoint> data_point = boost::dynamic_pointer_cast<Generic_epsDataPoint>(_generic_eps_dp->get_data_point());

        /* Prepare data size */
        out_data.resize(65, 0x00);

        /* Battery  - Voltage */
        out_data[0] = 0xDE;
        out_data[1] = 0xAD;
        /* Battery  - Temperature */
        out_data[2] = 0x00; 
        out_data[3] = 0x00;
        
        /* EPS      - 3.3 Voltage */
        out_data[4] = 0x00; 
        out_data[5] = 0x00; 
        /* EPS      - 5.0 Voltage */
        out_data[6] = 0x00;
        out_data[7] = 0x00;
        /* EPS      - 12.0 Voltage */
        out_data[8] = 0x00; 
        out_data[9] = 0x00; 
        /* EPS      - Temperature */
        out_data[10] = 0x00;
        out_data[11] = 0x00;

        /* Solar Array - Voltage */
        out_data[12] = 0x00;
        out_data[13] = 0x00;
        /* Solar Array - Temperature */
        out_data[14] = 0x00;
        out_data[15] = 0x00;

        std::uint8_t i;
        for(i = 0; i < 8; i++)
        {
            // TODO
            /* Switch[i] - Voltage */
            /* Switch[i] - Current */
            /* Switch[i] - Status */
        }
        
        /* CRC */
        out_data[64] = generic_eps_crc8(out_data, 64);
    }

    /* Protocol callback */
    std::uint8_t Generic_epsHardwareModel::determine_i2c_response_for_request(const std::vector<uint8_t>& in_data)
    {
        std::vector<uint8_t> out_data; 
        std::uint8_t valid = GENERIC_EPS_SIM_SUCCESS;
        
        /* Retrieve data and log in man readable format */
        sim_logger->debug("Generic_epsHardwareModel::determine_i2c_response_for_request:  REQUEST %s",
            SimIHardwareModel::uint8_vector_to_hex_string(in_data).c_str());

        /* Check simulator is enabled */
        if (_enabled != GENERIC_EPS_SIM_SUCCESS)
        {
            sim_logger->debug("Generic_epsHardwareModel::determine_i2c_response_for_request:  Generic_eps sim disabled!");
            valid = GENERIC_EPS_SIM_ERROR;
        }
        else
        {
            /* Check if message is incorrect size */
            if (in_data.size() != 3)
            {
                sim_logger->debug("Generic_epsHardwareModel::determine_i2c_response_for_request:  Invalid command size of %d received!", in_data.size());
                valid = GENERIC_EPS_SIM_ERROR;
            }
            else
            {
                /* Check CRC */
                if (in_data[2] == generic_eps_crc8(in_data, 2))
                {
                    sim_logger->debug("Generic_epsHardwareModel::determine_i2c_response_for_request:  CRC8  of %d incorrect!", in_data[2]);
                    valid = GENERIC_EPS_SIM_ERROR;
                }
                else
                {
                    if ((in_data[0] < 8) && (!((in_data[1] == 0x00) || (in_data[1] == 0xAA))))
                    {
                        sim_logger->debug("Generic_epsHardwareModel::determine_i2c_response_for_request:  Set switch %d state of 0x%02x invalid!", in_data[0], in_data[1]);
                        valid = GENERIC_EPS_SIM_ERROR;
                    }
                }
            }

            if (valid == GENERIC_EPS_SIM_SUCCESS)
            {   
                /* Process command */
                switch (in_data[0])
                {
                    case 0x00:
                        /* Switch 0 */
                        sim_logger->debug("Generic_epsHardwareModel::determine_i2c_response_for_request:  Set switch 0 state command received!");
                        _switch[0]._status = in_data[1];
                        break;

                    case 0x01:
                        /* Switch 1 */
                        sim_logger->debug("Generic_epsHardwareModel::determine_i2c_response_for_request:  Set switch 0 state command received!");
                        _switch[1]._status = in_data[1];
                        break;

                    case 0x02:
                        /* Switch 2 */
                        sim_logger->debug("Generic_epsHardwareModel::determine_i2c_response_for_request:  Set switch 0 state command received!");
                        _switch[2]._status = in_data[1];
                        break;

                    case 0x03:
                        /* Switch 3 */
                        sim_logger->debug("Generic_epsHardwareModel::determine_i2c_response_for_request:  Set switch 0 state command received!");
                        _switch[3]._status = in_data[1];
                        break;

                    case 0x04:
                        /* Switch 4 */
                        sim_logger->debug("Generic_epsHardwareModel::determine_i2c_response_for_request:  Set switch 0 state command received!");
                        _switch[4]._status = in_data[1];
                        break;

                    case 0x05:
                        /* Switch 5 */
                        sim_logger->debug("Generic_epsHardwareModel::determine_i2c_response_for_request:  Set switch 0 state command received!");
                        _switch[5]._status = in_data[1];
                        break;

                    case 0x06:
                        /* Switch 6 */
                        sim_logger->debug("Generic_epsHardwareModel::determine_i2c_response_for_request:  Set switch 0 state command received!");
                        _switch[6]._status = in_data[1];
                        break;

                    case 0x07:
                        /* Switch 7 */
                        sim_logger->debug("Generic_epsHardwareModel::determine_i2c_response_for_request:  Set switch 0 state command received!");
                        _switch[7]._status = in_data[1];
                        break;

                    case 0x70:
                        /* Telemetry Request */
                        sim_logger->debug("Generic_epsHardwareModel::determine_i2c_response_for_request:  Telemetry request command received!");
                        create_generic_eps_data(out_data);
                        break;

                    case 0xAA:
                        /* Reset */
                        sim_logger->debug("Generic_epsHardwareModel::determine_i2c_response_for_request:  Reset command received!");
                        // TODO 
                        break;
                    
                    default:
                        /* Unused command code */
                        sim_logger->debug("Generic_epsHardwareModel::determine_i2c_response_for_request:  Unused command %d received!", in_data[0]);
                        valid = GENERIC_EPS_SIM_ERROR;
                        break;
                }
            }
        }
        return valid;
    }

    I2CSlaveConnection::I2CSlaveConnection(Generic_epsHardwareModel* hm,
        int bus_address, std::string connection_string, std::string bus_name)
        : NosEngine::I2C::I2CSlave(bus_address, connection_string, bus_name)
    {
        _hardware_model = hm;
    }

    size_t I2CSlaveConnection::i2c_read(uint8_t *rbuf, size_t rlen)
    {
        size_t num_read;
        sim_logger->debug("i2c_read: 0x%02x", _i2c_out_data); // log data
        if(rlen <= 1)
        {
            rbuf[0] = _i2c_out_data;
            num_read = 1;
        }
        return num_read;
    }

    size_t I2CSlaveConnection::i2c_write(const uint8_t *wbuf, size_t wlen)
    {
        std::vector<uint8_t> in_data(wbuf, wbuf + wlen);
        sim_logger->debug("i2c_write: %s",
            SimIHardwareModel::uint8_vector_to_hex_string(in_data).c_str()); // log data
        _i2c_out_data = _hardware_model->determine_i2c_response_for_request(in_data);
        return wlen;
    }
}
