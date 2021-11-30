const char MAIN_page[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<body>

<h2>MK Smoke<h2>
<h3>Configuration</h3>

<form action="/action_page">
  On time (sec):<br>
  <input type="text" name="on_time" value="@@ON@@">
  <br>
  Off time (sec):<br>
  <input type="text" name="off_time" value="@@OFF@@">
  <br><br>
  <input type="submit" value="Save">
</form> 

</body>
</html>
)=====";
