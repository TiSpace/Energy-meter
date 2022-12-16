
      function renew() {
        fetch("/messen")
          .then(function (response) {
            return response.json();
          })
          .then(function (array) {
            document.querySelector("#Spg").innerHTML = array[0] + " V";

            //
            document.querySelector("#Cur").innerHTML = array[1] + " A";
            document.querySelector("#pow").innerHTML = array[2] + " W";
            document.querySelector("#ene").innerHTML = array[3] + " kWh";
            document.querySelector("#freq").innerHTML = array[4] + " Hz";
            document.querySelector("#pf").innerHTML = array[5];
            document.querySelector("#compileDate").innerHTML =
              array[6].toLocaleString();
            document.querySelector("#myFile").innerHTML = array[7];
            document.querySelector("#myMAC").innerHTML = array[8];
            document.querySelector("#mySIG").innerHTML = array[9];
          document.querySelector("#mySSID").innerHTML = array[10];
          
		  document.querySelector("#myIP").innerHTML = array[11];
		  console.log(array[11]);
		  document.querySelector("#TSInterval").innerHTML = array[12];
		  console.log(array[12]);
		  
          });
		  
		  
        var datetime = new Date();
        //console.log(datetime);
        document.getElementById("time").textContent = datetime; //it will print on html page
      }
      document.addEventListener("DOMContentLoaded", renew);
      setInterval(renew, 60000);




		  


// Reset aufrufen, aber erst bestätigen
	   function resetMeasure(){
		 //window.location.href = "https://cloud.alphaess.com/";
		 //window.open("https://cloud.alphaess.com/",'_blank');
		 //href="/RST"
		 	  let isExecuted = confirm("Sicher den Zähler zurücksetzen?");
			console.log(isExecuted); // OK = true, Cancel = false   https://sebhastian.com/javascript-confirmation-yes-no/
			if (isExecuted){
			window.open("/RESET");
			}
	  
	   }