<!--
	Sean Leisle
	cartProductInfo gets cart information from a SQL database table and
	returns a JSON encoded version of the data
-->

<?php
	$product;
	if (isset($_GET["cart"])) {
		$cart = $_GET["cart"];
	} else {
		die("No cart");
	}

	$servername = "";
	$databasename = "";
	$username = "";
	$password = "";

	try {
		$conn = new PDO("mysql:host=$servername;dbname=$databasename", $username, $password);
		$conn->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
	} catch (PDOException $e) {
		die("Connection failed: " . $e->getMessage());
	}

	// Create query
	$cartArray = explode("|", $cart);
	$query = "SELECT fileId, title, price, status FROM products WHERE ";
	for ($i = 0; $i < sizeof($cartArray); $i++) {
		$query .= "fileId = '" . $cartArray[$i] . "' OR "; 
	}
	$query = substr($query, 0, -4);

	$newDataRequest = $conn->prepare($query);
	$newDataRequest->execute();
	$newDataResult = $newDataRequest->setFetchMode(PDO::FETCH_ASSOC);
	$newData = $newDataRequest->fetchAll();
	$return = array();
	for ($i = 0; $i < sizeof($newData); $i++) {
		if ($newData[$i]["status"] != "0") {
			$newData[$i]["price"] = strval(intval($newData[$i]["price"]) * (intval($newData[$i]["status"]) / 100));
		}
		array_push($return, $newData[$i]);
	}

	echo json_encode($return);
?>