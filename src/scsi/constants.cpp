/*
LICENSE: BEGIN
===============================================================================
@author Shan Anand
@email anand.gs@gmail.com
@source https://github.com/shan-anand
@file constants.cpp
@brief SCSI constants, enum and macro definitions
===============================================================================
MIT License

Copyright (c) 2017 Shanmuga (Anand) Gunasekaran

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
===============================================================================
LICENSE: END
*/

/**
 * @file  Scsi_Constants.cpp
 * @brief Constant values in SCSI
 */

#include <scsi/constants.hpp>
#include <string.h>
#include "local.h"

using namespace sid;
using namespace sid::scsi;

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Implementation of functions
//
std::string sid::scsi::to_str(const sense_key& e)
{
  std::string out;
  const uint8_t v = static_cast<const uint8_t>(e);

  switch ( e )
  {
  case sense_key::no_sense:        out = "no sense"; break;
  case sense_key::recovered_error: out = "recovered error"; break;
  case sense_key::not_ready:       out = "not ready"; break;
  case sense_key::medium_error:    out = "medium error"; break;
  case sense_key::hardware_error:  out = "hardware error"; break;
  case sense_key::illegal_request: out = "illegal request"; break;
  case sense_key::unit_attention:  out = "unit attention"; break;
  case sense_key::data_protection: out = "data protection"; break;
  case sense_key::blank_check:     out = "blank check"; break;
  case sense_key::vendor_specific: out = "vendor specific"; break;
  case sense_key::copy_aborted:    out = "copy aborted"; break;
  case sense_key::command_aborted: out = "command aborted"; break;
  case sense_key::obsolete_0c:     out = "obsolete"; break;
  case sense_key::volume_overflow: out = "volume overflow"; break;
  case sense_key::miscompare:      out = "miscompare"; break;
  case sense_key::reserved_0f:     out = "reserved"; break;
  // DO NOT HANDLE default
  }

  if ( out.empty() )
    out = "unknown sense key";

  out += " [" + local::to_str(v, true) + "]";

  return out;
}

std::string sid::scsi::to_str(const code_page& e)
{
  std::string out;
  const uint8_t v = static_cast<const uint8_t>(e);

  switch ( e )
  {
  case code_page::supported_vpd_pages:               out = "supported vpd pages"; break;
  case code_page::unit_serial_number:                out = "unit serial number"; break;
  case code_page::device_identification:             out = "device identification"; break;
  case code_page::software_interface_identification: out = "software interface identification"; break;
  case code_page::management_network_address:        out = "management network address"; break;
  case code_page::extended_inquiry_data:             out = "extended inquiry data"; break;
  case code_page::mode_page_policy:                  out = "mode page policy"; break;
  case code_page::scsi_ports:                        out = "scsi ports"; break;
  case code_page::block_limits:                      out = "block limits"; break;
  case code_page::block_device_characteristics:      out = "block device characteristics"; break;
  case code_page::logical_block_provisioning:        out = "logical block provisioning"; break;
  case code_page::custom_vpd:                        out = "custom vpd"; break;
  // DO NOT HANDLE default
  }

  if ( out.empty() )
  {
    uint8_t u = static_cast<uint8_t>(e);
    if (  u >= 0x89 && u <= 0xAF )
      out = "reserved";
    else if ( u >= 0xB0 && u <= 0xBF )
      out = "device-specific";
    else if ( u >= 0xC0 && u <= 0xFF )
      out = "vendor-specific";
    else
      out = "undefined";
  }
  out += " [" + local::to_str(v, true) + "]";

  return out;
}

std::string sid::scsi::to_str(const peripheral_qualifier& e)
{
  std::string out;
  const uint8_t v = static_cast<const uint8_t>(e);

  switch ( e )
  {
  case peripheral_qualifier::connected:          out = "connected"; break;
  case peripheral_qualifier::disconnected:       out = "disconnected"; break;
  case peripheral_qualifier::reserved_02:        out = "reserved 02"; break;
  case peripheral_qualifier::not_supported:      out = "not supported"; break;
  case peripheral_qualifier::vendor_specific_04:
  case peripheral_qualifier::vendor_specific_05:
  case peripheral_qualifier::vendor_specific_06:
  case peripheral_qualifier::vendor_specific_07: out = "vendor specific"; break;
  // DO NOT HANDLE default
  }

  if ( out.empty() )
    out = "unknown qualifier";

  out += " [" + local::to_str(v, true) + "]";

  return out;
}

std::string sid::scsi::to_str(const peripheral_device_type& e)
{
  std::string out;
  const uint8_t v = static_cast<const uint8_t>(e);

  switch ( e )
  {
  case peripheral_device_type::direct_access:            out = "direct access"; break;
  case peripheral_device_type::sequential_access:        out = "sequential access"; break;
  case peripheral_device_type::printer:                  out = "printer"; break;
  case peripheral_device_type::processor:                out = "processor"; break;
  case peripheral_device_type::write_once:               out = "write once"; break;
  case peripheral_device_type::cd_dvd:                   out = "cd dvd"; break;
  case peripheral_device_type::scanner:                  out = "scanner"; break;
  case peripheral_device_type::optical_memory:           out = "optical memory"; break;
  case peripheral_device_type::medium_changer:           out = "medium changer"; break;
  case peripheral_device_type::communications:           out = "communications"; break;
  case peripheral_device_type::obsolete_0a:
  case peripheral_device_type::obsolete_0b:              out = "obsolete"; break;
  case peripheral_device_type::storate_array_controller: out = "storate array controller"; break;
  case peripheral_device_type::enclosure_services:       out = "enclosure services"; break;
  case peripheral_device_type::simplified_direct_access: out = "simplified direct access"; break;
  case peripheral_device_type::optical_card_rw:          out = "optical card rw"; break;
  case peripheral_device_type::bridge_controller:        out = "bridge controller"; break;
  case peripheral_device_type::object_based_storage:     out = "object based storage"; break;
  case peripheral_device_type::automation:               out = "automation"; break;
  case peripheral_device_type::reserved_13:
  case peripheral_device_type::reserved_14:
  case peripheral_device_type::reserved_15:
  case peripheral_device_type::reserved_16:
  case peripheral_device_type::reserved_17:
  case peripheral_device_type::reserved_18:
  case peripheral_device_type::reserved_19:
  case peripheral_device_type::reserved_1a:
  case peripheral_device_type::reserved_1b:
  case peripheral_device_type::reserved_1c:
  case peripheral_device_type::reserved_1d:    out = "reserved"; break;
  case peripheral_device_type::well_known_lun: out = "well known lun"; break;
  case peripheral_device_type::unknown:        out = "unknown"; break;
  // DO NOT HANDLE default
  }

  if ( out.empty() )
    out = "unknown device type";

  out += " [" + local::to_str(v, true) + "]";

  return out;
}

std::string sid::scsi::to_str(const protocol_id& e)
{
  std::string out;
  const uint8_t v = static_cast<const uint8_t>(e);

  switch ( e )
  {
  case protocol_id::fiber_channel: out = "fiber channel"; break;
  case protocol_id::parallel_scsi: out = "parallel scsi"; break;
  case protocol_id::ssa:           out = "ssa"; break;
  case protocol_id::ieee_1394:     out = "ieee 1394"; break;
  case protocol_id::rdma:          out = "rdma"; break;
  case protocol_id::iscsi:         out = "iscsi"; break;
  case protocol_id::sas:           out = "sas"; break;
  case protocol_id::adt:           out = "adt"; break;
  case protocol_id::ata:           out = "ata"; break;
  case protocol_id::reserved_9:    out = "reserved"; break;
  // DO NOT HANDLE default
  }

  if ( out.empty() )
    out = "unknown protocol id";

  out += " [" + local::to_str(v, true) + "]";

  return out;
}

std::string sid::scsi::to_str(const code_set& e)
{
  std::string out;
  const uint8_t v = static_cast<const uint8_t>(e);

  switch ( e )
  {
  case code_set::binary: out = "binary"; break;
  case code_set::ascii:  out = "ascii"; break;
  case code_set::utf8:   out = "utf8"; break;
  // DO NOT HANDLE default
  }

  if ( out.empty() )
    out = "unknown codeset";

  out += " [" + local::to_str(v, true) + "]";

  return out;
}

std::string sid::scsi::to_str(const association& e)
{
  std::string out;
  const uint8_t v = static_cast<const uint8_t>(e);

  switch ( e )
  {
  case association::logical_unit:  out = "logical unit"; break;
  case association::target_port:   out = "target port"; break;
  case association::target_device: out = "target device"; break;
  // DO NOT HANDLE default
  }

  if ( out.empty() )
    out = "unknown association";

  out += " [" + local::to_str(v, true) + "]";

  return out;
}

std::string sid::scsi::to_str(const identifier_type& e)
{
  std::string out;
  const uint8_t v = static_cast<const uint8_t>(e);

  switch ( e )
  {
  case identifier_type::vendor_specific:      out = "vendor specific"; break;
  case identifier_type::t10_vendor_id:        out = "t10 vendor id"; break;
  case identifier_type::eui_64:               out = "eui 64"; break;
  case identifier_type::naa:                  out = "naa"; break;
  case identifier_type::relative_target_port: out = "relative target port"; break;
  case identifier_type::target_port_group:    out = "target port group"; break;
  case identifier_type::logical_unit_group:   out = "logical unit group"; break;
  case identifier_type::md5_logical_unit_id:  out = "md5 logical unit id"; break;
  case identifier_type::scsi_name_string:     out = "scsi name string"; break;
  // DO NOT HANDLE default
  }

  if ( out.empty() )
    out = "unknown identifier type";

  out += " [" + local::to_str(v, true) + "]";

  return out;
}

