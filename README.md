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



To run NATIVELY set the following in common.h

//Change device to csemi for native / hmi for target

static QString device = "csemi";

//Path to driver login database (passdb_driver)

static QString pathTopassdb_driver = "/home/csemi/qtworkspace_new/HMItemplate/password_driver.db";

//Path to maintaince login database (passdb)

static QString pathTopassdb = "/home/csemi/qtworkspace_new/HMItemplate/password.db";

//Path to base directory

static QString base_dir ="/home/"+device+"/qtworkspace_new/HMItemplate/";



To run on HMI set the following in common.h

//Change device to csemi for native / hmi for target

static QString device = "hmi";

//Path to driver login database (passdb_driver)

static QString pathTopassdb_driver = "/home/"+device+"/HMI/password_driver.db";


//Path to maintaince login database (passdb)

static QString pathTopassdb = "/home/"+device+"/HMI/password.db";

//Path to base directory

static QString base_dir ="/home/"+device+"/HMI/";

