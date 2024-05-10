# HRM interface

## HRM connection

To connect HRM to Crossbox device:

* Turn on the Crossbox device
* Connect PC to Crossbox device using BLE
* Turn on your HRM device and bring it close to Crossbox
* Send *pair_hr_t* protobuf message with *delete_flag* set to 0 (*hrm_pair* in pc_interfacer app)
* When Crossbox connects to HRM it will print: "Heart Rate Monitor connected: \<NAME> \<MAC>"

After doing this Crossbox will save HRM device info for next connection.

## HRM connection delete

If you want to delete current HRM connection:

* Turn on the Crossbox device
* Connect PC to Crossbox device using BLE
* Send *pair_hr_t* protobuf message with *delete_flag* set to 1 (*hrm_pair_del* in pc_interfacer app)
* Crossbox will then delete saved connection info and print: "Deleting HRM Pair information."
  