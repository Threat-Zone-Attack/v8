<?php
function connectToDB($server, $user, $password, $database){
	$connection = mysql_connect($server, $user, $password);
	$getDB = mysql_select_db($database);
	if (!$connection || !$getDB)
		return false;
	else
		return true;
}

function executeQuery($query){
	$execution = mysql_query($query);
	if (!$execution || (!is_bool($execution) && mysql_num_rows($execution) == 0))
		return false;
	else if (!is_bool($execution)){
		$results = array();
		while ($row = mysql_fetch_assoc($execution))
			$results[] = $row;
		return $results;
	}
	if ($execution && is_bool($execution))
		return $execution;
}

function executeMultiQuery($query){
	$queryExploded = explode(';', $query);
	$queryCount = count($queryExploded);

	for ($i=0; $i<$queryCount-1; $i++)
		if (!executeQuery($queryExploded[$i]))
			return false;
	return true;
}

function filterString($string){
	$string = str_replace('<', "&lt;", $string);
	$string = str_replace('>', "&gt;", $string);
	$string = str_replace("'", "&apos;", $string);
	$string = str_replace('"', "&quot;", $string);
	$string = mysql_real_escape_string($string);
	
	return $string;
}
?>