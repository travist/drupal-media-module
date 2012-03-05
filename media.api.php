<?php

/**
 * @file
 * Hooks provided by the Media module.
 */

/**
 * Returns a list of plugins for the media browser.
 *
 * Media provides a CTools plugin API; this is one of those hooks. It should
 * return a nested array of plugin information, keyed by plugin name. Each
 * plugin info array may have the following keys:
 * - title (required): A name for the tab in the media browser.
 * - class (required): The class name of the handler. This class must
 *   implement a view() method, and may (should) extend the
 *   @link MediaBrowserPlugin MediaBrowserPlugin @endlink class.
 * - weight (optional): Integer to determine the tab order. Defaults to 0.
 * - access callback (optional): A callback for user access checks.
 * - access arguments (optional): An array of arguments for the user access
 *   check.
 *
 * Additional custom keys may be provided for use by the handler.
 *
 * @see hook_media_browser_plugin_info_alter()
 * @see media_get_browser_plugin_info()
 */
function hook_media_browser_plugin_info() {
  $info['media_upload'] = array(
    'title' => t('Upload'),
    'class' => 'MediaBrowserUpload',
    'weight' => -10,
    'access callback' => 'user_access',
    'access arguments' => array('create files'),
  );

  return $info;
}

/**
 * Alter the list of plugins for the media browser.
 *
 * @see hook_media_browser_plugin_info()
 * @see media_get_browser_plugin_info()
 */
function hook_media_browser_plugin_info_alter() {
  $info['media_upload']['title'] = t('Upload 2.0');
  $info['media_upload']['class'] = 'MediaBrowserUploadImproved';
}
