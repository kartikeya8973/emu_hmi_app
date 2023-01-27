# emu_hmi_app
Source code of the HMI application for the EMU/MEMU rake

To Open Qt creator

$ cd qtcreator-5.0.3/bin

$ ./qtcreator

Directory for HMItemplate (Name of hmi ui application) Source Code

/home/csemi/qtworkspace_new/HMItemplate

Directory for compiled code. HMI has no cross compilation as HMI runs desktop applications.

/home/csemi/qtworkspace_hmi/build-HMItemplate-Desktop-Debug

Name of application

HMItemplate

ON HMI (AAEON-BT05) 

Location of HMI application - /home/root/HMI
	
To run the application

Use start-up.sh script

OR

$ export LD_LIBRARY_PATH=/opt/qt515/lib/
 
$ cd /home/hmi/HMI

$ ./HMItemplate
