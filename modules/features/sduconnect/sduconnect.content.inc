<?php

/**
 * @file
 * Mapping of XML to node content.
 */

/**
 * Populate node object with PDC content.
 *
 * @param DOMXPath $xpath
 *   Xpath object.
 * @param object $node
 *   Node object.
 * @param bool $private
 *   Private flag.
 *
 * @throws \Exception
 */
function sduconnect_set_pdc_content(DOMXPath $xpath, &$node, $private = FALSE) {
  $formats = filter_formats();
  $default_format = reset($formats);

  $xpath->registerNameSpace('m', 'http://www.sduconnect.nl/Productoverzicht');
  $xpath->registerNameSpace('dcterms', 'http://purl.org/dc/terms/');
  $xpath->registerNameSpace('overheid', 'http://standaarden.overheid.nl/owms/terms/');
  $xpath->registerNameSpace('overheidproduct', 'http://standaarden.overheid.nl/product/terms/');
  $xpath->registerNameSpace('overheidsc', 'http://standaarden.overheid.nl/sc/terms/');

  $node->type = 'product';
  // Title.
  $node->title = (string) $xpath->query('//m:page/m:product/m:name')
    ->item(0)->nodeValue;

  // Body.
  $body = '';
  // Let op.
  $field = $xpath->query('//m:page/m:product/m:notice');
  if ($field->length) {
    $value = $field->item(0)->nodeValue;
    if (preg_match('/[a-z]/si', strip_tags($value))) {
      $body = '<div class="let-op"><strong>Let op:</strong> ' . $field->item(0)->nodeValue . '</div>';
    }
  }
  $body .= sduconnect_nodelist_to_string($xpath->query('//m:page/m:product/m:description'));

  $node->body = array(
    LANGUAGE_NONE => array(
      array(
        'value' => $body,
        'format' => $default_format->format,
      ),
    ),
  );

  // Related Product items.
  $items = sduconnect_nodelist_to_array($xpath->query('//m:page/m:product/m:product_relaties/m:product_relatie/@base_id'));
  if ($items) {
    $node->field_related_product = array(LANGUAGE_NONE => array());
    foreach ($items as $item) {
      $node->field_related_product[LANGUAGE_NONE][] = array('target_id' => SDUCONNECT_PRODUCT . '-' . trim($item));
    }
  }

  // Related FAQ items.
  $items = sduconnect_nodelist_to_array($xpath->query('//m:page/m:product/m:vac_relaties/m:vac_relatie/@base_id'));
  if ($items) {
    $node->field_related_faq = array(LANGUAGE_NONE => array());
    foreach ($items as $item) {
      $node->field_related_faq[LANGUAGE_NONE][] = array('target_id' => SDUCONNECT_FAQ . '-' . trim($item));
    }
  }

  $default_content_labels = array(
    'Let op' => '<strong>Let op:</strong> ',
    'Omschrijving' => '',
    'Gerelateerde producten' => '<h3>Gerelateerde producten</h3>',
    'Vraag en antwoord' => '<h3>Vraag en antwoord</h3>',
    'Meer informatie' => '<h3>Meer informatie</h3>',
    'Gerelateerde wetten / regelgeving' => '<h3>Gerelateerde wetten / regelgeving</h3>',
    'Voorwaarden' => '',
    'Aanpak' => '',
    'Formulieren' => '<h3>Formulieren</h3>',
    'Online aanvragen' => '<h3>Online aanvragen</h3>',
    'Termijn' => '',
    'Bezwaar en beroep' => '',
    'Kosten' => '',
    'Contact' => '',
    'Locatie' => '<h3>Locatie</h3>',
    'Indien periode' => '',
    'Beschikbaar budget' => '',
    'Maximale bijdrage' => '',
  );
  $content_labels = variable_get('sduconnect_content_labels', $default_content_labels);

  $default_content_structure = array(
    'before_tabs' => array(),
    '<h2>**Beschrijving</h2>' => array(
      'Let op',
      'Omschrijving',
      'Gerelateerde producten',
      'Vraag en antwoord',
      'Meer informatie',
      'Gerelateerde wetten / regelgeving',
    ),
    '<h2>**Voorwaarden</h2>' => array(
      'Voorwaarden',
    ),
    '<h2>**Aanpak</h2>' => array(
      'Aanpak',
      'Formulieren',
      'Online aanvragen',
    ),
    '<h2>**Termijn</h2>' => array(
      'Termijn',
    ),
    '<h2>**Bezwaar en beroep</h2>' => array(
      'Bezwaar en beroep',
    ),
    '<h2>**Kosten</h2>' => array(
      'Kosten',
    ),
    '<h2>**Contact</h2>' => array(
      'Contact',
      'Locatie',
      'Indien periode',
      'Beschikbaar budget',
      'Maximale bijdrage',
    ),
    'after_tabs' => array(),
  );
  $content_structure = variable_get('sduconnect_content_structure', $default_content_structure);

  $body_parts = array();

  // More info.
  $names = sduconnect_nodelist_to_array($xpath->query('//m:page/m:product/m:product_links/m:product_link/m:product_link_name'));
  $urls = sduconnect_nodelist_to_array($xpath->query('//m:page/m:product/m:product_links/m:product_link/m:product_link_url'));
  $items = array();
  foreach ($names as $i => $name) {
    $items[] = array(
      'url' => $urls[$i],
      'title' => $name,
    );
  }
  if ($items) {
    $node->field_product_more_information = array(LANGUAGE_NONE => $items);
  }

  // Related laws.
  $names = sduconnect_nodelist_to_array($xpath->query('//m:page/m:product/m:law_rules/m:law_rule/m:law_rule_name'));
  $urls = sduconnect_nodelist_to_array($xpath->query('//m:page/m:product/m:law_rules/m:law_rule/m:url'));
  $items = array();
  foreach ($names as $i => $name) {
    $items[] = array(
      'url' => $urls[$i],
      'title' => $name,
    );
  }
  if ($items) {
    $node->field_product_related_laws = array(LANGUAGE_NONE => $items);
  }

  // Forms.
  $names = sduconnect_nodelist_to_array($xpath->query('//m:page/m:product/m:formulieren_extern/m:formulier/m:label'));
  $urls = sduconnect_nodelist_to_array($xpath->query('//m:page/m:product/m:formulieren_extern/m:formulier/m:href'));
  $items = array();
  foreach ($names as $i => $name) {
    $items[] = array(
      'url' => $urls[$i],
      'title' => $name,
    );
  }
  if ($items) {
    $node->field_product_forms = array(LANGUAGE_NONE => $items);
  }

  // Helper array to map xml tags to fields in node.
  $mapping = array(
    '//m:page/m:product/m:contact' => 'field_product_contact',
    '//m:page/m:product/m:appeal' => 'field_product_objection',
    '//m:page/m:product/m:costs' => 'field_product_costs',
    '//m:page/m:product/m:procedure' => 'field_product_particulars',
    '//m:page/m:product/m:period' => 'field_product_period',
    '//m:page/m:product/m:conditions' => 'field_product_conditions',
  );

  // Here we set mapped fields.
  foreach ($mapping as $query => $filed_name) {
    $field = sduconnect_nodelist_to_string($xpath->query($query));
    if (strlen($field) > 0) {
      $node->$filed_name = array(
        LANGUAGE_NONE => array(
          array(
            'value' => $field,
            'format' => $default_format->format,
          ),
        ),
      );
    }
  }

  // Request online.
  $url = sduconnect_nodelist_to_string($xpath->query('//m:page/m:product/overheidproduct:scproduct/overheidproduct:meta/overheidproduct:scmeta/overheidproduct:aanvraagURL/@resourceIdentifier'));
  $items = array();
  if ($url) {
    $items[] = array(
      'href' => $url,
      'title' => 'Online aanvragen',
    );
  }
  if ($items) {
    $node->field_product_request = array(
      LANGUAGE_NONE => array(
        array(
          'value' => theme('links', array('links' => $items)),
          'format' => $default_format->format,
        ),
      ),
    );
  }

  // Location.
  $locations = array();
  $name = sduconnect_nodelist_to_array($xpath->query('//m:page/m:product/m:uitvoerende_instanties/m:instantie/m:naam'));
  // Postal address fields.
  $street = sduconnect_nodelist_to_array($xpath->query('//m:page/m:product/m:uitvoerende_instanties/m:instantie/m:p_straat'));
  $number = sduconnect_nodelist_to_array($xpath->query('//m:page/m:product/m:uitvoerende_instanties/m:instantie/m:p_nummer'));
  $postal_code = sduconnect_nodelist_to_array($xpath->query('//m:page/m:product/m:uitvoerende_instanties/m:instantie/m:p_postcode'));
  $po_box = sduconnect_nodelist_to_array($xpath->query('//m:page/m:product/m:uitvoerende_instanties/m:instantie/m:p_po_box'));
  $city = sduconnect_nodelist_to_array($xpath->query('//m:page/m:product/m:uitvoerende_instanties/m:instantie/m:p_woonplaats'));
  // Visiting address fields.
  $street_b = sduconnect_nodelist_to_array($xpath->query('//m:page/m:product/m:uitvoerende_instanties/m:instantie/m:b_straat'));
  $number_b = sduconnect_nodelist_to_array($xpath->query('//m:page/m:product/m:uitvoerende_instanties/m:instantie/m:b_nummer'));
  $postal_code_b = sduconnect_nodelist_to_array($xpath->query('//m:page/m:product/m:uitvoerende_instanties/m:instantie/m:b_pcode'));
  $city_b = sduconnect_nodelist_to_array($xpath->query('//m:page/m:product/m:uitvoerende_instanties/m:instantie/m:b_stad'));
  for ($i = 0; $i < count($name); ++$i) {
    $location = array();
    if ($name[$i]) {
      $location[] = $name[$i];
    }
    $visiting = empty($city[$i]);
    if ($visiting) {
      $location[] = $street_b[$i] ? trim($street_b[$i] . ' ' . $number_b[$i]) : '';
      $location[] = $postal_code_b[$i] . ' ' . $city_b[$i];
    }
    else {
      // PO Box often only contains a number in SDU.
      // Avoid a duplicate "Postbus " string in other cases.
      $po_box[$i] = preg_replace('/^postbus */i', '', $po_box[$i]);
      $location[] = $street[$i] ? trim($street[$i] . ' ' . $number[$i]) : ($po_box[$i] ? 'Postbus ' . $po_box[$i] : '');
      $location[] = $postal_code[$i] . ' ' . $city[$i];
    }
    $locations[] = '<p>' . implode('<br/>', array_map('check_plain', $location)) . '</p>';
  }
  if ($locations) {
    $body_parts['Locatie'] = $content_labels['Locatie'] . implode($locations);
  }

  // Indien periode.
  $field = $xpath->query('//m:page/m:product/m:subsidies');
  if ($field->length) {
    if (drupal_strlen((string) $field->item(0)->nodeValue)) {
      $body_parts['Indien periode'] = $content_labels['Indien periode'] . $field->item(0)->nodeValue;
    }
  }

  // Beschikbaar budget.
  $field = $xpath->query('//m:page/m:product/m:subsidies_budget');
  if ($field->length) {
    if (drupal_strlen((string) $field->item(0)->nodeValue)) {
      $body_parts['Beschikbaar budget'] = $content_labels['Beschikbaar budget'] . $field->item(0)->nodeValue;
    }
  }

  // Maximale bijdrage.
  $field = $xpath->query('//m:page/m:product/m:subsidies_maximale_bijdrage');
  if ($field->length) {
    if (drupal_strlen((string) $field->item(0)->nodeValue)) {
      $body_parts['Maximale bijdrage'] = $content_labels['Maximale bijdrage'] . $field->item(0)->nodeValue;
    }
  }

  // Tags.
  $a = sduconnect_nodelist_to_array($xpath->query('//m:page/m:product/m:doelgroepen/m:doelgroep'));
  $b = sduconnect_nodelist_to_array($xpath->query('//m:page/m:product/m:themes/m:theme/m:main/m:name'));
  $c = sduconnect_nodelist_to_array($xpath->query('//m:page/m:product/m:themes/m:theme/m:subthemes/m:subtheme/m:name'));
  $d = sduconnect_nodelist_to_array($xpath->query('//m:page/m:product/m:vind_themes/m:vind_theme/m:name'));
  $e = sduconnect_nodelist_to_array($xpath->query('//m:page/m:product/overheidproduct:scproduct/overheidproduct:meta/overheidproduct:owmsmantel/dcterms:subject'));
  $tags = array_unique(array_merge($a, $b, $c, $d, $e));
  $items = sduconnect_tags($tags);
  $node->field_tags = array(LANGUAGE_NONE => $items);

  // Samenwerkende Catalogi.
  $categories = sduconnect_nodelist_to_array($xpath->query('//m:page/m:product/m:doelgroepen/m:doelgroep'));
  $items = array();
  foreach ($categories as $category) {
    if ($category === 'organisatie/ondernemer') {
      $category = 'ondernemer';
    }
    $items[] = array(
      'value' => $category,
    );
  }
  // $node->field_sc_audience = array(LANGUAGE_NONE => $items); // @codingStandardsIgnoreLine

  // Start- and end date.
  $start = sduconnect_nodelist_to_string($xpath->query('//m:page/m:product/m:subsidies_startdate'));
  if ($start) {
    $start = implode('-', array_reverse(explode('-', $start))) . ' 00:00:00';
  }
  $end = sduconnect_nodelist_to_string($xpath->query('//m:page/m:product/m:subsidies_enddate'));
  if ($end) {
    $end = implode('-', array_reverse(explode('-', $end))) . ' 00:00:00';
  }
  $items = array();
  if ($start) {
    $items[] = array(
      'value' => $start,
      'value2' => $end,
      'timezone' => variable_get('date_default_timezone', 'Europe/Amsterdam'),
      'timezone_db' => 'UTC',
      'date_type' => 'datetime',
    );
  }
  // $node->field_date = array(LANGUAGE_NONE => $items); // @codingStandardsIgnoreLine

  // Group body_parts into tabs.
  $tabs = array();
  // @codingStandardsIgnoreStart
  /*
  foreach ($content_structure as $tab_title => $parts) {
    $tabs[$tab_title] = '';
    foreach($parts as $part_name) {
      if (isset($body_parts[$part_name])) {
        $tabs[$tab_title] .= $body_parts[$part_name];
      }
    }
  }
  // Merge tab content in body.
  $before_tabs = $tabs['before_tabs']; unset($tabs['before_tabs']);
  $after_tabs = $tabs['after_tabs']; unset($tabs['after_tabs']);
  $body = '';
  foreach ($tabs as $title => $content) {
    if (preg_match('/[a-z0-9]/si', strip_tags($content))) {
      $body .= $title . $content;
    }
  }
  $body = $before_tabs . $body . $after_tabs;
  $node->body = array(LANGUAGE_NONE => array(array(
    'value' => $body,
    'format' => 'filtered_html',
  )));
  */
  // @codingStandardsIgnoreEnd
}

