from onem2m import *

uri_cse = "http://127.0.0.1:8080/~/in-cse/in-name"

cnt = "node1"

ae = "Ultrasonic_Sensor"
create_ae(uri_cse, ae)
uri_ae = uri_cse + "/" + ae
create_cnt(uri_ae, cnt)

ae1 = "Temperature_Sensor"
create_ae(uri_cse, ae1)
uri_ae1 = uri_cse + "/" + ae1
create_cnt(uri_ae1, cnt)

ae2 = "Humidity_Sensor"
create_ae(uri_cse, ae2)
uri_ae2 = uri_cse + "/" + ae2
create_cnt(uri_ae2, cnt)

ae3 = "Heat_Index"
create_ae(uri_cse, ae3)
uri_ae3 = uri_cse + "/" + ae3
create_cnt(uri_ae3, cnt)

ae4 = "Alerts"
create_ae(uri_cse, ae4)
uri_ae4 = uri_cse + "/" + ae4
create_cnt(uri_ae4, cnt)
