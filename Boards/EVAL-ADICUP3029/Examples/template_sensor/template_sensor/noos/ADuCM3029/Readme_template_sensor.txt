            Analog Devices, Inc. Sensor Pack Application Example


Project Name: template_example_noos

Description:  Demonstrates the flow of sensor data to the host application via Bluetooth connectivity
              with the ADICUP3029. 
 

Overview:
=========
    This example demonstrates how to use any sensor to send data to the host app.
    The single threaded, non-RTOS program reads data from a random number generator and then
    sends the data to the host application via Bluetooth. The random number generator is a placeholder
    for a sensor read function.

    In debug mode, the data will be printed to the console. In release mode the data will be routed through 
    the UART to a host terminal. 



User Configuration Macros:
==========================
    ADI_APP_DISPATCH_TIMEOUT     (template_sensor_app.h) - This macro controls how frequently data samples are sent.
    ADI_APP_SIMULATE_SENSOR_TYPE (template_sensor_app.h) - This macro controls which sensor this demo will sumulate.
    													   The Android app will interpret the data being sent based 
    													   on this macro. It will display the data graph accordingly.

Hardware Setup:
===============
    In release mode the data will be printed to the host terminal. The UART redirection setup on the ADICUP3029 is detailed below:
        
        Switch       Position
        ======       ========
        S2           USB
        
Host App Setup:
===============
    Please refer to section "Installation" in the Android_Application_Users_Guide.pdf for details on
    how to install the Android app.

External connections:
=====================
    Connect the ADICUP3029 to the host PC using the USB cable.

How to build and run:
=====================
    In CCES, build the project ("Project" --> "Build All"). Then click one of the ".launch" files located in the Project
    Explorer, and then press "Debug" icon. This will open the "Debug" perspective in CCES. Run the program ("Run" -> "Resume").
    A shortcut in CCES to begin a debug session is to click on a ".launch" file and press F5 on the keyboard. 

    For a release build the output will be redirected to the host terminal via the UART. Click on "ADICUP3029_Release.launch" and 
    press F5 on the keyboard to begin the release build debug session. Before running the example, open a terminal (TeraTerm, Putty, etc) on the hose machine. Set up the serial port with the following settings:
        
        Serial Setup             Value
        ===========              ========
        Baud rate                9600
        Data                     8-bit
        Parity                   None
        Stop                     1-bit
        Flow control             None

    To use the demo:
    	1. Build and run the demo. 
    	2. On the phone/table open the Android application ADIotNode.
    	3. Press the "SCAN" button.
    	4. Click on "ADI_BLE_TEMPLATESENSOR" once it becomes available on the list of devices. This will create a connection to the ADICUP3029 board.
    	5. Click "DISCONNECT" to disconnect from the ADICUP3029 board.

Expected Result:
===============
	   Starting Template Sensor Example.

	   Waiting for connection. Initiate connection on central device please.

	   Connected!

	   GAP mode changed.

	   Data sent!

	   Data sent!

	   Data sent!
        
       (repeat until other BLE events occur)

References:
===========
    ADICUP3029 Schematic.
    ADI-SensorSoftware_1.0.0_Release_Notes.pdf
    EVAL-ADICUP3029_Users_Guide.pdf
    Android_Application_Users_Guide.pdf