/**
 * Populate node object with VAC content.
 *
 * @param object $xpath
 *   Xpath object.
 * @param object $node
 *   Node.
 * @param bool $private
 *   Import private fields.
 */
function sduconnect_set_vac_content($xpath, &$node, $private = FALSE) {
  $formats = filter_formats();
  $default_format = reset($formats);

  $xpath->registerNameSpace('vac', 'http://standaarden.overheid.nl/vac/');
  $xpath->registerNameSpace('dcterms', 'http://purl.org/dc/terms/');
  $xpath->registerNameSpace('overheid', 'http://standaarden.overheid.nl/owms/terms/');
  $xpath->registerNameSpace('overheidvac', 'http://standaarden.overheid.nl/vac/terms/');

  $node->type = 'faq';
  // Title.
  $node->title = sduconnect_nodelist_to_string($xpath->query('//vac:vacs/vac:vac/vac:body/vac:kanaal/vac:vraag'));

  // Body.
  $body = sduconnect_nodelist_to_string($xpath->query('//vac:vacs/vac:vac/vac:body/vac:kanaal/vac:antwoord/vac:antwoordTekst'));
  $body .= sduconnect_nodelist_to_string($xpath->query('//vac:vacs/vac:vac/vac:body/vac:kanaal/vac:antwoord/vac:antwoordProductVeld'));
  if ($private) {
    $body .= sduconnect_nodelist_to_string($xpath->query('//vac:vacs/vac:vac/vac:body/vac:kanaal/vac:onderwaterantwoord/vac:onderwaterantwoordTekst'));
    $body .= sduconnect_nodelist_to_string($xpath->query('//vac:vacs/vac:vac/vac:body/vac:kanaal/vac:onderwaterantwoord/vac:onderwaterantwoordProductVeld'));
  }

  $node->body = array(
    LANGUAGE_NONE => array(
      array(
        'value' => $body,
        'format' => $default_format->format,
      ),
    ),
  );

  // Explanation.
  $value = sduconnect_nodelist_to_string($xpath->query('//vac:vacs/vac:vac/vac:body/vac:kanaal/vac:toelichting'));
  $items = array();
  if (preg_match('/[a-z0-9]/si', strip_tags($value))) {
    $items[] = array(
      'value' => $value,
      'format' => $default_format->format,
    );
    $node->field_faq_additional_information = array(LANGUAGE_NONE => $items);
  }

  // Tags.
  $a = sduconnect_nodelist_to_array($xpath->query('//vac:vacs/vac:vac/vac:meta/vac:owmsmantel/dcterms:audience'));
  $b = sduconnect_nodelist_to_array($xpath->query('//vac:vacs/vac:vac/vac:meta/vac:vacmeta/vac:trefwoord'));
  $tags = array_unique(array_merge($a, $b));
  $items = sduconnect_tags($tags);
  $node->field_tags = array(LANGUAGE_NONE => $items);

  // Location.
  $locations = array();
  $name = sduconnect_nodelist_to_string($xpath->query('//vac:vacs/vac:vac/vac:body/vac:kanaal/vac:antwoord/vac:antwoordAdres/vac:instantie/vac:naam'));
  $street = sduconnect_nodelist_to_string($xpath->query('//vac:vacs/vac:vac/vac:body/vac:kanaal/vac:antwoord/vac:antwoordAdres/vac:instantie/vac:p_straat'));
  $number = sduconnect_nodelist_to_string($xpath->query('//vac:vacs/vac:vac/vac:body/vac:kanaal/vac:antwoord/vac:antwoordAdres/vac:instantie/vac:p_nummer'));
  $postal_code = sduconnect_nodelist_to_string($xpath->query('//vac:vacs/vac:vac/vac:body/vac:kanaal/vac:antwoord/vac:antwoordAdres/vac:instantie/vac:p_postcode'));
  $po_box = sduconnect_nodelist_to_string($xpath->query('//vac:vacs/vac:vac/vac:body/vac:kanaal/vac:antwoord/vac:antwoordAdres/vac:instantie/vac:p_po_box'));
  $city = sduconnect_nodelist_to_string($xpath->query('//vac:vacs/vac:vac/vac:body/vac:kanaal/vac:antwoord/vac:antwoordAdres/vac:instantie/vac:p_woonplaats'));

  if ($street) {
    $street = trim("$street $number");
  }
  else {
    $street = $po_box ? "Postbus $po_box" : '';
  }

  $location = array(
    'lid' => NULL,
    'name' => $name,
    'street' => $street,
    'additional' => '',
    'city' => $city,
    'province' => NULL,
    'postal_code' => $postal_code,
    'country' => NULL,
    'latitude' => NULL,
    'longitude' => NULL,
    'source' => NULL,
    'inhibit_geocode' => TRUE,
  );
  $settings = variable_get('location_settings_node_vac', array());
  $location['location_settings'] = $settings;
  if ($location['street']) {
    location_save($location);
    $locations[] = $location;
  }
  $node->field_location = array(LANGUAGE_NONE => $locations);

  // More information.
  $names = sduconnect_nodelist_to_array($xpath->query('//vac:vacs/vac:vac/vac:body/vac:kanaal/vac:verwijzingOverigeInfo'));
  $urls = sduconnect_nodelist_to_array($xpath->query('//vac:vacs/vac:vac/vac:body/vac:kanaal/vac:verwijzingOverigeInfo/@resourceIdentifier'));
  $items = array();
  foreach ($names as $i => $name) {
    $items[] = array(
      'url' => $urls[$i],
      'title' => $name,
    );
  }

  if ($items) {
    $node->field_faq_more_information = array(LANGUAGE_NONE => $items);
  }

  // Related Product items.
  $items = sduconnect_nodelist_to_array($xpath->query('//vac:vacs/vac:vac/vac:body/vac:verwijzingProduct/@resourceIdentifier'));
  if ($items) {
    $node->field_related_product = array(LANGUAGE_NONE => array());
    foreach ($items as $item) {
      $node->field_related_product[LANGUAGE_NONE][] = array('target_id' => SDUCONNECT_PRODUCT . '-' . trim($item));
    }
  }

  // Related FAQ items.
  $items = sduconnect_nodelist_to_array($xpath->query('//vac:vacs/vac:vac/vac:body/vac:verwijzingVac/@resourceIdentifier'));
  if ($items) {
    $node->field_related_faq = array(LANGUAGE_NONE => array());
    foreach ($items as $item) {
      $node->field_related_faq[LANGUAGE_NONE][] = array('target_id' => SDUCONNECT_FAQ . '-' . trim($item));
    }
  }
}

