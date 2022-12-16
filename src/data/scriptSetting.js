window.onload = function what() {
  renewSetting();
};
function renewSetting() {
  fetch("/setup")
    .then(function (response) {
      return response.json();
    })
    .then(function (array) {
      document.querySelector("#TSInterval").innerHTML = array[0];

      if (array[1] == "1") {
        document.querySelector("#storeTS").checked = true; //innerHTML = "checked";
        console.log("speichern");
      }
      //document.querySelector("#mySlider").innerHTML = array[1];
      document.querySelector("#mySlider").value = array[0];
    });
}

var slider = document.getElementById("mySlider");
var output = document.getElementById("TSInterval");
output.innerHTML = slider.value;

slider.oninput = function () {
  output.innerHTML = this.value;
};

//

	  /*
	  document.getElementById("myBtn").addEventListener("click", function() {
  alert("Hello World!");
        alert(document.getElementById("mySlider").value);
		alert(document.getElementById("storeTS").checked);

});
*/