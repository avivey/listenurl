var input = WScript.Arguments.Item(0);
var match = /^\s*([a-z]+:\/\/[a-zA-Z0-9_\.\/]+)\s*$/.exec(input);

if (match) {
  var shell = new ActiveXObject("WScript.Shell");
  shell.Run(match[1]);
} else {
  WScript.Echo("Bad input: " + input);
}
