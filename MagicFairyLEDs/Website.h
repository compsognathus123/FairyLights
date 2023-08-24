
const char htmlPage[] PROGMEM = R"rawliteral(

<!DOCTYPE html>
<html>
  <head>
    <title>Magic Fairy LEDs</title>
    <style>
      body {
        text-align: center;
      }
      
      .container {
        max-width: 600px;
        margin: 0 auto;
        padding: 20px;

      }
      .containerLocation {
        display: grid;
        grid-template-columns: 1fr 1fr; /* Specify the number of columns */
        align-items: center; /* Vertically center the content */
      }
      .column {
        flex: 1; 
      }

      .header {
        font-size: 24px;
        background-color: #f9f9f9;
        padding: 10px;
        margin-bottom: 20px;
        text-align: center;
      }

      .slider-label {
        display: inline-block;
        margin-right: 10px;
    font-weight: bold;
      }

      .section-title {
        margin-bottom: 10px;
        padding: 5px;
        background-color: #f9f9f9;
        text-align: center;
      }

      .preview-image {
        margin: 10px auto;
        max-width: 100%;
        cursor: pointer;
      }

      .checkbox-label {
        display: inline-block;
        margin-left: 10px;
    font-weight: bold;
      }

      .action-button {
        margin: 10px auto;
        display: block;
        background-color: #007bff;
        color: #fff;
        padding: 8px 16px;
        border: none;
        border-radius: 4px;
        font-size: 16px;
        cursor: pointer;
      }

      .dropdown {
        margin: 10px auto;
        display: block;
        padding: 6px;
        font-size: 16px;
      }
    </style>
    <script>
      function locateLEDs() {
        var xhr = new XMLHttpRequest();
        xhr.open('GET', '/locate', true);
        xhr.send();
        <!--location.reload();-->
      }

      function updatePreview() {
        var image = document.getElementById("previewImage");
        var timestamp = new Date().getTime();
        image.src = "/cam_preview?timestamp=" + timestamp;
      }

      function onDropdownChange() {
        var dropdown = document.getElementById("effectDropdown");
        var selectedEffect = dropdown.value;

        var xhr = new XMLHttpRequest();
        xhr.open('GET', '/set_settings?effect=' + selectedEffect, true);
        xhr.send();
      }

      function onSliderSpeedChange() {
        var sliderSpeed = document.getElementById("sliderSpeed");
        var speedValue = sliderSpeed.value;

        var xhr = new XMLHttpRequest();
        xhr.open('GET', '/set_settings?speed=' + speedValue, true);
        xhr.send();
      }

      function onSliderSizeChange() {
        var sliderSize = document.getElementById("sliderSize");
        var sizeValue = sliderSize.value;

        var xhr = new XMLHttpRequest();
        xhr.open('GET', '/set_settings?size=' + sizeValue, true);
        xhr.send();
      }
      
      function onSliderOverallBrightness() {
        var sliderBrightness = document.getElementById("sliderBrightness");
        var brightnessValue = sliderBrightness.value;

        var xhr = new XMLHttpRequest();
        xhr.open('GET', '/set_settings?overall_brightness=' + brightnessValue, true);
        xhr.send();
      }
      
      function onCheckboxCropChange() {
        var checkbox = document.getElementById("checkboxCrop");
        var ischecked = checkbox.checked;

        var xhr = new XMLHttpRequest();
        xhr.open('GET', '/set_settings?crop=' + ischecked, true);
        xhr.send();
      }
    </script>
  </head>
  <body>
    <div class="container">
      <h2 class="header">Magic Fairy LEDs</h2>
    
      <label for="sliderBrightness" class="slider-label">Brightness</label>
      <input type="range" id="sliderBrightness" min="0" max="255" value="128" onchange="onSliderOverallBrightness()">
      <br>
      
      <h3 class="section-title">Locating Algorithm</h3>
    
    <div class="containerLocation">
  <div class="column">
      <img id="previewImage" onclick="updatePreview()" src="/cam_preview" class="preview-image">
      <br>Click image to update.<br>
      <br>
    </div>
  
  <div class="column">
      <input type="checkbox" id="checkboxCrop" onchange="onCheckboxCropChange()">
      <label for="checkboxCrop" class="checkbox-label">Crop to LEDs</label>

      <br>
      <button type="button" onclick="locateLEDs()" class="action-button">Start Locating</button>
    </div>
    </div>

      <h3 class="section-title">Effect Magic</h3>
      <select id="effectDropdown" onchange="onDropdownChange()" class="dropdown">
        <option value="Vertical sinus">Vertical sinus</option>
        <option value="Radial sinus">Radial sinus</option>
        <option value="2D Noise">2D Noise</option>
      </select>
      <br>
      
      <label for="sliderSpeed" class="slider-label">Speed</label>
      <input type="range" id="sliderSpeed" min="1" max="254" value="128" onchange="onSliderSpeedChange()">
      <br>
      <br>
      
      <label for="sliderSize" class="slider-label">Size</label>
      <input type="range" id="sliderSize" min="1" max="254" value="128" onchange="onSliderSizeChange()">
    </div>
  </body>
</html>

)rawliteral";
