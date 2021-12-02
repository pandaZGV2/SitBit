from onem2m import *
import random
uri_cse = "http://127.0.0.1:8080/~/in-cse/in-name"
ae = "Ultrasonic_Sensor"
ae1 = "Temperature_Sensor"
ae2 = "Humidity_Sensor"
cnt = "node1"

create_ae(uri_cse, ae)
uri_ae = uri_cse + "/" + ae
create_cnt(uri_ae, cnt)

create_ae(uri_cse, ae1)
uri_ae1 = uri_cse + "/" + ae1
create_cnt(uri_ae1, cnt)

create_ae(uri_cse, ae2)
uri_ae2 = uri_cse + "/" + ae2
create_cnt(uri_ae2, cnt)