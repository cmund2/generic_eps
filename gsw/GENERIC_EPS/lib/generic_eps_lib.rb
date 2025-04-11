# Library for GENERIC_EPS Target
require 'cosmos'
require 'cosmos/script'

#
# Definitions
#
GENERIC_EPS_CMD_SLEEP = 0.25
GENERIC_EPS_RESPONSE_TIMEOUT = 5
GENERIC_EPS_TEST_LOOP_COUNT = 1
GENERIC_EPS_DEVICE_LOOP_COUNT = 5

#
# Functions
#
def get_eps_hk()
    cmd("GENERIC_EPS GENERIC_EPS_REQ_HK")
    wait_check_packet("GENERIC_EPS", "GENERIC_EPS_HK_TLM", 1, GENERIC_EPS_RESPONSE_TIMEOUT)
    sleep(GENERIC_EPS_CMD_SLEEP)
end

def eps_cmd(*command)
    count = tlm("GENERIC_EPS GENERIC_EPS_HK_TLM CMD_COUNT") + 1

    if (count == 256)
        count = 0
    end

    cmd(*command)
    get_eps_hk()
    current = tlm("GENERIC_EPS GENERIC_EPS_HK_TLM CMD_COUNT")
    if (current != count)
        # Try again
        cmd(*command)
        get_eps_hk()
        current = tlm("GENERIC_EPS GENERIC_EPS_HK_TLM CMD_COUNT")
        if (current != count)
            # Third times the charm
            cmd(*command)
            get_eps_hk()
            current = tlm("GENERIC_EPS GENERIC_EPS_HK_TLM CMD_COUNT")
        end
    end
    check("GENERIC_EPS GENERIC_EPS_HK_TLM CMD_COUNT >= #{count}")
end

def safe_eps()
    get_eps_hk()
end

def confirm_eps_data()
    dev_cmd_cnt = tlm("GENERIC_EPS GENERIC_EPS_HK_TLM DEVICE_COUNT")
    dev_cmd_err_cnt = tlm("GENERIC_EPS GENERIC_EPS_HK_TLM DEVICE_ERR_COUNT")
    
    get_eps_hk()
    

    get_eps_hk()
    check("GENERIC_EPS GENERIC_EPS_HK_TLM DEVICE_COUNT >= #{dev_cmd_cnt}")
    check("GENERIC_EPS GENERIC_EPS_HK_TLM DEVICE_ERR_COUNT == #{dev_cmd_err_cnt}")
end

def confirm_eps_data_loop()
    GENERIC_EPS_DEVICE_LOOP_COUNT.times do |n|
        confirm_eps_data()
    end
end

#
# Simulator Functions
#
def eps_prepare_ast()
    # Get to known state
    safe_eps()

    # Confirm data
    confirm_eps_data_loop()
end

def eps_sim_enable()
    cmd("SIM_CMDBUS_BRIDGE GENERIC_EPS_SIM_ENABLE")
end

def eps_sim_disable()
    cmd("SIM_CMDBUS_BRIDGE GENERIC_EPS_SIM_DISABLE")
end

def eps_sim_set_status(status)
    cmd("SIM_CMDBUS_BRIDGE GENERIC_EPS_SIM_SET_STATUS with STATUS #{status}")
end
