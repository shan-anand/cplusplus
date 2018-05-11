/*
LICENSE: BEGIN
===============================================================================
@author Shanmuga (Anand) Gunasekaran
@email anand.gs@gmail.com
@source https://github.com/shan-anand
@file IOBuffer.h
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

#include <Scsi/Constants.h>
#include <string.h>
#include "local.h"

using namespace Gratis::Scsi;


//////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Implementation of functions
//
std::string Gratis::Scsi::toString(const SenseKey& e)
{
  std::string out;
  const uint8_t v = static_cast<const uint8_t>(e);

  switch ( e )
  {
  case SenseKey::NO_SENSE:
    out = ""; break;
  case SenseKey::RECOVERED_ERROR:
    out = ""; break;
  case SenseKey::NOT_READY:
    out = ""; break;
  case SenseKey::MEDIUM_ERROR:
    out = ""; break;
  case SenseKey::HARDWARE_ERROR:
    out = ""; break;
  case SenseKey::ILLEGAL_REQUEST:
    out = ""; break;
  case SenseKey::UNIT_ATTENTION:
    out = ""; break;
  case SenseKey::DATA_PROTECTION:
    out = ""; break;
  case SenseKey::BLANK_CHECK:
    out = ""; break;
  case SenseKey::VENDOR_SPECIFIC:
    out = ""; break;
  case SenseKey::COPY_ABORTED:
    out = ""; break;
  case SenseKey::COMMAND_ABORTED:
    out = ""; break;
  case SenseKey::OBSOLETE_0C:
    out = ""; break;
  case SenseKey::VOLUME_OVERFLOW:
    out = ""; break;
  case SenseKey::MISCOMPARE:
    out = ""; break;
  case SenseKey::RESERVED_0F:
    out = ""; break;
  // DO NOT HANDLE default
  }

  if ( out.empty() )
  {
    out = "UNKNOWN SENSE KEY";
  }
  out += " [" + local::toString(v, true) + "]";

  return out;
}

std::string Gratis::Scsi::toString(const CodePage& e)
{
  std::string out;
  const uint8_t v = static_cast<const uint8_t>(e);

  switch ( e )
  {
  case CodePage::Supported_VPD_Pages:
    out = "Supported VPD Pages"; break;
  case CodePage::Unit_Serial_Number:
    out = "Unit Serial Number"; break;
  case CodePage::Device_Identification:
    out = "Device Identification"; break;
  case CodePage::Software_Interface_Identification:
    out = "Software Interface Identification"; break;
  case CodePage::Management_Network_Address:
    out = "Management Network Address"; break;
  case CodePage::Extended_Inquiry_Data:
    out = "Extended Inquiry Data"; break;
  case CodePage::Mode_Page_Policy:
    out = "Mode Page Policy"; break;
  case CodePage::SCSI_Ports:
    out = "SCSI Ports"; break;
  case CodePage::Block_Limits:
    out = "Block Limits"; break;
  case CodePage::Block_Device_Characteristics:
    out = "Block Device Characteristics"; break;
  case CodePage::Logical_Block_Provisioning:
    out = "Logical Block Provisioning"; break;
  case CodePage::Custom_VPD:
    out = "Custom VPD"; break;
  // DO NOT HANDLE default
  }

  if ( out.empty() )
  {
    uint8_t u = static_cast<uint8_t>(e);
    if (  u >= 0x89 && u <= 0xAF )
      out = "Reserved";
    else if ( u >= 0xB0 && u <= 0xBF )
      out = "Device specific";
    else if ( u >= 0xC0 && u <= 0xFF )
      out = "Vendor-Specific";
    else
      out = "Undefined";
  }
  out += " [" + local::toString(v, true) + "]";

  return out;
}

std::string Gratis::Scsi::toString(const Peripheral_Qualifier& e)
{
  std::string out;
  const uint8_t v = static_cast<const uint8_t>(e);

  switch ( e )
  {
  case Peripheral_Qualifier::Connected:
    out = ""; break;
  case Peripheral_Qualifier::Disconnected:
    out = ""; break;
  case Peripheral_Qualifier::Reserved_02:
    out = ""; break;
  case Peripheral_Qualifier::Not_Supported:
    out = ""; break;
  case Peripheral_Qualifier::Vendor_Specific_04:
    out = ""; break;
  case Peripheral_Qualifier::Vendor_Specific_05:
    out = ""; break;
  case Peripheral_Qualifier::Vendor_Specific_06:
    out = ""; break;
  case Peripheral_Qualifier::Vendor_Specific_07:
    out = ""; break;
  // DO NOT HANDLE default
  }

  if ( out.empty() )
  {
    out = "UNKNOWN QUALIFIER";
  }
  out += " [" + local::toString(v, true) + "]";

  return out;
}

std::string Gratis::Scsi::toString(const Peripheral_Device_Type& e)
{
  std::string out;
  const uint8_t v = static_cast<const uint8_t>(e);

  switch ( e )
  {
  case Peripheral_Device_Type::Direct_Access:
    out = ""; break;
  case Peripheral_Device_Type::Sequential_Access:
    out = ""; break;
  case Peripheral_Device_Type::Printer:
    out = ""; break;
  case Peripheral_Device_Type::Processor:
    out = ""; break;
  case Peripheral_Device_Type::Write_Once:
    out = ""; break;
  case Peripheral_Device_Type::Cd_Dvd:
    out = ""; break;
  case Peripheral_Device_Type::Scanner:
    out = ""; break;
  case Peripheral_Device_Type::Optical_Memory:
    out = ""; break;
  case Peripheral_Device_Type::Medium_Changer:
    out = ""; break;
  case Peripheral_Device_Type::Communications:
    out = ""; break;
  case Peripheral_Device_Type::Obsolete_0A:
    out = ""; break;
  case Peripheral_Device_Type::Obsolete_0B:
    out = ""; break;
  case Peripheral_Device_Type::Storate_Array_Controller:
    out = ""; break;
  case Peripheral_Device_Type::Enclosure_Services:
    out = ""; break;
  case Peripheral_Device_Type::Simplified_Direct_Access:
    out = ""; break;
  case Peripheral_Device_Type::Optical_Card_RW:
    out = ""; break;
  case Peripheral_Device_Type::Bridge_Controller:
    out = ""; break;
  case Peripheral_Device_Type::Object_Based_Storage:
    out = ""; break;
  case Peripheral_Device_Type::Automation:
    out = ""; break;
  case Peripheral_Device_Type::Reserved_13:
  case Peripheral_Device_Type::Reserved_14:
  case Peripheral_Device_Type::Reserved_15:
  case Peripheral_Device_Type::Reserved_16:
  case Peripheral_Device_Type::Reserved_17:
  case Peripheral_Device_Type::Reserved_18:
  case Peripheral_Device_Type::Reserved_19:
  case Peripheral_Device_Type::Reserved_1A:
  case Peripheral_Device_Type::Reserved_1B:
  case Peripheral_Device_Type::Reserved_1C:
  case Peripheral_Device_Type::Reserved_1D:
    out = ""; break;
  case Peripheral_Device_Type::Well_Known_Lun:
    out = ""; break;
  case Peripheral_Device_Type::Unknown:
    out = ""; break;
  // DO NOT HANDLE default
  }

  if ( out.empty() )
  {
    out = "UNKNOWN DEVICE TYPE";
  }
  out += " [" + local::toString(v, true) + "]";

  return out;
}

std::string Gratis::Scsi::toString(const ProtocolId& e)
{
  std::string out;
  const uint8_t v = static_cast<const uint8_t>(e);

  switch ( e )
  {
  case ProtocolId::Fiber_Channel:
    out = ""; break;
  case ProtocolId::Parallel_Scsi:
    out = ""; break;
  case ProtocolId::SSA:
    out = ""; break;
  case ProtocolId::IEEE_1394:
    out = ""; break;
  case ProtocolId::RDMA:
    out = ""; break;
  case ProtocolId::IScsi:
    out = ""; break;
  case ProtocolId::SAS:
    out = ""; break;
  case ProtocolId::ADT:
    out = ""; break;
  case ProtocolId::ATA:
    out = ""; break;
  case ProtocolId::Reserved_9:
    out = ""; break;
  // DO NOT HANDLE default
  }

  if ( out.empty() )
  {
    out = "UNKNOWN PROTOCOL ID";
  }
  out += " [" + local::toString(v, true) + "]";

  return out;
}

std::string Gratis::Scsi::toString(const CodeSet& e)
{
  std::string out;
  const uint8_t v = static_cast<const uint8_t>(e);

  switch ( e )
  {
  case CodeSet::Binary:
    out = ""; break;
  case CodeSet::ASCII:
    out = ""; break;
  case CodeSet::UTF8:
    out = ""; break;
  // DO NOT HANDLE default
  }

  if ( out.empty() )
  {
    out = "UNKNOWN CODESET";
  }
  out += " [" + local::toString(v, true) + "]";

  return out;
}

std::string Gratis::Scsi::toString(const Association& e)
{
  std::string out;
  const uint8_t v = static_cast<const uint8_t>(e);

  switch ( e )
  {
  case Association::Logical_Unit:
    out = ""; break;
  case Association::Target_Port:
    out = ""; break;
  case Association::Target_Device:
    out = ""; break;
  // DO NOT HANDLE default
  }

  if ( out.empty() )
  {
    out = "UNKNOWN ASSOCIATION";
  }
  out += " [" + local::toString(v, true) + "]";

  return out;
}

std::string Gratis::Scsi::toString(const IdentifierType& e)
{
  std::string out;
  const uint8_t v = static_cast<const uint8_t>(e);

  switch ( e )
  {
  case IdentifierType::Vendor_Specific:
    out = ""; break;
  case IdentifierType::T10_Vendor_Id:
    out = ""; break;
  case IdentifierType::EUI_64:
    out = ""; break;
  case IdentifierType::NAA:
    out = ""; break;
  case IdentifierType::Relative_Target_Port:
    out = ""; break;
  case IdentifierType::Target_Port_Group:
    out = ""; break;
  case IdentifierType::Logical_Unit_Group:
    out = ""; break;
  case IdentifierType::MD5_Logical_Unit_Id:
    out = ""; break;
  case IdentifierType::Scsi_Name_String:
    out = ""; break;
  // DO NOT HANDLE default
  }

  if ( out.empty() )
  {
    out = "UNKNOWN IDENTIFIER TYPE";
  }
  out += " [" + local::toString(v, true) + "]";

  return out;
}

std::string Gratis::Scsi::toString(const ASCQ& e)
{
  std::string out;
  const uint16_t v = static_cast<const uint16_t>(e);

  switch ( e )
  {
  case ASCQ::NO_ADDITIONAL_SENSE_INFORMATION:
    out = "NO ADDITIONAL SENSE INFORMATION"; break;
  case ASCQ::FILEMARK_DETECTED:
    out = "FILEMARK DETECTED"; break;
  case ASCQ::END_OF_PARTITION_MEDIUM_DETECTED:
    out = "END-OF-PARTITION/MEDIUM DETECTED"; break;
  case ASCQ::SETMARK_DETECTED:
    out = "SETMARK DETECTED"; break;
  case ASCQ::BEGINNING_OF_PARTITION_MEDIUM_DETECTED:
    out = "BEGINNING-OF-PARTITION/MEDIUM DETECTED"; break;
  case ASCQ::END_OF_DATA_DETECTED:
    out = "END-OF-DATA DETECTED"; break;
  case ASCQ::I_O_PROCESS_TERMINATED:
    out = "I/O PROCESS TERMINATED"; break;
  case ASCQ::PROGRAMMABLE_EARLY_WARNING_DETECTED:
    out = "PROGRAMMABLE EARLY WARNING DETECTED"; break;
  case ASCQ::AUDIO_PLAY_OPERATION_IN_PROGRESS:
    out = "AUDIO PLAY OPERATION IN PROGRESS"; break;
  case ASCQ::AUDIO_PLAY_OPERATION_PAUSED:
    out = "AUDIO PLAY OPERATION PAUSED"; break;
  case ASCQ::AUDIO_PLAY_OPERATION_SUCCESSFULLY_COMPLETED:
    out = "AUDIO PLAY OPERATION SUCCESSFULLY COMPLETED"; break;
  case ASCQ::AUDIO_PLAY_OPERATION_STOPPED_DUE_TO_ERROR:
    out = "AUDIO PLAY OPERATION STOPPED DUE TO ERROR"; break;
  case ASCQ::NO_CURRENT_AUDIO_STATUS_TO_RETURN:
    out = "NO CURRENT AUDIO STATUS TO RETURN"; break;
  case ASCQ::OPERATION_IN_PROGRESS:
    out = "OPERATION IN PROGRESS"; break;
  case ASCQ::CLEANING_REQUESTED:
    out = "CLEANING REQUESTED"; break;
  case ASCQ::ERASE_OPERATION_IN_PROGRESS:
    out = "ERASE OPERATION IN PROGRESS"; break;
  case ASCQ::LOCATE_OPERATION_IN_PROGRESS:
    out = "LOCATE OPERATION IN PROGRESS"; break;
  case ASCQ::REWIND_OPERATION_IN_PROGRESS:
    out = "REWIND OPERATION IN PROGRESS"; break;
  case ASCQ::SET_CAPACITY_OPERATION_IN_PROGRESS:
    out = "SET CAPACITY OPERATION IN PROGRESS"; break;
  case ASCQ::VERIFY_OPERATION_IN_PROGRESS:
    out = "VERIFY OPERATION IN PROGRESS"; break;
  case ASCQ::ATA_PASS_THROUGH_INFORMATION_AVAILABLE:
    out = "ATA PASS THROUGH INFORMATION AVAILABLE"; break;
  case ASCQ::CONFLICTING_SA_CREATION_REQUEST:
    out = "CONFLICTING SA CREATION REQUEST"; break;
  case ASCQ::LOGICAL_UNIT_TRANSITIONING_TO_ANOTHER_POWER_CONDITION:
    out = "LOGICAL UNIT TRANSITIONING TO ANOTHER POWER CONDITION"; break;
  case ASCQ::EXTENDED_COPY_INFORMATION_AVAILABLE:
    out = "EXTENDED COPY INFORMATION AVAILABLE"; break;
  case ASCQ::ATOMIC_COMMAND_ABORTED_DUE_TO_ACA:
    out = "ATOMIC COMMAND ABORTED DUE TO ACA"; break;
  case ASCQ::NO_INDEX_SECTOR_SIGNAL:
    out = "NO INDEX/SECTOR SIGNAL"; break;
  case ASCQ::NO_SEEK_COMPLETE:
    out = "NO SEEK COMPLETE"; break;
  case ASCQ::PERIPHERAL_DEVICE_WRITE_FAULT:
    out = "PERIPHERAL DEVICE WRITE FAULT"; break;
  case ASCQ::NO_WRITE_CURRENT:
    out = "NO WRITE CURRENT"; break;
  case ASCQ::EXCESSIVE_WRITE_ERRORS:
    out = "EXCESSIVE WRITE ERRORS"; break;
  case ASCQ::LOGICAL_UNIT_NOT_READY_CAUSE_NOT_REPORTABLE:
    out = "LOGICAL UNIT NOT READY:CAUSE NOT REPORTABLE"; break;
  case ASCQ::LOGICAL_UNIT_IS_IN_PROCESS_OF_BECOMING_READY:
    out = "LOGICAL UNIT IS IN PROCESS OF BECOMING READY"; break;
  case ASCQ::LOGICAL_UNIT_NOT_READY_INITIALIZING_COMMAND_REQUIRED:
    out = "LOGICAL UNIT NOT READY:INITIALIZING COMMAND REQUIRED"; break;
  case ASCQ::LOGICAL_UNIT_NOT_READY_MANUAL_INTERVENTION_REQUIRED:
    out = "LOGICAL UNIT NOT READY:MANUAL INTERVENTION REQUIRED"; break;
  case ASCQ::LOGICAL_UNIT_NOT_READY_FORMAT_IN_PROGRESS:
    out = "LOGICAL UNIT NOT READY:FORMAT IN PROGRESS"; break;
  case ASCQ::LOGICAL_UNIT_NOT_READY_REBUILD_IN_PROGRESS:
    out = "LOGICAL UNIT NOT READY:REBUILD IN PROGRESS"; break;
  case ASCQ::LOGICAL_UNIT_NOT_READY_RECALCULATION_IN_PROGRESS:
    out = "LOGICAL UNIT NOT READY:RECALCULATION IN PROGRESS"; break;
  case ASCQ::LOGICAL_UNIT_NOT_READY_OPERATION_IN_PROGRESS:
    out = "LOGICAL UNIT NOT READY:OPERATION IN PROGRESS"; break;
  case ASCQ::LOGICAL_UNIT_NOT_READY_LONG_WRITE_IN_PROGRESS:
    out = "LOGICAL UNIT NOT READY:LONG WRITE IN PROGRESS"; break;
  case ASCQ::LOGICAL_UNIT_NOT_READY_SELF_TEST_IN_PROGRESS:
    out = "LOGICAL UNIT NOT READY:SELF-TEST IN PROGRESS"; break;
  case ASCQ::LOGICAL_UNIT_NOT_ACCESSIBLE_ASYMMETRIC_ACCESS_STATE_TRANSITION:
    out = "LOGICAL UNIT NOT ACCESSIBLE:ASYMMETRIC ACCESS STATE TRANSITION"; break;
  case ASCQ::LOGICAL_UNIT_NOT_ACCESSIBLE_TARGET_PORT_IN_STANDBY_STATE:
    out = "LOGICAL UNIT NOT ACCESSIBLE:TARGET PORT IN STANDBY STATE"; break;
  case ASCQ::LOGICAL_UNIT_NOT_ACCESSIBLE_TARGET_PORT_IN_UNAVAILABLE_STATE:
    out = "LOGICAL UNIT NOT ACCESSIBLE:TARGET PORT IN UNAVAILABLE STATE"; break;
  case ASCQ::LOGICAL_UNIT_NOT_READY_STRUCTURE_CHECK_REQUIRED:
    out = "LOGICAL UNIT NOT READY:STRUCTURE CHECK REQUIRED"; break;
  case ASCQ::LOGICAL_UNIT_NOT_READY_SECURITY_SESSION_IN_PROGRESS:
    out = "LOGICAL UNIT NOT READY:SECURITY SESSION IN PROGRESS"; break;
  case ASCQ::LOGICAL_UNIT_NOT_READY_AUXILIARY_MEMORY_NOT_ACCESSIBLE:
    out = "LOGICAL UNIT NOT READY:AUXILIARY MEMORY NOT ACCESSIBLE"; break;
  case ASCQ::LOGICAL_UNIT_NOT_READY_NOTIFY_ENABLE_SPINUP_REQUIRED:
    out = "LOGICAL UNIT NOT READY:NOTIFY (ENABLE SPINUP) REQUIRED"; break;
  case ASCQ::LOGICAL_UNIT_NOT_READY_OFFLINE:
    out = "LOGICAL UNIT NOT READY:OFFLINE"; break;
  case ASCQ::LOGICAL_UNIT_NOT_READY_SA_CREATION_IN_PROGRESS:
    out = "LOGICAL UNIT NOT READY:SA CREATION IN PROGRESS"; break;
  case ASCQ::LOGICAL_UNIT_NOT_READY_SPACE_ALLOCATION_IN_PROGRESS:
    out = "LOGICAL UNIT NOT READY:SPACE ALLOCATION IN PROGRESS"; break;
  case ASCQ::LOGICAL_UNIT_NOT_READY_ROBOTICS_DISABLED:
    out = "LOGICAL UNIT NOT READY:ROBOTICS DISABLED"; break;
  case ASCQ::LOGICAL_UNIT_NOT_READY_CONFIGURATION_REQUIRED:
    out = "LOGICAL UNIT NOT READY:CONFIGURATION REQUIRED"; break;
  case ASCQ::LOGICAL_UNIT_NOT_READY_CALIBRATION_REQUIRED:
    out = "LOGICAL UNIT NOT READY:CALIBRATION REQUIRED"; break;
  case ASCQ::LOGICAL_UNIT_NOT_READY_A_DOOR_IS_OPEN:
    out = "LOGICAL UNIT NOT READY:A DOOR IS OPEN"; break;
  case ASCQ::LOGICAL_UNIT_NOT_READY_OPERATING_IN_SEQUENTIAL_MODE:
    out = "LOGICAL UNIT NOT READY:OPERATING IN SEQUENTIAL MODE"; break;
  case ASCQ::LOGICAL_UNIT_NOT_READY_START_STOP_UNIT_COMMAND_IN_PROGRESS:
    out = "LOGICAL UNIT NOT READY:START STOP UNIT COMMAND IN PROGRESS"; break;
  case ASCQ::LOGICAL_UNIT_NOT_READY_SANITIZE_IN_PROGRESS:
    out = "LOGICAL UNIT NOT READY:SANITIZE IN PROGRESS"; break;
  case ASCQ::LOGICAL_UNIT_NOT_READY_ADDITIONAL_POWER_USE_NOT_YET_GRANTED:
    out = "LOGICAL UNIT NOT READY:ADDITIONAL POWER USE NOT YET GRANTED"; break;
  case ASCQ::LOGICAL_UNIT_NOT_READY_CONFIGURATION_IN_PROGRESS:
    out = "LOGICAL UNIT NOT READY:CONFIGURATION IN PROGRESS"; break;
  case ASCQ::LOGICAL_UNIT_NOT_READY_MICROCODE_ACTIVATION_REQUIRED:
    out = "LOGICAL UNIT NOT READY:MICROCODE ACTIVATION REQUIRED"; break;
  case ASCQ::LOGICAL_UNIT_NOT_READY_MICROCODE_DOWNLOAD_REQUIRED:
    out = "LOGICAL UNIT NOT READY:MICROCODE DOWNLOAD REQUIRED"; break;
  case ASCQ::LOGICAL_UNIT_NOT_READY_LOGICAL_UNIT_RESET_REQUIRED:
    out = "LOGICAL UNIT NOT READY:LOGICAL UNIT RESET REQUIRED"; break;
  case ASCQ::LOGICAL_UNIT_NOT_READY_HARD_RESET_REQUIRED:
    out = "LOGICAL UNIT NOT READY:HARD RESET REQUIRED"; break;
  case ASCQ::LOGICAL_UNIT_NOT_READY_POWER_CYCLE_REQUIRED:
    out = "LOGICAL UNIT NOT READY:POWER CYCLE REQUIRED"; break;
  case ASCQ::LOGICAL_UNIT_NOT_READY_AFFILIATION_REQUIRED:
    out = "LOGICAL UNIT NOT READY:AFFILIATION REQUIRED"; break;
  case ASCQ::LOGICAL_UNIT_DOES_NOT_RESPOND_TO_SELECTION:
    out = "LOGICAL UNIT DOES NOT RESPOND TO SELECTION"; break;
  case ASCQ::NO_REFERENCE_POSITION_FOUND:
    out = "NO REFERENCE POSITION FOUND"; break;
  case ASCQ::MULTIPLE_PERIPHERAL_DEVICES_SELECTED:
    out = "MULTIPLE PERIPHERAL DEVICES SELECTED"; break;
  case ASCQ::LOGICAL_UNIT_COMMUNICATION_FAILURE:
    out = "LOGICAL UNIT COMMUNICATION FAILURE"; break;
  case ASCQ::LOGICAL_UNIT_COMMUNICATION_TIME_OUT:
    out = "LOGICAL UNIT COMMUNICATION TIME-OUT"; break;
  case ASCQ::LOGICAL_UNIT_COMMUNICATION_PARITY_ERROR:
    out = "LOGICAL UNIT COMMUNICATION PARITY ERROR"; break;
  case ASCQ::LOGICAL_UNIT_COMMUNICATION_CRC_ERROR_ULTRA_DMA_32_:
    out = "LOGICAL UNIT COMMUNICATION CRC ERROR (ULTRA-DMA/32)"; break;
  case ASCQ::UNREACHABLE_COPY_TARGET:
    out = "UNREACHABLE COPY TARGET"; break;
  case ASCQ::TRACK_FOLLOWING_ERROR:
    out = "TRACK FOLLOWING ERROR"; break;
  case ASCQ::TRACKING_SERVO_FAILURE:
    out = "TRACKING SERVO FAILURE"; break;
  case ASCQ::FOCUS_SERVO_FAILURE:
    out = "FOCUS SERVO FAILURE"; break;
  case ASCQ::SPINDLE_SERVO_FAILURE:
    out = "SPINDLE SERVO FAILURE"; break;
  case ASCQ::HEAD_SELECT_FAULT:
    out = "HEAD SELECT FAULT"; break;
  case ASCQ::VIBRATION_INDUCED_TRACKING_ERROR:
    out = "VIBRATION INDUCED TRACKING ERROR"; break;
  case ASCQ::ERROR_LOG_OVERFLOW:
    out = "ERROR LOG OVERFLOW"; break;
  case ASCQ::WARNING:
    out = "WARNING"; break;
  case ASCQ::WARNING_SPECIFIED_TEMPERATURE_EXCEEDED:
    out = "WARNING - SPECIFIED TEMPERATURE EXCEEDED"; break;
  case ASCQ::WARNING_ENCLOSURE_DEGRADED:
    out = "WARNING - ENCLOSURE DEGRADED"; break;
  case ASCQ::WARNING_BACKGROUND_SELF_TEST_FAILED:
    out = "WARNING - BACKGROUND SELF-TEST FAILED"; break;
  case ASCQ::WARNING_BACKGROUND_PRE_SCAN_DETECTED_MEDIUM_ERROR:
    out = "WARNING - BACKGROUND PRE-SCAN DETECTED MEDIUM ERROR"; break;
  case ASCQ::WARNING_BACKGROUND_MEDIUM_SCAN_DETECTED_MEDIUM_ERROR:
    out = "WARNING - BACKGROUND MEDIUM SCAN DETECTED MEDIUM ERROR"; break;
  case ASCQ::WARNING_NON_VOLATILE_CACHE_NOW_VOLATILE:
    out = "WARNING - NON-VOLATILE CACHE NOW VOLATILE"; break;
  case ASCQ::WARNING_DEGRADED_POWER_TO_NON_VOLATILE_CACHE:
    out = "WARNING - DEGRADED POWER TO NON-VOLATILE CACHE"; break;
  case ASCQ::WARNING_POWER_LOSS_EXPECTED:
    out = "WARNING - POWER LOSS EXPECTED"; break;
  case ASCQ::WARNING_DEVICE_STATISTICS_NOTIFICATION_ACTIVE:
    out = "WARNING - DEVICE STATISTICS NOTIFICATION ACTIVE"; break;
  case ASCQ::WARNING_HIGH_CRITICAL_TEMPERATURE_LIMIT_EXCEEDED:
    out = "WARNING - HIGH CRITICAL TEMPERATURE LIMIT EXCEEDED"; break;
  case ASCQ::WARNING_LOW_CRITICAL_TEMPERATURE_LIMIT_EXCEEDED:
    out = "WARNING - LOW CRITICAL TEMPERATURE LIMIT EXCEEDED"; break;
  case ASCQ::WARNING_HIGH_OPERATING_TEMPERATURE_LIMIT_EXCEEDED:
    out = "WARNING - HIGH OPERATING TEMPERATURE LIMIT EXCEEDED"; break;
  case ASCQ::WARNING_LOW_OPERATING_TEMPERATURE_LIMIT_EXCEEDED:
    out = "WARNING - LOW OPERATING TEMPERATURE LIMIT EXCEEDED"; break;
  case ASCQ::WARNING_HIGH_CRITICAL_HUMIDITY_LIMIT_EXCEEDED:
    out = "WARNING - HIGH CRITICAL HUMIDITY LIMIT EXCEEDED"; break;
  case ASCQ::WARNING_LOW_CRITICAL_HUMIDITY_LIMIT_EXCEEDED:
    out = "WARNING - LOW CRITICAL HUMIDITY LIMIT EXCEEDED"; break;
  case ASCQ::WARNING_HIGH_OPERATING_HUMIDITY_LIMIT_EXCEEDED:
    out = "WARNING - HIGH OPERATING HUMIDITY LIMIT EXCEEDED"; break;
  case ASCQ::WARNING_LOW_OPERATING_HUMIDITY_LIMIT_EXCEEDED:
    out = "WARNING - LOW OPERATING HUMIDITY LIMIT EXCEEDED"; break;
  case ASCQ::WARNING_MICROCODE_SECURITY_AT_RISK:
    out = "WARNING - MICROCODE SECURITY AT RISK"; break;
  case ASCQ::WARNING_MICROCODE_DIGITAL_SIGNATURE_VALIDATION_FAILURE:
    out = "WARNING - MICROCODE DIGITAL SIGNATURE VALIDATION FAILURE"; break;
  case ASCQ::WRITE_ERROR:
    out = "WRITE ERROR"; break;
  case ASCQ::WRITE_ERROR_RECOVERED_WITH_AUTO_REALLOCATION:
    out = "WRITE ERROR - RECOVERED WITH AUTO REALLOCATION"; break;
  case ASCQ::WRITE_ERROR_AUTO_REALLOCATION_FAILED:
    out = "WRITE ERROR - AUTO REALLOCATION FAILED"; break;
  case ASCQ::WRITE_ERROR_RECOMMEND_REASSIGNMENT:
    out = "WRITE ERROR - RECOMMEND REASSIGNMENT"; break;
  case ASCQ::COMPRESSION_CHECK_MISCOMPARE_ERROR:
    out = "COMPRESSION CHECK MISCOMPARE ERROR"; break;
  case ASCQ::DATA_EXPANSION_OCCURRED_DURING_COMPRESSION:
    out = "DATA EXPANSION OCCURRED DURING COMPRESSION"; break;
  case ASCQ::BLOCK_NOT_COMPRESSIBLE:
    out = "BLOCK NOT COMPRESSIBLE"; break;
  case ASCQ::WRITE_ERROR_RECOVERY_NEEDED:
    out = "WRITE ERROR - RECOVERY NEEDED"; break;
  case ASCQ::WRITE_ERROR_RECOVERY_FAILED:
    out = "WRITE ERROR - RECOVERY FAILED"; break;
  case ASCQ::WRITE_ERROR_LOSS_OF_STREAMING:
    out = "WRITE ERROR - LOSS OF STREAMING"; break;
  case ASCQ::WRITE_ERROR_PADDING_BLOCKS_ADDED:
    out = "WRITE ERROR - PADDING BLOCKS ADDED"; break;
  case ASCQ::AUXILIARY_MEMORY_WRITE_ERROR:
    out = "AUXILIARY MEMORY WRITE ERROR"; break;
  case ASCQ::WRITE_ERROR_UNEXPECTED_UNSOLICITED_DATA:
    out = "WRITE ERROR - UNEXPECTED UNSOLICITED DATA"; break;
  case ASCQ::WRITE_ERROR_NOT_ENOUGH_UNSOLICITED_DATA:
    out = "WRITE ERROR - NOT ENOUGH UNSOLICITED DATA"; break;
  case ASCQ::MULTIPLE_WRITE_ERRORS:
    out = "MULTIPLE WRITE ERRORS"; break;
  case ASCQ::DEFECTS_IN_ERROR_WINDOW:
    out = "DEFECTS IN ERROR WINDOW"; break;
  case ASCQ::INCOMPLETE_MULTIPLE_ATOMIC_WRITE_OPERATIONS:
    out = "INCOMPLETE MULTIPLE ATOMIC WRITE OPERATIONS"; break;
  case ASCQ::WRITE_ERROR_RECOVERY_SCAN_NEEDED:
    out = "WRITE ERROR - RECOVERY SCAN NEEDED"; break;
  case ASCQ::WRITE_ERROR_INSUFFICIENT_ZONE_RESOURCES:
    out = "WRITE ERROR - INSUFFICIENT ZONE RESOURCES"; break;
  case ASCQ::ERROR_DETECTED_BY_THIRD_PARTY_TEMPORARY_INITIATOR:
    out = "ERROR DETECTED BY THIRD PARTY TEMPORARY INITIATOR"; break;
  case ASCQ::THIRD_PARTY_DEVICE_FAILURE:
    out = "THIRD PARTY DEVICE FAILURE"; break;
  case ASCQ::COPY_TARGET_DEVICE_NOT_REACHABLE:
    out = "COPY TARGET DEVICE NOT REACHABLE"; break;
  case ASCQ::INCORRECT_COPY_TARGET_DEVICE_TYPE:
    out = "INCORRECT COPY TARGET DEVICE TYPE"; break;
  case ASCQ::COPY_TARGET_DEVICE_DATA_UNDERRUN:
    out = "COPY TARGET DEVICE DATA UNDERRUN"; break;
  case ASCQ::COPY_TARGET_DEVICE_DATA_OVERRUN:
    out = "COPY TARGET DEVICE DATA OVERRUN"; break;
  case ASCQ::INVALID_INFORMATION_UNIT:
    out = "INVALID INFORMATION UNIT"; break;
  case ASCQ::INFORMATION_UNIT_TOO_SHORT:
    out = "INFORMATION UNIT TOO SHORT"; break;
  case ASCQ::INFORMATION_UNIT_TOO_LONG:
    out = "INFORMATION UNIT TOO LONG"; break;
  case ASCQ::INVALID_FIELD_IN_COMMAND_INFORMATION_UNIT:
    out = "INVALID FIELD IN COMMAND INFORMATION UNIT"; break;
  case ASCQ::CUSTOM_0F00:
    out = "ASCQ 0F00"; break;
  case ASCQ::ID_CRC_OR_ECC_ERROR:
    out = "ID CRC OR ECC ERROR"; break;
  case ASCQ::LOGICAL_BLOCK_GUARD_CHECK_FAILED:
    out = "LOGICAL BLOCK GUARD CHECK FAILED"; break;
  case ASCQ::LOGICAL_BLOCK_APPLICATION_TAG_CHECK_FAILED:
    out = "LOGICAL BLOCK APPLICATION TAG CHECK FAILED"; break;
  case ASCQ::LOGICAL_BLOCK_REFERENCE_TAG_CHECK_FAILED:
    out = "LOGICAL BLOCK REFERENCE TAG CHECK FAILED"; break;
  case ASCQ::LOGICAL_BLOCK_PROTECTION_ERROR_ON_RECOVER_BUFFERED_DATA:
    out = "LOGICAL BLOCK PROTECTION ERROR ON RECOVER BUFFERED DATA"; break;
  case ASCQ::LOGICAL_BLOCK_PROTECTION_METHOD_ERROR:
    out = "LOGICAL BLOCK PROTECTION METHOD ERROR"; break;
  case ASCQ::UNRECOVERED_READ_ERROR:
    out = "UNRECOVERED READ ERROR"; break;
  case ASCQ::READ_RETRIES_EXHAUSTED:
    out = "READ RETRIES EXHAUSTED"; break;
  case ASCQ::ERROR_TOO_LONG_TO_CORRECT:
    out = "ERROR TOO LONG TO CORRECT"; break;
  case ASCQ::MULTIPLE_READ_ERRORS:
    out = "MULTIPLE READ ERRORS"; break;
  case ASCQ::UNRECOVERED_READ_ERROR_AUTO_REALLOCATE_FAILED:
    out = "UNRECOVERED READ ERROR - AUTO REALLOCATE FAILED"; break;
  case ASCQ::L_EC_UNCORRECTABLE_ERROR:
    out = "L-EC UNCORRECTABLE ERROR"; break;
  case ASCQ::CIRC_UNRECOVERED_ERROR:
    out = "CIRC UNRECOVERED ERROR"; break;
  case ASCQ::DATA_RE_SYNCHRONIZATION_ERROR:
    out = "DATA RE-SYNCHRONIZATION ERROR"; break;
  case ASCQ::INCOMPLETE_BLOCK_READ:
    out = "INCOMPLETE BLOCK READ"; break;
  case ASCQ::NO_GAP_FOUND:
    out = "NO GAP FOUND"; break;
  case ASCQ::MISCORRECTED_ERROR:
    out = "MISCORRECTED ERROR"; break;
  case ASCQ::UNRECOVERED_READ_ERROR_RECOMMEND_REASSIGNMENT:
    out = "UNRECOVERED READ ERROR - RECOMMEND REASSIGNMENT"; break;
  case ASCQ::UNRECOVERED_READ_ERROR_RECOMMEND_REWRITE_THE_DATA:
    out = "UNRECOVERED READ ERROR - RECOMMEND REWRITE THE DATA"; break;
  case ASCQ::DE_COMPRESSION_CRC_ERROR:
    out = "DE-COMPRESSION CRC ERROR"; break;
  case ASCQ::CANNOT_DECOMPRESS_USING_DECLARED_ALGORITHM:
    out = "CANNOT DECOMPRESS USING DECLARED ALGORITHM"; break;
  case ASCQ::ERROR_READING_UPC_EAN_NUMBER:
    out = "ERROR READING UPC/EAN NUMBER"; break;
  case ASCQ::ERROR_READING_ISRC_NUMBER:
    out = "ERROR READING ISRC NUMBER"; break;
  case ASCQ::READ_ERROR_LOSS_OF_STREAMING:
    out = "READ ERROR - LOSS OF STREAMING"; break;
  case ASCQ::AUXILIARY_MEMORY_READ_ERROR:
    out = "AUXILIARY MEMORY READ ERROR"; break;
  case ASCQ::READ_ERROR_FAILED_RETRANSMISSION_REQUEST:
    out = "READ ERROR - FAILED RETRANSMISSION REQUEST"; break;
  case ASCQ::READ_ERROR_LBA_MARKED_BAD_BY_APPLICATION_CLIENT:
    out = "READ ERROR - LBA MARKED BAD BY APPLICATION CLIENT"; break;
  case ASCQ::WRITE_AFTER_SANITIZE_REQUIRED:
    out = "WRITE AFTER SANITIZE REQUIRED"; break;
  case ASCQ::ADDRESS_MARK_NOT_FOUND_FOR_ID_FIELD:
    out = "ADDRESS MARK NOT FOUND FOR ID FIELD"; break;
  case ASCQ::ADDRESS_MARK_NOT_FOUND_FOR_DATA_FIELD:
    out = "ADDRESS MARK NOT FOUND FOR DATA FIELD"; break;
  case ASCQ::RECORDED_ENTITY_NOT_FOUND:
    out = "RECORDED ENTITY NOT FOUND"; break;
  case ASCQ::RECORD_NOT_FOUND:
    out = "RECORD NOT FOUND"; break;
  case ASCQ::FILEMARK_OR_SETMARK_NOT_FOUND:
    out = "FILEMARK OR SETMARK NOT FOUND"; break;
  case ASCQ::END_OF_DATA_NOT_FOUND:
    out = "END-OF-DATA NOT FOUND"; break;
  case ASCQ::BLOCK_SEQUENCE_ERROR:
    out = "BLOCK SEQUENCE ERROR"; break;
  case ASCQ::RECORD_NOT_FOUND_RECOMMEND_REASSIGNMENT:
    out = "RECORD NOT FOUND - RECOMMEND REASSIGNMENT"; break;
  case ASCQ::RECORD_NOT_FOUND_DATA_AUTO_REALLOCATED:
    out = "RECORD NOT FOUND - DATA AUTO-REALLOCATED"; break;
  case ASCQ::LOCATE_OPERATION_FAILURE:
    out = "LOCATE OPERATION FAILURE"; break;
  case ASCQ::RANDOM_POSITIONING_ERROR:
    out = "RANDOM POSITIONING ERROR"; break;
  case ASCQ::MECHANICAL_POSITIONING_ERROR:
    out = "MECHANICAL POSITIONING ERROR"; break;
  case ASCQ::POSITIONING_ERROR_DETECTED_BY_READ_OF_MEDIUM:
    out = "POSITIONING ERROR DETECTED BY READ OF MEDIUM"; break;
  case ASCQ::DATA_SYNCHRONIZATION_MARK_ERROR:
    out = "DATA SYNCHRONIZATION MARK ERROR"; break;
  case ASCQ::DATA_SYNC_ERROR_DATA_REWRITTEN:
    out = "DATA SYNC ERROR - DATA REWRITTEN"; break;
  case ASCQ::DATA_SYNC_ERROR_RECOMMEND_REWRITE:
    out = "DATA SYNC ERROR - RECOMMEND REWRITE"; break;
  case ASCQ::DATA_SYNC_ERROR_DATA_AUTO_REALLOCATED:
    out = "DATA SYNC ERROR - DATA AUTO-REALLOCATED"; break;
  case ASCQ::DATA_SYNC_ERROR_RECOMMEND_REASSIGNMENT:
    out = "DATA SYNC ERROR - RECOMMEND REASSIGNMENT"; break;
  case ASCQ::RECOVERED_DATA_WITH_NO_ERROR_CORRECTION_APPLIED:
    out = "RECOVERED DATA WITH NO ERROR CORRECTION APPLIED"; break;
  case ASCQ::RECOVERED_DATA_WITH_RETRIES:
    out = "RECOVERED DATA WITH RETRIES"; break;
  case ASCQ::RECOVERED_DATA_WITH_POSITIVE_HEAD_OFFSET:
    out = "RECOVERED DATA WITH POSITIVE HEAD OFFSET"; break;
  case ASCQ::RECOVERED_DATA_WITH_NEGATIVE_HEAD_OFFSET:
    out = "RECOVERED DATA WITH NEGATIVE HEAD OFFSET"; break;
  case ASCQ::RECOVERED_DATA_WITH_RETRIES_AND_OR_CIRC_APPLIED:
    out = "RECOVERED DATA WITH RETRIES AND/OR CIRC APPLIED"; break;
  case ASCQ::RECOVERED_DATA_USING_PREVIOUS_SECTOR_ID:
    out = "RECOVERED DATA USING PREVIOUS SECTOR ID"; break;
  case ASCQ::RECOVERED_DATA_WITHOUT_ECC_DATA_AUTO_REALLOCATED:
    out = "RECOVERED DATA WITHOUT ECC - DATA AUTO-REALLOCATED"; break;
  case ASCQ::RECOVERED_DATA_WITHOUT_ECC_RECOMMEND_REASSIGNMENT:
    out = "RECOVERED DATA WITHOUT ECC - RECOMMEND REASSIGNMENT"; break;
  case ASCQ::RECOVERED_DATA_WITHOUT_ECC_RECOMMEND_REWRITE:
    out = "RECOVERED DATA WITHOUT ECC - RECOMMEND REWRITE"; break;
  case ASCQ::RECOVERED_DATA_WITHOUT_ECC_DATA_REWRITTEN:
    out = "RECOVERED DATA WITHOUT ECC - DATA REWRITTEN"; break;
  case ASCQ::RECOVERED_DATA_WITH_ERROR_CORRECTION_APPLIED:
    out = "RECOVERED DATA WITH ERROR CORRECTION APPLIED"; break;
  case ASCQ::RECOVERED_DATA_WITH_ERROR_CORR_AND_RETRIES_APPLIED:
    out = "RECOVERED DATA WITH ERROR CORR. & RETRIES APPLIED"; break;
  case ASCQ::RECOVERED_DATA_DATA_AUTO_REALLOCATED:
    out = "RECOVERED DATA - DATA AUTO-REALLOCATED"; break;
  case ASCQ::RECOVERED_DATA_WITH_CIRC:
    out = "RECOVERED DATA WITH CIRC"; break;
  case ASCQ::RECOVERED_DATA_WITH_L_EC:
    out = "RECOVERED DATA WITH L-EC"; break;
  case ASCQ::RECOVERED_DATA_RECOMMEND_REASSIGNMENT:
    out = "RECOVERED DATA - RECOMMEND REASSIGNMENT"; break;
  case ASCQ::RECOVERED_DATA_RECOMMEND_REWRITE:
    out = "RECOVERED DATA - RECOMMEND REWRITE"; break;
  case ASCQ::RECOVERED_DATA_WITH_ECC_DATA_REWRITTEN:
    out = "RECOVERED DATA WITH ECC - DATA REWRITTEN"; break;
  case ASCQ::RECOVERED_DATA_WITH_LINKING:
    out = "RECOVERED DATA WITH LINKING"; break;
  case ASCQ::DEFECT_LIST_ERROR:
    out = "DEFECT LIST ERROR"; break;
  case ASCQ::DEFECT_LIST_NOT_AVAILABLE:
    out = "DEFECT LIST NOT AVAILABLE"; break;
  case ASCQ::DEFECT_LIST_ERROR_IN_PRIMARY_LIST:
    out = "DEFECT LIST ERROR IN PRIMARY LIST"; break;
  case ASCQ::DEFECT_LIST_ERROR_IN_GROWN_LIST:
    out = "DEFECT LIST ERROR IN GROWN LIST"; break;
  case ASCQ::PARAMETER_LIST_LENGTH_ERROR:
    out = "PARAMETER LIST LENGTH ERROR"; break;
  case ASCQ::SYNCHRONOUS_DATA_TRANSFER_ERROR:
    out = "SYNCHRONOUS DATA TRANSFER ERROR"; break;
  case ASCQ::DEFECT_LIST_NOT_FOUND:
    out = "DEFECT LIST NOT FOUND"; break;
  case ASCQ::PRIMARY_DEFECT_LIST_NOT_FOUND:
    out = "PRIMARY DEFECT LIST NOT FOUND"; break;
  case ASCQ::GROWN_DEFECT_LIST_NOT_FOUND:
    out = "GROWN DEFECT LIST NOT FOUND"; break;
  case ASCQ::MISCOMPARE_DURING_VERIFY_OPERATION:
    out = "MISCOMPARE DURING VERIFY OPERATION"; break;
  case ASCQ::MISCOMPARE_VERIFY_OF_UNMAPPED_LBA:
    out = "MISCOMPARE VERIFY OF UNMAPPED LBA"; break;
  case ASCQ::RECOVERED_ID_WITH_ECC_CORRECTION:
    out = "RECOVERED ID WITH ECC CORRECTION"; break;
  case ASCQ::PARTIAL_DEFECT_LIST_TRANSFER:
    out = "PARTIAL DEFECT LIST TRANSFER"; break;
  case ASCQ::INVALID_COMMAND_OPERATION_CODE:
    out = "INVALID COMMAND OPERATION CODE"; break;
  case ASCQ::ACCESS_DENIED_INITIATOR_PENDING_ENROLLED:
    out = "ACCESS DENIED - INITIATOR PENDING-ENROLLED"; break;
  case ASCQ::ACCESS_DENIED_NO_ACCESS_RIGHTS:
    out = "ACCESS DENIED - NO ACCESS RIGHTS"; break;
  case ASCQ::ACCESS_DENIED_INVALID_MGMT_ID_KEY:
    out = "ACCESS DENIED - INVALID MGMT ID KEY"; break;
  case ASCQ::ILLEGAL_COMMAND_WHILE_IN_WRITE_CAPABLE_STATE:
    out = "ILLEGAL COMMAND WHILE IN WRITE CAPABLE STATE"; break;
  case ASCQ::OBSOLETE_2005:
    out = "OBSOLETE 0x2005"; break;
  case ASCQ::ILLEGAL_COMMAND_WHILE_IN_EXPLICIT_ADDRESS_MODE:
    out = "ILLEGAL COMMAND WHILE IN EXPLICIT ADDRESS MODE"; break;
  case ASCQ::ILLEGAL_COMMAND_WHILE_IN_IMPLICIT_ADDRESS_MODE:
    out = "ILLEGAL COMMAND WHILE IN IMPLICIT ADDRESS MODE"; break;
  case ASCQ::ACCESS_DENIED_ENROLLMENT_CONFLICT:
    out = "ACCESS DENIED - ENROLLMENT CONFLICT"; break;
  case ASCQ::ACCESS_DENIED_INVALID_LU_IDENTIFIER:
    out = "ACCESS DENIED - INVALID LU IDENTIFIER"; break;
  case ASCQ::ACCESS_DENIED_INVALID_PROXY_TOKEN:
    out = "ACCESS DENIED - INVALID PROXY TOKEN"; break;
  case ASCQ::ACCESS_DENIED_ACL_LUN_CONFLICT:
    out = "ACCESS DENIED - ACL LUN CONFLICT"; break;
  case ASCQ::ILLEGAL_COMMAND_WHEN_NOT_IN_APPEND_ONLY_MODE:
    out = "ILLEGAL COMMAND WHEN NOT IN APPEND-ONLY MODE"; break;
  case ASCQ::NOT_AN_ADMINISTRATIVE_LOGICAL_UNIT:
    out = "NOT AN ADMINISTRATIVE LOGICAL UNIT"; break;
  case ASCQ::NOT_A_SUBSIDIARY_LOGICAL_UNIT:
    out = "NOT A SUBSIDIARY LOGICAL UNIT"; break;
  case ASCQ::NOT_A_CONGLOMERATE_LOGICAL_UNIT:
    out = "NOT A CONGLOMERATE LOGICAL UNIT"; break;
  case ASCQ::LOGICAL_BLOCK_ADDRESS_OUT_OF_RANGE:
    out = "LOGICAL BLOCK ADDRESS OUT OF RANGE"; break;
  case ASCQ::INVALID_ELEMENT_ADDRESS:
    out = "INVALID ELEMENT ADDRESS"; break;
  case ASCQ::INVALID_ADDRESS_FOR_WRITE:
    out = "INVALID ADDRESS FOR WRITE"; break;
  case ASCQ::INVALID_WRITE_CROSSING_LAYER_JUMP:
    out = "INVALID WRITE CROSSING LAYER JUMP"; break;
  case ASCQ::UNALIGNED_WRITE_COMMAND:
    out = "UNALIGNED WRITE COMMAND"; break;
  case ASCQ::WRITE_BOUNDARY_VIOLATION:
    out = "WRITE BOUNDARY VIOLATION"; break;
  case ASCQ::ATTEMPT_TO_READ_INVALID_DATA:
    out = "ATTEMPT TO READ INVALID DATA"; break;
  case ASCQ::READ_BOUNDARY_VIOLATION:
    out = "READ BOUNDARY VIOLATION"; break;
  case ASCQ::MISALIGNED_WRITE_COMMAND:
    out = "MISALIGNED WRITE COMMAND"; break;
  case ASCQ::ILLEGAL_FUNCTION_USE_20_00_24_00_OR_26_00_:
    out = "ILLEGAL FUNCTION (USE 20 00:24 00:OR 26 00)"; break;
  case ASCQ::INVALID_TOKEN_OPERATION_CAUSE_NOT_REPORTABLE:
    out = "INVALID TOKEN OPERATION:CAUSE NOT REPORTABLE"; break;
  case ASCQ::INVALID_TOKEN_OPERATION_UNSUPPORTED_TOKEN_TYPE:
    out = "INVALID TOKEN OPERATION:UNSUPPORTED TOKEN TYPE"; break;
  case ASCQ::INVALID_TOKEN_OPERATION_REMOTE_TOKEN_USAGE_NOT_SUPPORTED:
    out = "INVALID TOKEN OPERATION:REMOTE TOKEN USAGE NOT SUPPORTED"; break;
  case ASCQ::INVALID_TOKEN_OPERATION_REMOTE_ROD_TOKEN_CREATION_NOT_SUPPORTED:
    out = "INVALID TOKEN OPERATION:REMOTE ROD TOKEN CREATION NOT SUPPORTED"; break;
  case ASCQ::INVALID_TOKEN_OPERATION_TOKEN_UNKNOWN:
    out = "INVALID TOKEN OPERATION:TOKEN UNKNOWN"; break;
  case ASCQ::INVALID_TOKEN_OPERATION_TOKEN_CORRUPT:
    out = "INVALID TOKEN OPERATION:TOKEN CORRUPT"; break;
  case ASCQ::INVALID_TOKEN_OPERATION_TOKEN_REVOKED:
    out = "INVALID TOKEN OPERATION:TOKEN REVOKED"; break;
  case ASCQ::INVALID_TOKEN_OPERATION_TOKEN_EXPIRED:
    out = "INVALID TOKEN OPERATION:TOKEN EXPIRED"; break;
  case ASCQ::INVALID_TOKEN_OPERATION_TOKEN_CANCELLED:
    out = "INVALID TOKEN OPERATION:TOKEN CANCELLED"; break;
  case ASCQ::INVALID_TOKEN_OPERATION_TOKEN_DELETED:
    out = "INVALID TOKEN OPERATION:TOKEN DELETED"; break;
  case ASCQ::INVALID_TOKEN_OPERATION_INVALID_TOKEN_LENGTH:
    out = "INVALID TOKEN OPERATION:INVALID TOKEN LENGTH"; break;
  case ASCQ::INVALID_FIELD_IN_CDB:
    out = "INVALID FIELD IN CDB"; break;
  case ASCQ::CDB_DECRYPTION_ERROR:
    out = "CDB DECRYPTION ERROR"; break;
  case ASCQ::OBSOLETE_2402:
    out = "OBSOLETE 0x2402"; break;
  case ASCQ::OBSOLETE_2403:
    out = "OBSOLETE 0x2403"; break;
  case ASCQ::SECURITY_AUDIT_VALUE_FROZEN:
    out = "SECURITY AUDIT VALUE FROZEN"; break;
  case ASCQ::SECURITY_WORKING_KEY_FROZEN:
    out = "SECURITY WORKING KEY FROZEN"; break;
  case ASCQ::NONCE_NOT_UNIQUE:
    out = "NONCE NOT UNIQUE"; break;
  case ASCQ::NONCE_TIMESTAMP_OUT_OF_RANGE:
    out = "NONCE TIMESTAMP OUT OF RANGE"; break;
  case ASCQ::INVALID_XCDB:
    out = "INVALID XCDB"; break;
  case ASCQ::INVALID_FAST_FORMAT:
    out = "INVALID FAST FORMAT"; break;
  case ASCQ::LOGICAL_UNIT_NOT_SUPPORTED:
    out = "LOGICAL UNIT NOT SUPPORTED"; break;
  case ASCQ::INVALID_FIELD_IN_PARAMETER_LIST:
    out = "INVALID FIELD IN PARAMETER LIST"; break;
  case ASCQ::PARAMETER_NOT_SUPPORTED:
    out = "PARAMETER NOT SUPPORTED"; break;
  case ASCQ::PARAMETER_VALUE_INVALID:
    out = "PARAMETER VALUE INVALID"; break;
  case ASCQ::THRESHOLD_PARAMETERS_NOT_SUPPORTED:
    out = "THRESHOLD PARAMETERS NOT SUPPORTED"; break;
  case ASCQ::INVALID_RELEASE_OF_PERSISTENT_RESERVATION:
    out = "INVALID RELEASE OF PERSISTENT RESERVATION"; break;
  case ASCQ::DATA_DECRYPTION_ERROR:
    out = "DATA DECRYPTION ERROR"; break;
  case ASCQ::TOO_MANY_TARGET_DESCRIPTORS:
    out = "TOO MANY TARGET DESCRIPTORS"; break;
  case ASCQ::UNSUPPORTED_TARGET_DESCRIPTOR_TYPE_CODE:
    out = "UNSUPPORTED TARGET DESCRIPTOR TYPE CODE"; break;
  case ASCQ::TOO_MANY_SEGMENT_DESCRIPTORS:
    out = "TOO MANY SEGMENT DESCRIPTORS"; break;
  case ASCQ::UNSUPPORTED_SEGMENT_DESCRIPTOR_TYPE_CODE:
    out = "UNSUPPORTED SEGMENT DESCRIPTOR TYPE CODE"; break;
  case ASCQ::UNEXPECTED_INEXACT_SEGMENT:
    out = "UNEXPECTED INEXACT SEGMENT"; break;
  case ASCQ::INLINE_DATA_LENGTH_EXCEEDED:
    out = "INLINE DATA LENGTH EXCEEDED"; break;
  case ASCQ::INVALID_OPERATION_FOR_COPY_SOURCE_OR_DESTINATION:
    out = "INVALID OPERATION FOR COPY SOURCE OR DESTINATION"; break;
  case ASCQ::COPY_SEGMENT_GRANULARITY_VIOLATION:
    out = "COPY SEGMENT GRANULARITY VIOLATION"; break;
  case ASCQ::INVALID_PARAMETER_WHILE_PORT_IS_ENABLED:
    out = "INVALID PARAMETER WHILE PORT IS ENABLED"; break;
  case ASCQ::INVALID_DATA_OUT_BUFFER_INTEGRITY_CHECK_VALUE:
    out = "INVALID DATA-OUT BUFFER INTEGRITY CHECK VALUE"; break;
  case ASCQ::DATA_DECRYPTION_KEY_FAIL_LIMIT_REACHED:
    out = "DATA DECRYPTION KEY FAIL LIMIT REACHED"; break;
  case ASCQ::INCOMPLETE_KEY_ASSOCIATED_DATA_SET:
    out = "INCOMPLETE KEY-ASSOCIATED DATA SET"; break;
  case ASCQ::VENDOR_SPECIFIC_KEY_REFERENCE_NOT_FOUND:
    out = "VENDOR SPECIFIC KEY REFERENCE NOT FOUND"; break;
  case ASCQ::APPLICATION_TAG_MODE_PAGE_IS_INVALID:
    out = "APPLICATION TAG MODE PAGE IS INVALID"; break;
  case ASCQ::TAPE_STREAM_MIRRORING_PREVENTED:
    out = "TAPE STREAM MIRRORING PREVENTED"; break;
  case ASCQ::COPY_SOURCE_OR_COPY_DESTINATION_NOT_AUTHORIZED:
    out = "COPY SOURCE OR COPY DESTINATION NOT AUTHORIZED"; break;
  case ASCQ::WRITE_PROTECTED:
    out = "WRITE PROTECTED"; break;
  case ASCQ::HARDWARE_WRITE_PROTECTED:
    out = "HARDWARE WRITE PROTECTED"; break;
  case ASCQ::LOGICAL_UNIT_SOFTWARE_WRITE_PROTECTED:
    out = "LOGICAL UNIT SOFTWARE WRITE PROTECTED"; break;
  case ASCQ::ASSOCIATED_WRITE_PROTECT:
    out = "ASSOCIATED WRITE PROTECT"; break;
  case ASCQ::PERSISTENT_WRITE_PROTECT:
    out = "PERSISTENT WRITE PROTECT"; break;
  case ASCQ::PERMANENT_WRITE_PROTECT:
    out = "PERMANENT WRITE PROTECT"; break;
  case ASCQ::CONDITIONAL_WRITE_PROTECT:
    out = "CONDITIONAL WRITE PROTECT"; break;
  case ASCQ::SPACE_ALLOCATION_FAILED_WRITE_PROTECT:
    out = "SPACE ALLOCATION FAILED WRITE PROTECT"; break;
  case ASCQ::ZONE_IS_READ_ONLY:
    out = "ZONE IS READ ONLY"; break;
  case ASCQ::NOT_READY_TO_READY_CHANGE_MEDIUM_MAY_HAVE_CHANGED:
    out = "NOT READY TO READY CHANGE:MEDIUM MAY HAVE CHANGED"; break;
  case ASCQ::IMPORT_OR_EXPORT_ELEMENT_ACCESSED:
    out = "IMPORT OR EXPORT ELEMENT ACCESSED"; break;
  case ASCQ::FORMAT_LAYER_MAY_HAVE_CHANGED:
    out = "FORMAT-LAYER MAY HAVE CHANGED"; break;
  case ASCQ::IMPORT_EXPORT_ELEMENT_ACCESSED_MEDIUM_CHANGED:
    out = "IMPORT/EXPORT ELEMENT ACCESSED:MEDIUM CHANGED"; break;
  case ASCQ::POWER_ON_RESET_OR_BUS_DEVICE_RESET_OCCURRED:
    out = "POWER ON:RESET:OR BUS DEVICE RESET OCCURRED"; break;
  case ASCQ::POWER_ON_OCCURRED:
    out = "POWER ON OCCURRED"; break;
  case ASCQ::SCSI_BUS_RESET_OCCURRED:
    out = "SCSI BUS RESET OCCURRED"; break;
  case ASCQ::BUS_DEVICE_RESET_FUNCTION_OCCURRED:
    out = "BUS DEVICE RESET FUNCTION OCCURRED"; break;
  case ASCQ::DEVICE_INTERNAL_RESET:
    out = "DEVICE INTERNAL RESET"; break;
  case ASCQ::TRANSCEIVER_MODE_CHANGED_TO_SINGLE_ENDED:
    out = "TRANSCEIVER MODE CHANGED TO SINGLE-ENDED"; break;
  case ASCQ::TRANSCEIVER_MODE_CHANGED_TO_LVD:
    out = "TRANSCEIVER MODE CHANGED TO LVD"; break;
  case ASCQ::I_T_NEXUS_LOSS_OCCURRED:
    out = "I_T NEXUS LOSS OCCURRED"; break;
  case ASCQ::PARAMETERS_CHANGED:
    out = "PARAMETERS CHANGED"; break;
  case ASCQ::MODE_PARAMETERS_CHANGED:
    out = "MODE PARAMETERS CHANGED"; break;
  case ASCQ::LOG_PARAMETERS_CHANGED:
    out = "LOG PARAMETERS CHANGED"; break;
  case ASCQ::RESERVATIONS_PREEMPTED:
    out = "RESERVATIONS PREEMPTED"; break;
  case ASCQ::RESERVATIONS_RELEASED:
    out = "RESERVATIONS RELEASED"; break;
  case ASCQ::REGISTRATIONS_PREEMPTED:
    out = "REGISTRATIONS PREEMPTED"; break;
  case ASCQ::ASYMMETRIC_ACCESS_STATE_CHANGED:
    out = "ASYMMETRIC ACCESS STATE CHANGED"; break;
  case ASCQ::IMPLICIT_ASYMMETRIC_ACCESS_STATE_TRANSITION_FAILED:
    out = "IMPLICIT ASYMMETRIC ACCESS STATE TRANSITION FAILED"; break;
  case ASCQ::PRIORITY_CHANGED:
    out = "PRIORITY CHANGED"; break;
  case ASCQ::CAPACITY_DATA_HAS_CHANGED:
    out = "CAPACITY DATA HAS CHANGED"; break;
  case ASCQ::ERROR_HISTORY_I_T_NEXUS_CLEARED:
    out = "ERROR HISTORY I_T NEXUS CLEARED"; break;
  case ASCQ::ERROR_HISTORY_SNAPSHOT_RELEASED:
    out = "ERROR HISTORY SNAPSHOT RELEASED"; break;
  case ASCQ::ERROR_RECOVERY_ATTRIBUTES_HAVE_CHANGED:
    out = "ERROR RECOVERY ATTRIBUTES HAVE CHANGED"; break;
  case ASCQ::DATA_ENCRYPTION_CAPABILITIES_CHANGED:
    out = "DATA ENCRYPTION CAPABILITIES CHANGED"; break;
  case ASCQ::TIMESTAMP_CHANGED:
    out = "TIMESTAMP CHANGED"; break;
  case ASCQ::DATA_ENCRYPTION_PARAMETERS_CHANGED_BY_ANOTHER_I_T_NEXUS:
    out = "DATA ENCRYPTION PARAMETERS CHANGED BY ANOTHER I_T NEXUS"; break;
  case ASCQ::DATA_ENCRYPTION_PARAMETERS_CHANGED_BY_VENDOR_SPECIFIC_EVENT:
    out = "DATA ENCRYPTION PARAMETERS CHANGED BY VENDOR SPECIFIC EVENT"; break;
  case ASCQ::DATA_ENCRYPTION_KEY_INSTANCE_COUNTER_HAS_CHANGED:
    out = "DATA ENCRYPTION KEY INSTANCE COUNTER HAS CHANGED"; break;
  case ASCQ::SA_CREATION_CAPABILITIES_DATA_HAS_CHANGED:
    out = "SA CREATION CAPABILITIES DATA HAS CHANGED"; break;
  case ASCQ::MEDIUM_REMOVAL_PREVENTION_PREEMPTED:
    out = "MEDIUM REMOVAL PREVENTION PREEMPTED"; break;
  case ASCQ::ZONE_RESET_WRITE_POINTER_RECOMMENDED:
    out = "ZONE RESET WRITE POINTER RECOMMENDED"; break;
  case ASCQ::COPY_CANNOT_EXECUTE_SINCE_HOST_CANNOT_DISCONNECT:
    out = "COPY CANNOT EXECUTE SINCE HOST CANNOT DISCONNECT"; break;
  case ASCQ::COMMAND_SEQUENCE_ERROR:
    out = "COMMAND SEQUENCE ERROR"; break;
  case ASCQ::TOO_MANY_WINDOWS_SPECIFIED:
    out = "TOO MANY WINDOWS SPECIFIED"; break;
  case ASCQ::INVALID_COMBINATION_OF_WINDOWS_SPECIFIED:
    out = "INVALID COMBINATION OF WINDOWS SPECIFIED"; break;
  case ASCQ::CURRENT_PROGRAM_AREA_IS_NOT_EMPTY:
    out = "CURRENT PROGRAM AREA IS NOT EMPTY"; break;
  case ASCQ::CURRENT_PROGRAM_AREA_IS_EMPTY:
    out = "CURRENT PROGRAM AREA IS EMPTY"; break;
  case ASCQ::ILLEGAL_POWER_CONDITION_REQUEST:
    out = "ILLEGAL POWER CONDITION REQUEST"; break;
  case ASCQ::PERSISTENT_PREVENT_CONFLICT:
    out = "PERSISTENT PREVENT CONFLICT"; break;
  case ASCQ::PREVIOUS_BUSY_STATUS:
    out = "PREVIOUS BUSY STATUS"; break;
  case ASCQ::PREVIOUS_TASK_SET_FULL_STATUS:
    out = "PREVIOUS TASK SET FULL STATUS"; break;
  case ASCQ::PREVIOUS_RESERVATION_CONFLICT_STATUS:
    out = "PREVIOUS RESERVATION CONFLICT STATUS"; break;
  case ASCQ::PARTITION_OR_COLLECTION_CONTAINS_USER_OBJECTS:
    out = "PARTITION OR COLLECTION CONTAINS USER OBJECTS"; break;
  case ASCQ::NOT_RESERVED:
    out = "NOT RESERVED"; break;
  case ASCQ::ORWRITE_GENERATION_DOES_NOT_MATCH:
    out = "ORWRITE GENERATION DOES NOT MATCH"; break;
  case ASCQ::RESET_WRITE_POINTER_NOT_ALLOWED:
    out = "RESET WRITE POINTER NOT ALLOWED"; break;
  case ASCQ::ZONE_IS_OFFLINE:
    out = "ZONE IS OFFLINE"; break;
  case ASCQ::STREAM_NOT_OPEN:
    out = "STREAM NOT OPEN"; break;
  case ASCQ::UNWRITTEN_DATA_IN_ZONE:
    out = "UNWRITTEN DATA IN ZONE"; break;
  case ASCQ::DESCRIPTOR_FORMAT_SENSE_DATA_REQUIRED:
    out = "DESCRIPTOR FORMAT SENSE DATA REQUIRED"; break;
  case ASCQ::OVERWRITE_ERROR_ON_UPDATE_IN_PLACE:
    out = "OVERWRITE ERROR ON UPDATE IN PLACE"; break;
  case ASCQ::INSUFFICIENT_TIME_FOR_OPERATION:
    out = "INSUFFICIENT TIME FOR OPERATION"; break;
  case ASCQ::COMMAND_TIMEOUT_BEFORE_PROCESSING:
    out = "COMMAND TIMEOUT BEFORE PROCESSING"; break;
  case ASCQ::COMMAND_TIMEOUT_DURING_PROCESSING:
    out = "COMMAND TIMEOUT DURING PROCESSING"; break;
  case ASCQ::COMMAND_TIMEOUT_DURING_PROCESSING_DUE_TO_ERROR_RECOVERY:
    out = "COMMAND TIMEOUT DURING PROCESSING DUE TO ERROR RECOVERY"; break;
  case ASCQ::COMMANDS_CLEARED_BY_ANOTHER_INITIATOR:
    out = "COMMANDS CLEARED BY ANOTHER INITIATOR"; break;
  case ASCQ::COMMANDS_CLEARED_BY_POWER_LOSS_NOTIFICATION:
    out = "COMMANDS CLEARED BY POWER LOSS NOTIFICATION"; break;
  case ASCQ::COMMANDS_CLEARED_BY_DEVICE_SERVER:
    out = "COMMANDS CLEARED BY DEVICE SERVER"; break;
  case ASCQ::SOME_COMMANDS_CLEARED_BY_QUEUING_LAYER_EVENT:
    out = "SOME COMMANDS CLEARED BY QUEUING LAYER EVENT"; break;
  case ASCQ::INCOMPATIBLE_MEDIUM_INSTALLED:
    out = "INCOMPATIBLE MEDIUM INSTALLED"; break;
  case ASCQ::CANNOT_READ_MEDIUM_UNKNOWN_FORMAT:
    out = "CANNOT READ MEDIUM - UNKNOWN FORMAT"; break;
  case ASCQ::CANNOT_READ_MEDIUM_INCOMPATIBLE_FORMAT:
    out = "CANNOT READ MEDIUM - INCOMPATIBLE FORMAT"; break;
  case ASCQ::CLEANING_CARTRIDGE_INSTALLED:
    out = "CLEANING CARTRIDGE INSTALLED"; break;
  case ASCQ::CANNOT_WRITE_MEDIUM_UNKNOWN_FORMAT:
    out = "CANNOT WRITE MEDIUM - UNKNOWN FORMAT"; break;
  case ASCQ::CANNOT_WRITE_MEDIUM_INCOMPATIBLE_FORMAT:
    out = "CANNOT WRITE MEDIUM - INCOMPATIBLE FORMAT"; break;
  case ASCQ::CANNOT_FORMAT_MEDIUM_INCOMPATIBLE_MEDIUM:
    out = "CANNOT FORMAT MEDIUM - INCOMPATIBLE MEDIUM"; break;
  case ASCQ::CLEANING_FAILURE:
    out = "CLEANING FAILURE"; break;
  case ASCQ::CANNOT_WRITE_APPLICATION_CODE_MISMATCH:
    out = "CANNOT WRITE - APPLICATION CODE MISMATCH"; break;
  case ASCQ::CURRENT_SESSION_NOT_FIXATED_FOR_APPEND:
    out = "CURRENT SESSION NOT FIXATED FOR APPEND"; break;
  case ASCQ::CLEANING_REQUEST_REJECTED:
    out = "CLEANING REQUEST REJECTED"; break;
  case ASCQ::WORM_MEDIUM_OVERWRITE_ATTEMPTED:
    out = "WORM MEDIUM - OVERWRITE ATTEMPTED"; break;
  case ASCQ::WORM_MEDIUM_INTEGRITY_CHECK:
    out = "WORM MEDIUM - INTEGRITY CHECK"; break;
  case ASCQ::MEDIUM_NOT_FORMATTED:
    out = "MEDIUM NOT FORMATTED"; break;
  case ASCQ::INCOMPATIBLE_VOLUME_TYPE:
    out = "INCOMPATIBLE VOLUME TYPE"; break;
  case ASCQ::INCOMPATIBLE_VOLUME_QUALIFIER:
    out = "INCOMPATIBLE VOLUME QUALIFIER"; break;
  case ASCQ::CLEANING_VOLUME_EXPIRED:
    out = "CLEANING VOLUME EXPIRED"; break;
  case ASCQ::MEDIUM_FORMAT_CORRUPTED:
    out = "MEDIUM FORMAT CORRUPTED"; break;
  case ASCQ::FORMAT_COMMAND_FAILED:
    out = "FORMAT COMMAND FAILED"; break;
  case ASCQ::ZONED_FORMATTING_FAILED_DUE_TO_SPARE_LINKING:
    out = "ZONED FORMATTING FAILED DUE TO SPARE LINKING"; break;
  case ASCQ::SANITIZE_COMMAND_FAILED:
    out = "SANITIZE COMMAND FAILED"; break;
  case ASCQ::NO_DEFECT_SPARE_LOCATION_AVAILABLE:
    out = "NO DEFECT SPARE LOCATION AVAILABLE"; break;
  case ASCQ::DEFECT_LIST_UPDATE_FAILURE:
    out = "DEFECT LIST UPDATE FAILURE"; break;
  case ASCQ::TAPE_LENGTH_ERROR:
    out = "TAPE LENGTH ERROR"; break;
  case ASCQ::ENCLOSURE_FAILURE:
    out = "ENCLOSURE FAILURE"; break;
  case ASCQ::ENCLOSURE_SERVICES_FAILURE:
    out = "ENCLOSURE SERVICES FAILURE"; break;
  case ASCQ::UNSUPPORTED_ENCLOSURE_FUNCTION:
    out = "UNSUPPORTED ENCLOSURE FUNCTION"; break;
  case ASCQ::ENCLOSURE_SERVICES_UNAVAILABLE:
    out = "ENCLOSURE SERVICES UNAVAILABLE"; break;
  case ASCQ::ENCLOSURE_SERVICES_TRANSFER_FAILURE:
    out = "ENCLOSURE SERVICES TRANSFER FAILURE"; break;
  case ASCQ::ENCLOSURE_SERVICES_TRANSFER_REFUSED:
    out = "ENCLOSURE SERVICES TRANSFER REFUSED"; break;
  case ASCQ::ENCLOSURE_SERVICES_CHECKSUM_ERROR:
    out = "ENCLOSURE SERVICES CHECKSUM ERROR"; break;
  case ASCQ::RIBBON_INK_OR_TONER_FAILURE:
    out = "RIBBON:INK:OR TONER FAILURE"; break;
  case ASCQ::ROUNDED_PARAMETER:
    out = "ROUNDED PARAMETER"; break;
  case ASCQ::EVENT_STATUS_NOTIFICATION:
    out = "EVENT STATUS NOTIFICATION"; break;
  case ASCQ::ESN_POWER_MANAGEMENT_CLASS_EVENT:
    out = "ESN - POWER MANAGEMENT CLASS EVENT"; break;
  case ASCQ::ESN_MEDIA_CLASS_EVENT:
    out = "ESN - MEDIA CLASS EVENT"; break;
  case ASCQ::ESN_DEVICE_BUSY_CLASS_EVENT:
    out = "ESN - DEVICE BUSY CLASS EVENT"; break;
  case ASCQ::THIN_PROVISIONING_SOFT_THRESHOLD_REACHED:
    out = "THIN PROVISIONING SOFT THRESHOLD REACHED"; break;
  case ASCQ::SAVING_PARAMETERS_NOT_SUPPORTED:
    out = "SAVING PARAMETERS NOT SUPPORTED"; break;
  case ASCQ::MEDIUM_NOT_PRESENT:
    out = "MEDIUM NOT PRESENT"; break;
  case ASCQ::MEDIUM_NOT_PRESENT_TRAY_CLOSED:
    out = "MEDIUM NOT PRESENT - TRAY CLOSED"; break;
  case ASCQ::MEDIUM_NOT_PRESENT_TRAY_OPEN:
    out = "MEDIUM NOT PRESENT - TRAY OPEN"; break;
  case ASCQ::MEDIUM_NOT_PRESENT_LOADABLE:
    out = "MEDIUM NOT PRESENT - LOADABLE"; break;
  case ASCQ::MEDIUM_NOT_PRESENT_MEDIUM_AUXILIARY_MEMORY_ACCESSIBLE:
    out = "MEDIUM NOT PRESENT - MEDIUM AUXILIARY MEMORY ACCESSIBLE"; break;
  case ASCQ::SEQUENTIAL_POSITIONING_ERROR:
    out = "SEQUENTIAL POSITIONING ERROR"; break;
  case ASCQ::TAPE_POSITION_ERROR_AT_BEGINNING_OF_MEDIUM:
    out = "TAPE POSITION ERROR AT BEGINNING-OF-MEDIUM"; break;
  case ASCQ::TAPE_POSITION_ERROR_AT_END_OF_MEDIUM:
    out = "TAPE POSITION ERROR AT END-OF-MEDIUM"; break;
  case ASCQ::TAPE_OR_ELECTRONIC_VERTICAL_FORMS_UNIT_NOT_READY:
    out = "TAPE OR ELECTRONIC VERTICAL FORMS UNIT NOT READY"; break;
  case ASCQ::SLEW_FAILURE:
    out = "SLEW FAILURE"; break;
  case ASCQ::PAPER_JAM:
    out = "PAPER JAM"; break;
  case ASCQ::FAILED_TO_SENSE_TOP_OF_FORM:
    out = "FAILED TO SENSE TOP-OF-FORM"; break;
  case ASCQ::FAILED_TO_SENSE_BOTTOM_OF_FORM:
    out = "FAILED TO SENSE BOTTOM-OF-FORM"; break;
  case ASCQ::REPOSITION_ERROR:
    out = "REPOSITION ERROR"; break;
  case ASCQ::READ_PAST_END_OF_MEDIUM:
    out = "READ PAST END OF MEDIUM"; break;
  case ASCQ::READ_PAST_BEGINNING_OF_MEDIUM:
    out = "READ PAST BEGINNING OF MEDIUM"; break;
  case ASCQ::POSITION_PAST_END_OF_MEDIUM:
    out = "POSITION PAST END OF MEDIUM"; break;
  case ASCQ::POSITION_PAST_BEGINNING_OF_MEDIUM:
    out = "POSITION PAST BEGINNING OF MEDIUM"; break;
  case ASCQ::MEDIUM_DESTINATION_ELEMENT_FULL:
    out = "MEDIUM DESTINATION ELEMENT FULL"; break;
  case ASCQ::MEDIUM_SOURCE_ELEMENT_EMPTY:
    out = "MEDIUM SOURCE ELEMENT EMPTY"; break;
  case ASCQ::END_OF_MEDIUM_REACHED:
    out = "END OF MEDIUM REACHED"; break;
  case ASCQ::MEDIUM_MAGAZINE_NOT_ACCESSIBLE:
    out = "MEDIUM MAGAZINE NOT ACCESSIBLE"; break;
  case ASCQ::MEDIUM_MAGAZINE_REMOVED:
    out = "MEDIUM MAGAZINE REMOVED"; break;
  case ASCQ::MEDIUM_MAGAZINE_INSERTED:
    out = "MEDIUM MAGAZINE INSERTED"; break;
  case ASCQ::MEDIUM_MAGAZINE_LOCKED:
    out = "MEDIUM MAGAZINE LOCKED"; break;
  case ASCQ::MEDIUM_MAGAZINE_UNLOCKED:
    out = "MEDIUM MAGAZINE UNLOCKED"; break;
  case ASCQ::MECHANICAL_POSITIONING_OR_CHANGER_ERROR:
    out = "MECHANICAL POSITIONING OR CHANGER ERROR"; break;
  case ASCQ::READ_PAST_END_OF_USER_OBJECT:
    out = "READ PAST END OF USER OBJECT"; break;
  case ASCQ::ELEMENT_DISABLED:
    out = "ELEMENT DISABLED"; break;
  case ASCQ::ELEMENT_ENABLED:
    out = "ELEMENT ENABLED"; break;
  case ASCQ::DATA_TRANSFER_DEVICE_REMOVED:
    out = "DATA TRANSFER DEVICE REMOVED"; break;
  case ASCQ::DATA_TRANSFER_DEVICE_INSERTED:
    out = "DATA TRANSFER DEVICE INSERTED"; break;
  case ASCQ::TOO_MANY_LOGICAL_OBJECTS_ON_PARTITION_TO_SUPPORT_OPERATION:
    out = "TOO MANY LOGICAL OBJECTS ON PARTITION TO SUPPORT OPERATION"; break;
  case ASCQ::CUSTOM_3C00:
    out = "ASCQ 3C00"; break;
  case ASCQ::INVALID_BITS_IN_IDENTIFY_MESSAGE:
    out = "INVALID BITS IN IDENTIFY MESSAGE"; break;
  case ASCQ::LOGICAL_UNIT_HAS_NOT_SELF_CONFIGURED_YET:
    out = "LOGICAL UNIT HAS NOT SELF-CONFIGURED YET"; break;
  case ASCQ::LOGICAL_UNIT_FAILURE:
    out = "LOGICAL UNIT FAILURE"; break;
  case ASCQ::TIMEOUT_ON_LOGICAL_UNIT:
    out = "TIMEOUT ON LOGICAL UNIT"; break;
  case ASCQ::LOGICAL_UNIT_FAILED_SELF_TEST:
    out = "LOGICAL UNIT FAILED SELF-TEST"; break;
  case ASCQ::LOGICAL_UNIT_UNABLE_TO_UPDATE_SELF_TEST_LOG:
    out = "LOGICAL UNIT UNABLE TO UPDATE SELF-TEST LOG"; break;
  case ASCQ::TARGET_OPERATING_CONDITIONS_HAVE_CHANGED:
    out = "TARGET OPERATING CONDITIONS HAVE CHANGED"; break;
  case ASCQ::MICROCODE_HAS_BEEN_CHANGED:
    out = "MICROCODE HAS BEEN CHANGED"; break;
  case ASCQ::CHANGED_OPERATING_DEFINITION:
    out = "CHANGED OPERATING DEFINITION"; break;
  case ASCQ::INQUIRY_DATA_HAS_CHANGED:
    out = "INQUIRY DATA HAS CHANGED"; break;
  case ASCQ::COMPONENT_DEVICE_ATTACHED:
    out = "COMPONENT DEVICE ATTACHED"; break;
  case ASCQ::DEVICE_IDENTIFIER_CHANGED:
    out = "DEVICE IDENTIFIER CHANGED"; break;
  case ASCQ::REDUNDANCY_GROUP_CREATED_OR_MODIFIED:
    out = "REDUNDANCY GROUP CREATED OR MODIFIED"; break;
  case ASCQ::REDUNDANCY_GROUP_DELETED:
    out = "REDUNDANCY GROUP DELETED"; break;
  case ASCQ::SPARE_CREATED_OR_MODIFIED:
    out = "SPARE CREATED OR MODIFIED"; break;
  case ASCQ::SPARE_DELETED:
    out = "SPARE DELETED"; break;
  case ASCQ::VOLUME_SET_CREATED_OR_MODIFIED:
    out = "VOLUME SET CREATED OR MODIFIED"; break;
  case ASCQ::VOLUME_SET_DELETED:
    out = "VOLUME SET DELETED"; break;
  case ASCQ::VOLUME_SET_DEASSIGNED:
    out = "VOLUME SET DEASSIGNED"; break;
  case ASCQ::VOLUME_SET_REASSIGNED:
    out = "VOLUME SET REASSIGNED"; break;
  case ASCQ::REPORTED_LUNS_DATA_HAS_CHANGED:
    out = "REPORTED LUNS DATA HAS CHANGED"; break;
  case ASCQ::ECHO_BUFFER_OVERWRITTEN:
    out = "ECHO BUFFER OVERWRITTEN"; break;
  case ASCQ::MEDIUM_LOADABLE:
    out = "MEDIUM LOADABLE"; break;
  case ASCQ::MEDIUM_AUXILIARY_MEMORY_ACCESSIBLE:
    out = "MEDIUM AUXILIARY MEMORY ACCESSIBLE"; break;
  case ASCQ::iSCSI_IP_ADDRESS_ADDED:
    out = "iSCSI IP ADDRESS ADDED"; break;
  case ASCQ::iSCSI_IP_ADDRESS_REMOVED:
    out = "iSCSI IP ADDRESS REMOVED"; break;
  case ASCQ::iSCSI_IP_ADDRESS_CHANGED:
    out = "iSCSI IP ADDRESS CHANGED"; break;
  case ASCQ::INSPECT_REFERRALS_SENSE_DESCRIPTORS:
    out = "INSPECT REFERRALS SENSE DESCRIPTORS"; break;
  case ASCQ::MICROCODE_HAS_BEEN_CHANGED_WITHOUT_RESET:
    out = "MICROCODE HAS BEEN CHANGED WITHOUT RESET"; break;
  case ASCQ::ZONE_TRANSITION_TO_FULL:
    out = "ZONE TRANSITION TO FULL"; break;
  case ASCQ::BIND_COMPLETED:
    out = "BIND COMPLETED"; break;
  case ASCQ::BIND_REDIRECTED:
    out = "BIND REDIRECTED"; break;
  case ASCQ::SUBSIDIARY_BINDING_CHANGED:
    out = "SUBSIDIARY BINDING CHANGED"; break;
  case ASCQ::RAM_FAILURE_SHOULD_USE_40_NN:
    out = "RAM FAILURE (SHOULD USE 40 NN)"; break;
  case ASCQ::DATA_PATH_FAILURE_SHOULD_USE_40_NN:
    out = "DATA PATH FAILURE (SHOULD USE 40 NN)"; break;
  case ASCQ::POWER_ON_OR_SELF_TEST_FAILURE_SHOULD_USE_40_NN:
    out = "POWER-ON OR SELF-TEST FAILURE (SHOULD USE 40 NN)"; break;
  case ASCQ::MESSAGE_ERROR:
    out = "MESSAGE ERROR"; break;
  case ASCQ::INTERNAL_TARGET_FAILURE:
    out = "INTERNAL TARGET FAILURE"; break;
  case ASCQ::PERSISTENT_RESERVATION_INFORMATION_LOST:
    out = "PERSISTENT RESERVATION INFORMATION LOST"; break;
  case ASCQ::ATA_DEVICE_FAILED_SET_FEATURES:
    out = "ATA DEVICE FAILED SET FEATURES"; break;
  case ASCQ::SELECT_OR_RESELECT_FAILURE:
    out = "SELECT OR RESELECT FAILURE"; break;
  case ASCQ::UNSUCCESSFUL_SOFT_RESET:
    out = "UNSUCCESSFUL SOFT RESET"; break;
  case ASCQ::SCSI_PARITY_ERROR:
    out = "SCSI PARITY ERROR"; break;
  case ASCQ::DATA_PHASE_CRC_ERROR_DETECTED:
    out = "DATA PHASE CRC ERROR DETECTED"; break;
  case ASCQ::SCSI_PARITY_ERROR_DETECTED_DURING_ST_DATA_PHASE:
    out = "SCSI PARITY ERROR DETECTED DURING ST DATA PHASE"; break;
  case ASCQ::INFORMATION_UNIT_iuCRC_ERROR_DETECTED:
    out = "INFORMATION UNIT iuCRC ERROR DETECTED"; break;
  case ASCQ::ASYNCHRONOUS_INFORMATION_PROTECTION_ERROR_DETECTED:
    out = "ASYNCHRONOUS INFORMATION PROTECTION ERROR DETECTED"; break;
  case ASCQ::PROTOCOL_SERVICE_CRC_ERROR:
    out = "PROTOCOL SERVICE CRC ERROR"; break;
  case ASCQ::PHY_TEST_FUNCTION_IN_PROGRESS:
    out = "PHY TEST FUNCTION IN PROGRESS"; break;
  case ASCQ::SOME_COMMANDS_CLEARED_BY_ISCSI_PROTOCOL_EVENT:
    out = "SOME COMMANDS CLEARED BY ISCSI PROTOCOL EVENT"; break;
  case ASCQ::INITIATOR_DETECTED_ERROR_MESSAGE_RECEIVED:
    out = "INITIATOR DETECTED ERROR MESSAGE RECEIVED"; break;
  case ASCQ::INVALID_MESSAGE_ERROR:
    out = "INVALID MESSAGE ERROR"; break;
  case ASCQ::COMMAND_PHASE_ERROR:
    out = "COMMAND PHASE ERROR"; break;
  case ASCQ::DATA_PHASE_ERROR:
    out = "DATA PHASE ERROR"; break;
  case ASCQ::INVALID_TARGET_PORT_TRANSFER_TAG_RECEIVED:
    out = "INVALID TARGET PORT TRANSFER TAG RECEIVED"; break;
  case ASCQ::TOO_MUCH_WRITE_DATA:
    out = "TOO MUCH WRITE DATA"; break;
  case ASCQ::ACK_NAK_TIMEOUT:
    out = "ACK/NAK TIMEOUT"; break;
  case ASCQ::NAK_RECEIVED:
    out = "NAK RECEIVED"; break;
  case ASCQ::DATA_OFFSET_ERROR:
    out = "DATA OFFSET ERROR"; break;
  case ASCQ::INITIATOR_RESPONSE_TIMEOUT:
    out = "INITIATOR RESPONSE TIMEOUT"; break;
  case ASCQ::CONNECTION_LOST:
    out = "CONNECTION LOST"; break;
  case ASCQ::DATA_IN_BUFFER_OVERFLOW_DATA_BUFFER_SIZE:
    out = "DATA-IN BUFFER OVERFLOW - DATA BUFFER SIZE"; break;
  case ASCQ::DATA_IN_BUFFER_OVERFLOW_DATA_BUFFER_DESCRIPTOR_AREA:
    out = "DATA-IN BUFFER OVERFLOW - DATA BUFFER DESCRIPTOR AREA"; break;
  case ASCQ::DATA_IN_BUFFER_ERROR:
    out = "DATA-IN BUFFER ERROR"; break;
  case ASCQ::DATA_OUT_BUFFER_OVERFLOW_DATA_BUFFER_SIZE:
    out = "DATA-OUT BUFFER OVERFLOW - DATA BUFFER SIZE"; break;
  case ASCQ::DATA_OUT_BUFFER_OVERFLOW_DATA_BUFFER_DESCRIPTOR_AREA:
    out = "DATA-OUT BUFFER OVERFLOW - DATA BUFFER DESCRIPTOR AREA"; break;
  case ASCQ::DATA_OUT_BUFFER_ERROR:
    out = "DATA-OUT BUFFER ERROR"; break;
  case ASCQ::PCIE_FABRIC_ERROR:
    out = "PCIE FABRIC ERROR"; break;
  case ASCQ::PCIE_COMPLETION_TIMEOUT:
    out = "PCIE COMPLETION TIMEOUT"; break;
  case ASCQ::PCIE_COMPLETER_ABORT:
    out = "PCIE COMPLETER ABORT"; break;
  case ASCQ::PCIE_POISONED_TLP_RECEIVED:
    out = "PCIE POISONED TLP RECEIVED"; break;
  case ASCQ::PCIE_ECRC_CHECK_FAILED:
    out = "PCIE ECRC CHECK FAILED"; break;
  case ASCQ::PCIE_UNSUPPORTED_REQUEST:
    out = "PCIE UNSUPPORTED REQUEST"; break;
  case ASCQ::PCIE_ACS_VIOLATION:
    out = "PCIE ACS VIOLATION"; break;
  case ASCQ::PCIE_TLP_PREFIX_BLOCKED:
    out = "PCIE TLP PREFIX BLOCKED"; break;
  case ASCQ::LOGICAL_UNIT_FAILED_SELF_CONFIGURATION:
    out = "LOGICAL UNIT FAILED SELF-CONFIGURATION"; break;
  case ASCQ::OVERLAPPED_COMMANDS_ATTEMPTED:
    out = "OVERLAPPED COMMANDS ATTEMPTED"; break;
  case ASCQ::CUSTOM_4F00:
    out = "ASCQ 4F00"; break;
  case ASCQ::WRITE_APPEND_ERROR:
    out = "WRITE APPEND ERROR"; break;
  case ASCQ::WRITE_APPEND_POSITION_ERROR:
    out = "WRITE APPEND POSITION ERROR"; break;
  case ASCQ::POSITION_ERROR_RELATED_TO_TIMING:
    out = "POSITION ERROR RELATED TO TIMING"; break;
  case ASCQ::ERASE_FAILURE:
    out = "ERASE FAILURE"; break;
  case ASCQ::ERASE_FAILURE_INCOMPLETE_ERASE_OPERATION_DETECTED:
    out = "ERASE FAILURE - INCOMPLETE ERASE OPERATION DETECTED"; break;
  case ASCQ::CARTRIDGE_FAULT:
    out = "CARTRIDGE FAULT"; break;
  case ASCQ::MEDIA_LOAD_OR_EJECT_FAILED:
    out = "MEDIA LOAD OR EJECT FAILED"; break;
  case ASCQ::UNLOAD_TAPE_FAILURE:
    out = "UNLOAD TAPE FAILURE"; break;
  case ASCQ::MEDIUM_REMOVAL_PREVENTED:
    out = "MEDIUM REMOVAL PREVENTED"; break;
  case ASCQ::MEDIUM_REMOVAL_PREVENTED_BY_DATA_TRANSFER_ELEMENT:
    out = "MEDIUM REMOVAL PREVENTED BY DATA TRANSFER ELEMENT"; break;
  case ASCQ::MEDIUM_THREAD_OR_UNTHREAD_FAILURE:
    out = "MEDIUM THREAD OR UNTHREAD FAILURE"; break;
  case ASCQ::VOLUME_IDENTIFIER_INVALID:
    out = "VOLUME IDENTIFIER INVALID"; break;
  case ASCQ::VOLUME_IDENTIFIER_MISSING:
    out = "VOLUME IDENTIFIER MISSING"; break;
  case ASCQ::DUPLICATE_VOLUME_IDENTIFIER:
    out = "DUPLICATE VOLUME IDENTIFIER"; break;
  case ASCQ::ELEMENT_STATUS_UNKNOWN:
    out = "ELEMENT STATUS UNKNOWN"; break;
  case ASCQ::DATA_TRANSFER_DEVICE_ERROR_LOAD_FAILED:
    out = "DATA TRANSFER DEVICE ERROR - LOAD FAILED"; break;
  case ASCQ::DATA_TRANSFER_DEVICE_ERROR_UNLOAD_FAILED:
    out = "DATA TRANSFER DEVICE ERROR - UNLOAD FAILED"; break;
  case ASCQ::DATA_TRANSFER_DEVICE_ERROR_UNLOAD_MISSING:
    out = "DATA TRANSFER DEVICE ERROR - UNLOAD MISSING"; break;
  case ASCQ::DATA_TRANSFER_DEVICE_ERROR_EJECT_FAILED:
    out = "DATA TRANSFER DEVICE ERROR - EJECT FAILED"; break;
  case ASCQ::DATA_TRANSFER_DEVICE_ERROR_LIBRARY_COMMUNICATION_FAILED:
    out = "DATA TRANSFER DEVICE ERROR - LIBRARY COMMUNICATION FAILED"; break;
  case ASCQ::SCSI_TO_HOST_SYSTEM_INTERFACE_FAILURE:
    out = "SCSI TO HOST SYSTEM INTERFACE FAILURE"; break;
  case ASCQ::SYSTEM_RESOURCE_FAILURE:
    out = "SYSTEM RESOURCE FAILURE"; break;
  case ASCQ::SYSTEM_BUFFER_FULL:
    out = "SYSTEM BUFFER FULL"; break;
  case ASCQ::INSUFFICIENT_RESERVATION_RESOURCES:
    out = "INSUFFICIENT RESERVATION RESOURCES"; break;
  case ASCQ::INSUFFICIENT_RESOURCES:
    out = "INSUFFICIENT RESOURCES"; break;
  case ASCQ::INSUFFICIENT_REGISTRATION_RESOURCES:
    out = "INSUFFICIENT REGISTRATION RESOURCES"; break;
  case ASCQ::INSUFFICIENT_ACCESS_CONTROL_RESOURCES:
    out = "INSUFFICIENT ACCESS CONTROL RESOURCES"; break;
  case ASCQ::AUXILIARY_MEMORY_OUT_OF_SPACE:
    out = "AUXILIARY MEMORY OUT OF SPACE"; break;
  case ASCQ::QUOTA_ERROR:
    out = "QUOTA ERROR"; break;
  case ASCQ::MAXIMUM_NUMBER_OF_SUPPLEMENTAL_DECRYPTION_KEYS_EXCEEDED:
    out = "MAXIMUM NUMBER OF SUPPLEMENTAL DECRYPTION KEYS EXCEEDED"; break;
  case ASCQ::MEDIUM_AUXILIARY_MEMORY_NOT_ACCESSIBLE:
    out = "MEDIUM AUXILIARY MEMORY NOT ACCESSIBLE"; break;
  case ASCQ::DATA_CURRENTLY_UNAVAILABLE:
    out = "DATA CURRENTLY UNAVAILABLE"; break;
  case ASCQ::INSUFFICIENT_POWER_FOR_OPERATION:
    out = "INSUFFICIENT POWER FOR OPERATION"; break;
  case ASCQ::INSUFFICIENT_RESOURCES_TO_CREATE_ROD:
    out = "INSUFFICIENT RESOURCES TO CREATE ROD"; break;
  case ASCQ::INSUFFICIENT_RESOURCES_TO_CREATE_ROD_TOKEN:
    out = "INSUFFICIENT RESOURCES TO CREATE ROD TOKEN"; break;
  case ASCQ::INSUFFICIENT_ZONE_RESOURCES:
    out = "INSUFFICIENT ZONE RESOURCES"; break;
  case ASCQ::INSUFFICIENT_ZONE_RESOURCES_TO_COMPLETE_WRITE:
    out = "INSUFFICIENT ZONE RESOURCES TO COMPLETE WRITE"; break;
  case ASCQ::MAXIMUM_NUMBER_OF_STREAMS_OPEN:
    out = "MAXIMUM NUMBER OF STREAMS OPEN"; break;
  case ASCQ::INSUFFICIENT_RESOURCES_TO_BIND:
    out = "INSUFFICIENT RESOURCES TO BIND"; break;
  case ASCQ::CUSTOM_5600:
    out = "ASCQ 5600"; break;
  case ASCQ::UNABLE_TO_RECOVER_TABLE_OF_CONTENTS:
    out = "UNABLE TO RECOVER TABLE-OF-CONTENTS"; break;
  case ASCQ::GENERATION_DOES_NOT_EXIST:
    out = "GENERATION DOES NOT EXIST"; break;
  case ASCQ::UPDATED_BLOCK_READ:
    out = "UPDATED BLOCK READ"; break;
  case ASCQ::OPERATOR_REQUEST_OR_STATE_CHANGE_INPUT:
    out = "OPERATOR REQUEST OR STATE CHANGE INPUT"; break;
  case ASCQ::OPERATOR_MEDIUM_REMOVAL_REQUEST:
    out = "OPERATOR MEDIUM REMOVAL REQUEST"; break;
  case ASCQ::OPERATOR_SELECTED_WRITE_PROTECT:
    out = "OPERATOR SELECTED WRITE PROTECT"; break;
  case ASCQ::OPERATOR_SELECTED_WRITE_PERMIT:
    out = "OPERATOR SELECTED WRITE PERMIT"; break;
  case ASCQ::LOG_EXCEPTION:
    out = "LOG EXCEPTION"; break;
  case ASCQ::THRESHOLD_CONDITION_MET:
    out = "THRESHOLD CONDITION MET"; break;
  case ASCQ::LOG_COUNTER_AT_MAXIMUM:
    out = "LOG COUNTER AT MAXIMUM"; break;
  case ASCQ::LOG_LIST_CODES_EXHAUSTED:
    out = "LOG LIST CODES EXHAUSTED"; break;
  case ASCQ::RPL_STATUS_CHANGE:
    out = "RPL STATUS CHANGE"; break;
  case ASCQ::SPINDLES_SYNCHRONIZED:
    out = "SPINDLES SYNCHRONIZED"; break;
  case ASCQ::SPINDLES_NOT_SYNCHRONIZED:
    out = "SPINDLES NOT SYNCHRONIZED"; break;
  case ASCQ::FAILURE_PREDICTION_THRESHOLD_EXCEEDED:
    out = "FAILURE PREDICTION THRESHOLD EXCEEDED"; break;
  case ASCQ::MEDIA_FAILURE_PREDICTION_THRESHOLD_EXCEEDED:
    out = "MEDIA FAILURE PREDICTION THRESHOLD EXCEEDED"; break;
  case ASCQ::LOGICAL_UNIT_FAILURE_PREDICTION_THRESHOLD_EXCEEDED:
    out = "LOGICAL UNIT FAILURE PREDICTION THRESHOLD EXCEEDED"; break;
  case ASCQ::SPARE_AREA_EXHAUSTION_PREDICTION_THRESHOLD_EXCEEDED:
    out = "SPARE AREA EXHAUSTION PREDICTION THRESHOLD EXCEEDED"; break;
  case ASCQ::HARDWARE_IMPENDING_FAILURE_GENERAL_HARD_DRIVE_FAILURE:
    out = "HARDWARE IMPENDING FAILURE GENERAL HARD DRIVE FAILURE"; break;
  case ASCQ::HARDWARE_IMPENDING_FAILURE_DRIVE_ERROR_RATE_TOO_HIGH:
    out = "HARDWARE IMPENDING FAILURE DRIVE ERROR RATE TOO HIGH"; break;
  case ASCQ::HARDWARE_IMPENDING_FAILURE_DATA_ERROR_RATE_TOO_HIGH:
    out = "HARDWARE IMPENDING FAILURE DATA ERROR RATE TOO HIGH"; break;
  case ASCQ::HARDWARE_IMPENDING_FAILURE_SEEK_ERROR_RATE_TOO_HIGH:
    out = "HARDWARE IMPENDING FAILURE SEEK ERROR RATE TOO HIGH"; break;
  case ASCQ::HARDWARE_IMPENDING_FAILURE_TOO_MANY_BLOCK_REASSIGNS:
    out = "HARDWARE IMPENDING FAILURE TOO MANY BLOCK REASSIGNS"; break;
  case ASCQ::HARDWARE_IMPENDING_FAILURE_ACCESS_TIMES_TOO_HIGH:
    out = "HARDWARE IMPENDING FAILURE ACCESS TIMES TOO HIGH"; break;
  case ASCQ::HARDWARE_IMPENDING_FAILURE_START_UNIT_TIMES_TOO_HIGH:
    out = "HARDWARE IMPENDING FAILURE START UNIT TIMES TOO HIGH"; break;
  case ASCQ::HARDWARE_IMPENDING_FAILURE_CHANNEL_PARAMETRICS:
    out = "HARDWARE IMPENDING FAILURE CHANNEL PARAMETRICS"; break;
  case ASCQ::HARDWARE_IMPENDING_FAILURE_CONTROLLER_DETECTED:
    out = "HARDWARE IMPENDING FAILURE CONTROLLER DETECTED"; break;
  case ASCQ::HARDWARE_IMPENDING_FAILURE_THROUGHPUT_PERFORMANCE:
    out = "HARDWARE IMPENDING FAILURE THROUGHPUT PERFORMANCE"; break;
  case ASCQ::HARDWARE_IMPENDING_FAILURE_SEEK_TIME_PERFORMANCE:
    out = "HARDWARE IMPENDING FAILURE SEEK TIME PERFORMANCE"; break;
  case ASCQ::HARDWARE_IMPENDING_FAILURE_SPIN_UP_RETRY_COUNT:
    out = "HARDWARE IMPENDING FAILURE SPIN-UP RETRY COUNT"; break;
  case ASCQ::HARDWARE_IMPENDING_FAILURE_DRIVE_CALIBRATION_RETRY_COUNT:
    out = "HARDWARE IMPENDING FAILURE DRIVE CALIBRATION RETRY COUNT"; break;
  case ASCQ::HARDWARE_IMPENDING_FAILURE_POWER_LOSS_PROTECTION_CIRCUIT:
    out = "HARDWARE IMPENDING FAILURE POWER LOSS PROTECTION CIRCUIT"; break;
  case ASCQ::CONTROLLER_IMPENDING_FAILURE_GENERAL_HARD_DRIVE_FAILURE:
    out = "CONTROLLER IMPENDING FAILURE GENERAL HARD DRIVE FAILURE"; break;
  case ASCQ::CONTROLLER_IMPENDING_FAILURE_DRIVE_ERROR_RATE_TOO_HIGH:
    out = "CONTROLLER IMPENDING FAILURE DRIVE ERROR RATE TOO HIGH"; break;
  case ASCQ::CONTROLLER_IMPENDING_FAILURE_DATA_ERROR_RATE_TOO_HIGH:
    out = "CONTROLLER IMPENDING FAILURE DATA ERROR RATE TOO HIGH"; break;
  case ASCQ::CONTROLLER_IMPENDING_FAILURE_SEEK_ERROR_RATE_TOO_HIGH:
    out = "CONTROLLER IMPENDING FAILURE SEEK ERROR RATE TOO HIGH"; break;
  case ASCQ::CONTROLLER_IMPENDING_FAILURE_TOO_MANY_BLOCK_REASSIGNS:
    out = "CONTROLLER IMPENDING FAILURE TOO MANY BLOCK REASSIGNS"; break;
  case ASCQ::CONTROLLER_IMPENDING_FAILURE_ACCESS_TIMES_TOO_HIGH:
    out = "CONTROLLER IMPENDING FAILURE ACCESS TIMES TOO HIGH"; break;
  case ASCQ::CONTROLLER_IMPENDING_FAILURE_START_UNIT_TIMES_TOO_HIGH:
    out = "CONTROLLER IMPENDING FAILURE START UNIT TIMES TOO HIGH"; break;
  case ASCQ::CONTROLLER_IMPENDING_FAILURE_CHANNEL_PARAMETRICS:
    out = "CONTROLLER IMPENDING FAILURE CHANNEL PARAMETRICS"; break;
  case ASCQ::CONTROLLER_IMPENDING_FAILURE_CONTROLLER_DETECTED:
    out = "CONTROLLER IMPENDING FAILURE CONTROLLER DETECTED"; break;
  case ASCQ::CONTROLLER_IMPENDING_FAILURE_THROUGHPUT_PERFORMANCE:
    out = "CONTROLLER IMPENDING FAILURE THROUGHPUT PERFORMANCE"; break;
  case ASCQ::CONTROLLER_IMPENDING_FAILURE_SEEK_TIME_PERFORMANCE:
    out = "CONTROLLER IMPENDING FAILURE SEEK TIME PERFORMANCE"; break;
  case ASCQ::CONTROLLER_IMPENDING_FAILURE_SPIN_UP_RETRY_COUNT:
    out = "CONTROLLER IMPENDING FAILURE SPIN-UP RETRY COUNT"; break;
  case ASCQ::CONTROLLER_IMPENDING_FAILURE_DRIVE_CALIBRATION_RETRY_COUNT:
    out = "CONTROLLER IMPENDING FAILURE DRIVE CALIBRATION RETRY COUNT"; break;
  case ASCQ::DATA_CHANNEL_IMPENDING_FAILURE_GENERAL_HARD_DRIVE_FAILURE:
    out = "DATA CHANNEL IMPENDING FAILURE GENERAL HARD DRIVE FAILURE"; break;
  case ASCQ::DATA_CHANNEL_IMPENDING_FAILURE_DRIVE_ERROR_RATE_TOO_HIGH:
    out = "DATA CHANNEL IMPENDING FAILURE DRIVE ERROR RATE TOO HIGH"; break;
  case ASCQ::DATA_CHANNEL_IMPENDING_FAILURE_DATA_ERROR_RATE_TOO_HIGH:
    out = "DATA CHANNEL IMPENDING FAILURE DATA ERROR RATE TOO HIGH"; break;
  case ASCQ::DATA_CHANNEL_IMPENDING_FAILURE_SEEK_ERROR_RATE_TOO_HIGH:
    out = "DATA CHANNEL IMPENDING FAILURE SEEK ERROR RATE TOO HIGH"; break;
  case ASCQ::DATA_CHANNEL_IMPENDING_FAILURE_TOO_MANY_BLOCK_REASSIGNS:
    out = "DATA CHANNEL IMPENDING FAILURE TOO MANY BLOCK REASSIGNS"; break;
  case ASCQ::DATA_CHANNEL_IMPENDING_FAILURE_ACCESS_TIMES_TOO_HIGH:
    out = "DATA CHANNEL IMPENDING FAILURE ACCESS TIMES TOO HIGH"; break;
  case ASCQ::DATA_CHANNEL_IMPENDING_FAILURE_START_UNIT_TIMES_TOO_HIGH:
    out = "DATA CHANNEL IMPENDING FAILURE START UNIT TIMES TOO HIGH"; break;
  case ASCQ::DATA_CHANNEL_IMPENDING_FAILURE_CHANNEL_PARAMETRICS:
    out = "DATA CHANNEL IMPENDING FAILURE CHANNEL PARAMETRICS"; break;
  case ASCQ::DATA_CHANNEL_IMPENDING_FAILURE_CONTROLLER_DETECTED:
    out = "DATA CHANNEL IMPENDING FAILURE CONTROLLER DETECTED"; break;
  case ASCQ::DATA_CHANNEL_IMPENDING_FAILURE_THROUGHPUT_PERFORMANCE:
    out = "DATA CHANNEL IMPENDING FAILURE THROUGHPUT PERFORMANCE"; break;
  case ASCQ::DATA_CHANNEL_IMPENDING_FAILURE_SEEK_TIME_PERFORMANCE:
    out = "DATA CHANNEL IMPENDING FAILURE SEEK TIME PERFORMANCE"; break;
  case ASCQ::DATA_CHANNEL_IMPENDING_FAILURE_SPIN_UP_RETRY_COUNT:
    out = "DATA CHANNEL IMPENDING FAILURE SPIN-UP RETRY COUNT"; break;
  case ASCQ::DATA_CHANNEL_IMPENDING_FAILURE_DRIVE_CALIBRATION_RETRY_COUNT:
    out = "DATA CHANNEL IMPENDING FAILURE DRIVE CALIBRATION RETRY COUNT"; break;
  case ASCQ::SERVO_IMPENDING_FAILURE_GENERAL_HARD_DRIVE_FAILURE:
    out = "SERVO IMPENDING FAILURE GENERAL HARD DRIVE FAILURE"; break;
  case ASCQ::SERVO_IMPENDING_FAILURE_DRIVE_ERROR_RATE_TOO_HIGH:
    out = "SERVO IMPENDING FAILURE DRIVE ERROR RATE TOO HIGH"; break;
  case ASCQ::SERVO_IMPENDING_FAILURE_DATA_ERROR_RATE_TOO_HIGH:
    out = "SERVO IMPENDING FAILURE DATA ERROR RATE TOO HIGH"; break;
  case ASCQ::SERVO_IMPENDING_FAILURE_SEEK_ERROR_RATE_TOO_HIGH:
    out = "SERVO IMPENDING FAILURE SEEK ERROR RATE TOO HIGH"; break;
  case ASCQ::SERVO_IMPENDING_FAILURE_TOO_MANY_BLOCK_REASSIGNS:
    out = "SERVO IMPENDING FAILURE TOO MANY BLOCK REASSIGNS"; break;
  case ASCQ::SERVO_IMPENDING_FAILURE_ACCESS_TIMES_TOO_HIGH:
    out = "SERVO IMPENDING FAILURE ACCESS TIMES TOO HIGH"; break;
  case ASCQ::SERVO_IMPENDING_FAILURE_START_UNIT_TIMES_TOO_HIGH:
    out = "SERVO IMPENDING FAILURE START UNIT TIMES TOO HIGH"; break;
  case ASCQ::SERVO_IMPENDING_FAILURE_CHANNEL_PARAMETRICS:
    out = "SERVO IMPENDING FAILURE CHANNEL PARAMETRICS"; break;
  case ASCQ::SERVO_IMPENDING_FAILURE_CONTROLLER_DETECTED:
    out = "SERVO IMPENDING FAILURE CONTROLLER DETECTED"; break;
  case ASCQ::SERVO_IMPENDING_FAILURE_THROUGHPUT_PERFORMANCE:
    out = "SERVO IMPENDING FAILURE THROUGHPUT PERFORMANCE"; break;
  case ASCQ::SERVO_IMPENDING_FAILURE_SEEK_TIME_PERFORMANCE:
    out = "SERVO IMPENDING FAILURE SEEK TIME PERFORMANCE"; break;
  case ASCQ::SERVO_IMPENDING_FAILURE_SPIN_UP_RETRY_COUNT:
    out = "SERVO IMPENDING FAILURE SPIN-UP RETRY COUNT"; break;
  case ASCQ::SERVO_IMPENDING_FAILURE_DRIVE_CALIBRATION_RETRY_COUNT:
    out = "SERVO IMPENDING FAILURE DRIVE CALIBRATION RETRY COUNT"; break;
  case ASCQ::SPINDLE_IMPENDING_FAILURE_GENERAL_HARD_DRIVE_FAILURE:
    out = "SPINDLE IMPENDING FAILURE GENERAL HARD DRIVE FAILURE"; break;
  case ASCQ::SPINDLE_IMPENDING_FAILURE_DRIVE_ERROR_RATE_TOO_HIGH:
    out = "SPINDLE IMPENDING FAILURE DRIVE ERROR RATE TOO HIGH"; break;
  case ASCQ::SPINDLE_IMPENDING_FAILURE_DATA_ERROR_RATE_TOO_HIGH:
    out = "SPINDLE IMPENDING FAILURE DATA ERROR RATE TOO HIGH"; break;
  case ASCQ::SPINDLE_IMPENDING_FAILURE_SEEK_ERROR_RATE_TOO_HIGH:
    out = "SPINDLE IMPENDING FAILURE SEEK ERROR RATE TOO HIGH"; break;
  case ASCQ::SPINDLE_IMPENDING_FAILURE_TOO_MANY_BLOCK_REASSIGNS:
    out = "SPINDLE IMPENDING FAILURE TOO MANY BLOCK REASSIGNS"; break;
  case ASCQ::SPINDLE_IMPENDING_FAILURE_ACCESS_TIMES_TOO_HIGH:
    out = "SPINDLE IMPENDING FAILURE ACCESS TIMES TOO HIGH"; break;
  case ASCQ::SPINDLE_IMPENDING_FAILURE_START_UNIT_TIMES_TOO_HIGH:
    out = "SPINDLE IMPENDING FAILURE START UNIT TIMES TOO HIGH"; break;
  case ASCQ::SPINDLE_IMPENDING_FAILURE_CHANNEL_PARAMETRICS:
    out = "SPINDLE IMPENDING FAILURE CHANNEL PARAMETRICS"; break;
  case ASCQ::SPINDLE_IMPENDING_FAILURE_CONTROLLER_DETECTED:
    out = "SPINDLE IMPENDING FAILURE CONTROLLER DETECTED"; break;
  case ASCQ::SPINDLE_IMPENDING_FAILURE_THROUGHPUT_PERFORMANCE:
    out = "SPINDLE IMPENDING FAILURE THROUGHPUT PERFORMANCE"; break;
  case ASCQ::SPINDLE_IMPENDING_FAILURE_SEEK_TIME_PERFORMANCE:
    out = "SPINDLE IMPENDING FAILURE SEEK TIME PERFORMANCE"; break;
  case ASCQ::SPINDLE_IMPENDING_FAILURE_SPIN_UP_RETRY_COUNT:
    out = "SPINDLE IMPENDING FAILURE SPIN-UP RETRY COUNT"; break;
  case ASCQ::SPINDLE_IMPENDING_FAILURE_DRIVE_CALIBRATION_RETRY_COUNT:
    out = "SPINDLE IMPENDING FAILURE DRIVE CALIBRATION RETRY COUNT"; break;
  case ASCQ::FIRMWARE_IMPENDING_FAILURE_GENERAL_HARD_DRIVE_FAILURE:
    out = "FIRMWARE IMPENDING FAILURE GENERAL HARD DRIVE FAILURE"; break;
  case ASCQ::FIRMWARE_IMPENDING_FAILURE_DRIVE_ERROR_RATE_TOO_HIGH:
    out = "FIRMWARE IMPENDING FAILURE DRIVE ERROR RATE TOO HIGH"; break;
  case ASCQ::FIRMWARE_IMPENDING_FAILURE_DATA_ERROR_RATE_TOO_HIGH:
    out = "FIRMWARE IMPENDING FAILURE DATA ERROR RATE TOO HIGH"; break;
  case ASCQ::FIRMWARE_IMPENDING_FAILURE_SEEK_ERROR_RATE_TOO_HIGH:
    out = "FIRMWARE IMPENDING FAILURE SEEK ERROR RATE TOO HIGH"; break;
  case ASCQ::FIRMWARE_IMPENDING_FAILURE_TOO_MANY_BLOCK_REASSIGNS:
    out = "FIRMWARE IMPENDING FAILURE TOO MANY BLOCK REASSIGNS"; break;
  case ASCQ::FIRMWARE_IMPENDING_FAILURE_ACCESS_TIMES_TOO_HIGH:
    out = "FIRMWARE IMPENDING FAILURE ACCESS TIMES TOO HIGH"; break;
  case ASCQ::FIRMWARE_IMPENDING_FAILURE_START_UNIT_TIMES_TOO_HIGH:
    out = "FIRMWARE IMPENDING FAILURE START UNIT TIMES TOO HIGH"; break;
  case ASCQ::FIRMWARE_IMPENDING_FAILURE_CHANNEL_PARAMETRICS:
    out = "FIRMWARE IMPENDING FAILURE CHANNEL PARAMETRICS"; break;
  case ASCQ::FIRMWARE_IMPENDING_FAILURE_CONTROLLER_DETECTED:
    out = "FIRMWARE IMPENDING FAILURE CONTROLLER DETECTED"; break;
  case ASCQ::FIRMWARE_IMPENDING_FAILURE_THROUGHPUT_PERFORMANCE:
    out = "FIRMWARE IMPENDING FAILURE THROUGHPUT PERFORMANCE"; break;
  case ASCQ::FIRMWARE_IMPENDING_FAILURE_SEEK_TIME_PERFORMANCE:
    out = "FIRMWARE IMPENDING FAILURE SEEK TIME PERFORMANCE"; break;
  case ASCQ::FIRMWARE_IMPENDING_FAILURE_SPIN_UP_RETRY_COUNT:
    out = "FIRMWARE IMPENDING FAILURE SPIN-UP RETRY COUNT"; break;
  case ASCQ::FIRMWARE_IMPENDING_FAILURE_DRIVE_CALIBRATION_RETRY_COUNT:
    out = "FIRMWARE IMPENDING FAILURE DRIVE CALIBRATION RETRY COUNT"; break;
  case ASCQ::MEDIA_IMPENDING_FAILURE_ENDURANCE_LIMIT_MET:
    out = "MEDIA IMPENDING FAILURE ENDURANCE LIMIT MET"; break;
  case ASCQ::FAILURE_PREDICTION_THRESHOLD_EXCEEDED_FALSE_:
    out = "FAILURE PREDICTION THRESHOLD EXCEEDED (FALSE)"; break;
  case ASCQ::LOW_POWER_CONDITION_ON:
    out = "LOW POWER CONDITION ON"; break;
  case ASCQ::IDLE_CONDITION_ACTIVATED_BY_TIMER:
    out = "IDLE CONDITION ACTIVATED BY TIMER"; break;
  case ASCQ::STANDBY_CONDITION_ACTIVATED_BY_TIMER:
    out = "STANDBY CONDITION ACTIVATED BY TIMER"; break;
  case ASCQ::IDLE_CONDITION_ACTIVATED_BY_COMMAND:
    out = "IDLE CONDITION ACTIVATED BY COMMAND"; break;
  case ASCQ::STANDBY_CONDITION_ACTIVATED_BY_COMMAND:
    out = "STANDBY CONDITION ACTIVATED BY COMMAND"; break;
  case ASCQ::IDLE_B_CONDITION_ACTIVATED_BY_TIMER:
    out = "IDLE_B CONDITION ACTIVATED BY TIMER"; break;
  case ASCQ::IDLE_B_CONDITION_ACTIVATED_BY_COMMAND:
    out = "IDLE_B CONDITION ACTIVATED BY COMMAND"; break;
  case ASCQ::IDLE_C_CONDITION_ACTIVATED_BY_TIMER:
    out = "IDLE_C CONDITION ACTIVATED BY TIMER"; break;
  case ASCQ::IDLE_C_CONDITION_ACTIVATED_BY_COMMAND:
    out = "IDLE_C CONDITION ACTIVATED BY COMMAND"; break;
  case ASCQ::STANDBY_Y_CONDITION_ACTIVATED_BY_TIMER:
    out = "STANDBY_Y CONDITION ACTIVATED BY TIMER"; break;
  case ASCQ::STANDBY_Y_CONDITION_ACTIVATED_BY_COMMAND:
    out = "STANDBY_Y CONDITION ACTIVATED BY COMMAND"; break;
  case ASCQ::POWER_STATE_CHANGE_TO_ACTIVE:
    out = "POWER STATE CHANGE TO ACTIVE"; break;
  case ASCQ::POWER_STATE_CHANGE_TO_IDLE:
    out = "POWER STATE CHANGE TO IDLE"; break;
  case ASCQ::POWER_STATE_CHANGE_TO_STANDBY:
    out = "POWER STATE CHANGE TO STANDBY"; break;
  case ASCQ::POWER_STATE_CHANGE_TO_SLEEP:
    out = "POWER STATE CHANGE TO SLEEP"; break;
  case ASCQ::POWER_STATE_CHANGE_TO_DEVICE_CONTROL:
    out = "POWER STATE CHANGE TO DEVICE CONTROL"; break;
  case ASCQ::CUSTOM_5F00:
    out = "ASCQ 5F00"; break;
  case ASCQ::LAMP_FAILURE:
    out = "LAMP FAILURE"; break;
  case ASCQ::VIDEO_ACQUISITION_ERROR:
    out = "VIDEO ACQUISITION ERROR"; break;
  case ASCQ::UNABLE_TO_ACQUIRE_VIDEO:
    out = "UNABLE TO ACQUIRE VIDEO"; break;
  case ASCQ::OUT_OF_FOCUS:
    out = "OUT OF FOCUS"; break;
  case ASCQ::SCAN_HEAD_POSITIONING_ERROR:
    out = "SCAN HEAD POSITIONING ERROR"; break;
  case ASCQ::END_OF_USER_AREA_ENCOUNTERED_ON_THIS_TRACK:
    out = "END OF USER AREA ENCOUNTERED ON THIS TRACK"; break;
  case ASCQ::PACKET_DOES_NOT_FIT_IN_AVAILABLE_SPACE:
    out = "PACKET DOES NOT FIT IN AVAILABLE SPACE"; break;
  case ASCQ::ILLEGAL_MODE_FOR_THIS_TRACK:
    out = "ILLEGAL MODE FOR THIS TRACK"; break;
  case ASCQ::INVALID_PACKET_SIZE:
    out = "INVALID PACKET SIZE"; break;
  case ASCQ::VOLTAGE_FAULT:
    out = "VOLTAGE FAULT"; break;
  case ASCQ::AUTOMATIC_DOCUMENT_FEEDER_COVER_UP:
    out = "AUTOMATIC DOCUMENT FEEDER COVER UP"; break;
  case ASCQ::AUTOMATIC_DOCUMENT_FEEDER_LIFT_UP:
    out = "AUTOMATIC DOCUMENT FEEDER LIFT UP"; break;
  case ASCQ::DOCUMENT_JAM_IN_AUTOMATIC_DOCUMENT_FEEDER:
    out = "DOCUMENT JAM IN AUTOMATIC DOCUMENT FEEDER"; break;
  case ASCQ::DOCUMENT_MISS_FEED_AUTOMATIC_IN_DOCUMENT_FEEDER:
    out = "DOCUMENT MISS FEED AUTOMATIC IN DOCUMENT FEEDER"; break;
  case ASCQ::CONFIGURATION_FAILURE:
    out = "CONFIGURATION FAILURE"; break;
  case ASCQ::CONFIGURATION_OF_INCAPABLE_LOGICAL_UNITS_FAILED:
    out = "CONFIGURATION OF INCAPABLE LOGICAL UNITS FAILED"; break;
  case ASCQ::ADD_LOGICAL_UNIT_FAILED:
    out = "ADD LOGICAL UNIT FAILED"; break;
  case ASCQ::MODIFICATION_OF_LOGICAL_UNIT_FAILED:
    out = "MODIFICATION OF LOGICAL UNIT FAILED"; break;
  case ASCQ::EXCHANGE_OF_LOGICAL_UNIT_FAILED:
    out = "EXCHANGE OF LOGICAL UNIT FAILED"; break;
  case ASCQ::REMOVE_OF_LOGICAL_UNIT_FAILED:
    out = "REMOVE OF LOGICAL UNIT FAILED"; break;
  case ASCQ::ATTACHMENT_OF_LOGICAL_UNIT_FAILED:
    out = "ATTACHMENT OF LOGICAL UNIT FAILED"; break;
  case ASCQ::CREATION_OF_LOGICAL_UNIT_FAILED:
    out = "CREATION OF LOGICAL UNIT FAILED"; break;
  case ASCQ::ASSIGN_FAILURE_OCCURRED:
    out = "ASSIGN FAILURE OCCURRED"; break;
  case ASCQ::MULTIPLY_ASSIGNED_LOGICAL_UNIT:
    out = "MULTIPLY ASSIGNED LOGICAL UNIT"; break;
  case ASCQ::SET_TARGET_PORT_GROUPS_COMMAND_FAILED:
    out = "SET TARGET PORT GROUPS COMMAND FAILED"; break;
  case ASCQ::ATA_DEVICE_FEATURE_NOT_ENABLED:
    out = "ATA DEVICE FEATURE NOT ENABLED"; break;
  case ASCQ::COMMAND_REJECTED:
    out = "COMMAND REJECTED"; break;
  case ASCQ::EXPLICIT_BIND_NOT_ALLOWED:
    out = "EXPLICIT BIND NOT ALLOWED"; break;
  case ASCQ::LOGICAL_UNIT_NOT_CONFIGURED:
    out = "LOGICAL UNIT NOT CONFIGURED"; break;
  case ASCQ::SUBSIDIARY_LOGICAL_UNIT_NOT_CONFIGURED:
    out = "SUBSIDIARY LOGICAL UNIT NOT CONFIGURED"; break;
  case ASCQ::DATA_LOSS_ON_LOGICAL_UNIT:
    out = "DATA LOSS ON LOGICAL UNIT"; break;
  case ASCQ::MULTIPLE_LOGICAL_UNIT_FAILURES:
    out = "MULTIPLE LOGICAL UNIT FAILURES"; break;
  case ASCQ::PARITY_DATA_MISMATCH:
    out = "PARITY/DATA MISMATCH"; break;
  case ASCQ::INFORMATIONAL_REFER_TO_LOG:
    out = "INFORMATIONAL:REFER TO LOG"; break;
  case ASCQ::STATE_CHANGE_HAS_OCCURRED:
    out = "STATE CHANGE HAS OCCURRED"; break;
  case ASCQ::REDUNDANCY_LEVEL_GOT_BETTER:
    out = "REDUNDANCY LEVEL GOT BETTER"; break;
  case ASCQ::REDUNDANCY_LEVEL_GOT_WORSE:
    out = "REDUNDANCY LEVEL GOT WORSE"; break;
  case ASCQ::REBUILD_FAILURE_OCCURRED:
    out = "REBUILD FAILURE OCCURRED"; break;
  case ASCQ::RECALCULATE_FAILURE_OCCURRED:
    out = "RECALCULATE FAILURE OCCURRED"; break;
  case ASCQ::COMMAND_TO_LOGICAL_UNIT_FAILED:
    out = "COMMAND TO LOGICAL UNIT FAILED"; break;
  case ASCQ::COPY_PROTECTION_KEY_EXCHANGE_FAILURE_AUTHENTICATION_FAILURE:
    out = "COPY PROTECTION KEY EXCHANGE FAILURE - AUTHENTICATION FAILURE"; break;
  case ASCQ::COPY_PROTECTION_KEY_EXCHANGE_FAILURE_KEY_NOT_PRESENT:
    out = "COPY PROTECTION KEY EXCHANGE FAILURE - KEY NOT PRESENT"; break;
  case ASCQ::COPY_PROTECTION_KEY_EXCHANGE_FAILURE_KEY_NOT_ESTABLISHED:
    out = "COPY PROTECTION KEY EXCHANGE FAILURE - KEY NOT ESTABLISHED"; break;
  case ASCQ::READ_OF_SCRAMBLED_SECTOR_WITHOUT_AUTHENTICATION:
    out = "READ OF SCRAMBLED SECTOR WITHOUT AUTHENTICATION"; break;
  case ASCQ::MEDIA_REGION_CODE_IS_MISMATCHED_TO_LOGICAL_UNIT_REGION:
    out = "MEDIA REGION CODE IS MISMATCHED TO LOGICAL UNIT REGION"; break;
  case ASCQ::DRIVE_REGION_MUST_BE_PERMANENT_REGION_RESET_COUNT_ERROR:
    out = "DRIVE REGION MUST BE PERMANENT/REGION RESET COUNT ERROR"; break;
  case ASCQ::INSUFFICIENT_BLOCK_COUNT_FOR_BINDING_NONCE_RECORDING:
    out = "INSUFFICIENT BLOCK COUNT FOR BINDING NONCE RECORDING"; break;
  case ASCQ::CONFLICT_IN_BINDING_NONCE_RECORDING:
    out = "CONFLICT IN BINDING NONCE RECORDING"; break;
  case ASCQ::INSUFFICIENT_PERMISSION:
    out = "INSUFFICIENT PERMISSION"; break;
  case ASCQ::INVALID_DRIVE_HOST_PAIRING_SERVER:
    out = "INVALID DRIVE-HOST PAIRING SERVER"; break;
  case ASCQ::DRIVE_HOST_PAIRING_SUSPENDED:
    out = "DRIVE-HOST PAIRING SUSPENDED"; break;
  case ASCQ::DECOMPRESSION_EXCEPTION_LONG_ALGORITHM_ID:
    out = "DECOMPRESSION EXCEPTION LONG ALGORITHM ID"; break;
  case ASCQ::SESSION_FIXATION_ERROR:
    out = "SESSION FIXATION ERROR"; break;
  case ASCQ::SESSION_FIXATION_ERROR_WRITING_LEAD_IN:
    out = "SESSION FIXATION ERROR WRITING LEAD-IN"; break;
  case ASCQ::SESSION_FIXATION_ERROR_WRITING_LEAD_OUT:
    out = "SESSION FIXATION ERROR WRITING LEAD-OUT"; break;
  case ASCQ::SESSION_FIXATION_ERROR_INCOMPLETE_TRACK_IN_SESSION:
    out = "SESSION FIXATION ERROR - INCOMPLETE TRACK IN SESSION"; break;
  case ASCQ::EMPTY_OR_PARTIALLY_WRITTEN_RESERVED_TRACK:
    out = "EMPTY OR PARTIALLY WRITTEN RESERVED TRACK"; break;
  case ASCQ::NO_MORE_TRACK_RESERVATIONS_ALLOWED:
    out = "NO MORE TRACK RESERVATIONS ALLOWED"; break;
  case ASCQ::RMZ_EXTENSION_IS_NOT_ALLOWED:
    out = "RMZ EXTENSION IS NOT ALLOWED"; break;
  case ASCQ::NO_MORE_TEST_ZONE_EXTENSIONS_ARE_ALLOWED:
    out = "NO MORE TEST ZONE EXTENSIONS ARE ALLOWED"; break;
  case ASCQ::CD_CONTROL_ERROR:
    out = "CD CONTROL ERROR"; break;
  case ASCQ::POWER_CALIBRATION_AREA_ALMOST_FULL:
    out = "POWER CALIBRATION AREA ALMOST FULL"; break;
  case ASCQ::POWER_CALIBRATION_AREA_IS_FULL:
    out = "POWER CALIBRATION AREA IS FULL"; break;
  case ASCQ::POWER_CALIBRATION_AREA_ERROR:
    out = "POWER CALIBRATION AREA ERROR"; break;
  case ASCQ::PROGRAM_MEMORY_AREA_UPDATE_FAILURE:
    out = "PROGRAM MEMORY AREA UPDATE FAILURE"; break;
  case ASCQ::PROGRAM_MEMORY_AREA_IS_FULL:
    out = "PROGRAM MEMORY AREA IS FULL"; break;
  case ASCQ::RMA_PMA_IS_ALMOST_FULL:
    out = "RMA/PMA IS ALMOST FULL"; break;
  case ASCQ::CURRENT_POWER_CALIBRATION_AREA_ALMOST_FULL:
    out = "CURRENT POWER CALIBRATION AREA ALMOST FULL"; break;
  case ASCQ::CURRENT_POWER_CALIBRATION_AREA_IS_FULL:
    out = "CURRENT POWER CALIBRATION AREA IS FULL"; break;
  case ASCQ::RDZ_IS_FULL:
    out = "RDZ IS FULL"; break;
  case ASCQ::SECURITY_ERROR:
    out = "SECURITY ERROR"; break;
  case ASCQ::UNABLE_TO_DECRYPT_DATA:
    out = "UNABLE TO DECRYPT DATA"; break;
  case ASCQ::UNENCRYPTED_DATA_ENCOUNTERED_WHILE_DECRYPTING:
    out = "UNENCRYPTED DATA ENCOUNTERED WHILE DECRYPTING"; break;
  case ASCQ::INCORRECT_DATA_ENCRYPTION_KEY:
    out = "INCORRECT DATA ENCRYPTION KEY"; break;
  case ASCQ::CRYPTOGRAPHIC_INTEGRITY_VALIDATION_FAILED:
    out = "CRYPTOGRAPHIC INTEGRITY VALIDATION FAILED"; break;
  case ASCQ::ERROR_DECRYPTING_DATA:
    out = "ERROR DECRYPTING DATA"; break;
  case ASCQ::UNKNOWN_SIGNATURE_VERIFICATION_KEY:
    out = "UNKNOWN SIGNATURE VERIFICATION KEY"; break;
  case ASCQ::ENCRYPTION_PARAMETERS_NOT_USEABLE:
    out = "ENCRYPTION PARAMETERS NOT USEABLE"; break;
  case ASCQ::DIGITAL_SIGNATURE_VALIDATION_FAILURE:
    out = "DIGITAL SIGNATURE VALIDATION FAILURE"; break;
  case ASCQ::ENCRYPTION_MODE_MISMATCH_ON_READ:
    out = "ENCRYPTION MODE MISMATCH ON READ"; break;
  case ASCQ::ENCRYPTED_BLOCK_NOT_RAW_READ_ENABLED:
    out = "ENCRYPTED BLOCK NOT RAW READ ENABLED"; break;
  case ASCQ::INCORRECT_ENCRYPTION_PARAMETERS:
    out = "INCORRECT ENCRYPTION PARAMETERS"; break;
  case ASCQ::UNABLE_TO_DECRYPT_PARAMETER_LIST:
    out = "UNABLE TO DECRYPT PARAMETER LIST"; break;
  case ASCQ::ENCRYPTION_ALGORITHM_DISABLED:
    out = "ENCRYPTION ALGORITHM DISABLED"; break;
  case ASCQ::SA_CREATION_PARAMETER_VALUE_INVALID:
    out = "SA CREATION PARAMETER VALUE INVALID"; break;
  case ASCQ::SA_CREATION_PARAMETER_VALUE_REJECTED:
    out = "SA CREATION PARAMETER VALUE REJECTED"; break;
  case ASCQ::INVALID_SA_USAGE:
    out = "INVALID SA USAGE"; break;
  case ASCQ::DATA_ENCRYPTION_CONFIGURATION_PREVENTED:
    out = "DATA ENCRYPTION CONFIGURATION PREVENTED"; break;
  case ASCQ::SA_CREATION_PARAMETER_NOT_SUPPORTED:
    out = "SA CREATION PARAMETER NOT SUPPORTED"; break;
  case ASCQ::AUTHENTICATION_FAILED:
    out = "AUTHENTICATION FAILED"; break;
  case ASCQ::EXTERNAL_DATA_ENCRYPTION_KEY_MANAGER_ACCESS_ERROR:
    out = "EXTERNAL DATA ENCRYPTION KEY MANAGER ACCESS ERROR"; break;
  case ASCQ::EXTERNAL_DATA_ENCRYPTION_KEY_MANAGER_ERROR:
    out = "EXTERNAL DATA ENCRYPTION KEY MANAGER ERROR"; break;
  case ASCQ::EXTERNAL_DATA_ENCRYPTION_KEY_NOT_FOUND:
    out = "EXTERNAL DATA ENCRYPTION KEY NOT FOUND"; break;
  case ASCQ::EXTERNAL_DATA_ENCRYPTION_REQUEST_NOT_AUTHORIZED:
    out = "EXTERNAL DATA ENCRYPTION REQUEST NOT AUTHORIZED"; break;
  case ASCQ::EXTERNAL_DATA_ENCRYPTION_CONTROL_TIMEOUT:
    out = "EXTERNAL DATA ENCRYPTION CONTROL TIMEOUT"; break;
  case ASCQ::EXTERNAL_DATA_ENCRYPTION_CONTROL_ERROR:
    out = "EXTERNAL DATA ENCRYPTION CONTROL ERROR"; break;
  case ASCQ::LOGICAL_UNIT_ACCESS_NOT_AUTHORIZED:
    out = "LOGICAL UNIT ACCESS NOT AUTHORIZED"; break;
  case ASCQ::SECURITY_CONFLICT_IN_TRANSLATED_DEVICE:
    out = "SECURITY CONFLICT IN TRANSLATED DEVICE"; break;
  case ASCQ::CUSTOM_7500:
    out = "ASCQ 7500"; break;
  case ASCQ::CUSTOM_7600:
    out = "ASCQ 7600"; break;
  case ASCQ::CUSTOM_7700:
    out = "ASCQ 7700"; break;
  case ASCQ::CUSTOM_7800:
    out = "ASCQ 7800"; break;
  case ASCQ::CUSTOM_7900:
    out = "ASCQ 7900"; break;
  case ASCQ::CUSTOM_7A00:
    out = "ASCQ 7A00"; break;
  case ASCQ::CUSTOM_7B00:
    out = "ASCQ 7B00"; break;
  case ASCQ::CUSTOM_7C00:
    out = "ASCQ 7C00"; break;
  case ASCQ::CUSTOM_7D00:
    out = "ASCQ 7D00"; break;
  case ASCQ::CUSTOM_7E00:
    out = "ASCQ 7E00"; break;
  case ASCQ::CUSTOM_7F00:
    out = "ASCQ 7F00"; break;
  // DO NOT HANDLE default
  }

  if ( out.empty() )
  {
    //case ASCQ::DIAGNOSTIC_FAILURE_ON_COMPONENT_NN_80h_FFh_: 0x40NN
    //out = "DIAGNOSTIC FAILURE ON COMPONENT NN (80h-FFh)"; break;
    //case ASCQ::TAGGED_OVERLAPPED_COMMANDS_NN_=_TASK_TAG_: 0x4DNN
    //out = "TAGGED OVERLAPPED COMMANDS (NN = TASK TAG)"; break;
    //case ASCQ::DECOMPRESSION_EXCEPTION_SHORT_ALGORITHM_ID_OF_NN: 0x70NN
    //out = "DECOMPRESSION EXCEPTION SHORT ALGORITHM ID OF NN"; break;
    out = "UNKNOWN ASCQ";
  }
  out += " [" + local::toString(v, true) + "]";

  return out;
}