/**
 * Convert DOMXML node list to string.
 *
 * @param object $node_list
 *   List of nodes.
 *
 * @return string
 *   Trimmed string value.
 */
function sduconnect_nodelist_to_string($node_list) {
  if ($node_list->length) {
    return trim($node_list->item(0)->nodeValue);
  }
  return '';
}

/**
 * Convert DOMXML node list to array.
 *
 * @param object $node_list
 *   List of nodes.
 *
 * @return array
 *   Array of values.
 */
function sduconnect_nodelist_to_array($node_list) {
  $output = array();
  foreach ($node_list as $node) {
    $output[] = $node->nodeValue;
  }
  return $output;
}

/**
 * Get term id's for tags.
 *
 * @param string $names
 *   Term name.
 *
 * @return array
 *   Array of tid's.
 */
function sduconnect_tags($names) {
  $output = array();
  $vocab = 'tags';
  $names = array_filter(array_unique(array_map('trim', $names)));
  foreach ($names as $name) {
    $query = db_select('taxonomy_term_data', 'td')
      ->fields('td', array('tid'))
      ->condition('td.name', $name)
      ->range(0, 1);
    $v = $query->join('taxonomy_vocabulary', 'v', 'td.vid = v.vid');
    $query->condition("$v.machine_name", $vocab);
    $tid = $query->execute()->fetchField();
    if (!$tid) {
      $vid = db_select('taxonomy_vocabulary', 'v')
        ->fields('v', array('vid'))
        ->condition('v.machine_name', $vocab)
        ->execute()
        ->fetchField();
      $term = (object) array(
        'name' => $name,
        'vid' => $vid,
        'vocabulary_machine_name' => $vocab,
      );
      taxonomy_term_save($term);
      $tid = $term->tid;
    }
    $output[] = array(
      'tid' => $tid,
    );
  }
  return $output;
}