

const myHeaders = new Headers();

const myRequest = new Request("http://127.0.0.1:8080/~/in-cse/in-name/Ultrasonic_Sensor/node1/?rcn=4", {
  method: "GET",
  headers: {
    "X-M2M-Origin": "admin:admin",
    "Content-type": "application/json",
    "Access-Control-Allow-Origin": "*",
    "Access-Control-Allow-Credentials": "true",
  },
  mode: "cors",
});

var data = fetch(myRequest)
console.log(data);
