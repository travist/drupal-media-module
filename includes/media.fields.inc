<?php

/**
 * @file
 * Provide the media file selector widget and media field formatters to the
 * Fields API.
 */

/**
 * Implements hook_field_widget_info().
 */
function media_field_widget_info() {
  return array(
    'media_generic' => array(
      'label' => t('Media file selector'),
      'field types' => array('file', 'image'),
      'settings' => array(
        'progress_indicator' => 'throbber',
        'allowed_types' => array('image'),
        'browser_plugins' => array(),
        'allowed_schemes' => array('public', 'private'),
      ),
      'behaviors' => array(
        'multiple values' => FIELD_BEHAVIOR_DEFAULT,
        'default value' => FIELD_BEHAVIOR_NONE,
      ),
    ),
  );
}

/**
 * Implements hook_field_formatter_info().
 *
 * Provides legacy support for the "Large filetype icon" file field formatter.
 * This was originally used when media entities contained file fields. The
 * current file entity architecture no longer needs this, but people may
 * have used this formatter for other file fields on their website.
 *
 * @todo Some day, remove this.
 */
function media_field_formatter_info() {
  $formatters = array(
    'media_large_icon' => array(
      'label' => t('Large filetype icon'),
      'field types' => array('file'),
    ),
    'media_player' => array(
      'label' => t('Media Player'),
      'description' => t('Play this file within a Media Player.'),
      'field types' => array('file', 'text'),
      'settings' => array(
        'template' => 'default',
        'preload' => TRUE,
        'autoplay' => FALSE,
        'loop' => FALSE,
        'width' => '100%',
        'height' => '400px',
        'volume' => 80,
        'sources' => FALSE,
        'debug' => FALSE
      )
    ),
  );
  return $formatters;
}

/**
 * Implements hook_field_formatter_settings_form
 */
function media_field_formatter_settings_form($field, $instance, $view_mode, $form, &$form_state) {
  $display = $instance['display'][$view_mode];
  $settings = $display['settings'];
  $element = array();
  if ($display['type'] == 'media_player') {

    // Get the player information and templates.
    $info = media_get_player_info();
    $templates = array_keys($info['templates']);
    $templates = array_combine($templates, $templates);

    $element['template'] = array(
      '#title' => t('Template'),
      '#type' => 'select',
      '#options' => $templates,
      '#default_value' => $settings['template']
    );

    $element['preload'] = array(
      '#title' => t('Preload'),
      '#type' => 'checkbox',
      '#default_value' => $settings['preload']
    );

    $element['autoplay'] = array(
      '#title' => t('Autoplay'),
      '#type' => 'checkbox',
      '#default_value' => $settings['autoplay']
    );

    $element['loop'] = array(
      '#title' => t('Loop'),
      '#type' => 'checkbox',
      '#default_value' => $settings['loop']
    );

    $element['width'] = array(
      '#title' => t('Width'),
      '#type' => 'textfield',
      '#default_value' => $settings['width']
    );

    $element['height'] = array(
      '#title' => t('Height'),
      '#type' => 'textfield',
      '#default_value' => $settings['height']
    );

    $element['volume'] = array(
      '#title' => t('Initial Volume (0 - 100)'),
      '#type' => 'textfield',
      '#default_value' => $settings['volume']
    );

    $element['sources'] = array(
      '#title' => t('Allow multiple sources'),
      '#description' => t('Checking this will turn multiple instances of files into multiple sources within the media element.'),
      '#type' => 'checkbox',
      '#default_value' => $settings['sources']
    );

    $element['debug'] = array(
      '#title' => t('Debug Mode'),
      '#type' => 'checkbox',
      '#default_value' => $settings['debug']
    );
  }
  return $element;
}

/**
 * Implements hook_field_formatter_settings_summary
 */
function media_field_formatter_settings_summary($field, $instance, $view_mode) {
  $display = $instance['display'][$view_mode];
  $settings = $display['settings'];
  $summary = '';
  if ($display['type'] == 'media_player') {
    $header = array('Setting', 'Value');
    $rows = array();
    foreach ($settings as $name => $value) {
      $rows[] = array($name, $value);
    }
    $summary = theme('table', array('header' => $header, 'rows' => $rows));
  }
  return $summary;
}

/**
 * Returns the settings for this video or audio element.
 */
function media_player_get_attributes($settings) {
  $attributes = array();
  $element_settings = array('preload', 'autoplay', 'loop');
  foreach ($settings as $name => $value) {
    if ($value && in_array($name, $element_settings)) {
      $attributes[$name] = NULL;
    }
  }

  // Set the ID, width and height.
  $attributes['id'] = $settings['id'] . '-player';
  $attributes['width'] = '100%';
  $attributes['height'] = '100%';
  return $attributes;
}

/**
 * Implements hook_field_formatter_view().
 *
 * Legacy support for the "Large filetype icon" file field formatter.
 * @see media_field_formatter_info()
 */
