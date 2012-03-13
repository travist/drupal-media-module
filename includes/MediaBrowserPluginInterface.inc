<?php

/**
 * @file
 * Definition of MediaBrowserPluginInterface.
 */

/**
 * Defines a Media browser plugin.
 *
 * Extends the MediaBrowserPluginInterface with methods expected by all
 * Media browser classes.
 */
interface MediaBrowserPluginInterface {
  /**
   * Set up the plugin class.
   *
   * @param $info
   *   An array of plugin info from hook_media_browser_plugin_info()
   *   implementations.
   * @param $params
   *   An array of parameters which came in is $_GET['params']. The expected
   *   parameters are still being defined.
   *   - 'types': array of media types to support
   *   - 'multiselect': boolean; TRUE enables multiselect
   */
  public function __construct($info, $params);
}
