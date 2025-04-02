require 'cosmos'
require 'cosmos/script'
require "cfs_lib.rb"
#require 'math'

##
## NOOP
##
initial_command_count = tlm("GENERIC_EPS GENERIC_EPS_HK_TLM CMD_COUNT")
initial_error_count = tlm("GENERIC_EPS GENERIC_EPS_HK_TLM CMD_ERR_COUNT")
initial_device_error_count = tlm("GENERIC_EPS GENERIC_EPS_HK_TLM DEVICE_ERR_COUNT")
cmd("GENERIC_EPS GENERIC_EPS_NOOP_CC")
wait_check("GENERIC_EPS GENERIC_EPS_HK_TLM CMD_COUNT > #{initial_command_count}", 30)
wait_check("GENERIC_EPS GENERIC_EPS_HK_TLM CMD_ERR_COUNT == #{initial_error_count}", 30)
wait_check("GENERIC_EPS GENERIC_EPS_HK_TLM DEVICE_ERR_COUNT == #{initial_device_error_count}", 30)

sleep(5)

##
## HK without Device
##
initial_error_count = tlm("GENERIC_EPS GENERIC_EPS_HK_TLM CMD_ERR_COUNT")
initial_device_error_count = tlm("GENERIC_EPS GENERIC_EPS_HK_TLM DEVICE_ERR_COUNT")
cmd("GENERIC_EPS GENERIC_EPS_REQ_HK")
wait_check("GENERIC_EPS GENERIC_EPS_HK_TLM CMD_ERR_COUNT == #{initial_error_count}", 30)
wait_check("GENERIC_EPS GENERIC_EPS_HK_TLM DEVICE_ERR_COUNT == #{initial_device_error_count}", 30)

sleep(5)

##
## Housekeeping w/ Device
##
initial_error_count = tlm("GENERIC_EPS GENERIC_EPS_HK_TLM CMD_ERR_COUNT")
initial_device_error_count = tlm("GENERIC_EPS GENERIC_EPS_HK_TLM DEVICE_ERR_COUNT")
cmd("GENERIC_EPS GENERIC_EPS_REQ_HK")
wait_check("GENERIC_EPS GENERIC_EPS_HK_TLM CMD_ERR_COUNT == #{initial_error_count}", 30)
wait_check("GENERIC_EPS GENERIC_EPS_HK_TLM DEVICE_ERR_COUNT == #{initial_device_error_count}", 30)

sleep(5)

#
# Data w/ Device
#
initial_error_count = tlm("GENERIC_EPS GENERIC_EPS_HK_TLM CMD_ERR_COUNT")
initial_device_error_count = tlm("GENERIC_EPS GENERIC_EPS_HK_TLM DEVICE_ERR_COUNT")

cmd("GENERIC_EPS GENERIC_EPS_REQ_DATA")

svb0 = tlm("SIM_42_TRUTH SIM_42_TRUTH_DATA SVB_0")
svb1 = tlm("SIM_42_TRUTH SIM_42_TRUTH_DATA SVB_1")
svb2 = tlm("SIM_42_TRUTH SIM_42_TRUTH_DATA SVB_2")

eps_alpha = tlm("GENERIC_EPS GENERIC_EPS_DATA_TLM GENERIC_EPS_ALPHA")
eps_beta = tlm("GENERIC_EPS GENERIC_EPS_DATA_TLM GENERIC_EPS_BETA")

fss_error = tlm("GENERIC_FSS GENERIC_FSS_DATA_TLM GENERIC_FSS_ERROR_CODE")

truth_42_alpha = -Math.atan2(svb2, svb0)
truth_42_beta = Math.atan2(svb1, svb0)

truth_42_alpha_diff = (eps_alpha - truth_42_alpha).abs()
truth_42_beta_diff = (eps_beta - truth_42_beta).abs()
diff_margin = 0.025

wait_check("GENERIC_EPS GENERIC_EPS_HK_TLM CMD_ERR_COUNT == #{initial_error_count}", 30)
wait_check("GENERIC_EPS GENERIC_EPS_HK_TLM DEVICE_ERR_COUNT == #{initial_device_error_count}", 30)
if fss_error == 0
  wait_check_expression("truth_42_alpha_diff <= diff_margin # #{truth_42_alpha_diff} <= #{diff_margin}", 15)

  wait_check_expression("truth_42_beta_diff <= diff_margin # #{truth_42_beta_diff} <= #{diff_margin}", 15)
end

sleep(5)

##
## Reset Counters
##
device_count_initial = tlm("GENERIC_EPS GENERIC_EPS_HK_TLM DEVICE_COUNT")
cmd("GENERIC_EPS GENERIC_EPS_RST_COUNTERS_CC")
wait_check("GENERIC_EPS GENERIC_EPS_HK_TLM CMD_COUNT == 0", 30)
wait_check("GENERIC_EPS GENERIC_EPS_HK_TLM CMD_ERR_COUNT == 0", 30)
wait_check("GENERIC_EPS GENERIC_EPS_HK_TLM DEVICE_COUNT <= #{device_count_initial}", 30)
wait_check("GENERIC_EPS GENERIC_EPS_HK_TLM DEVICE_ERR_COUNT == 0", 30)