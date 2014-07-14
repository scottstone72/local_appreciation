<?php

/**
 * Render a list of entries in the database.
 */
function local_app_company_list() {
  global $user;
  // Check if user has permission to access this page.
  if(!user_access('view admin pages')) {
    return MENU_ACCESS_DENIED;
  }

  // Grab fields from users table.
  $entries = db_select('local_app_company', 'l')
    ->fields('l', array('id','company_name','image_path','image_file_id','coupon_value'))
    // Return the result in object format.
    ->execute();
  $output = '';

  // Get all entries in the {local_app_company} table.
  if (isset($entries)) {
    // Create header array for table
    $header = array(
      'id' => t('ID'),
      'company_name' => t('Company Name'),
      'image' => t('Image'),
      'coupon_value' => t('Coupon Value'),
      'edit_company' => t('Edit Coupon'),
      'delete_company' => t('Delete Company'),
    );

    ctools_include('ajax');
    ctools_include('modal');
    ctools_modal_add_js();
    //ctools_add_js('ajax-responder');

    // Create our own javascript that will be used to theme a modal.
    $sample_style = array(
      'local-app-edit-company-modal-style' => array( // local_app_modal_style is the identifier to our modal theme
        'modalSize' => array(
          'type' => 'fixed',
          'width' => 400,
          'height' => 320,
          'addWidth' => 10,
          'addHeight' => 10,
          'contentRight' => 40,
          'contentBottom' => 0,
        ),
        'modalOptions' => array(
          'opacity' => .6,
          'background-color' => 'black',
        ),
        'animation' => 'fadeIn',
        'animationSpeed' => 'fast',
        'modalTheme' => 'local_app',// append our custom theme to end of namespace in js file
        'closeImage' => theme('image', array(
            'path' => ctools_image_path('icon-close-window.png', 'local_app', 'images'),
            'alt' => t('Close window'), 'title' => t('Close window'))
        ),
        'closeText' => "<span id='close-modal-button'>Close</span>",
        /*'throbber' => theme('image', array(
          'path' => ctools_image_path('ajax-loader.gif', 'local_app'),
          'alt' => t('Loading...'), 'title' => t('Loading'))),*/
      ),
      // add another style for delete confirm page popup
      'local-app-delete-company-modal-style' => array( // local_app_modal_style is the identifier to our modal theme
        'modalSize' => array(
          'type' => 'fixed',
          'width' => 600,
          'height' => 260,
          'addWidth' => 10,
          'addHeight' => 10,
          'contentRight' => 0,
          'contentBottom' => 0,
        ),
        'modalOptions' => array(
          'opacity' => .6,
          'background-color' => 'black',
        ),
        'animation' => 'fadeIn',
        'animationSpeed' => 'fast',
        'modalTheme' => 'local_app',// append our custom theme to end of namespace in js file
        'closeImage' => theme('image', array(
            'path' => ctools_image_path('icon-close-window.png', 'local_app', 'images'),
            'alt' => t('Close window'), 'title' => t('Close window'))
        ),
        'closeText' => "<span id='close-modal-button'>Close</span>",
        /*'throbber' => theme('image', array(
          'path' => ctools_image_path('ajax-loader.gif', 'local_app'),
          'alt' => t('Loading...'), 'title' => t('Loading'))),*/
      ),
    );

    drupal_add_js($sample_style, 'setting');// add our js settings for the modal popup
    // load our custom css and js files for our custom modal
    ctools_add_js('local_app','local_app','js'); // load our custom js file
    ctools_add_css('local_app','local_app','css');// load our custom css file
    $rows = array();
    // Build the $rows array
    foreach ($entries as $entry) {
      // Sanitize the data before handing it off to the theme layer.
      $entry_arr = array_map('check_plain', (array) $entry);

      // Link for modal forms
      $links['edit_company_form'] = ctools_modal_text_button(t('Edit Coupon'), 'local_app/admin/company_list/nojs/edit/' . $entry_arr['id'],
        t('Edit Coupon Value'), 'ctools-modal-local-app-edit-company-modal-style');

      $links['delete_company_form'] = ctools_modal_text_button(t('Delete Company'), 'local_app/admin/company_list/nojs/delete/' . $entry_arr['id'],
        t('Delete Company Data'), 'ctools-modal-local-app-delete-company-modal-style');

      // Display and style image in the company list.
      $img_url = $entry_arr['image_path'];  // the orig image uri
      $style = 'thumbnail';  // or any other custom image style you've created via /admin/config/media/image-styles

      // Build the rows and add data
      // Add span tags with UID as identifier for ajax command in submit function
      // to remove row after deleting user with modal form.
      $rows[] = array(
        'id' => '<span id="local-app-modal-ajax-' . $entry_arr['id'] . '">' . $entry_arr['id'],
        'company_name' => $entry_arr['company_name'],
        'image_path' => '<img src="' . image_style_url($style, $img_url) .  '">',
        'coupon_value' => '<span id="coupon-value-' . $entry_arr['id']  . '">$' . $entry_arr["coupon_value"],
        'edit_user' => $links['edit_company_form'],
        'delete_user' => $links['delete_company_form'] . '</span>',
      );
    }

    $output = theme('table', array('header' => $header, 'rows' => $rows));
    //dsm($output);
  } else {
    drupal_set_message(t('No entries have been added yet.'));
  }

  return $output;

}