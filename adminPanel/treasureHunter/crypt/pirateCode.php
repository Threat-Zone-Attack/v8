<?php
function pirateCodeEncrypt($key, $string, $hexOutput){
	$keyLen = strlen($key);
	$stringLen = strlen($string);
	$result = "";
	$currSym;
	
	for ($i=0; $i<$stringLen; $i+=$keyLen){
		for ($j=$i; $j<$i+$keyLen && $j<$stringLen; $j++){
			$currSym = ord($string[$j]) + ord($key[$j-$i]);
			$result .= chr($currSym);
		}
	}
	
	if ($hexOutput)
		$result = strToHex($result);
	
	return $result;
}

function pirateCodeDecrypt($key, $encryptedOrig, $unhexInput){
	$keyLen = strlen($key);
	$result = "";
	$currSym;
	
	if ($unhexInput)
		$encrypted = hexToStr($encryptedOrig);
	else
		$encrypted = $encryptedOrig;
		
	$encryptedLen = strlen($encrypted);
	
	for ($i=0; $i<$encryptedLen; $i+=$keyLen){
		for ($j=$i; $j<$i+$keyLen && $j<$encryptedLen; $j++){
			$currSym = ord($encrypted[$j]) - ord($key[$j-$i]);
			$result .= chr($currSym);
		}
	}
	
	return $result;
}

//echo pirateCodeEncrypt("11111111111", "this is a test string and it is longer than the key", true);
//echo pirateCodeDecrypt("11111111111", "a5999aa4519aa4519251a596a4a551a4a5a39a9f9851929f95519aa5519aa4519da09f9896a351a599929f51a59996519c96aa", true);
?>