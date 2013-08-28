<?php
error_reporting(E_ALL);

// echo "<h2>TCP/IP Connection</h2>\n";
// 
// 	/* Get the port for the WWW service. */
// 	$service_port = "5005";//getservbyname('www', 'tcp');
// 
// 	/* Get the IP address for the target host. */
// 	$address ="192.168.33.50";// gethostbyname('www.example.com');
// 
// /* Create a TCP/IP socket. */
// $socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
// if ($socket === false) {
//     echo "socket_create() failed: reason: " . socket_strerror(socket_last_error()) . "\n";
// } else {
//     echo "OK.\n";
// }
// 
// echo "Attempting to connect to '$address' on port '$service_port'...";
// $result = socket_connect($socket, $address, $service_port);
// if ($result === false) {
//     echo "socket_connect() failed.\nReason: ($result) " . socket_strerror(socket_last_error($socket)) . "\n";
// } else {
//     echo "OK.\n";
// }
// 
// $in = "setText::02::Aggggooo:O";
// 
// $out = '';
// 
// echo "Sending HTTP HEAD request...";
// socket_write($socket, $in, strlen($in));
// echo "OK.\n";
// 
// echo "Reading response:\n\n";
// while ($out = socket_read($socket, 2048)) {
//     echo $out;
// }
// 
// echo "Closing socket...";
// socket_close($socket);
// echo "OK.\n\n";

class busCanSocket{
	private $menu;
	/* Get the port for the WWW service. */
	private $service_port = "5005";//getservbyname('www', 'tcp');

	/* Get the IP address for the target host. */
	private $address ="192.168.33.50";// gethostbyname('www.example.com');

	private $socket;
	
	public function sendCommand($msg=""){
		/* Create a TCP/IP socket. */
		$this->socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
		if ($this->socket === false) {
			echo "socket_create() failed: reason: " . socket_strerror(socket_last_error()) . "\n";
		} else {
			echo "OK.\n";
		}

		echo "Attempting to connect to '{$this->address}' on port '$service_port'...";
		$result = socket_connect($this->socket, $this->address, $this->service_port);
		if ($result === false) {
			echo "socket_connect() failed.\nReason: ($result) " . socket_strerror(socket_last_error($this->socket)) . "\n";
		} else {
			echo "OK.\n";
		}

		$in = "setText::02::Aggggooo:O";

		$out = '';

		echo "Sending HTTP HEAD request...";
		socket_write($this->socket, $msg, strlen($msg));
		echo "OK.\n";

		echo "Reading response:\n\n";
		while ($out = socket_read($this->socket, 2048)) {
		echo $out;
		}

		echo "Closing socket...";
		socket_close($this->socket);
		echo "OK.\n\n";
	}
	public function printText(){
		echo '<form action="server.php" method="post">'
			.'<p>Comando: <input type="text" name="cmd" /></p>'
			. '<p>Nodo: <input type="text" name="node" /></p>'
			.'<p>Argum: <input type="text" name="args" /></p>'
			.'<p><input type="submit" /></p>'
			.'</form>';
	}
	public function main(){
		$this->printText();
		if (isset($_POST["cmd"])){
			$this->sendCommand($_POST["cmd"]."::".$_POST["node"]."::".$_POST["args"]);
		}
	}	

}
$program= new busCanSocket();
$program->main();
?>