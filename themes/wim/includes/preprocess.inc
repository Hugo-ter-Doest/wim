<?php
/**
 * @file
 * The primary PHP file for the Drupal Wim theme.
 *
 * This file contain implementations of the template_preprocess_HOOK() only.
 */

/**
 * Implements template_preprocess_node().
 */
function wim_preprocess_node(&$variables) {
  $node = $variables['node'];
  $view_mode = $variables['view_mode'];

  // Hook suggestion node--content_type--view_mode.tpl.php.
  $variables['theme_hook_suggestions'][] = 'node__' . $node->type . '__' . strtr($view_mode, '_', '-');

  // Add read more link variable to template.
  $variables['read_more_link'] = l(t('Read more'), $variables['node_url']);
}