<?php

/**
 * @file
 * Definition of views_entity_terms_plugin_argument_default_taxonomy_tid.
 */

/**
 * Taxonomy tid default argument extended to find terms referenced by entityreference.
 */
class views_entity_terms_plugin_argument_default_taxonomy_tid extends views_plugin_argument_default_taxonomy_tid {

  function option_definition() {
    $options = parent::option_definition();

    // Add option to use terms from entityreference.
    $options['node_term_entity'] = array('default' => FALSE, 'bool' => TRUE);

    return $options;
  }

  function options_form(&$form, &$form_state) {
    $form['term_page'] = array(
      '#type' => 'checkbox',
      '#title' => t('Load default filter from term page'),
      '#default_value' => $this->options['term_page'],
    );
    $form['node'] = array(
      '#type' => 'checkbox',
      '#title' => t('Load default filter from node page, that\'s good for related taxonomy blocks'),
      '#default_value' => $this->options['node'],
    );

    // Add form element for new option, dependent on the "default filter from node page" option.
    $form['node_term_entity'] = array(
      '#type' => 'checkbox',
      '#title' => t('Use entity referenced terms'),
      '#default_value' => $this->options['node_term_entity'],
      '#process' => array('form_process_checkbox', 'ctools_dependent_process'),
      '#dependency' => array(
        'edit-options-argument-default-taxonomy-tid-node' => array(1),
      ),
    );

    $form['limit'] = array(
      '#type' => 'checkbox',
      '#title' => t('Limit terms by vocabulary'),
      '#default_value' => $this->options['limit'],
      '#process' => array('form_process_checkbox', 'ctools_dependent_process'),
      '#dependency' => array(
        'edit-options-argument-default-taxonomy-tid-node' => array(1),
      ),
    );

    $options = array();
    $vocabularies = taxonomy_get_vocabularies();
    foreach ($vocabularies as $voc) {
      $options[$voc->machine_name] = check_plain($voc->name);
    }

    $form['vocabularies'] = array(
      '#prefix' => '<div><div id="edit-options-vids">',
      '#suffix' => '</div></div>',
      '#type' => 'checkboxes',
      '#title' => t('Vocabularies'),
      '#options' => $options,
      '#default_value' => $this->options['vocabularies'],
      '#process' => array('form_process_checkboxes', 'ctools_dependent_process'),
      '#dependency' => array(
        'edit-options-argument-default-taxonomy-tid-limit' => array(1),
        'edit-options-argument-default-taxonomy-tid-node' => array(1),
      ),
    );

    $form['anyall'] = array(
      '#type' => 'radios',
      '#title' => t('Multiple-value handling'),
      '#default_value' => $this->options['anyall'],
      '#process' => array('form_process_radios', 'ctools_dependent_process'),
      '#options' => array(
        ',' => t('Filter to items that share all terms'),
        '+' => t('Filter to items that share any term'),
      ),
      '#dependency' => array(
        'edit-options-argument-default-taxonomy-tid-node' => array(1),
      ),
    );
  }

  function get_argument() {
    // Load default argument from taxonomy page.
    if (!empty($this->options['term_page'])) {
      if (arg(0) == 'taxonomy' && arg(1) == 'term' && is_numeric(arg(2))) {
        return arg(2);
      }
    }
    // Load default argument from node.
    if (!empty($this->options['node'])) {
      foreach (range(1, 3) as $i) {
        $node = menu_get_object('node', $i);
        if (!empty($node)) {
          break;
        }
      }
      // Just check, if a node could be detected.
      if ($node) {
        $taxonomy = array();
        $fields = field_info_instances('node', $node->type);
        foreach ($fields as $name => $info) {
          $field_info = field_info_field($name);
          if ($field_info['type'] == 'taxonomy_term_reference') {
            $items = field_get_items('node', $node, $name);
            if (is_array($items)) {
              foreach ($items as $item) {
                $taxonomy[$item['tid']] = $field_info['settings']['allowed_values'][0]['vocabulary'];
              }
            }
          }
          // Check for terms referenced by entityreference, if that option has been selected.
          elseif (!empty($this->options['node_term_entity']) && $field_info['type'] == 'entityreference') {
            $items = field_get_items('node', $node, $name);
            if (is_array($items)) {
              foreach ($items as $item) {
                $taxonomy[$item['target_id']] = key($field_info['settings']['handler_settings']['target_bundles']);
              }
            }
          }
        }
        if (!empty($this->options['limit'])) {
          $tids = array();
          // filter by vocabulary
          foreach ($taxonomy as $tid => $vocab) {
            if (!empty($this->options['vocabularies'][$vocab])) {
              $tids[] = $tid;
            }
          }
          return implode($this->options['anyall'], $tids);
        }
        // Return all tids.
        else {
          return implode($this->options['anyall'], array_keys($taxonomy));
        }
      }
    }

    // If the current page is a view that takes tid as an argument,
    // find the tid argument and return it.
    $views_page = views_get_page_view();
    if ($views_page && isset($views_page->argument['tid'])) {
      return $views_page->argument['tid']->argument;
    }
  }
}
