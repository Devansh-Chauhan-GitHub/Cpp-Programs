<?php
//Simple function for Beta Plugin soon will be packages 
function ktg_single_content($url,$host,$key) {
	$king = getPage('https://app.kinggrabber.com/api/v1/host/'.$host.'/?url='.$url,$key);
	$json =  json_decode($king);
	if($json->code == 200 && $json->title != ''){
	return array(
		"status"=>$json->status,
		"title"=>trim(strip_tags($json->title)),
		"link"=>$json->data,
		"content"=>join("\n",$json->data),
		"total"=>count($json->data)
	);
	}else{
		if($json->message){
		return json_decode($king,1);
		}else{
			return array(
				'status'=>'failed',
				'message'=>'Check your URL its seem incorrect or something'
				);
		}
	}
}
function ktg_get_content($url,$host,$key) {
	$king = getPage('https://app.kinggrabber.com/api/v1/grab/'.$host.'/?type=1&url='.$url,$key);
	$json =  json_decode($king);
	if($json->code == 200 && $json->title != ''){
	return array(
		"status"=>$json->status,
		"title"=>$json->title,
		"slug"=>$json->slug,
		"content"=>$json->data,
		"judul"=>$json->judul,
		"url"=>$url,
		"chapter"=>trim($json->chapter)
	);
	}else{
		return json_decode($king,1);
	}
}
function ktg_check_api($key) {
	$king = getPage('https://app.kinggrabber.com/api/v1/check',$key);
	$data = json_decode($king);
	return $data;
}
function getPage($endpoint,$key){
$output = wp_remote_get($endpoint, array(
    'headers' => array(
        'X-KEY' => $key,
    ),
));
    return $output['body'];
}