std::string sid::scsi::to_str(const ascq& e)
{
  std::string out;
  const uint16_t v = static_cast<const uint16_t>(e);

  switch ( e )
  {
  case ascq::no_additional_sense_information:
    out = "no additional sense information"; break;                                                                 
  case ascq::filemark_detected:
    out = "filemark detected"; break;                                                                                             
  case ascq::end_of_partition_medium_detected:
    out = "end-of-partition/medium detected"; break;                                                               
  case ascq::setmark_detected:
    out = "setmark detected"; break;                                                                                               
  case ascq::beginning_of_partition_medium_detected:
    out = "beginning-of-partition/medium detected"; break;                                                   
  case ascq::end_of_data_detected:
    out = "end-of-data detected"; break;                                                                                       
  case ascq::io_process_terminated:
    out = "i/o process terminated"; break;                                                                                   
  case ascq::programmable_early_warning_detected:
    out = "programmable early warning detected"; break;                                                         
  case ascq::audio_play_operation_in_progress:
    out = "audio play operation in progress"; break;                                                               
  case ascq::audio_play_operation_paused:
    out = "audio play operation paused"; break;                                                                         
  case ascq::audio_play_operation_successfully_completed:
    out = "audio play operation successfully completed"; break;                                         
  case ascq::audio_play_operation_stopped_due_to_error:
    out = "audio play operation stopped due to error"; break;                                             
  case ascq::no_current_audio_status_to_return:
    out = "no current audio status to return"; break;                                                             
  case ascq::operation_in_progress:
    out = "operation in progress"; break;                                                                                     
  case ascq::cleaning_requested:
    out = "cleaning requested"; break;                                                                                           
  case ascq::erase_operation_in_progress:
    out = "erase operation in progress"; break;                                                                         
  case ascq::locate_operation_in_progress:
    out = "locate operation in progress"; break;                                                                       
  case ascq::rewind_operation_in_progress:
    out = "rewind operation in progress"; break;                                                                       
  case ascq::set_capacity_operation_in_progress:
    out = "set capacity operation in progress"; break;                                                           
  case ascq::verify_operation_in_progress:
    out = "verify operation in progress"; break;                                                                       
  case ascq::ata_pass_through_information_available:
    out = "ata pass through information available"; break;                                                   
  case ascq::conflicting_sa_creation_request:
    out = "conflicting sa creation request"; break;                                                                 
  case ascq::logical_unit_transitioning_to_another_power_condition:
    out = "logical unit transitioning to another power condition"; break;                     
  case ascq::extended_copy_information_available:
    out = "extended copy information available"; break;                                                         
  case ascq::atomic_command_aborted_due_to_aca:
    out = "atomic command aborted due to aca"; break;                                                             
  case ascq::no_index_sector_signal:
    out = "no index/sector signal"; break;                                                                                   
  case ascq::no_seek_complete:
    out = "no seek complete"; break;                                                                                               
  case ascq::peripheral_device_write_fault:
    out = "peripheral device write fault"; break;                                                                     
  case ascq::no_write_current:
    out = "no write current"; break;                                                                                               
  case ascq::excessive_write_errors:
    out = "excessive write errors"; break;                                                                                   
  case ascq::logical_unit_not_ready_cause_not_reportable:
    out = "logical unit not ready:cause not reportable"; break;                                        
  case ascq::logical_unit_is_in_process_of_becoming_ready:
    out = "logical unit is in process of becoming ready"; break;                                       
  case ascq::logical_unit_not_ready_initializing_command_required:
    out = "logical unit not ready:initializing command required"; break;                      
  case ascq::logical_unit_not_ready_manual_intervention_required:
    out = "logical unit not ready:manual intervention required"; break;                        
  case ascq::logical_unit_not_ready_format_in_progress:
    out = "logical unit not ready:format in progress"; break;                                            
  case ascq::logical_unit_not_ready_rebuild_in_progress:
    out = "logical unit not ready:rebuild in progress"; break;                                          
  case ascq::logical_unit_not_ready_recalculation_in_progress:
    out = "logical unit not ready:recalculation in progress"; break;                              
  case ascq::logical_unit_not_ready_operation_in_progress:
    out = "logical unit not ready:operation in progress"; break;                                      
  case ascq::logical_unit_not_ready_long_write_in_progress:
    out = "logical unit not ready:long write in progress"; break;                                    
  case ascq::logical_unit_not_ready_self_test_in_progress:
    out = "logical unit not ready:self-test in progress"; break;                                      
  case ascq::logical_unit_not_accessible_asymmetric_access_state_transition:
    out = "logical unit not accessible:asymmetric access state transition"; break;  
  case ascq::logical_unit_not_accessible_target_port_in_standby_state:
    out = "logical unit not accessible:target port in standby state"; break;              
  case ascq::logical_unit_not_accessible_target_port_in_unavailable_state:
    out = "logical unit not accessible:target port in unavailable state"; break;      
  case ascq::logical_unit_not_ready_structure_check_required:
    out = "logical unit not ready:structure check required"; break;                                
  case ascq::logical_unit_not_ready_security_session_in_progress:
    out = "logical unit not ready:security session in progress"; break;                        
  case ascq::logical_unit_not_ready_auxiliary_memory_not_accessible:
    out = "logical unit not ready:auxiliary memory not accessible"; break;                  
  case ascq::logical_unit_not_ready_notify_enable_spinup_required:
    out = "logical unit not ready:notify (enable spinup) required"; break;                    
  case ascq::logical_unit_not_ready_offline:
    out = "logical unit not ready:offline"; break;                                                                  
  case ascq::logical_unit_not_ready_sa_creation_in_progress:
    out = "logical unit not ready:sa creation in progress"; break;                                  
  case ascq::logical_unit_not_ready_space_allocation_in_progress:
    out = "logical unit not ready:space allocation in progress"; break;                        
  case ascq::logical_unit_not_ready_robotics_disabled:
    out = "logical unit not ready:robotics disabled"; break;                                              
  case ascq::logical_unit_not_ready_configuration_required:
    out = "logical unit not ready:configuration required"; break;                                    
  case ascq::logical_unit_not_ready_calibration_required:
    out = "logical unit not ready:calibration required"; break;                                        
  case ascq::logical_unit_not_ready_a_door_is_open:
    out = "logical unit not ready:a door is open"; break;                                                    
  case ascq::logical_unit_not_ready_operating_in_sequential_mode:
    out = "logical unit not ready:operating in sequential mode"; break;                        
  case ascq::logical_unit_not_ready_start_stop_unit_command_in_progress:
    out = "logical unit not ready:start stop unit command in progress"; break;          
  case ascq::logical_unit_not_ready_sanitize_in_progress:
    out = "logical unit not ready:sanitize in progress"; break;                                        
  case ascq::logical_unit_not_ready_additional_power_use_not_yet_granted:
    out = "logical unit not ready:additional power use not yet granted"; break;        
  case ascq::logical_unit_not_ready_configuration_in_progress:
    out = "logical unit not ready:configuration in progress"; break;                              
  case ascq::logical_unit_not_ready_microcode_activation_required:
    out = "logical unit not ready:microcode activation required"; break;                      
  case ascq::logical_unit_not_ready_microcode_download_required:
    out = "logical unit not ready:microcode download required"; break;                          
  case ascq::logical_unit_not_ready_logical_unit_reset_required:
    out = "logical unit not ready:logical unit reset required"; break;                          
  case ascq::logical_unit_not_ready_hard_reset_required:
    out = "logical unit not ready:hard reset required"; break;                                          
  case ascq::logical_unit_not_ready_power_cycle_required:
    out = "logical unit not ready:power cycle required"; break;                                        
  case ascq::logical_unit_not_ready_affiliation_required:
    out = "logical unit not ready:affiliation required"; break;                                        
  case ascq::logical_unit_does_not_respond_to_selection:
    out = "logical unit does not respond to selection"; break;                                           
  case ascq::no_reference_position_found:
    out = "no reference position found"; break;                                                                         
  case ascq::multiple_peripheral_devices_selected:
    out = "multiple peripheral devices selected"; break;                                                       
  case ascq::logical_unit_communication_failure:
    out = "logical unit communication failure"; break;                                                           
  case ascq::logical_unit_communication_time_out:
    out = "logical unit communication time-out"; break;                                                         
  case ascq::logical_unit_communication_parity_error:
    out = "logical unit communication parity error"; break;                                                 
  case ascq::logical_unit_communication_crc_error_ultra_dma_32_:
    out = "logical unit communication crc error (ultra-dma/32)"; break;                          
  case ascq::unreachable_copy_target:
    out = "unreachable copy target"; break;                                                                                 
  case ascq::track_following_error:
    out = "track following error"; break;                                                                                     
  case ascq::tracking_servo_failure:
    out = "tracking servo failure"; break;                                                                                   
  case ascq::focus_servo_failure:
    out = "focus servo failure"; break;                                                                                         
  case ascq::spindle_servo_failure:
    out = "spindle servo failure"; break;                                                                                     
  case ascq::head_select_fault:
    out = "head select fault"; break;                                                                                             
  case ascq::vibration_induced_tracking_error:
    out = "vibration induced tracking error"; break;                                                               
  case ascq::error_log_overflow:
    out = "error log overflow"; break;                                                                                           
  case ascq::warning:
    out = "warning"; break;                                                                                                                 
  case ascq::warning_specified_temperature_exceeded:
    out = "warning - specified temperature exceeded"; break;                                                 
  case ascq::warning_enclosure_degraded:
    out = "warning - enclosure degraded"; break;                                                                         
  case ascq::warning_background_self_test_failed:
    out = "warning - background self-test failed"; break;                                                       
  case ascq::warning_background_pre_scan_detected_medium_error:
    out = "warning - background pre-scan detected medium error"; break;                           
  case ascq::warning_background_medium_scan_detected_medium_error:
    out = "warning - background medium scan detected medium error"; break;                     
  case ascq::warning_non_volatile_cache_now_volatile:
    out = "warning - non-volatile cache now volatile"; break;                                               
  case ascq::warning_degraded_power_to_non_volatile_cache:
    out = "warning - degraded power to non-volatile cache"; break;                                     
  case ascq::warning_power_loss_expected:
    out = "warning - power loss expected"; break;                                                                       
  case ascq::warning_device_statistics_notification_active:
    out = "warning - device statistics notification active"; break;                                   
  case ascq::warning_high_critical_temperature_limit_exceeded:
    out = "warning - high critical temperature limit exceeded"; break;                             
  case ascq::warning_low_critical_temperature_limit_exceeded:
    out = "warning - low critical temperature limit exceeded"; break;                               
  case ascq::warning_high_operating_temperature_limit_exceeded:
    out = "warning - high operating temperature limit exceeded"; break;                           
  case ascq::warning_low_operating_temperature_limit_exceeded:
    out = "warning - low operating temperature limit exceeded"; break;                             
  case ascq::warning_high_critical_humidity_limit_exceeded:
    out = "warning - high critical humidity limit exceeded"; break;                                   
  case ascq::warning_low_critical_humidity_limit_exceeded:
    out = "warning - low critical humidity limit exceeded"; break;                                     
  case ascq::warning_high_operating_humidity_limit_exceeded:
    out = "warning - high operating humidity limit exceeded"; break;                                 
  case ascq::warning_low_operating_humidity_limit_exceeded:
    out = "warning - low operating humidity limit exceeded"; break;                                   
  case ascq::warning_microcode_security_at_risk:
    out = "warning - microcode security at risk"; break;                                                         
  case ascq::warning_microcode_digital_signature_validation_failure:
    out = "warning - microcode digital signature validation failure"; break;                 
  case ascq::write_error:
    out = "write error"; break;                                                                                                         
  case ascq::write_error_recovered_with_auto_reallocation:
    out = "write error - recovered with auto reallocation"; break;                                     
  case ascq::write_error_auto_reallocation_failed:
    out = "write error - auto reallocation failed"; break;                                                     
  case ascq::write_error_recommend_reassignment:
    out = "write error - recommend reassignment"; break;                                                         
  case ascq::compression_check_miscompare_error:
    out = "compression check miscompare error"; break;                                                           
  case ascq::data_expansion_occurred_during_compression:
    out = "data expansion occurred during compression"; break;                                           
  case ascq::block_not_compressible:
    out = "block not compressible"; break;                                                                                   
  case ascq::write_error_recovery_needed:
    out = "write error - recovery needed"; break;                                                                       
  case ascq::write_error_recovery_failed:
    out = "write error - recovery failed"; break;                                                                       
  case ascq::write_error_loss_of_streaming:
    out = "write error - loss of streaming"; break;                                                                   
  case ascq::write_error_padding_blocks_added:
    out = "write error - padding blocks added"; break;                                                             
  case ascq::auxiliary_memory_write_error:
    out = "auxiliary memory write error"; break;                                                                       
  case ascq::write_error_unexpected_unsolicited_data:
    out = "write error - unexpected unsolicited data"; break;                                               
  case ascq::write_error_not_enough_unsolicited_data:
    out = "write error - not enough unsolicited data"; break;                                               
  case ascq::multiple_write_errors:
    out = "multiple write errors"; break;                                                                                     
  case ascq::defects_in_error_window:
    out = "defects in error window"; break;                                                                                 
  case ascq::incomplete_multiple_atomic_write_operations:
    out = "incomplete multiple atomic write operations"; break;                                         
  case ascq::write_error_recovery_scan_needed:
    out = "write error - recovery scan needed"; break;                                                             
  case ascq::write_error_insufficient_zone_resources:
    out = "write error - insufficient zone resources"; break;                                               
  case ascq::error_detected_by_third_party_temporary_initiator:
    out = "error detected by third party temporary initiator"; break;                             
  case ascq::third_party_device_failure:
    out = "third party device failure"; break;                                                                           
  case ascq::copy_target_device_not_reachable:
    out = "copy target device not reachable"; break;                                                               
  case ascq::incorrect_copy_target_device_type:
    out = "incorrect copy target device type"; break;                                                             
  case ascq::copy_target_device_data_underrun:
    out = "copy target device data underrun"; break;                                                               
  case ascq::copy_target_device_data_overrun:
    out = "copy target device data overrun"; break;                                                                 
  case ascq::invalid_information_unit:
    out = "invalid information unit"; break;                                                                               
  case ascq::information_unit_too_short:
    out = "information unit too short"; break;                                                                           
  case ascq::information_unit_too_long:
    out = "information unit too long"; break;                                                                             
  case ascq::invalid_field_in_command_information_unit:
    out = "invalid field in command information unit"; break;                                             
  case ascq::custom_0f00:
    out = "ascq 0f00"; break;                                                                                                                  
  case ascq::id_crc_or_ecc_error:
    out = "id crc or ecc error"; break;                                                                                         
  case ascq::logical_block_guard_check_failed:
    out = "logical block guard check failed"; break;                                                               
  case ascq::logical_block_application_tag_check_failed:
    out = "logical block application tag check failed"; break;                                           
  case ascq::logical_block_reference_tag_check_failed:
    out = "logical block reference tag check failed"; break;                                               
  case ascq::logical_block_protection_error_on_recover_buffered_data:
    out = "logical block protection error on recover buffered data"; break;                 
  case ascq::logical_block_protection_method_error:
    out = "logical block protection method error"; break;                                                     
  case ascq::unrecovered_read_error:
    out = "unrecovered read error"; break;                                                                                   
  case ascq::read_retries_exhausted:
    out = "read retries exhausted"; break;                                                                                   
  case ascq::error_too_long_to_correct:
    out = "error too long to correct"; break;                                                                             
  case ascq::multiple_read_errors:
    out = "multiple read errors"; break;                                                                                       
  case ascq::unrecovered_read_error_auto_reallocate_failed:
    out = "unrecovered read error - auto reallocate failed"; break;                                   
  case ascq::l_ec_uncorrectable_error:
    out = "l-ec uncorrectable error"; break;                                                                               
  case ascq::circ_unrecovered_error:
    out = "circ unrecovered error"; break;                                                                                   
  case ascq::data_re_synchronization_error:
    out = "data re-synchronization error"; break;                                                                     
  case ascq::incomplete_block_read:
    out = "incomplete block read"; break;                                                                                     
  case ascq::no_gap_found:
    out = "no gap found"; break;                                                                                                       
  case ascq::miscorrected_error:
    out = "miscorrected error"; break;                                                                                           
  case ascq::unrecovered_read_error_recommend_reassignment:
    out = "unrecovered read error - recommend reassignment"; break;                                   
  case ascq::unrecovered_read_error_recommend_rewrite_the_data:
    out = "unrecovered read error - recommend rewrite the data"; break;                           
  case ascq::de_compression_crc_error:
    out = "de-compression crc error"; break;                                                                               
  case ascq::cannot_decompress_using_declared_algorithm:
    out = "cannot decompress using declared algorithm"; break;                                           
  case ascq::error_reading_upc_ean_number:
    out = "error reading upc/ean number"; break;                                                                       
  case ascq::error_reading_isrc_number:
    out = "error reading isrc number"; break;                                                                             
  case ascq::read_error_loss_of_streaming:
    out = "read error - loss of streaming"; break;                                                                     
  case ascq::auxiliary_memory_read_error:
    out = "auxiliary memory read error"; break;                                                                         
  case ascq::read_error_failed_retransmission_request:
    out = "read error - failed retransmission request"; break;                                             
  case ascq::read_error_lba_marked_bad_by_application_client:
    out = "read error - lba marked bad by application client"; break;                               
  case ascq::write_after_sanitize_required:
    out = "write after sanitize required"; break;                                                                     
  case ascq::address_mark_not_found_for_id_field:
    out = "address mark not found for id field"; break;                                                         
  case ascq::address_mark_not_found_for_data_field:
    out = "address mark not found for data field"; break;                                                     
  case ascq::recorded_entity_not_found:
    out = "recorded entity not found"; break;                                                                             
  case ascq::record_not_found:
    out = "record not found"; break;                                                                                               
  case ascq::filemark_or_setmark_not_found:
    out = "filemark or setmark not found"; break;                                                                     
  case ascq::end_of_data_not_found:
    out = "end-of-data not found"; break;                                                                                     
  case ascq::block_sequence_error:
    out = "block sequence error"; break;                                                                                       
  case ascq::record_not_found_recommend_reassignment:
    out = "record not found - recommend reassignment"; break;                                               
  case ascq::record_not_found_data_auto_reallocated:
    out = "record not found - data auto-reallocated"; break;                                                 
  case ascq::locate_operation_failure:
    out = "locate operation failure"; break;                                                                               
  case ascq::random_positioning_error:
    out = "random positioning error"; break;                                                                               
  case ascq::mechanical_positioning_error:
    out = "mechanical positioning error"; break;                                                                       
  case ascq::positioning_error_detected_by_read_of_medium:
    out = "positioning error detected by read of medium"; break;                                       
  case ascq::data_synchronization_mark_error:
    out = "data synchronization mark error"; break;                                                                 
  case ascq::data_sync_error_data_rewritten:
    out = "data sync error - data rewritten"; break;                                                                 
  case ascq::data_sync_error_recommend_rewrite:
    out = "data sync error - recommend rewrite"; break;                                                           
  case ascq::data_sync_error_data_auto_reallocated:
    out = "data sync error - data auto-reallocated"; break;                                                   
  case ascq::data_sync_error_recommend_reassignment:
    out = "data sync error - recommend reassignment"; break;                                                 
  case ascq::recovered_data_with_no_error_correction_applied:
    out = "recovered data with no error correction applied"; break;                                 
  case ascq::recovered_data_with_retries:
    out = "recovered data with retries"; break;                                                                         
  case ascq::recovered_data_with_positive_head_offset:
    out = "recovered data with positive head offset"; break;                                               
  case ascq::recovered_data_with_negative_head_offset:
    out = "recovered data with negative head offset"; break;                                               
  case ascq::recovered_data_with_retries_and_or_circ_applied:
    out = "recovered data with retries and/or circ applied"; break;                                 
  case ascq::recovered_data_using_previous_sector_id:
    out = "recovered data using previous sector id"; break;                                                 
  case ascq::recovered_data_without_ecc_data_auto_reallocated:
    out = "recovered data without ecc - data auto-reallocated"; break;                             
  case ascq::recovered_data_without_ecc_recommend_reassignment:
    out = "recovered data without ecc - recommend reassignment"; break;                           
  case ascq::recovered_data_without_ecc_recommend_rewrite:
    out = "recovered data without ecc - recommend rewrite"; break;                                     
  case ascq::recovered_data_without_ecc_data_rewritten:
    out = "recovered data without ecc - data rewritten"; break;                                           
  case ascq::recovered_data_with_error_correction_applied:
    out = "recovered data with error correction applied"; break;                                       
  case ascq::recovered_data_with_error_corr_and_retries_applied:
    out = "recovered data with error corr. & retries applied"; break;                             
  case ascq::recovered_data_data_auto_reallocated:
    out = "recovered data - data auto-reallocated"; break;                                                     
  case ascq::recovered_data_with_circ:
    out = "recovered data with circ"; break;                                                                               
  case ascq::recovered_data_with_l_ec:
    out = "recovered data with l-ec"; break;                                                                               
  case ascq::recovered_data_recommend_reassignment:
    out = "recovered data - recommend reassignment"; break;                                                   
  case ascq::recovered_data_recommend_rewrite:
    out = "recovered data - recommend rewrite"; break;                                                             
  case ascq::recovered_data_with_ecc_data_rewritten:
    out = "recovered data with ecc - data rewritten"; break;                                                 
  case ascq::recovered_data_with_linking:
    out = "recovered data with linking"; break;                                                                         
  case ascq::defect_list_error:
    out = "defect list error"; break;                                                                                             
  case ascq::defect_list_not_available:
    out = "defect list not available"; break;                                                                             
  case ascq::defect_list_error_in_primary_list:
    out = "defect list error in primary list"; break;                                                             
  case ascq::defect_list_error_in_grown_list:
    out = "defect list error in grown list"; break;                                                                 
  case ascq::parameter_list_length_error:
    out = "parameter list length error"; break;                                                                         
  case ascq::synchronous_data_transfer_error:
    out = "synchronous data transfer error"; break;                                                                 
  case ascq::defect_list_not_found:
    out = "defect list not found"; break;                                                                                     
  case ascq::primary_defect_list_not_found:
    out = "primary defect list not found"; break;                                                                     
  case ascq::grown_defect_list_not_found:
    out = "grown defect list not found"; break;                                                                         
  case ascq::miscompare_during_verify_operation:
    out = "miscompare during verify operation"; break;                                                           
  case ascq::miscompare_verify_of_unmapped_lba:
    out = "miscompare verify of unmapped lba"; break;                                                             
  case ascq::recovered_id_with_ecc_correction:
    out = "recovered id with ecc correction"; break;                                                               
  case ascq::partial_defect_list_transfer:
    out = "partial defect list transfer"; break;                                                                       
  case ascq::invalid_command_operation_code:
    out = "invalid command operation code"; break;                                                                   
  case ascq::access_denied_initiator_pending_enrolled:
    out = "access denied - initiator pending-enrolled"; break;                                             
  case ascq::access_denied_no_access_rights:
    out = "access denied - no access rights"; break;                                                                 
  case ascq::access_denied_invalid_mgmt_id_key:
    out = "access denied - invalid mgmt id key"; break;                                                           
  case ascq::illegal_command_while_in_write_capable_state:
    out = "illegal command while in write capable state"; break;                                       
  case ascq::obsolete_2005:
    out = "obsolete 0x2005"; break;                                                                                                               
  case ascq::illegal_command_while_in_explicit_address_mode:
    out = "illegal command while in explicit address mode"; break;                                   
  case ascq::illegal_command_while_in_implicit_address_mode:
    out = "illegal command while in implicit address mode"; break;                                   
  case ascq::access_denied_enrollment_conflict:
    out = "access denied - enrollment conflict"; break;                                                           
  case ascq::access_denied_invalid_lu_identifier:
    out = "access denied - invalid lu identifier"; break;                                                       
  case ascq::access_denied_invalid_proxy_token:
    out = "access denied - invalid proxy token"; break;                                                           
  case ascq::access_denied_acl_lun_conflict:
    out = "access denied - acl lun conflict"; break;                                                                 
  case ascq::illegal_command_when_not_in_append_only_mode:
    out = "illegal command when not in append-only mode"; break;                                       
  case ascq::not_an_administrative_logical_unit:
    out = "not an administrative logical unit"; break;                                                           
  case ascq::not_a_subsidiary_logical_unit:
    out = "not a subsidiary logical unit"; break;                                                                     
  case ascq::not_a_conglomerate_logical_unit:
    out = "not a conglomerate logical unit"; break;                                                                 
  case ascq::logical_block_address_out_of_range:
    out = "logical block address out of range"; break;                                                           
  case ascq::invalid_element_address:
    out = "invalid element address"; break;                                                                                 
  case ascq::invalid_address_for_write:
    out = "invalid address for write"; break;                                                                             
  case ascq::invalid_write_crossing_layer_jump:
    out = "invalid write crossing layer jump"; break;                                                             
  case ascq::unaligned_write_command:
    out = "unaligned write command"; break;                                                                                 
  case ascq::write_boundary_violation:
    out = "write boundary violation"; break;                                                                               
  case ascq::attempt_to_read_invalid_data:
    out = "attempt to read invalid data"; break;                                                                       
  case ascq::read_boundary_violation:
    out = "read boundary violation"; break;                                                                                 
  case ascq::misaligned_write_command:
    out = "misaligned write command"; break;                                                                               
  case ascq::illegal_function_use_20_00_24_00_or_26_00_:
    out = "illegal function (use 20 00:24 00:or 26 00)"; break;                                        
  case ascq::invalid_token_operation_cause_not_reportable:
    out = "invalid token operation:cause not reportable"; break;                                      
  case ascq::invalid_token_operation_unsupported_token_type:
    out = "invalid token operation:unsupported token type"; break;                                  
  case ascq::invalid_token_operation_remote_token_usage_not_supported:
    out = "invalid token operation:remote token usage not supported"; break;              
  case ascq::invalid_token_operation_remote_rod_token_creation_not_supported:
    out = "invalid token operation:remote rod token creation not supported"; break;
  case ascq::invalid_token_operation_token_unknown:
    out = "invalid token operation:token unknown"; break;                                                    
  case ascq::invalid_token_operation_token_corrupt:
    out = "invalid token operation:token corrupt"; break;                                                    
  case ascq::invalid_token_operation_token_revoked:
    out = "invalid token operation:token revoked"; break;                                                    
  case ascq::invalid_token_operation_token_expired:
    out = "invalid token operation:token expired"; break;                                                    
  case ascq::invalid_token_operation_token_cancelled:
    out = "invalid token operation:token cancelled"; break;                                                
  case ascq::invalid_token_operation_token_deleted:
    out = "invalid token operation:token deleted"; break;                                                    
  case ascq::invalid_token_operation_invalid_token_length:
    out = "invalid token operation:invalid token length"; break;                                      
  case ascq::invalid_field_in_cdb:
    out = "invalid field in cdb"; break;                                                                                       
  case ascq::cdb_decryption_error:
    out = "cdb decryption error"; break;                                                                                       
  case ascq::obsolete_2402:
    out = "obsolete 0x2402"; break;                                                                                                               
  case ascq::obsolete_2403:
    out = "obsolete 0x2403"; break;                                                                                                               
  case ascq::security_audit_value_frozen:
    out = "security audit value frozen"; break;                                                                         
  case ascq::security_working_key_frozen:
    out = "security working key frozen"; break;                                                                         
  case ascq::nonce_not_unique:
    out = "nonce not unique"; break;                                                                                               
  case ascq::nonce_timestamp_out_of_range:
    out = "nonce timestamp out of range"; break;                                                                       
  case ascq::invalid_xcdb:
    out = "invalid xcdb"; break;                                                                                                       
  case ascq::invalid_fast_format:
    out = "invalid fast format"; break;                                                                                         
  case ascq::logical_unit_not_supported:
    out = "logical unit not supported"; break;                                                                           
  case ascq::invalid_field_in_parameter_list:
    out = "invalid field in parameter list"; break;                                                                 
  case ascq::parameter_not_supported:
    out = "parameter not supported"; break;                                                                                 
  case ascq::parameter_value_invalid:
    out = "parameter value invalid"; break;                                                                                 
  case ascq::threshold_parameters_not_supported:
    out = "threshold parameters not supported"; break;                                                           
  case ascq::invalid_release_of_persistent_reservation:
    out = "invalid release of persistent reservation"; break;                                             
  case ascq::data_decryption_error:
    out = "data decryption error"; break;                                                                                     
  case ascq::too_many_target_descriptors:
    out = "too many target descriptors"; break;                                                                         
  case ascq::unsupported_target_descriptor_type_code:
    out = "unsupported target descriptor type code"; break;                                                 
  case ascq::too_many_segment_descriptors:
    out = "too many segment descriptors"; break;                                                                       
  case ascq::unsupported_segment_descriptor_type_code:
    out = "unsupported segment descriptor type code"; break;                                               
  case ascq::unexpected_inexact_segment:
    out = "unexpected inexact segment"; break;                                                                           
  case ascq::inline_data_length_exceeded:
    out = "inline data length exceeded"; break;                                                                         
  case ascq::invalid_operation_for_copy_source_or_destination:
    out = "invalid operation for copy source or destination"; break;                               
  case ascq::copy_segment_granularity_violation:
    out = "copy segment granularity violation"; break;                                                           
  case ascq::invalid_parameter_while_port_is_enabled:
    out = "invalid parameter while port is enabled"; break;                                                 
  case ascq::invalid_data_out_buffer_integrity_check_value:
    out = "invalid data-out buffer integrity check value"; break;                                     
  case ascq::data_decryption_key_fail_limit_reached:
    out = "data decryption key fail limit reached"; break;                                                   
  case ascq::incomplete_key_associated_data_set:
    out = "incomplete key-associated data set"; break;                                                           
  case ascq::vendor_specific_key_reference_not_found:
    out = "vendor specific key reference not found"; break;                                                 
  case ascq::application_tag_mode_page_is_invalid:
    out = "application tag mode page is invalid"; break;                                                       
  case ascq::tape_stream_mirroring_prevented:
    out = "tape stream mirroring prevented"; break;                                                                 
  case ascq::copy_source_or_copy_destination_not_authorized:
    out = "copy source or copy destination not authorized"; break;                                   
  case ascq::write_protected:
    out = "write protected"; break;                                                                                                 
  case ascq::hardware_write_protected:
    out = "hardware write protected"; break;                                                                               
  case ascq::logical_unit_software_write_protected:
    out = "logical unit software write protected"; break;                                                     
  case ascq::associated_write_protect:
    out = "associated write protect"; break;                                                                               
  case ascq::persistent_write_protect:
    out = "persistent write protect"; break;                                                                               
  case ascq::permanent_write_protect:
    out = "permanent write protect"; break;                                                                                 
  case ascq::conditional_write_protect:
    out = "conditional write protect"; break;                                                                             
  case ascq::space_allocation_failed_write_protect:
    out = "space allocation failed write protect"; break;                                                     
  case ascq::zone_is_read_only:
    out = "zone is read only"; break;                                                                                             
  case ascq::not_ready_to_ready_change_medium_may_have_changed:
    out = "not ready to ready change:medium may have changed"; break;                            
  case ascq::import_or_export_element_accessed:
    out = "import or export element accessed"; break;                                                             
  case ascq::format_layer_may_have_changed:
    out = "format-layer may have changed"; break;                                                                     
  case ascq::import_export_element_accessed_medium_changed:
    out = "import/export element accessed:medium changed"; break;                                    
  case ascq::power_on_reset_or_bus_device_reset_occurred:
    out = "power on:reset:or bus device reset occurred"; break;                                       
  case ascq::power_on_occurred:
    out = "power on occurred"; break;                                                                                             
  case ascq::scsi_bus_reset_occurred:
    out = "scsi bus reset occurred"; break;                                                                                 
  case ascq::bus_device_reset_function_occurred:
    out = "bus device reset function occurred"; break;                                                           
  case ascq::device_internal_reset:
    out = "device internal reset"; break;                                                                                     
  case ascq::transceiver_mode_changed_to_single_ended:
    out = "transceiver mode changed to single-ended"; break;                                               
  case ascq::transceiver_mode_changed_to_lvd:
    out = "transceiver mode changed to lvd"; break;                                                                 
  case ascq::i_t_nexus_loss_occurred:
    out = "i_t nexus loss occurred"; break;                                                                                 
  case ascq::parameters_changed:
    out = "parameters changed"; break;                                                                                           
  case ascq::mode_parameters_changed:
    out = "mode parameters changed"; break;                                                                                 
  case ascq::log_parameters_changed:
    out = "log parameters changed"; break;                                                                                   
  case ascq::reservations_preempted:
    out = "reservations preempted"; break;                                                                                   
  case ascq::reservations_released:
    out = "reservations released"; break;                                                                                     
  case ascq::registrations_preempted:
    out = "registrations preempted"; break;                                                                                 
  case ascq::asymmetric_access_state_changed:
    out = "asymmetric access state changed"; break;                                                                 
  case ascq::implicit_asymmetric_access_state_transition_failed:
    out = "implicit asymmetric access state transition failed"; break;                           
  case ascq::priority_changed:
    out = "priority changed"; break;                                                                                               
  case ascq::capacity_data_has_changed:
    out = "capacity data has changed"; break;                                                                             
  case ascq::error_history_i_t_nexus_cleared:
    out = "error history i_t nexus cleared"; break;                                                                 
  case ascq::error_history_snapshot_released:
    out = "error history snapshot released"; break;                                                                 
  case ascq::error_recovery_attributes_have_changed:
    out = "error recovery attributes have changed"; break;                                                   
  case ascq::data_encryption_capabilities_changed:
    out = "data encryption capabilities changed"; break;                                                       
  case ascq::timestamp_changed:
    out = "timestamp changed"; break;                                                                                             
  case ascq::data_encryption_parameters_changed_by_another_i_t_nexus:
    out = "data encryption parameters changed by another i_t nexus"; break;                 
  case ascq::data_encryption_parameters_changed_by_vendor_specific_event:
    out = "data encryption parameters changed by vendor specific event"; break;         
  case ascq::data_encryption_key_instance_counter_has_changed:
    out = "data encryption key instance counter has changed"; break;                               
  case ascq::sa_creation_capabilities_data_has_changed:
    out = "sa creation capabilities data has changed"; break;                                             
  case ascq::medium_removal_prevention_preempted:
    out = "medium removal prevention preempted"; break;                                                         
  case ascq::zone_reset_write_pointer_recommended:
    out = "zone reset write pointer recommended"; break;                                                       
  case ascq::copy_cannot_execute_since_host_cannot_disconnect:
    out = "copy cannot execute since host cannot disconnect"; break;                               
  case ascq::command_sequence_error:
    out = "command sequence error"; break;                                                                                   
  case ascq::too_many_windows_specified:
    out = "too many windows specified"; break;                                                                           
  case ascq::invalid_combination_of_windows_specified:
    out = "invalid combination of windows specified"; break;                                               
  case ascq::current_program_area_is_not_empty:
    out = "current program area is not empty"; break;                                                             
  case ascq::current_program_area_is_empty:
    out = "current program area is empty"; break;                                                                     
  case ascq::illegal_power_condition_request:
    out = "illegal power condition request"; break;                                                                 
  case ascq::persistent_prevent_conflict:
    out = "persistent prevent conflict"; break;                                                                         
  case ascq::previous_busy_status:
    out = "previous busy status"; break;                                                                                       
  case ascq::previous_task_set_full_status:
    out = "previous task set full status"; break;                                                                     
  case ascq::previous_reservation_conflict_status:
    out = "previous reservation conflict status"; break;                                                       
  case ascq::partition_or_collection_contains_user_objects:
    out = "partition or collection contains user objects"; break;                                     
  case ascq::not_reserved:
    out = "not reserved"; break;                                                                                                       
  case ascq::orwrite_generation_does_not_match:
    out = "orwrite generation does not match"; break;                                                             
  case ascq::reset_write_pointer_not_allowed:
    out = "reset write pointer not allowed"; break;                                                                 
  case ascq::zone_is_offline:
    out = "zone is offline"; break;                                                                                                 
  case ascq::stream_not_open:
    out = "stream not open"; break;                                                                                                 
  case ascq::unwritten_data_in_zone:
    out = "unwritten data in zone"; break;                                                                                   
  case ascq::descriptor_format_sense_data_required:
    out = "descriptor format sense data required"; break;                                                     
  case ascq::overwrite_error_on_update_in_place:
    out = "overwrite error on update in place"; break;                                                           
  case ascq::insufficient_time_for_operation:
    out = "insufficient time for operation"; break;                                                                 
  case ascq::command_timeout_before_processing:
    out = "command timeout before processing"; break;                                                             
  case ascq::command_timeout_during_processing:
    out = "command timeout during processing"; break;                                                             
  case ascq::command_timeout_during_processing_due_to_error_recovery:
    out = "command timeout during processing due to error recovery"; break;                 
  case ascq::commands_cleared_by_another_initiator:
    out = "commands cleared by another initiator"; break;                                                     
  case ascq::commands_cleared_by_power_loss_notification:
    out = "commands cleared by power loss notification"; break;                                         
  case ascq::commands_cleared_by_device_server:
    out = "commands cleared by device server"; break;                                                             
  case ascq::some_commands_cleared_by_queuing_layer_event:
    out = "some commands cleared by queuing layer event"; break;                                       
  case ascq::incompatible_medium_installed:
    out = "incompatible medium installed"; break;                                                                     
  case ascq::cannot_read_medium_unknown_format:
    out = "cannot read medium - unknown format"; break;                                                           
  case ascq::cannot_read_medium_incompatible_format:
    out = "cannot read medium - incompatible format"; break;                                                 
  case ascq::cleaning_cartridge_installed:
    out = "cleaning cartridge installed"; break;                                                                       
  case ascq::cannot_write_medium_unknown_format:
    out = "cannot write medium - unknown format"; break;                                                         
  case ascq::cannot_write_medium_incompatible_format:
    out = "cannot write medium - incompatible format"; break;                                               
  case ascq::cannot_format_medium_incompatible_medium:
    out = "cannot format medium - incompatible medium"; break;                                             
  case ascq::cleaning_failure:
    out = "cleaning failure"; break;                                                                                               
  case ascq::cannot_write_application_code_mismatch:
    out = "cannot write - application code mismatch"; break;                                                 
  case ascq::current_session_not_fixated_for_append:
    out = "current session not fixated for append"; break;                                                   
  case ascq::cleaning_request_rejected:
    out = "cleaning request rejected"; break;                                                                             
  case ascq::worm_medium_overwrite_attempted:
    out = "worm medium - overwrite attempted"; break;                                                               
  case ascq::worm_medium_integrity_check:
    out = "worm medium - integrity check"; break;                                                                       
  case ascq::medium_not_formatted:
    out = "medium not formatted"; break;                                                                                       
  case ascq::incompatible_volume_type:
    out = "incompatible volume type"; break;                                                                               
  case ascq::incompatible_volume_qualifier:
    out = "incompatible volume qualifier"; break;                                                                     
  case ascq::cleaning_volume_expired:
    out = "cleaning volume expired"; break;                                                                                 
  case ascq::medium_format_corrupted:
    out = "medium format corrupted"; break;                                                                                 
  case ascq::format_command_failed:
    out = "format command failed"; break;                                                                                     
  case ascq::zoned_formatting_failed_due_to_spare_linking:
    out = "zoned formatting failed due to spare linking"; break;                                       
  case ascq::sanitize_command_failed:
    out = "sanitize command failed"; break;                                                                                 
  case ascq::no_defect_spare_location_available:
    out = "no defect spare location available"; break;                                                           
  case ascq::defect_list_update_failure:
    out = "defect list update failure"; break;                                                                           
  case ascq::tape_length_error:
    out = "tape length error"; break;                                                                                             
  case ascq::enclosure_failure:
    out = "enclosure failure"; break;                                                                                             
  case ascq::enclosure_services_failure:
    out = "enclosure services failure"; break;                                                                           
  case ascq::unsupported_enclosure_function:
    out = "unsupported enclosure function"; break;                                                                   
  case ascq::enclosure_services_unavailable:
    out = "enclosure services unavailable"; break;                                                                   
  case ascq::enclosure_services_transfer_failure:
    out = "enclosure services transfer failure"; break;                                                         
  case ascq::enclosure_services_transfer_refused:
    out = "enclosure services transfer refused"; break;                                                         
  case ascq::enclosure_services_checksum_error:
    out = "enclosure services checksum error"; break;                                                             
  case ascq::ribbon_ink_or_toner_failure:
    out = "ribbon:ink:or toner failure"; break;                                                                       
  case ascq::rounded_parameter:
    out = "rounded parameter"; break;                                                                                             
  case ascq::event_status_notification:
    out = "event status notification"; break;                                                                             
  case ascq::esn_power_management_class_event:
    out = "esn - power management class event"; break;                                                             
  case ascq::esn_media_class_event:
    out = "esn - media class event"; break;                                                                                   
  case ascq::esn_device_busy_class_event:
    out = "esn - device busy class event"; break;                                                                       
  case ascq::thin_provisioning_soft_threshold_reached:
    out = "thin provisioning soft threshold reached"; break;                                               
  case ascq::saving_parameters_not_supported:
    out = "saving parameters not supported"; break;                                                                 
  case ascq::medium_not_present:
    out = "medium not present"; break;                                                                                           
  case ascq::medium_not_present_tray_closed:
    out = "medium not present - tray closed"; break;                                                                 
  case ascq::medium_not_present_tray_open:
    out = "medium not present - tray open"; break;                                                                     
  case ascq::medium_not_present_loadable:
    out = "medium not present - loadable"; break;                                                                       
  case ascq::medium_not_present_medium_auxiliary_memory_accessible:
    out = "medium not present - medium auxiliary memory accessible"; break;                   
  case ascq::sequential_positioning_error:
    out = "sequential positioning error"; break;                                                                       
  case ascq::tape_position_error_at_beginning_of_medium:
    out = "tape position error at beginning-of-medium"; break;                                           
  case ascq::tape_position_error_at_end_of_medium:
    out = "tape position error at end-of-medium"; break;                                                       
  case ascq::tape_or_electronic_vertical_forms_unit_not_ready:
    out = "tape or electronic vertical forms unit not ready"; break;                               
  case ascq::slew_failure:
    out = "slew failure"; break;                                                                                                       
  case ascq::paper_jam:
    out = "paper jam"; break;                                                                                                             
  case ascq::failed_to_sense_top_of_form:
    out = "failed to sense top-of-form"; break;                                                                         
  case ascq::failed_to_sense_bottom_of_form:
    out = "failed to sense bottom-of-form"; break;                                                                   
  case ascq::reposition_error:
    out = "reposition error"; break;                                                                                               
  case ascq::read_past_end_of_medium:
    out = "read past end of medium"; break;                                                                                 
  case ascq::read_past_beginning_of_medium:
    out = "read past beginning of medium"; break;                                                                     
  case ascq::position_past_end_of_medium:
    out = "position past end of medium"; break;                                                                         
  case ascq::position_past_beginning_of_medium:
    out = "position past beginning of medium"; break;                                                             
  case ascq::medium_destination_element_full:
    out = "medium destination element full"; break;                                                                 
  case ascq::medium_source_element_empty:
    out = "medium source element empty"; break;                                                                         
  case ascq::end_of_medium_reached:
    out = "end of medium reached"; break;                                                                                     
  case ascq::medium_magazine_not_accessible:
    out = "medium magazine not accessible"; break;                                                                   
  case ascq::medium_magazine_removed:
    out = "medium magazine removed"; break;                                                                                 
  case ascq::medium_magazine_inserted:
    out = "medium magazine inserted"; break;                                                                               
  case ascq::medium_magazine_locked:
    out = "medium magazine locked"; break;                                                                                   
  case ascq::medium_magazine_unlocked:
    out = "medium magazine unlocked"; break;                                                                               
  case ascq::mechanical_positioning_or_changer_error:
    out = "mechanical positioning or changer error"; break;                                                 
  case ascq::read_past_end_of_user_object:
    out = "read past end of user object"; break;                                                                       
  case ascq::element_disabled:
    out = "element disabled"; break;                                                                                               
  case ascq::element_enabled:
    out = "element enabled"; break;                                                                                                 
  case ascq::data_transfer_device_removed:
    out = "data transfer device removed"; break;                                                                       
  case ascq::data_transfer_device_inserted:
    out = "data transfer device inserted"; break;                                                                     
  case ascq::too_many_logical_objects_on_partition_to_support_operation:
    out = "too many logical objects on partition to support operation"; break;           
  case ascq::custom_3c00:
    out = "ascq 3c00"; break;                                                                                                                  
  case ascq::invalid_bits_in_identify_message:
    out = "invalid bits in identify message"; break;                                                               
  case ascq::logical_unit_has_not_self_configured_yet:
    out = "logical unit has not self-configured yet"; break;                                               
  case ascq::logical_unit_failure:
    out = "logical unit failure"; break;                                                                                       
  case ascq::timeout_on_logical_unit:
    out = "timeout on logical unit"; break;                                                                                 
  case ascq::logical_unit_failed_self_test:
    out = "logical unit failed self-test"; break;                                                                     
  case ascq::logical_unit_unable_to_update_self_test_log:
    out = "logical unit unable to update self-test log"; break;                                         
  case ascq::target_operating_conditions_have_changed:
    out = "target operating conditions have changed"; break;                                               
  case ascq::microcode_has_been_changed:
    out = "microcode has been changed"; break;                                                                           
  case ascq::changed_operating_definition:
    out = "changed operating definition"; break;                                                                       
  case ascq::inquiry_data_has_changed:
    out = "inquiry data has changed"; break;                                                                               
  case ascq::component_device_attached:
    out = "component device attached"; break;                                                                             
  case ascq::device_identifier_changed:
    out = "device identifier changed"; break;                                                                             
  case ascq::redundancy_group_created_or_modified:
    out = "redundancy group created or modified"; break;                                                       
  case ascq::redundancy_group_deleted:
    out = "redundancy group deleted"; break;                                                                               
  case ascq::spare_created_or_modified:
    out = "spare created or modified"; break;                                                                             
  case ascq::spare_deleted:
    out = "spare deleted"; break;                                                                                                     
  case ascq::volume_set_created_or_modified:
    out = "volume set created or modified"; break;                                                                   
  case ascq::volume_set_deleted:
    out = "volume set deleted"; break;                                                                                           
  case ascq::volume_set_deassigned:
    out = "volume set deassigned"; break;                                                                                     
  case ascq::volume_set_reassigned:
    out = "volume set reassigned"; break;                                                                                     
  case ascq::reported_luns_data_has_changed:
    out = "reported luns data has changed"; break;                                                                   
  case ascq::echo_buffer_overwritten:
    out = "echo buffer overwritten"; break;                                                                                 
  case ascq::medium_loadable:
    out = "medium loadable"; break;                                                                                                 
  case ascq::medium_auxiliary_memory_accessible:
    out = "medium auxiliary memory accessible"; break;                                                           
  case ascq::iscsi_ip_address_added:
    out = "iscsi ip address added"; break;                                                                                   
  case ascq::iscsi_ip_address_removed:
    out = "iscsi ip address removed"; break;                                                                               
  case ascq::iscsi_ip_address_changed:
    out = "iscsi ip address changed"; break;                                                                               
  case ascq::inspect_referrals_sense_descriptors:
    out = "inspect referrals sense descriptors"; break;                                                         
  case ascq::microcode_has_been_changed_without_reset:
    out = "microcode has been changed without reset"; break;                                               
  case ascq::zone_transition_to_full:
    out = "zone transition to full"; break;                                                                                 
  case ascq::bind_completed:
    out = "bind completed"; break;                                                                                                   
  case ascq::bind_redirected:
    out = "bind redirected"; break;                                                                                                 
  case ascq::subsidiary_binding_changed:
    out = "subsidiary binding changed"; break;                                                                           
  case ascq::ram_failure_should_use_40_nn:
    out = "ram failure (should use 40 nn)"; break;                                                                    
  case ascq::data_path_failure_should_use_40_nn:
    out = "data path failure (should use 40 nn)"; break;                                                        
  case ascq::power_on_or_self_test_failure_should_use_40_nn:
    out = "power-on or self-test failure (should use 40 nn)"; break;                                
  case ascq::message_error:
    out = "message error"; break;                                                                                                     
  case ascq::internal_target_failure:
    out = "internal target failure"; break;                                                                                 
  case ascq::persistent_reservation_information_lost:
    out = "persistent reservation information lost"; break;                                                 
  case ascq::ata_device_failed_set_features:
    out = "ata device failed set features"; break;                                                                   
  case ascq::select_or_reselect_failure:
    out = "select or reselect failure"; break;                                                                           
  case ascq::unsuccessful_soft_reset:
    out = "unsuccessful soft reset"; break;                                                                                 
  case ascq::scsi_parity_error:
    out = "scsi parity error"; break;                                                                                             
  case ascq::data_phase_crc_error_detected:
    out = "data phase crc error detected"; break;                                                                     
  case ascq::scsi_parity_error_detected_during_st_data_phase:
    out = "scsi parity error detected during st data phase"; break;                                 
  case ascq::information_unit_iucrc_error_detected:
    out = "information unit iucrc error detected"; break;                                                     
  case ascq::asynchronous_information_protection_error_detected:
    out = "asynchronous information protection error detected"; break;                           
  case ascq::protocol_service_crc_error:
    out = "protocol service crc error"; break;                                                                           
  case ascq::phy_test_function_in_progress:
    out = "phy test function in progress"; break;                                                                     
  case ascq::some_commands_cleared_by_iscsi_protocol_event:
    out = "some commands cleared by iscsi protocol event"; break;                                     
  case ascq::initiator_detected_error_message_received:
    out = "initiator detected error message received"; break;                                             
  case ascq::invalid_message_error:
    out = "invalid message error"; break;                                                                                     
  case ascq::command_phase_error:
    out = "command phase error"; break;                                                                                         
  case ascq::data_phase_error:
    out = "data phase error"; break;                                                                                               
  case ascq::invalid_target_port_transfer_tag_received:
    out = "invalid target port transfer tag received"; break;                                             
  case ascq::too_much_write_data:
    out = "too much write data"; break;                                                                                         
  case ascq::ack_nak_timeout:
    out = "ack/nak timeout"; break;                                                                                                 
  case ascq::nak_received:
    out = "nak received"; break;                                                                                                       
  case ascq::data_offset_error:
    out = "data offset error"; break;                                                                                             
  case ascq::initiator_response_timeout:
    out = "initiator response timeout"; break;                                                                           
  case ascq::connection_lost:
    out = "connection lost"; break;                                                                                                 
  case ascq::data_in_buffer_overflow_data_buffer_size:
    out = "data-in buffer overflow - data buffer size"; break;                                             
  case ascq::data_in_buffer_overflow_data_buffer_descriptor_area:
    out = "data-in buffer overflow - data buffer descriptor area"; break;                       
  case ascq::data_in_buffer_error:
    out = "data-in buffer error"; break;                                                                                       
  case ascq::data_out_buffer_overflow_data_buffer_size:
    out = "data-out buffer overflow - data buffer size"; break;                                           
  case ascq::data_out_buffer_overflow_data_buffer_descriptor_area:
    out = "data-out buffer overflow - data buffer descriptor area"; break;                     
  case ascq::data_out_buffer_error:
    out = "data-out buffer error"; break;                                                                                     
  case ascq::pcie_fabric_error:
    out = "pcie fabric error"; break;                                                                                             
  case ascq::pcie_completion_timeout:
    out = "pcie completion timeout"; break;                                                                                 
  case ascq::pcie_completer_abort:
    out = "pcie completer abort"; break;                                                                                       
  case ascq::pcie_poisoned_tlp_received:
    out = "pcie poisoned tlp received"; break;                                                                           
  case ascq::pcie_ecrc_check_failed:
    out = "pcie ecrc check failed"; break;                                                                                   
  case ascq::pcie_unsupported_request:
    out = "pcie unsupported request"; break;                                                                               
  case ascq::pcie_acs_violation:
    out = "pcie acs violation"; break;                                                                                           
  case ascq::pcie_tlp_prefix_blocked:
    out = "pcie tlp prefix blocked"; break;                                                                                 
  case ascq::logical_unit_failed_self_configuration:
    out = "logical unit failed self-configuration"; break;                                                   
  case ascq::overlapped_commands_attempted:
    out = "overlapped commands attempted"; break;                                                                     
  case ascq::custom_4f00:
    out = "ascq 4f00"; break;                                                                                                                  
  case ascq::write_append_error:
    out = "write append error"; break;                                                                                           
  case ascq::write_append_position_error:
    out = "write append position error"; break;                                                                         
  case ascq::position_error_related_to_timing:
    out = "position error related to timing"; break;                                                               
  case ascq::erase_failure:
    out = "erase failure"; break;                                                                                                     
  case ascq::erase_failure_incomplete_erase_operation_detected:
    out = "erase failure - incomplete erase operation detected"; break;                           
  case ascq::cartridge_fault:
    out = "cartridge fault"; break;                                                                                                 
  case ascq::media_load_or_eject_failed:
    out = "media load or eject failed"; break;                                                                           
  case ascq::unload_tape_failure:
    out = "unload tape failure"; break;                                                                                         
  case ascq::medium_removal_prevented:
    out = "medium removal prevented"; break;                                                                               
  case ascq::medium_removal_prevented_by_data_transfer_element:
    out = "medium removal prevented by data transfer element"; break;                             
  case ascq::medium_thread_or_unthread_failure:
    out = "medium thread or unthread failure"; break;                                                             
  case ascq::volume_identifier_invalid:
    out = "volume identifier invalid"; break;                                                                             
  case ascq::volume_identifier_missing:
    out = "volume identifier missing"; break;                                                                             
  case ascq::duplicate_volume_identifier:
    out = "duplicate volume identifier"; break;                                                                         
  case ascq::element_status_unknown:
    out = "element status unknown"; break;                                                                                   
  case ascq::data_transfer_device_error_load_failed:
    out = "data transfer device error - load failed"; break;                                                 
  case ascq::data_transfer_device_error_unload_failed:
    out = "data transfer device error - unload failed"; break;                                             
  case ascq::data_transfer_device_error_unload_missing:
    out = "data transfer device error - unload missing"; break;                                           
  case ascq::data_transfer_device_error_eject_failed:
    out = "data transfer device error - eject failed"; break;                                               
  case ascq::data_transfer_device_error_library_communication_failed:
    out = "data transfer device error - library communication failed"; break;               
  case ascq::scsi_to_host_system_interface_failure:
    out = "scsi to host system interface failure"; break;                                                     
  case ascq::system_resource_failure:
    out = "system resource failure"; break;                                                                                 
  case ascq::system_buffer_full:
    out = "system buffer full"; break;                                                                                           
  case ascq::insufficient_reservation_resources:
    out = "insufficient reservation resources"; break;                                                           
  case ascq::insufficient_resources:
    out = "insufficient resources"; break;                                                                                   
  case ascq::insufficient_registration_resources:
    out = "insufficient registration resources"; break;                                                         
  case ascq::insufficient_access_control_resources:
    out = "insufficient access control resources"; break;                                                     
  case ascq::auxiliary_memory_out_of_space:
    out = "auxiliary memory out of space"; break;                                                                     
  case ascq::quota_error:
    out = "quota error"; break;                                                                                                         
  case ascq::maximum_number_of_supplemental_decryption_keys_exceeded:
    out = "maximum number of supplemental decryption keys exceeded"; break;                 
  case ascq::medium_auxiliary_memory_not_accessible:
    out = "medium auxiliary memory not accessible"; break;                                                   
  case ascq::data_currently_unavailable:
    out = "data currently unavailable"; break;                                                                           
  case ascq::insufficient_power_for_operation:
    out = "insufficient power for operation"; break;                                                               
  case ascq::insufficient_resources_to_create_rod:
    out = "insufficient resources to create rod"; break;                                                       
  case ascq::insufficient_resources_to_create_rod_token:
    out = "insufficient resources to create rod token"; break;                                           
  case ascq::insufficient_zone_resources:
    out = "insufficient zone resources"; break;                                                                         
  case ascq::insufficient_zone_resources_to_complete_write:
    out = "insufficient zone resources to complete write"; break;                                     
  case ascq::maximum_number_of_streams_open:
    out = "maximum number of streams open"; break;                                                                   
  case ascq::insufficient_resources_to_bind:
    out = "insufficient resources to bind"; break;                                                                   
  case ascq::custom_5600:
    out = "ascq 5600"; break;                                                                                                                  
  case ascq::unable_to_recover_table_of_contents:
    out = "unable to recover table-of-contents"; break;                                                         
  case ascq::generation_does_not_exist:
    out = "generation does not exist"; break;                                                                             
  case ascq::updated_block_read:
    out = "updated block read"; break;                                                                                           
  case ascq::operator_request_or_state_change_input:
    out = "operator request or state change input"; break;                                                   
  case ascq::operator_medium_removal_request:
    out = "operator medium removal request"; break;                                                                 
  case ascq::operator_selected_write_protect:
    out = "operator selected write protect"; break;                                                                 
  case ascq::operator_selected_write_permit:
    out = "operator selected write permit"; break;                                                                   
  case ascq::log_exception:
    out = "log exception"; break;                                                                                                     
  case ascq::threshold_condition_met:
    out = "threshold condition met"; break;                                                                                 
  case ascq::log_counter_at_maximum:
    out = "log counter at maximum"; break;                                                                                   
  case ascq::log_list_codes_exhausted:
    out = "log list codes exhausted"; break;                                                                               
  case ascq::rpl_status_change:
    out = "rpl status change"; break;                                                                                             
  case ascq::spindles_synchronized:
    out = "spindles synchronized"; break;                                                                                     
  case ascq::spindles_not_synchronized:
    out = "spindles not synchronized"; break;                                                                             
  case ascq::failure_prediction_threshold_exceeded:
    out = "failure prediction threshold exceeded"; break;                                                     
  case ascq::media_failure_prediction_threshold_exceeded:
    out = "media failure prediction threshold exceeded"; break;                                         
  case ascq::logical_unit_failure_prediction_threshold_exceeded:
    out = "logical unit failure prediction threshold exceeded"; break;                           
  case ascq::spare_area_exhaustion_prediction_threshold_exceeded:
    out = "spare area exhaustion prediction threshold exceeded"; break;                         
  case ascq::hardware_impending_failure_general_hard_drive_failure:
    out = "hardware impending failure general hard drive failure"; break;                     
  case ascq::hardware_impending_failure_drive_error_rate_too_high:
    out = "hardware impending failure drive error rate too high"; break;                       
  case ascq::hardware_impending_failure_data_error_rate_too_high:
    out = "hardware impending failure data error rate too high"; break;                         
  case ascq::hardware_impending_failure_seek_error_rate_too_high:
    out = "hardware impending failure seek error rate too high"; break;                         
  case ascq::hardware_impending_failure_too_many_block_reassigns:
    out = "hardware impending failure too many block reassigns"; break;                         
  case ascq::hardware_impending_failure_access_times_too_high:
    out = "hardware impending failure access times too high"; break;                               
  case ascq::hardware_impending_failure_start_unit_times_too_high:
    out = "hardware impending failure start unit times too high"; break;                       
  case ascq::hardware_impending_failure_channel_parametrics:
    out = "hardware impending failure channel parametrics"; break;                                   
  case ascq::hardware_impending_failure_controller_detected:
    out = "hardware impending failure controller detected"; break;                                   
  case ascq::hardware_impending_failure_throughput_performance:
    out = "hardware impending failure throughput performance"; break;                             
  case ascq::hardware_impending_failure_seek_time_performance:
    out = "hardware impending failure seek time performance"; break;                               
  case ascq::hardware_impending_failure_spin_up_retry_count:
    out = "hardware impending failure spin-up retry count"; break;                                   
  case ascq::hardware_impending_failure_drive_calibration_retry_count:
    out = "hardware impending failure drive calibration retry count"; break;               
  case ascq::hardware_impending_failure_power_loss_protection_circuit:
    out = "hardware impending failure power loss protection circuit"; break;               
  case ascq::controller_impending_failure_general_hard_drive_failure:
    out = "controller impending failure general hard drive failure"; break;                 
  case ascq::controller_impending_failure_drive_error_rate_too_high:
    out = "controller impending failure drive error rate too high"; break;                   
  case ascq::controller_impending_failure_data_error_rate_too_high:
    out = "controller impending failure data error rate too high"; break;                     
  case ascq::controller_impending_failure_seek_error_rate_too_high:
    out = "controller impending failure seek error rate too high"; break;                     
  case ascq::controller_impending_failure_too_many_block_reassigns:
    out = "controller impending failure too many block reassigns"; break;                     
  case ascq::controller_impending_failure_access_times_too_high:
    out = "controller impending failure access times too high"; break;                           
  case ascq::controller_impending_failure_start_unit_times_too_high:
    out = "controller impending failure start unit times too high"; break;                   
  case ascq::controller_impending_failure_channel_parametrics:
    out = "controller impending failure channel parametrics"; break;                               
  case ascq::controller_impending_failure_controller_detected:
    out = "controller impending failure controller detected"; break;                               
  case ascq::controller_impending_failure_throughput_performance:
    out = "controller impending failure throughput performance"; break;                         
  case ascq::controller_impending_failure_seek_time_performance:
    out = "controller impending failure seek time performance"; break;                           
  case ascq::controller_impending_failure_spin_up_retry_count:
    out = "controller impending failure spin-up retry count"; break;                               
  case ascq::controller_impending_failure_drive_calibration_retry_count:
    out = "controller impending failure drive calibration retry count"; break;           
  case ascq::data_channel_impending_failure_general_hard_drive_failure:
    out = "data channel impending failure general hard drive failure"; break;             
  case ascq::data_channel_impending_failure_drive_error_rate_too_high:
    out = "data channel impending failure drive error rate too high"; break;               
  case ascq::data_channel_impending_failure_data_error_rate_too_high:
    out = "data channel impending failure data error rate too high"; break;                 
  case ascq::data_channel_impending_failure_seek_error_rate_too_high:
    out = "data channel impending failure seek error rate too high"; break;                 
  case ascq::data_channel_impending_failure_too_many_block_reassigns:
    out = "data channel impending failure too many block reassigns"; break;                 
  case ascq::data_channel_impending_failure_access_times_too_high:
    out = "data channel impending failure access times too high"; break;                       
  case ascq::data_channel_impending_failure_start_unit_times_too_high:
    out = "data channel impending failure start unit times too high"; break;               
  case ascq::data_channel_impending_failure_channel_parametrics:
    out = "data channel impending failure channel parametrics"; break;                           
  case ascq::data_channel_impending_failure_controller_detected:
    out = "data channel impending failure controller detected"; break;                           
  case ascq::data_channel_impending_failure_throughput_performance:
    out = "data channel impending failure throughput performance"; break;                     
  case ascq::data_channel_impending_failure_seek_time_performance:
    out = "data channel impending failure seek time performance"; break;                       
  case ascq::data_channel_impending_failure_spin_up_retry_count:
    out = "data channel impending failure spin-up retry count"; break;                           
  case ascq::data_channel_impending_failure_drive_calibration_retry_count:
    out = "data channel impending failure drive calibration retry count"; break;       
  case ascq::servo_impending_failure_general_hard_drive_failure:
    out = "servo impending failure general hard drive failure"; break;                           
  case ascq::servo_impending_failure_drive_error_rate_too_high:
    out = "servo impending failure drive error rate too high"; break;                             
  case ascq::servo_impending_failure_data_error_rate_too_high:
    out = "servo impending failure data error rate too high"; break;                               
  case ascq::servo_impending_failure_seek_error_rate_too_high:
    out = "servo impending failure seek error rate too high"; break;                               
  case ascq::servo_impending_failure_too_many_block_reassigns:
    out = "servo impending failure too many block reassigns"; break;                               
  case ascq::servo_impending_failure_access_times_too_high:
    out = "servo impending failure access times too high"; break;                                     
  case ascq::servo_impending_failure_start_unit_times_too_high:
    out = "servo impending failure start unit times too high"; break;                             
  case ascq::servo_impending_failure_channel_parametrics:
    out = "servo impending failure channel parametrics"; break;                                         
  case ascq::servo_impending_failure_controller_detected:
    out = "servo impending failure controller detected"; break;                                         
  case ascq::servo_impending_failure_throughput_performance:
    out = "servo impending failure throughput performance"; break;                                   
  case ascq::servo_impending_failure_seek_time_performance:
    out = "servo impending failure seek time performance"; break;                                     
  case ascq::servo_impending_failure_spin_up_retry_count:
    out = "servo impending failure spin-up retry count"; break;                                         
  case ascq::servo_impending_failure_drive_calibration_retry_count:
    out = "servo impending failure drive calibration retry count"; break;                     
  case ascq::spindle_impending_failure_general_hard_drive_failure:
    out = "spindle impending failure general hard drive failure"; break;                       
  case ascq::spindle_impending_failure_drive_error_rate_too_high:
    out = "spindle impending failure drive error rate too high"; break;                         
  case ascq::spindle_impending_failure_data_error_rate_too_high:
    out = "spindle impending failure data error rate too high"; break;                           
  case ascq::spindle_impending_failure_seek_error_rate_too_high:
    out = "spindle impending failure seek error rate too high"; break;                           
  case ascq::spindle_impending_failure_too_many_block_reassigns:
    out = "spindle impending failure too many block reassigns"; break;                           
  case ascq::spindle_impending_failure_access_times_too_high:
    out = "spindle impending failure access times too high"; break;                                 
  case ascq::spindle_impending_failure_start_unit_times_too_high:
    out = "spindle impending failure start unit times too high"; break;                         
  case ascq::spindle_impending_failure_channel_parametrics:
    out = "spindle impending failure channel parametrics"; break;                                     
  case ascq::spindle_impending_failure_controller_detected:
    out = "spindle impending failure controller detected"; break;                                     
  case ascq::spindle_impending_failure_throughput_performance:
    out = "spindle impending failure throughput performance"; break;                               
  case ascq::spindle_impending_failure_seek_time_performance:
    out = "spindle impending failure seek time performance"; break;                                 
  case ascq::spindle_impending_failure_spin_up_retry_count:
    out = "spindle impending failure spin-up retry count"; break;                                     
  case ascq::spindle_impending_failure_drive_calibration_retry_count:
    out = "spindle impending failure drive calibration retry count"; break;                 
  case ascq::firmware_impending_failure_general_hard_drive_failure:
    out = "firmware impending failure general hard drive failure"; break;                     
  case ascq::firmware_impending_failure_drive_error_rate_too_high:
    out = "firmware impending failure drive error rate too high"; break;                       
  case ascq::firmware_impending_failure_data_error_rate_too_high:
    out = "firmware impending failure data error rate too high"; break;                         
  case ascq::firmware_impending_failure_seek_error_rate_too_high:
    out = "firmware impending failure seek error rate too high"; break;                         
  case ascq::firmware_impending_failure_too_many_block_reassigns:
    out = "firmware impending failure too many block reassigns"; break;                         
  case ascq::firmware_impending_failure_access_times_too_high:
    out = "firmware impending failure access times too high"; break;                               
  case ascq::firmware_impending_failure_start_unit_times_too_high:
    out = "firmware impending failure start unit times too high"; break;                       
  case ascq::firmware_impending_failure_channel_parametrics:
    out = "firmware impending failure channel parametrics"; break;                                   
  case ascq::firmware_impending_failure_controller_detected:
    out = "firmware impending failure controller detected"; break;                                   
  case ascq::firmware_impending_failure_throughput_performance:
    out = "firmware impending failure throughput performance"; break;                             
  case ascq::firmware_impending_failure_seek_time_performance:
    out = "firmware impending failure seek time performance"; break;                               
  case ascq::firmware_impending_failure_spin_up_retry_count:
    out = "firmware impending failure spin-up retry count"; break;                                   
  case ascq::firmware_impending_failure_drive_calibration_retry_count:
    out = "firmware impending failure drive calibration retry count"; break;               
  case ascq::media_impending_failure_endurance_limit_met:
    out = "media impending failure endurance limit met"; break;                                         
  case ascq::failure_prediction_threshold_exceeded_false_:
    out = "failure prediction threshold exceeded (false)"; break;                                      
  case ascq::low_power_condition_on:
    out = "low power condition on"; break;                                                                                   
  case ascq::idle_condition_activated_by_timer:
    out = "idle condition activated by timer"; break;                                                             
  case ascq::standby_condition_activated_by_timer:
    out = "standby condition activated by timer"; break;                                                       
  case ascq::idle_condition_activated_by_command:
    out = "idle condition activated by command"; break;                                                         
  case ascq::standby_condition_activated_by_command:
    out = "standby condition activated by command"; break;                                                   
  case ascq::idle_b_condition_activated_by_timer:
    out = "idle_b condition activated by timer"; break;                                                         
  case ascq::idle_b_condition_activated_by_command:
    out = "idle_b condition activated by command"; break;                                                     
  case ascq::idle_c_condition_activated_by_timer:
    out = "idle_c condition activated by timer"; break;                                                         
  case ascq::idle_c_condition_activated_by_command:
    out = "idle_c condition activated by command"; break;                                                     
  case ascq::standby_y_condition_activated_by_timer:
    out = "standby_y condition activated by timer"; break;                                                   
  case ascq::standby_y_condition_activated_by_command:
    out = "standby_y condition activated by command"; break;                                               
  case ascq::power_state_change_to_active:
    out = "power state change to active"; break;                                                                       
  case ascq::power_state_change_to_idle:
    out = "power state change to idle"; break;                                                                           
  case ascq::power_state_change_to_standby:
    out = "power state change to standby"; break;                                                                     
  case ascq::power_state_change_to_sleep:
    out = "power state change to sleep"; break;                                                                         
  case ascq::power_state_change_to_device_control:
    out = "power state change to device control"; break;                                                       
  case ascq::custom_5f00:
    out = "ascq 5f00"; break;                                                                                                                  
  case ascq::lamp_failure:
    out = "lamp failure"; break;                                                                                                       
  case ascq::video_acquisition_error:
    out = "video acquisition error"; break;                                                                                 
  case ascq::unable_to_acquire_video:
    out = "unable to acquire video"; break;                                                                                 
  case ascq::out_of_focus:
    out = "out of focus"; break;                                                                                                       
  case ascq::scan_head_positioning_error:
    out = "scan head positioning error"; break;                                                                         
  case ascq::end_of_user_area_encountered_on_this_track:
    out = "end of user area encountered on this track"; break;                                           
  case ascq::packet_does_not_fit_in_available_space:
    out = "packet does not fit in available space"; break;                                                   
  case ascq::illegal_mode_for_this_track:
    out = "illegal mode for this track"; break;                                                                         
  case ascq::invalid_packet_size:
    out = "invalid packet size"; break;                                                                                         
  case ascq::voltage_fault:
    out = "voltage fault"; break;                                                                                                     
  case ascq::automatic_document_feeder_cover_up:
    out = "automatic document feeder cover up"; break;                                                           
  case ascq::automatic_document_feeder_lift_up:
    out = "automatic document feeder lift up"; break;                                                             
  case ascq::document_jam_in_automatic_document_feeder:
    out = "document jam in automatic document feeder"; break;                                             
  case ascq::document_miss_feed_automatic_in_document_feeder:
    out = "document miss feed automatic in document feeder"; break;                                 
  case ascq::configuration_failure:
    out = "configuration failure"; break;                                                                                     
  case ascq::configuration_of_incapable_logical_units_failed:
    out = "configuration of incapable logical units failed"; break;                                 
  case ascq::add_logical_unit_failed:
    out = "add logical unit failed"; break;                                                                                 
  case ascq::modification_of_logical_unit_failed:
    out = "modification of logical unit failed"; break;                                                         
  case ascq::exchange_of_logical_unit_failed:
    out = "exchange of logical unit failed"; break;                                                                 
  case ascq::remove_of_logical_unit_failed:
    out = "remove of logical unit failed"; break;                                                                     
  case ascq::attachment_of_logical_unit_failed:
    out = "attachment of logical unit failed"; break;                                                             
  case ascq::creation_of_logical_unit_failed:
    out = "creation of logical unit failed"; break;                                                                 
  case ascq::assign_failure_occurred:
    out = "assign failure occurred"; break;                                                                                 
  case ascq::multiply_assigned_logical_unit:
    out = "multiply assigned logical unit"; break;                                                                   
  case ascq::set_target_port_groups_command_failed:
    out = "set target port groups command failed"; break;                                                     
  case ascq::ata_device_feature_not_enabled:
    out = "ata device feature not enabled"; break;                                                                   
  case ascq::command_rejected:
    out = "command rejected"; break;                                                                                               
  case ascq::explicit_bind_not_allowed:
    out = "explicit bind not allowed"; break;                                                                             
  case ascq::logical_unit_not_configured:
    out = "logical unit not configured"; break;                                                                         
  case ascq::subsidiary_logical_unit_not_configured:
    out = "subsidiary logical unit not configured"; break;                                                   
  case ascq::data_loss_on_logical_unit:
    out = "data loss on logical unit"; break;                                                                             
  case ascq::multiple_logical_unit_failures:
    out = "multiple logical unit failures"; break;                                                                   
  case ascq::parity_data_mismatch:
    out = "parity/data mismatch"; break;                                                                                       
  case ascq::informational_refer_to_log:
    out = "informational:refer to log"; break;                                                                          
  case ascq::state_change_has_occurred:
    out = "state change has occurred"; break;                                                                             
  case ascq::redundancy_level_got_better:
    out = "redundancy level got better"; break;                                                                         
  case ascq::redundancy_level_got_worse:
    out = "redundancy level got worse"; break;                                                                           
  case ascq::rebuild_failure_occurred:
    out = "rebuild failure occurred"; break;                                                                               
  case ascq::recalculate_failure_occurred:
    out = "recalculate failure occurred"; break;                                                                       
  case ascq::command_to_logical_unit_failed:
    out = "command to logical unit failed"; break;                                                                   
  case ascq::copy_protection_key_exchange_failure_authentication_failure:
    out = "copy protection key exchange failure - authentication failure"; break;       
  case ascq::copy_protection_key_exchange_failure_key_not_present:
    out = "copy protection key exchange failure - key not present"; break;                     
  case ascq::copy_protection_key_exchange_failure_key_not_established:
    out = "copy protection key exchange failure - key not established"; break;             
  case ascq::read_of_scrambled_sector_without_authentication:
    out = "read of scrambled sector without authentication"; break;                                 
  case ascq::media_region_code_is_mismatched_to_logical_unit_region:
    out = "media region code is mismatched to logical unit region"; break;                   
  case ascq::drive_region_must_be_permanent_region_reset_count_error:
    out = "drive region must be permanent/region reset count error"; break;                 
  case ascq::insufficient_block_count_for_binding_nonce_recording:
    out = "insufficient block count for binding nonce recording"; break;                       
  case ascq::conflict_in_binding_nonce_recording:
    out = "conflict in binding nonce recording"; break;                                                         
  case ascq::insufficient_permission:
    out = "insufficient permission"; break;                                                                                 
  case ascq::invalid_drive_host_pairing_server:
    out = "invalid drive-host pairing server"; break;                                                             
  case ascq::drive_host_pairing_suspended:
    out = "drive-host pairing suspended"; break;                                                                       
  case ascq::decompression_exception_long_algorithm_id:
    out = "decompression exception long algorithm id"; break;                                             
  case ascq::session_fixation_error:
    out = "session fixation error"; break;                                                                                   
  case ascq::session_fixation_error_writing_lead_in:
    out = "session fixation error writing lead-in"; break;                                                   
  case ascq::session_fixation_error_writing_lead_out:
    out = "session fixation error writing lead-out"; break;                                                 
  case ascq::session_fixation_error_incomplete_track_in_session:
    out = "session fixation error - incomplete track in session"; break;                         
  case ascq::empty_or_partially_written_reserved_track:
    out = "empty or partially written reserved track"; break;                                             
  case ascq::no_more_track_reservations_allowed:
    out = "no more track reservations allowed"; break;                                                           
  case ascq::rmz_extension_is_not_allowed:
    out = "rmz extension is not allowed"; break;                                                                       
  case ascq::no_more_test_zone_extensions_are_allowed:
    out = "no more test zone extensions are allowed"; break;                                               
  case ascq::cd_control_error:
    out = "cd control error"; break;                                                                                               
  case ascq::power_calibration_area_almost_full:
    out = "power calibration area almost full"; break;                                                           
  case ascq::power_calibration_area_is_full:
    out = "power calibration area is full"; break;                                                                   
  case ascq::power_calibration_area_error:
    out = "power calibration area error"; break;                                                                       
  case ascq::program_memory_area_update_failure:
    out = "program memory area update failure"; break;                                                           
  case ascq::program_memory_area_is_full:
    out = "program memory area is full"; break;                                                                         
  case ascq::rma_pma_is_almost_full:
    out = "rma/pma is almost full"; break;                                                                                   
  case ascq::current_power_calibration_area_almost_full:
    out = "current power calibration area almost full"; break;                                           
  case ascq::current_power_calibration_area_is_full:
    out = "current power calibration area is full"; break;                                                   
  case ascq::rdz_is_full:
    out = "rdz is full"; break;                                                                                                         
  case ascq::security_error:
    out = "security error"; break;                                                                                                   
  case ascq::unable_to_decrypt_data:
    out = "unable to decrypt data"; break;                                                                                   
  case ascq::unencrypted_data_encountered_while_decrypting:
    out = "unencrypted data encountered while decrypting"; break;                                     
  case ascq::incorrect_data_encryption_key:
    out = "incorrect data encryption key"; break;                                                                     
  case ascq::cryptographic_integrity_validation_failed:
    out = "cryptographic integrity validation failed"; break;                                             
  case ascq::error_decrypting_data:
    out = "error decrypting data"; break;                                                                                     
  case ascq::unknown_signature_verification_key:
    out = "unknown signature verification key"; break;                                                           
  case ascq::encryption_parameters_not_useable:
    out = "encryption parameters not useable"; break;                                                             
  case ascq::digital_signature_validation_failure:
    out = "digital signature validation failure"; break;                                                       
  case ascq::encryption_mode_mismatch_on_read:
    out = "encryption mode mismatch on read"; break;                                                               
  case ascq::encrypted_block_not_raw_read_enabled:
    out = "encrypted block not raw read enabled"; break;                                                       
  case ascq::incorrect_encryption_parameters:
    out = "incorrect encryption parameters"; break;                                                                 
  case ascq::unable_to_decrypt_parameter_list:
    out = "unable to decrypt parameter list"; break;                                                               
  case ascq::encryption_algorithm_disabled:
    out = "encryption algorithm disabled"; break;                                                                     
  case ascq::sa_creation_parameter_value_invalid:
    out = "sa creation parameter value invalid"; break;                                                         
  case ascq::sa_creation_parameter_value_rejected:
    out = "sa creation parameter value rejected"; break;                                                       
  case ascq::invalid_sa_usage:
    out = "invalid sa usage"; break;                                                                                               
  case ascq::data_encryption_configuration_prevented:
    out = "data encryption configuration prevented"; break;                                                 
  case ascq::sa_creation_parameter_not_supported:
    out = "sa creation parameter not supported"; break;                                                         
  case ascq::authentication_failed:
    out = "authentication failed"; break;                                                                                     
  case ascq::external_data_encryption_key_manager_access_error:
    out = "external data encryption key manager access error"; break;                             
  case ascq::external_data_encryption_key_manager_error:
    out = "external data encryption key manager error"; break;                                           
  case ascq::external_data_encryption_key_not_found:
    out = "external data encryption key not found"; break;                                                   
  case ascq::external_data_encryption_request_not_authorized:
    out = "external data encryption request not authorized"; break;                                 
  case ascq::external_data_encryption_control_timeout:
    out = "external data encryption control timeout"; break;                                               
  case ascq::external_data_encryption_control_error:
    out = "external data encryption control error"; break;                                                   
  case ascq::logical_unit_access_not_authorized:
    out = "logical unit access not authorized"; break;                                                           
  case ascq::security_conflict_in_translated_device:
    out = "security conflict in translated device"; break;                                                   
  case ascq::custom_7500:
    out = "ascq 7500"; break;                                                                                                                  
  case ascq::custom_7600:
    out = "ascq 7600"; break;                                                                                                                  
  case ascq::custom_7700:
    out = "ascq 7700"; break;                                                                                                                  
  case ascq::custom_7800:
    out = "ascq 7800"; break;                                                                                                                  
  case ascq::custom_7900:
    out = "ascq 7900"; break;                                                                                                                  
  case ascq::custom_7a00:
    out = "ascq 7a00"; break;                                                                                                                  
  case ascq::custom_7b00:
    out = "ascq 7b00"; break;                                                                                                                  
  case ascq::custom_7c00:
    out = "ascq 7c00"; break;                                                                                                                  
  case ascq::custom_7d00:
    out = "ascq 7d00"; break;                                                                                                                  
  case ascq::custom_7e00:
    out = "ascq 7e00"; break;                                                                                                                  
  case ascq::custom_7f00:
    out = "ascq 7f00"; break;
  // do not handle default
  }

  if ( out.empty() )
  {
    //case ascq::diagnostic_failure_on_component_nn_80h_ffh_: 0x40nn
    //out = "diagnostic failure on component nn (80h-ffh)"; break;
    //case ascq::tagged_overlapped_commands_nn_=_task_tag_: 0x4dnn
    //out = "tagged overlapped commands (nn = task tag)"; break;
    //case ascq::decompression_exception_short_algorithm_id_of_nn: 0x70nn
    //out = "decompression exception short algorithm id of nn"; break;
    out = "unknown ascq";
  }
  out += " [" + local::to_str(v, true) + "]";

  return out;
}
