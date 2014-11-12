// global array holding messages to be sent
var bus_msg_que = [];

var grabbed_data = false; // false until web scraping finishes

// Function to send a message to the Pebble using AppMessage API
function sendMessage() {
	
	var req = new XMLHttpRequest();
	var lat = "35.910092";
	var lon = "-79.05321789999999";
	var url = "https://www.nextbus.com/webkit/predsByLoc.jsp?lat="+lat+"&"+"lon="+lon+
							"&maxDis=2300&maxNumStops=99&accuracy=30&"+
							"caller=showPredsBasedOnLoc&moreStopsSelected=chapel-hill";
							
	//var url = "http://192.168.1.144/";
	req.open( "GET", url, true );
	req.responseType = "document";
	req.onload = function(e) {
		if (req.readyState == 4 && req.status == 200) {
			var currentNode = req.responseXML.getElementsByClassName("plainText")[0].childNodes[0].nextSibling;

			var route;
			var direction;
			var stop;
			var estimate;
			var num_stops_recorded = 0;

			while (currentNode && num_stops_recorded < 20) {
				if (currentNode.className == "inBetweenRouteSpacer") {
					route = currentNode.childNodes[1].innerHTML;
				} 
				else if (currentNode.className == "link normalDirLinkColor" ||
							currentNode.className == "link altDirLinkColor") {
					direction = currentNode.childNodes[1].childNodes[1].innerHTML;
					stop = currentNode.childNodes[1].childNodes[3].data.trim().substr(6);
					estimate = currentNode.childNodes[1].childNodes[7].innerHTML.split(" ")[0];

					if (estimate != "Arriving") {
						estimate = estimate + " min";
					}
					
					num_stops_recorded += 1;
					
					// send the recorded data to pebble, newlines denoted by '$' delimiter
					bus_msg_que.push({"1": route + "\n" +
											direction + "\n" +
											stop + "\n" + 
											estimate + "\n"});
				}
				currentNode = currentNode.nextElementSibling;
			}
			
			grabbed_data = true;
			
			//tell the c part that we're done
			bus_msg_que.push({"1":"done"});
			
		} else {
			Pebble.sendAppMessage({"2": "Error connecting to server."});
		}
		
		sendMsg();
		
	};
	req.send();
	
	// PRO TIP: If you are sending more than one message, or a complex set of messages, 
	// it is important that you setup an ackHandler and a nackHandler and call 
	// Pebble.sendAppMessage({ /* Message here */ }, ackHandler, nackHandler), which 
	// will designate the ackHandler and nackHandler that will be called upon the Pebble 
	// ack-ing or nack-ing the message you just sent. The specified nackHandler will 
	// also be called if your message send attempt times out.
}

function sendMsg() {
	if (grabbed_data === false) {
		sendMessage();
	}
	if (bus_msg_que.length > 0) {
		Pebble.sendAppMessage(bus_msg_que[0], sendNextMsg, sendMsg);
	}
}

function sendNextMsg() {
	if (bus_msg_que.length > 0) {
		bus_msg_que.shift();
		sendMsg();
	}
}


// Called when JS is ready
Pebble.addEventListener("ready", function(e) {
	sendMessage();
});												
// Called when incoming message from the Pebble is received
Pebble.addEventListener("appmessage",	
								function(e) {
									sendMessage();
								}
							);