function media_field_formatter_view($entity_type, $entity, $field, $instance, $langcode, $items, $display) {
  $element = array();
  switch ($display['type']) {
    case 'media_large_icon':
      foreach ($items as $delta => $item) {
        $element[$delta] = array(
          '#theme' => 'media_formatter_large_icon',
          '#file' => (object) $item,
        );
      }
      break;
    case 'media_player':
      // Get the display settings.
      $settings = $display['settings'];

      // Get the ID for this media player.
      $id = 'player-' . drupal_clean_css_identifier($field['field_name']);

      // If they wish to show all sources within a single media element.
      if ($settings['sources']) {

        // Get the media tag.
        $mediatag = '';
        foreach ($items as $delta => $item) {
          if ($mediatag = media_get_media_type((object)$item)) {
            break;
          }
        }

        // If the mediatag exists, then theme the player.
        if ($mediatag) {
          $settings['id'] = $id;
          $element[$delta] = array(
            '#theme' => 'media_player',
            '#tag' => $mediatag,
            '#attributes' => media_player_get_attributes($settings),
            '#settings' => $settings,
            '#sources' => $items
          );
        }
      }
      else {

        // Iterate through all the items.
        foreach ($items as $delta => $item) {

          // Get the media tag.
          if ($mediatag = media_get_media_type((object)$item)) {
            $settings['id'] = $id . '-' . $delta;
            $element[$delta] = array(
              '#theme' => 'media_player',
              '#tag' => $mediatag,
              '#attributes' => media_player_get_attributes($settings),
              '#settings' => $settings,
              '#sources' => array($item)
            );
          }
        }
      }
      break;
  }

  return $element;
}

/**
 * Implements hook_field_widget_settings_form().
 */
function media_field_widget_settings_form($field, $instance) {
  $widget = $instance['widget'];
  $settings = $widget['settings'];
  $form = array();

  // Setup type selection form
  $types = media_type_get_types();
  $options = array();
  foreach ($types as $key => $definition) {
    $options[$key] = $definition->label;
  }

  $streams = file_get_stream_wrappers();

  $form['allowed_types'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Allowed remote media types'),
    '#options' => $options,
    '#default_value' => $settings['allowed_types'],
    '#description' => t('Media types which are allowed for this field when using remote streams.'),
    '#weight' => 1,
    '#access' => count(file_get_stream_wrappers(STREAM_WRAPPERS_LOCAL)) != count($streams),
  );

  $options = array();
  unset($streams['temporary']);
  foreach ($streams as $scheme => $data) {
    $options[$scheme] = t('@scheme (@name)', array('@scheme' => $scheme . '://', '@name' => $data['name']));
  }
  $form['allowed_schemes'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Allowed URI schemes'),
    '#options' => $options,
    '#default_value' => $settings['allowed_schemes'],
    '#description' => t('URI schemes include public:// and private:// which are the Drupal files directories, and may also refer to remote sites.'),
    '#weight' => 2,
  );

  $plugins = media_get_browser_plugin_info();
  $form['browser_plugins'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Enabled browser plugins'),
    '#options' => array(),
    '#default_value' => $settings['browser_plugins'],
    '#description' => t('If no plugins are selected, they will all be available.'),
  );
  foreach ($plugins as $key => $plugin) {
    $form['browser_plugins']['#options'][$key] = !empty($plugin['title']) ? $plugin['title'] : $key;
  }

  return $form;
}

/**
 * Implements hook_field_widget_form().
 */
function media_field_widget_form(&$form, &$form_state, $field, $instance, $langcode, $items, $delta, $element) {
  $field_settings = $instance['settings'];
  $widget_settings = $instance['widget']['settings'];

  // @todo The Field API supports automatic serialization / unserialization, so
  //   this should no longer be needed. After verifying with a module that uses
  //   the 'data' column, remove this.
  // @see media_field_widget_value()
  $current_value = array();
  if (isset($items[$delta])) {
    $current_value = $items[$delta];
    // @todo $items[$delta] is sometimes a loaded media entity (an object)
    //   rather than an array. This conflicts with Field API expectations (for
    //   example, it results in fatal errors when previewing a node with a
    //   multi-valued media field), so should be fixed. In the meantime, don't
    //   assume that $current_value is an array.
    if (is_array($current_value) && isset($current_value['data']) && is_string($current_value['data'])) {
      $current_value['data'] = unserialize($current_value['data']);
    }
  }

  $element += array(
    '#type' => 'media', // Would like to make this a fieldset, but throws some weird warning about element_children... not sure what it is about yet.
    '#collapsed' => TRUE,
    '#default_value' => $current_value,
    '#required' => $instance['required'],
    '#media_options' => array(
      'global' => array(
        'types' => array_filter($widget_settings['allowed_types']),
        'enabledPlugins' => array_filter($instance['widget']['settings']['browser_plugins']),
        'schemes' => $widget_settings['allowed_schemes'],
        'file_directory' => isset($field_settings['file_directory']) ? $field_settings['file_directory'] : '',
        'file_extensions' => isset($field_settings['file_extensions']) ? $field_settings['file_extensions'] : media_variable_get('file_extensions'),
        'max_filesize' => isset($field_settings['max_filesize']) ? $field_settings['max_filesize'] : 0,
        'uri_scheme' => !empty($field['settings']['uri_scheme']) ? $field['settings']['uri_scheme'] : file_default_scheme(),
      ),
    ),
  );

  if ($field['type'] == 'file') {
    $element['display'] = array(
      '#type' => 'value',
      '#value' => 1,
    );
  }

  // Add image field specific validators.
  if ($field['type'] == 'image') {
    if ($field_settings['min_resolution'] || $field_settings['max_resolution']) {
      $element['#media_options']['global']['min_resolution'] = $field_settings['min_resolution'];
      $element['#media_options']['global']['max_resolution'] = $field_settings['max_resolution'];
    }
  }

  return $element;
}

/**
 * @todo Is this function ever called? If not, remove it. The Field API now
 *   supports automatic serialization / unserialization, so this should no
 *   longer be needed. After verifying with a module that uses the 'data'
 *   column, remove this.
 *
 * @see media_field_widget_form()
 */
function media_field_widget_value($element, $input, $form_state) {
  $return = $input;

  if (!is_array($return)) {
    $return = array();
  }

  if (isset($return['data'])) {
    $return['data'] = serialize($return['data']);
  }

  $return += array(
    'fid' => 0,
    'title' => '',
    'data' => NULL,
  );

  return $return;
}
