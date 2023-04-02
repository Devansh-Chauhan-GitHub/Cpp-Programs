<?php 
// include(dirname(__FILE__)."/ktg_function.php");

function ktg_tools_page() {
	add_management_page('King Grabber Multi Grabber', 'King Grabber Multi', 'manage_options', 'ktg_tools', 'ktg_tools');
}


add_action( 'wp_ajax_ktg_multi_content', 'ktg_multi_content' );
function ktg_multi_content() {		
	$post = $_POST;		
		header("Content-type: application/json");
	$key = get_option('ktg_options')['api_key'];
	if(sanitize_text_field($post['url']) && sanitize_text_field($post['host'])) {
		$data = ktg_single_content(sanitize_text_field($post['url']),sanitize_text_field($post['host']),$key);
		echo json_encode($data);		
	}else{
	echo json_encode(array('status'=>'failed','message'=>'Parameter URL & Host is required to finding series'));
	}
	wp_die();
}
function get_the_categories( $parent = 0 ) 
{
    $categories = get_categories( "hide_empty=0&parent=$parent" );

    if ( $categories && count($categories) > 1) {
    	echo '<tr valign="top"><input type="hidden" id="kg_cat" value="1"><th scope="row">Categories Comic</th><td><select class="kg-search" id="kg_category" style="width:90%"><option disabled selected>Select Categories</option>';
        foreach ( $categories as $cat ) {
            if ( $cat->category_parent == $parent ) {
            	echo '<option value="'.$cat->term_id.'" >'.$cat->name.'</option>';
            }
        }
        echo '</td></tr></select>';
    }
}
add_action( 'wp_ajax_ktg_save_content', 'ktg_save_content' );
function ktg_save_content() {		
			$set = get_option('ktg_options');
	$post = $_POST;
	$key = $set['api_key'];
	if(sanitize_text_field($post['url']) && sanitize_text_field($post['host']) && sanitize_text_field($post['manga_id'])) {
		$data = ktg_get_content(sanitize_text_field($post['url']),sanitize_text_field($post['host']),$key);
		header("Content-type: application/json");
		if($data['status'] == 'success'){
    $user_id = get_current_user_id();

    $args = array(
        'post_author' => $user_id,
        'post_content'   => $data['content'],
        'post_content_filtered' => '',
        'post_title'     => $data['title'],
        'post_excerpt' => '',
        'post_status' => sanitize_text_field($post['post_type']),
        'post_type' => 'post',
        'comment_status' => '',
        'ping_status' => '',
        'post_password' => '',
        'to_ping' =>  '',
        'pinged' => '',
        'post_parent' => 0,
        'menu_order' => 0,
        'guid' => '',
        'import_id' => 0,
        'context' => '',
    );
    $post_id = @wp_insert_post($args);
		if(sanitize_text_field($post['category']) != '') {wp_set_post_categories( $post_id, array(sanitize_text_field($post['category'])) );}
		if(isset($set['meta_judul'])) add_post_meta($post_id, $set['meta_judul'], $data['judul']);
		if(isset($set['meta_chapter'])) add_post_meta($post_id, $set['meta_chapter'], str_pad($data['chapter'], 2, '0', STR_PAD_LEFT));
		if(isset($set['meta_komik'])) add_post_meta($post_id, $set['meta_komik'], sanitize_text_field($post['manga_id']));
	echo json_encode(array('error'=>0,'msg'=>'Successfully add '.$data['title'].' as Post ID '.$post_id));
}else{
	echo json_encode(array('error'=>2,'msg'=>sanitize_text_field($post['url']).' - '.$data['message']));
}
	}
	wp_die();
}
function ktg_tools() {	
?>
	<div class="poststuff wrap">
<div id="tools-body" class="metabox-holder">
<div style="width: 100%;" class="postbox-container">
<div class=""><div id="ktg-url-source" class="postbox ">
<h2 class="hndle" style="
    cursor: unset;
"><span>Multi Grabber by King Grabber</span></h2>

	<div class="inside wrap" id="ktg_tools" style="margin: 0px 0">
		<div id="ktg_message"></div>				
			<table class="form-table">
			<tr valign="top">
			<th scope="row">Host Grabber</th>
			<td>
				<select id="host_id" style="width:90%">
					<option disabled selected>Loading Host</option>
				</select>
			</td>
			</tr>
			<tr valign="top">
			<th scope="row">URL Series</th>
			<td>
				<input type="text" style='width:90%' name="series_url" id="series_url" placeholder="http://example.com">	
			<a href="#" class="button" id="check_multi" onClick='ktg_js_single()' style="text-align: right;">Find Series</a><br>
			<span id="title_series" style="padding: 5px;"></span>
			</td>
			</tr>

			<tr valign="top">
			<th scope="row">Local Comic</th>
			<td>
				<select id="manga_id" style="width:90%" class="kg-search">
					<option selected disabled>Select Comic</option>
				<?php
			$set = get_option('ktg_options');
	global $wpdb;
$result = $wpdb->get_results ( "
    SELECT * 
    FROM  $wpdb->posts
        WHERE `post_status` = 'publish' AND `post_type` = '".$set['post_type']."'
" );
foreach ( $result as $page )
{
	echo '<option value="'.$page->ID.'">'.$page->post_title.'</option>';
}
		?>
</select>
			</td>
			</tr>
				<?php get_the_categories();?>
			<tr valign="top" id="chapter_div" style="display: none">
			<th scope="row">Found <span id="total_found">0</span> Chapter</th>
			<td>
				<textarea rows='4' style='width:100%' id="list_multi"></textarea>		
			</td>
			</tr>
		</table>
    <form method="post" id="multime_form" style="display: none">
    <div align="center">
        <b>Timeout :</b> <input type="number" name="fail" id="fail" value="10" style="height: 30px;margin: 5px" />
        <b>Delay :</b> <input type="number" name="delay" id="delay" value="0" style="max-width:50px;height: 30px;margin: 5px" />s
        &nbsp;
				<select id="post_type">
					<option value="draft" selected>Draft</option>
					<option value="publish">Publish</option>
					<option value="pending">Pending Review</option>
				</select><br/>
        <input type="button" class = "submit-button" value="Start Grabber" id="submit_kg" />&nbsp;
        <input type="button" class = "stop-button" value="Stop" id="stop" /><br />
        <img id="loading" src="data:image/gif;base64,R0lGODlhAQABAPcAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACH5BAEAAP8ALAAAAAABAAEAAAgEAP8FBAA7" / style="margin: -10px;display: block;margin-left: auto;margin-right: auto;">
        <span id="checkStatus"></span><br>
        <span id="stat">Total <b id="total_count">0</b>/<b id="totaled">0</b></span>
    </div>
</form>
<div id="result">
    <fieldset class="fieldset">
        <legend class="sukses">Success: <span id="sukses_count">0</span></legend>
        <div id="sukses"></div>
    </fieldset>
    <fieldset class="fieldset">
        <legend class="gagal">Failed: <span id="gagal_count">0</span></legend>
        <div id="gagal"></div>
    </fieldset>
    <fieldset class="fieldset">
        <legend class="sukses">Unknown:</legend>
        <div id="wrong"></div>
    </fieldset>
</div>
</div>
</div>






</div></div>
</div><!-- /tools-body -->
<br class="clear">
</div>
<?php 
}
