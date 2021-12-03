from onem2m import *

uri_cse = "http://127.0.0.1:8080/~/in-cse/in-name/"

ae = "Ultrasonic_Sensor"
delete_ae(uri_cse + ae)

ae1 = "Temperature_Sensor"
delete_ae(uri_cse + ae1)

ae2 = "Humidity_Sensor"
delete_ae(uri_cse + ae2)

ae3 = "Heat_Index"
delete_ae(uri_cse + ae3)

ae4 = "Threshold_Values"
delete_ae(uri_cse + ae4)
