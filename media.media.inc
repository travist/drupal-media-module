<?php

/**
 * @file
 * Media module integration for the Media module.
 */

/**
 * Implements hook_media_browser_plugin_info().
 */
function media_media_browser_plugin_info() {
  $info['upload'] = array(
    'title' => t('Upload'),
    'weight' => -10,
    'class' => 'MediaBrowserUpload',
    // @todo Replace with appropriate file access function when that gets
    //   committed to File Entity project.
    'access callback' => 'media_access',
    'access arguments' => array('edit'),
  );

  $info['library'] = array(
    'title' => t('Library'),
    'weight' => 10,
    'class' => 'MediaBrowserLibrary',
    // @todo Replace with appropriate file access function when that gets
    //   committed to File Entity project.
    'access callback' => 'media_access',
    'access arguments' => array('view'),
  );

  // Add a plugin for each View display using the 'media_browser' display type.
  foreach (views_get_enabled_views() as $view) {
    foreach ($view->display as $display) {
      if ($display->display_plugin == 'media_browser') {
        $title = $display->display_title;
        if (!empty($display->display_options['title'])) {
          $title = $display->display_options['title'];
        }
        $info["{$view->name}--{$display->id}"] = array(
          'title' => $title,
          'weight' => 11, // @TODO make this configurable.
          'class' => 'MediaBrowserView',
          'view_name' => $view->name,
          'view_display_id' => $display->id,
        );
      }
    }
  }

  return $info;
}
