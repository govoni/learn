// AJAX Hello World Demo http://www.hackorama.com/ajax

// Make a POST to the server 
// and pass on any data from browser
// via the XMLHTTPRequest

function talktoServer(){
	var req = newXMLHttpRequest();
	//register the callback handler function
  	var callbackHandler = getReadyStateHandler(req, updateMsgOnBrowser);
  	req.onreadystatechange = callbackHandler;
  	req.open("POST", "servertime.php", true);
  	req.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
  	//get the value from the text input element and send it to server
  	var testmsg = document.getElementById("testmsg");
  	var msg_value = testmsg.value;
  	req.send("msg="+msg_value);
}

// This is the callback functions that gets called
// for the response from the server with the XML data

var lastPing = 0;
function updateMsgOnBrowser(testXML) {

	var test = testXML.getElementsByTagName("test")[0];
	var message = testXML.getElementsByTagName("message")[0];
	var ip = testXML.getElementsByTagName("ip")[0];

	var timestamp = test.getAttribute("timestamp");
	if (timestamp > lastPing) {
		lastPing = timestamp;

		var ip_value = ip.firstChild.nodeValue;
		var message_value = message.firstChild.nodeValue;

		var msg_display = document.getElementById("msg_display");
		msg_display.innerHTML = " Server got the  message: \"" + 
			message_value + "\"" +
			"<br>Server IP: "+ ip_value + 
			" Server Timestamp: \""+ timestamp + "\"" ;
	}
}


//the following two functions are helper infrastructure to 
//craete a XMLHTTPRequest and register a listner callback function

function newXMLHttpRequest() {
	var xmlreq = false;
	if (window.XMLHttpRequest) {
		xmlreq = new XMLHttpRequest();
	} else if (window.ActiveXObject) {
    		// Try ActiveX
		try { 
			xmlreq = new ActiveXObject("Msxml2.XMLHTTP");
		} catch (e1) { 
			// first method failed 
			try {
				xmlreq = new ActiveXObject("Microsoft.XMLHTTP");
			} catch (e2) {
				 // both methods failed 
			} 
		}
 	}
   	return xmlreq;
} 

function getReadyStateHandler(req, responseXmlHandler) {
	return function () {
	if (req.readyState == 4) {
		if (req.status == 200) {
        		responseXmlHandler(req.responseXML);
		} else {
			var hellomsg = document.getElementById("hellomsg");
			hellomsg.innerHTML = "ERROR: "+ req.status;
      		}
    	}
 	}
}

