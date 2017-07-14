<!--
	Sean Leisle
	blogGet gets and parse various blog post text files on the server and 
	returns a JSON encoded version of their contents
-->

<?php
	
	if (!isset($_GET["post"])) {
		die("No post specified");
	}

	$post = $_GET["post"];
	$filePath = "blog/";

	if (strstr($post, "all")) {
		$files = glob($filePath . "[0-9]*.txt");
		$posts = array();
		for ($i = 0; $i < sizeof($files); $i++) {
			$curFile = file_get_contents($files[$i]);
			$posts[$i] = separateFile($curFile, $i);
		}
		echo json_encode($posts);
	} elseif (strstr($post, "blogInfo")) {
		$file = file_get_contents(glob($filePath . $post . ".txt")[0]);
		$return = array();
		$curFile = explode("\n", $file);
		$return["featured"] = explode(",", $curFile[0]);
		echo json_encode($return);
	} else {
		$curFile = file_get_contents($filePath . $post . ".txt");
		$return = json_encode(separateFile($curFile, $post));
		echo $return;
	}

	function separateFile($curFile, $number) {
		$curSplit = explode("\n", $curFile);
		$cur = array();
		if (isset($curSplit[0])) {
			$cur["title"] = $curSplit[0];
		}
		if (isset($curSplit[1])) {
			$cur["date"] = $curSplit[1];
		}
		if (isset($curSplit[2])) {
			$cur["author"] = $curSplit[2];
		}
		if (isset($curSplit[3])) {
			$cur["content"] = $curSplit[3];
		}
		if (isset($number)) {
			$cur["number"] = $number;
		}

		return $cur;
	}

?>