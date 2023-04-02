<?php 
// include(dirname(__FILE__)."/ktg_function.php");


function ktg_SetDefaults() {
	$tmp = get_option('ktg_options');
	if($tmp === FALSE) {		
		$arr = array(	"api_key" => "TEST",
						"download_img" => 1,
						"meta_chapter" => "",
						"meta_komik" => "",
						"meta_judul" => "",
						"post_type" => "comic",
		);
		update_option('ktg_options', $arr);
	}
}
add_action( 'wp_ajax_ktg_api_check', 'ktg_api_check' );
function ktg_api_check() {		
	$post = $_POST;		
		header("Content-type: application/json");
	if(sanitize_text_field($post['action']) && sanitize_text_field($post['api_key'])) {
		$data = ktg_check_api(sanitize_text_field($post['api_key']));
		echo json_encode($data);		
	}else{
	echo json_encode(array('status'=>'failed','message'=>'API Key is Required to check'));
	}
	wp_die();
}
function ktg_settings() {	
?>
	<style>
		#ktg_settings {
			border:1px dotted #999999;
			box-shadow:1px 1px 10px #999999;
			padding:20px;
		}
		#ktg_title {
			font-weight:bold;
			border-bottom:3px double #cccccc;
		}
	</style>
	<div class='wrap' id='ktg_settings'>
		<div id='ktg_title'><h2>Setting King Grabber</h2></div>						
		<form method="post" action="options.php">
		<div id="ktg_message"></div>	
		<?php 
			settings_fields('ktg_options'); 			
			$set = get_option('ktg_options');
		?>				
				<table class="form-table">
			<tr valign="top">
			<th scope="row">API Key</th>
			<td>
				<input type="hidden" id="status_key" value="0">
				<input type="text" name="ktg_options[api_key]" id="api_key" style="width: 90%" placeholder="eyXXXXXXXXXXXXXXXXXXXXX" value="<?php echo @$set['api_key']?>">
			<a href="#" class="button" id="check_multi" onClick='ktg_api_check()' style="text-align: right;">Check API Key</a><br>
			</td>
			</tr>
			<tr valign="top">
			<th scope="row">Download Image</th>
			<td>
				<select name="ktg_options[download_img]" id="download_img" style="width: 100%" class="postform" disabled="">
	<option class="level-0" value="1" <?php echo @$set['download_img'] == 1 ? 'selected':''?>>True</option>
	<option class="level-0" value="0" <?php echo($set['download_img']  == 0 ? 'selected':'')?>>False</option>
</select>
			</td>
			</tr>
			<tr valign="top">
			<th scope="row">Meta Chapter</th>
			<td>
				<input type="text" name="ktg_options[meta_chapter]" style="width: 100%" placeholder="Empty if u dont know" value="<?php echo @$set['meta_chapter']?>">
			</td>
			</tr>
			<tr valign="top">
			<th scope="row">Meta Komik name</th>
			<td>
				<input type="text" name="ktg_options[meta_komik]" style="width: 100%" placeholder="Empty if u dont know" value="<?php echo @$set['meta_komik']?>">
			</td>
			</tr>
			<tr valign="top">
			<th scope="row">Meta Judul</th>
			<td>
				<input type="text" name="ktg_options[meta_judul]" style="width: 100%" placeholder="Empty if u dont know" value="<?php echo @$set['meta_judul']?>">
			</td>
			</tr>
			<tr valign="top">
			<th scope="row">Index Post Type</th>
			<td>
				<input type="text" name="ktg_options[post_type]" style="width: 100%" placeholder="comic" value="<?php echo @$set['post_type']?>">
			</td>
			</tr>
		</table>
			<button id="ktg_save" class="button button-primary"><?php _e('Save Changes') ?></button>
			<input type="hidden" id="ktg_sub" value="<?php _e('Save Changes') ?>" />	
		</form>	
	</div>
<?php 
}

function ktg_validate($input) {
		
		$input['api_key'] =  wp_filter_nohtml_kses($input['api_key']);
		$input['download_img'] =  wp_filter_nohtml_kses($input['download_img']);
		$input['meta_chapter'] =  wp_filter_nohtml_kses($input['meta_chapter']);
		$input['meta_komik'] =  wp_filter_nohtml_kses($input['meta_komik']);		
		$input['post_type'] =  wp_filter_nohtml_kses($input['post_type']);		
		
		return $input;
	}