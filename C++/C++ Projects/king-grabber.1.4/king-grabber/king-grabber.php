<?php
/**
 * @package King Grabber
 * @version 1.4
 */
/*
Plugin Name: King Grabber
Plugin URI: http://kinggrabber.com
Description: King Grabber, this plugin will grab your favorite Host Comic chapter and more Community service easyliy. 
Author: Fais
Version: 1.4
Author URI: http://fais.tech/
License: GPLv3 or later
*/

/*
Copyright (C) 2018  Fais (email : me@fais.tech)
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

define('ktg_ROOT', dirname(__FILE__));
include(dirname(__FILE__)."/ktg_settings.php");
include(dirname(__FILE__)."/ktg_function.php");
include(dirname(__FILE__)."/ktg_tools.php");
$loginizer = array();
register_activation_hook(__FILE__, 'ktg_SetDefaults');
register_uninstall_hook(__FILE__, 'ktg_delete_plugin');
add_action( 'admin_init', 'ktg_init' );
add_action('admin_menu', 'kinggrabber_menu');

// Shows the admin menu of King Grabber
function kinggrabber_menu() {
	
	global $wp_version, $kinggrabber;
	
	// Add the menu page
	add_menu_page(__('King Grabber Dashboard'), __('King Grabber'), 'activate_plugins', 'ktg_tools', 'ktg_tools', 'data:image/svg+xml;utf8;base64,PD94bWwgdmVyc2lvbj0iMS4wIiBlbmNvZGluZz0iaXNvLTg4NTktMSI/Pgo8IS0tIEdlbmVyYXRvcjogQWRvYmUgSWxsdXN0cmF0b3IgMTcuMS4wLCBTVkcgRXhwb3J0IFBsdWctSW4gLiBTVkcgVmVyc2lvbjogNi4wMCBCdWlsZCAwKSAgLS0+CjwhRE9DVFlQRSBzdmcgUFVCTElDICItLy9XM0MvL0RURCBTVkcgMS4xLy9FTiIgImh0dHA6Ly93d3cudzMub3JnL0dyYXBoaWNzL1NWRy8xLjEvRFREL3N2ZzExLmR0ZCI+CjxzdmcgeG1sbnM9Imh0dHA6Ly93d3cudzMub3JnLzIwMDAvc3ZnIiB4bWxuczp4bGluaz0iaHR0cDovL3d3dy53My5vcmcvMTk5OS94bGluayIgdmVyc2lvbj0iMS4xIiBpZD0iQ2FwYV8xIiB4PSIwcHgiIHk9IjBweCIgdmlld0JveD0iMCAwIDI2Ny41IDI2Ny41IiBzdHlsZT0iZW5hYmxlLWJhY2tncm91bmQ6bmV3IDAgMCAyNjcuNSAyNjcuNTsiIHhtbDpzcGFjZT0icHJlc2VydmUiIHdpZHRoPSIxNnB4IiBoZWlnaHQ9IjE2cHgiPgo8cGF0aCBkPSJNMjU2Ljk3NSwxMDAuMzRjMC4wNDEsMC43MzYtMC4wMTMsMS40ODUtMC4xOTgsMi4yMjlsLTE2LjUsNjZjLTAuODMyLDMuMzI1LTMuODEyLDUuNjYzLTcuMjM4LDUuNjgxbC05OSwwLjUgIGMtMC4wMTMsMC0wLjAyNSwwLTAuMDM4LDBIMzVjLTMuNDQ0LDAtNi40NDUtMi4zNDYtNy4yNzctNS42ODhsLTE2LjUtNjYuMjVjLTAuMTktMC43NjQtMC4yNDUtMS41MzQtMC4xOTctMi4yODkgIEM0LjY0Myw5OC41MTIsMCw5Mi41MzksMCw4NS41YzAtOC42ODUsNy4wNjUtMTUuNzUsMTUuNzUtMTUuNzVTMzEuNSw3Ni44MTUsMzEuNSw4NS41YzAsNC44OTEtMi4yNDEsOS4yNjctNS43NSwxMi4xNTggIGwyMC42NTgsMjAuODE0YzUuMjIxLDUuMjYxLDEyLjQ2Niw4LjI3NywxOS44NzgsOC4yNzdjOC43NjQsMCwxNy4xMi00LjE2MiwyMi4zODItMTEuMTM1bDMzLjk1LTQ0Ljk4NCAgQzExOS43NjYsNjcuNzgsMTE4LDYzLjg0MiwxMTgsNTkuNWMwLTguNjg1LDcuMDY1LTE1Ljc1LDE1Ljc1LTE1Ljc1czE1Ljc1LDcuMDY1LDE1Ljc1LDE1Ljc1YzAsNC4yMTItMS42NzIsOC4wMzUtNC4zNzUsMTAuODY0ICBjMC4wMDksMC4wMTIsMC4wMiwwLjAyMiwwLjAyOSwwLjAzNWwzMy43MDQsNDUuMTA4YzUuMjYsNy4wNCwxMy42NDYsMTEuMjQzLDIyLjQzNSwxMS4yNDNjNy40OCwwLDE0LjUxNC0yLjkxMywxOS44MDMtOC4yMDMgIGwyMC43ODgtMjAuNzg4QzIzOC4zMDEsOTQuODY5LDIzNiw5MC40NTEsMjM2LDg1LjVjMC04LjY4NSw3LjA2NS0xNS43NSwxNS43NS0xNS43NXMxNS43NSw3LjA2NSwxNS43NSwxNS43NSAgQzI2Ny41LDkyLjM1MSwyNjMuMDk1LDk4LjE3OCwyNTYuOTc1LDEwMC4zNHogTTIzOC42NjcsMTk4LjI1YzAtNC4xNDItMy4zNTgtNy41LTcuNS03LjVoLTE5NGMtNC4xNDIsMC03LjUsMy4zNTgtNy41LDcuNXYxOCAgYzAsNC4xNDIsMy4zNTgsNy41LDcuNSw3LjVoMTk0YzQuMTQyLDAsNy41LTMuMzU4LDcuNS03LjVWMTk4LjI1eiIgZmlsbD0iI0ZGRkZGRiIvPgo8Zz4KPC9nPgo8Zz4KPC9nPgo8Zz4KPC9nPgo8Zz4KPC9nPgo8Zz4KPC9nPgo8Zz4KPC9nPgo8Zz4KPC9nPgo8Zz4KPC9nPgo8Zz4KPC9nPgo8Zz4KPC9nPgo8Zz4KPC9nPgo8Zz4KPC9nPgo8Zz4KPC9nPgo8Zz4KPC9nPgo8Zz4KPC9nPgo8L3N2Zz4K');
	
	// Multi Grabber
	add_submenu_page('ktg_tools', __('King Grabber Multi'), __('Multi Grabber'), 'publish_posts', 'ktg_tools', 'ktg_tools');
	
	// Setting Plugin
	add_submenu_page('ktg_tools', __('King Grabber Settings'), __('Settings'), 'manage_options', 'ktg_settings', 'ktg_settings');
	
}
add_action( 'admin_menu', 'ktg_create_form' );
function ktg_create_form() {	
	add_meta_box( 'ktg-url-source', 'King Grabber', 'ktg_url_form', 'post', 'normal', 'high' );
}

add_action( 'admin_enqueue_scripts', 'ktg_enqueue' );
function ktg_enqueue($hook) {
	wp_enqueue_style('ktg_style', plugins_url('ktg_style.css?_='.time(), __FILE__));
	wp_enqueue_script('select2', plugins_url( '/component/kgsearch.min.js', __FILE__ ), array('jquery') );
	wp_enqueue_script( 'ajax-script', plugins_url( '/component/ktg_script.js?_='.time(), __FILE__ ), array('jquery') );	
	wp_localize_script( 'ajax-script', 'ktg_object',
	array( 
	'ajax_url' => admin_url( 'admin-ajax.php' )			
	) );
}


add_action( 'wp_ajax_ktg_grab_content', 'ktg_grab_content' );
function ktg_grab_content() {		
	$post = $_POST;
	$key = get_option('ktg_options')['api_key'];
	if(sanitize_text_field($post['url']) && sanitize_text_field($post['host'])) {
		$data = ktg_get_content(sanitize_text_field($post['url']),sanitize_text_field($post['host']),$key);
		header("Content-type: application/json");
		echo json_encode($data);		
	}
	wp_die();
}
function ktg_delete_plugin() {
	delete_option('ktg_options');
}

function ktg_init(){		
	register_setting( 'ktg_options', 'ktg_options','ktg_validate' );
}

function ktg_url_form( $object, $box ) {	
			$set = get_option('ktg_options');
?>		
				
	<div style="padding:10px;background: #dce4ea;box-shadow: 4px 4px 0px #b0bfcb;" class="wrap" id="ktg_tools">
		<div id="ktg_message"></div>	
				<select id="host_id" style="width:90%">
					<option disabled selected>Loading Host</option>
				</select>
	<input type="hidden" name="api_key" value="<?php echo $set['api_key'];?>">
	<input type="hidden" name="meta_komik" value="<?php echo $set['meta_komik'];?>">
	<input type="hidden" name="meta_chapter" value="<?php echo $set['meta_chapter'];?>">
	<input type="hidden" name="meta_judul" value="<?php echo $set['meta_judul'];?>">
	<table cellspacing='0'>
		<tr>
			<td width='98%'><input type='url' style='width:100%' id='ktg_source' /></td>
			<td width='5%'><button class='button button-primary' style='cursor:pointer' id='ktg_grab_button' onClick='ktg_js_grab()' type='button'>Grab Manga</button></td>
		</tr>
	</table>
	</div>
	<p class='howto' style="display: none">Enter URL Chapter <b class="myhosto"></b>, input only a valid URL chapter </p>

<?php }

